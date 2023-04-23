#include "CommandBuffer.h"
#include "../Core/Exception.h"
#include "RenderDevice.h"

#include <mutex>
#include <queue>
#include <stack>

using namespace YaGE;

namespace {

static constexpr const uint32_t DEFAULT_PAGE_SIZE = 0x200000; // 2 MiB

enum class TempBufferType {
    Upload,
    UnorderedAccess,
};

class TempBufferPage final : public GpuResource {
public:
    /// @brief
    ///   Create a new temp buffer page.
    ///
    /// @param bufferType   Type of the temp buffer page.
    /// @param size         Size in byte of the temp buffer page.
    ///
    /// @throw RenderAPIException
    ///   Thrown if failed to create D3D12 resource.
    TempBufferPage(TempBufferType bufferType, size_t size);

    /// @brief
    ///   Create a new temp buffer page with default size.
    ///
    /// @param bufferType   Type of the temp buffer page.
    ///
    /// @throw RenderAPIException
    ///   Thrown if failed to create D3D12 resource.
    TempBufferPage(TempBufferType bufferType) : TempBufferPage(bufferType, DEFAULT_PAGE_SIZE) {}

    /// @brief
    ///   Move constructor of Page.
    ///
    /// @param other    The page to be moved.
    TempBufferPage(TempBufferPage &&other) noexcept
        : GpuResource(std::move(other)), size(other.size), data(other.data), gpuAddress(other.gpuAddress) {
        other.size       = 0;
        other.data       = nullptr;
        other.gpuAddress = 0;
    }

    /// @brief
    ///   Destroy this temp buffer page.
    ~TempBufferPage() noexcept override;

    /// @brief
    ///   Map resource to a typed system address. Only available for update buffer.
    ///
    /// @tparam T   Type of the mapped address.
    ///
    /// @return T *
    ///   Return pointer to the mapped resource.
    template <typename T>
    YAGE_NODISCARD YAGE_FORCEINLINE auto Map() noexcept -> T * {
        return static_cast<T *>(data);
    }

    /// @brief
    ///   Get GPU address to the start of this temp buffer page.
    ///
    /// @return uint64_t
    ///   Return GPU address to the start of this temp buffer page.
    YAGE_NODISCARD YAGE_FORCEINLINE auto GpuAddress() const noexcept -> uint64_t { return gpuAddress; }

    /// @brief
    ///   Checks if this is a default page.
    ///
    /// @return bool
    /// @retval true   This is a default page.
    /// @retval false  This is not a default page.
    YAGE_NODISCARD YAGE_FORCEINLINE auto IsDefaultPage() const noexcept -> bool { return size == DEFAULT_PAGE_SIZE; }

private:
    /// @brief  Size in byte of this temp buffer page.
    size_t size;

    /// @brief  CPU pointer to start of this temp buffer page.
    void *data;

    /// @brief  GPU address to start of this temp buffer page.
    uint64_t gpuAddress;
};

TempBufferPage::TempBufferPage(TempBufferType bufferType, size_t size)
    : GpuResource(), size(size), data(nullptr), gpuAddress(0) {
    RenderDevice &device = RenderDevice::Singleton();

    if (bufferType == TempBufferType::Upload) {
        const D3D12_HEAP_PROPERTIES heapProps{
            /* Type                 = */ D3D12_HEAP_TYPE_UPLOAD,
            /* CPUPageProperty      = */ D3D12_CPU_PAGE_PROPERTY_UNKNOWN,
            /* MemoryPoolPreference = */ D3D12_MEMORY_POOL_UNKNOWN,
            /* CreationNodeMask     = */ 0,
            /* VisibleNodeMask      = */ 0,
        };

        const D3D12_RESOURCE_DESC desc{
            /* Dimension        = */ D3D12_RESOURCE_DIMENSION_BUFFER,
            /* Alignment        = */ 0,
            /* Width            = */ size,
            /* Height           = */ 1,
            /* DepthOrArraySize = */ 1,
            /* MipLevels        = */ 1,
            /* Format           = */ DXGI_FORMAT_UNKNOWN,
            /* SampleDesc       = */
            {
                /* Count   = */ 1,
                /* Quality = */ 0,
            },
            /* Layout = */ D3D12_TEXTURE_LAYOUT_ROW_MAJOR,
            /* Flags  = */ D3D12_RESOURCE_FLAG_NONE,
        };

        HRESULT hr = device.Device()->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &desc,
                                                              D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
                                                              IID_PPV_ARGS(resource.GetAddressOf()));
        if (FAILED(hr))
            throw RenderAPIException(hr, u"Failed to create temporary upload buffer page.");

        usageState = D3D12_RESOURCE_STATE_GENERIC_READ;
        resource->Map(0, nullptr, &data);
        gpuAddress = resource->GetGPUVirtualAddress();
    } else {
        const D3D12_HEAP_PROPERTIES heapProps{
            /* Type                 = */ D3D12_HEAP_TYPE_DEFAULT,
            /* CPUPageProperty      = */ D3D12_CPU_PAGE_PROPERTY_UNKNOWN,
            /* MemoryPoolPreference = */ D3D12_MEMORY_POOL_UNKNOWN,
            /* CreationNodeMask     = */ 0,
            /* VisibleNodeMask      = */ 0,
        };

        const D3D12_RESOURCE_DESC desc{
            /* Dimension        = */ D3D12_RESOURCE_DIMENSION_BUFFER,
            /* Alignment        = */ 0,
            /* Width            = */ size,
            /* Height           = */ 1,
            /* DepthOrArraySize = */ 1,
            /* MipLevels        = */ 1,
            /* Format           = */ DXGI_FORMAT_UNKNOWN,
            /* SampleDesc       = */
            {
                /* Count   = */ 1,
                /* Quality = */ 0,
            },
            /* Layout = */ D3D12_TEXTURE_LAYOUT_ROW_MAJOR,
            /* Flags  = */ D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
        };

        HRESULT hr = device.Device()->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &desc,
                                                              D3D12_RESOURCE_STATE_COMMON, nullptr,
                                                              IID_PPV_ARGS(resource.GetAddressOf()));
        if (FAILED(hr))
            throw RenderAPIException(hr, u"Failed to create temporary unordered access buffer page.");

        usageState = D3D12_RESOURCE_STATE_COMMON;
        gpuAddress = resource->GetGPUVirtualAddress();
    }
}

TempBufferPage::~TempBufferPage() noexcept {
    if (data != nullptr) {
        resource->Unmap(0, nullptr);
        data = nullptr;
    }
}

class TempBufferPageManager {
public:
    /// @brief
    ///   Create a temp buffer page manager.
    TempBufferPageManager();

    /// @brief
    ///   Copy constructor of temp buffer page manager is disabled.
    TempBufferPageManager(const TempBufferPageManager &) = delete;

    /// @brief
    ///   Copy assignment of temp buffer page manager is disabled.
    auto operator=(const TempBufferPageManager &) = delete;

    /// @brief
    ///   Destroy this temp buffer page manager.
    ~TempBufferPageManager() noexcept;

    /// @brief
    ///   Allocate a new upload temp buffer page.
    ///
    /// @param size     Expected size in byte of the new temp buffer page.
    ///
    /// @return TempBufferPage *
    ///   Return pointer to the new temp buffer page.
    /// @throw RenderAPIException
    ///   Throw if failed to create new temp buffer page.
    YAGE_NODISCARD auto AllocateUploadPage(size_t size) -> TempBufferPage *;

    /// @brief
    ///   Allocate a new unordered access temp buffer page.
    ///
    /// @param size     Expected size in byte of the new temp buffer page.
    ///
    /// @return TempBufferPage *
    ///   Return pointer to the new temp buffer page.
    /// @throw RenderAPIException
    ///   Throw if failed to create new temp buffer page.
    YAGE_NODISCARD auto AllocateUnorderedAccessPage(size_t size) -> TempBufferPage *;

    /// @brief
    ///   Free retired upload pages.
    ///
    /// @param syncPoint    The sync point that indicates when the retired pages can be reused.
    /// @param pages        The pages to be freed.
    auto FreeUploadPages(uint64_t syncPoint, const std::vector<void *> &pages) noexcept -> void;

    /// @brief
    ///   Free retired unordered access pages.
    ///
    /// @param syncPoint    The sync point that indicates when the retired pages can be reused.
    /// @param pages        The pages to be freed.
    auto FreeUnorderedAccessPages(uint64_t syncPoint, const std::vector<void *> &pages) noexcept -> void;

    /// @brief
    ///   Get the singleton instance of temp buffer page manager.
    ///
    /// @return TempBufferPageManager &
    ///   Return reference to the temp buffer page manager singleton instance.
    YAGE_NODISCARD static auto Singleton() -> TempBufferPageManager &;

private:
    /// @brief  The render device that is used to create new buffer pages and sync with GPU.
    RenderDevice &renderDevice;

    /// @brief  Temp buffer page pool. This is used to cache all allocated pages.
    std::stack<TempBufferPage> pagePool;

    /// @brief  Mutex to protect page pool.
    mutable std::mutex pagePoolMutex;

    /// @brief  Retired upload pages to be reused.
    std::queue<std::pair<uint64_t, TempBufferPage *>> retiredUploadPages;

    /// @brief  Mutex to protect retired upload page queue.
    mutable std::mutex retiredUploadQueueMutex;

    /// @brief  Retired unordered access pages to be reused.
    std::queue<std::pair<uint64_t, TempBufferPage *>> retiredUnorderedAccessPages;

    /// @brief  Mutex to protect retired unordered access page queue.
    mutable std::mutex retiredUnorderedAccessQueueMutex;

    /// @brief  Queue of pages to be deleted.
    std::queue<std::pair<uint64_t, TempBufferPage *>> deletionQueue;

    /// @brief  Mutex to protect deletion queue.
    mutable std::mutex deletionQueueMutex;
};

TempBufferPageManager::TempBufferPageManager()
    : renderDevice(RenderDevice::Singleton()),
      pagePool(),
      pagePoolMutex(),
      retiredUploadPages(),
      retiredUploadQueueMutex(),
      retiredUnorderedAccessPages(),
      retiredUnorderedAccessQueueMutex(),
      deletionQueue(),
      deletionQueueMutex() {}

TempBufferPageManager::~TempBufferPageManager() noexcept {
    renderDevice.Sync();
    std::lock_guard<std::mutex> lock(deletionQueueMutex);
    while (!deletionQueue.empty()) {
        delete deletionQueue.front().second;
        deletionQueue.pop();
    }
}

YAGE_NODISCARD auto TempBufferPageManager::AllocateUploadPage(size_t size) -> TempBufferPage * {
    // Align up size.
    size = (size + 255) & ~size_t(255);

    if (size <= DEFAULT_PAGE_SIZE) {
        { // Try to get a free page from free page queue.
            std::lock_guard<std::mutex> lock(retiredUploadQueueMutex);
            if (!retiredUploadPages.empty()) {
                if (renderDevice.IsSyncPointReached(retiredUploadPages.front().first)) {
                    auto page = retiredUploadPages.front().second;
                    retiredUploadPages.pop();
                    return page;
                }
            }
        }

        { // Create a new page.
            TempBufferPage newPage(TempBufferType::Upload);

            std::lock_guard<std::mutex> lock(pagePoolMutex);
            pagePool.emplace(std::move(newPage));
            return &pagePool.top();
        }
    }

    return new TempBufferPage(TempBufferType::Upload, size);
}

YAGE_NODISCARD auto TempBufferPageManager::AllocateUnorderedAccessPage(size_t size) -> TempBufferPage * {
    // Align up size.
    size = (size + 255) & ~size_t(255);

    if (size <= DEFAULT_PAGE_SIZE) {
        { // Try to get a free page from free page queue.
            std::lock_guard<std::mutex> lock(retiredUnorderedAccessQueueMutex);
            if (!retiredUnorderedAccessPages.empty()) {
                if (renderDevice.IsSyncPointReached(retiredUnorderedAccessPages.front().first)) {
                    auto page = retiredUnorderedAccessPages.front().second;
                    retiredUnorderedAccessPages.pop();
                    return page;
                }
            }
        }

        { // Create a new page.
            TempBufferPage newPage(TempBufferType::UnorderedAccess);

            std::lock_guard<std::mutex> lock(pagePoolMutex);
            pagePool.emplace(std::move(newPage));
            return &pagePool.top();
        }
    }

    return new TempBufferPage(TempBufferType::UnorderedAccess, size);
}

auto TempBufferPageManager::FreeUploadPages(uint64_t syncPoint, const std::vector<void *> &pages) noexcept -> void {
    { // Free default pages.
        std::lock_guard<std::mutex> lock(retiredUploadQueueMutex);
        for (const auto &page : pages) {
            auto bufferPage = static_cast<TempBufferPage *>(page);
            if (bufferPage->IsDefaultPage())
                retiredUploadPages.emplace(syncPoint, bufferPage);
        }
    }

    { // Free deletion pages.
        std::lock_guard<std::mutex> lock(deletionQueueMutex);
        for (const auto &page : pages) {
            auto bufferPage = static_cast<TempBufferPage *>(page);
            if (!bufferPage->IsDefaultPage())
                deletionQueue.emplace(syncPoint, bufferPage);
        }
    }
}

auto TempBufferPageManager::FreeUnorderedAccessPages(uint64_t syncPoint, const std::vector<void *> &pages) noexcept
    -> void {
    { // Free default pages.
        std::lock_guard<std::mutex> lock(retiredUnorderedAccessQueueMutex);
        for (const auto &page : pages) {
            auto bufferPage = static_cast<TempBufferPage *>(page);
            if (bufferPage->IsDefaultPage())
                retiredUnorderedAccessPages.emplace(syncPoint, bufferPage);
        }
    }

    { // Free deletion pages.
        std::lock_guard<std::mutex> lock(deletionQueueMutex);
        for (const auto &page : pages) {
            auto bufferPage = static_cast<TempBufferPage *>(page);
            if (!bufferPage->IsDefaultPage())
                deletionQueue.emplace(syncPoint, bufferPage);
        }
    }
}

YAGE_NODISCARD auto TempBufferPageManager::Singleton() -> TempBufferPageManager & {
    static TempBufferPageManager instance;
    return instance;
}

} // namespace

YaGE::CommandBuffer::TempBufferAllocator::TempBufferAllocator() noexcept
    : uploadPage(),
      uploadPageOffset(),
      retiredUploadPages(),
      unorderedAccessPage(),
      unorderedAccessPageOffset(),
      retiredUnorderedAccessPages() {}

YaGE::CommandBuffer::TempBufferAllocator::~TempBufferAllocator() noexcept {
    CleanUp(RenderDevice::Singleton().AcquireSyncPoint());
}

YAGE_NODISCARD auto YaGE::CommandBuffer::TempBufferAllocator::AllocateUploadBuffer(size_t size)
    -> TempBufferAllocation {
    // Align up allocate size.
    size = (size + 255) & ~size_t(255);

    TempBufferPageManager &pageManager = TempBufferPageManager::Singleton();

    // Allocate a single page if size is larger than default page size.
    if (size >= DEFAULT_PAGE_SIZE) {
        TempBufferPage *page = pageManager.AllocateUploadPage(size);
        retiredUploadPages.push_back(page);

        return TempBufferAllocation{
            /* resource   = */ page,
            /* size       = */ size,
            /* offset     = */ 0,
            /* data       = */ page->Map<void>(),
            /* gpuAddress = */ page->GpuAddress(),
        };
    }

    // There is no enought space in current page, retire current page.
    if (uploadPage != nullptr && uploadPageOffset + size > DEFAULT_PAGE_SIZE) {
        retiredUploadPages.push_back(uploadPage);
        uploadPage = nullptr;
    }

    // Allocate a new page if current page is null.
    if (uploadPage == nullptr) {
        uploadPage       = pageManager.AllocateUploadPage(DEFAULT_PAGE_SIZE);
        uploadPageOffset = 0;
    }

    TempBufferPage *const page = static_cast<TempBufferPage *>(uploadPage);

    TempBufferAllocation allocation{
        /* resource   = */ page,
        /* size       = */ size,
        /* offset     = */ uploadPageOffset,
        /* data       = */ page->Map<uint8_t>() + uploadPageOffset,
        /* gpuAddress = */ page->GpuAddress() + uploadPageOffset,
    };

    uploadPageOffset += size;
    return allocation;
}

YAGE_NODISCARD auto YaGE::CommandBuffer::TempBufferAllocator::AllocateUnorderedAccessBuffer(size_t size)
    -> TempBufferAllocation {
    // Align up allocate size.
    size = (size + 255) & ~size_t(255);

    TempBufferPageManager &pageManager = TempBufferPageManager::Singleton();

    // Allocate a single page if size is larger than default page size.
    if (size >= DEFAULT_PAGE_SIZE) {
        TempBufferPage *page = pageManager.AllocateUnorderedAccessPage(size);
        retiredUnorderedAccessPages.push_back(page);

        return TempBufferAllocation{
            /* resource   = */ page,
            /* size       = */ size,
            /* offset     = */ 0,
            /* data       = */ page->Map<void>(),
            /* gpuAddress = */ page->GpuAddress(),
        };
    }

    // There is no enought space in current page, retire current page.
    if (unorderedAccessPage != nullptr && unorderedAccessPageOffset + size > DEFAULT_PAGE_SIZE) {
        retiredUnorderedAccessPages.push_back(unorderedAccessPage);
        unorderedAccessPage = nullptr;
    }

    // Allocate a new page if current page is null.
    if (unorderedAccessPage == nullptr) {
        unorderedAccessPage       = pageManager.AllocateUnorderedAccessPage(DEFAULT_PAGE_SIZE);
        unorderedAccessPageOffset = 0;
    }

    TempBufferPage *const page = static_cast<TempBufferPage *>(unorderedAccessPage);

    TempBufferAllocation allocation{
        /* resource   = */ page,
        /* size       = */ size,
        /* offset     = */ unorderedAccessPageOffset,
        /* data       = */ page->Map<uint8_t>() + unorderedAccessPageOffset,
        /* gpuAddress = */ page->GpuAddress() + unorderedAccessPageOffset,
    };

    unorderedAccessPageOffset += size;
    return allocation;
}

auto YaGE::CommandBuffer::TempBufferAllocator::CleanUp(uint64_t syncPoint) noexcept -> void {
    TempBufferPageManager &pageManager = TempBufferPageManager::Singleton();

    if (uploadPage != nullptr) {
        retiredUploadPages.push_back(uploadPage);
        uploadPage = nullptr;
    }

    if (!retiredUploadPages.empty()) {
        pageManager.FreeUploadPages(syncPoint, retiredUploadPages);
        retiredUploadPages.clear();
    }

    if (unorderedAccessPage != nullptr) {
        retiredUnorderedAccessPages.push_back(unorderedAccessPage);
        unorderedAccessPage = nullptr;
    }

    if (!retiredUnorderedAccessPages.empty()) {
        pageManager.FreeUnorderedAccessPages(syncPoint, retiredUnorderedAccessPages);
        retiredUnorderedAccessPages.clear();
    }
}

YaGE::CommandBuffer::CommandBuffer()
    : renderDevice(RenderDevice::Singleton()),
      commandList(),
      allocator(),
      lastSubmitSyncPoint(),
      tempBufferAllocator() {
    // Acquire allocator.
    allocator = renderDevice.AcquireCommandAllocator();

    // Create command list.
    HRESULT hr = renderDevice.Device()->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, allocator, nullptr,
                                                          IID_PPV_ARGS(commandList.GetAddressOf()));

    if (FAILED(hr)) {
        renderDevice.FreeCommandAllocator(0, allocator);
        throw RenderAPIException(hr, u"Failed to create command list.");
    }
}

YaGE::CommandBuffer::~CommandBuffer() noexcept {
    if (allocator != nullptr) {
        renderDevice.FreeCommandAllocator(lastSubmitSyncPoint, allocator);
        allocator = nullptr;
    }

    tempBufferAllocator.CleanUp(lastSubmitSyncPoint);
}

auto YaGE::CommandBuffer::Submit() -> uint64_t {
    commandList->Close();

    { // Submit command list to execute.
        ID3D12CommandList *list = commandList.Get();
        renderDevice.CommandQueue()->ExecuteCommandLists(1, &list);
    }

    // Acquire sync point.
    lastSubmitSyncPoint = renderDevice.AcquireSyncPoint();

    // Clean up temp buffer allocator.
    tempBufferAllocator.CleanUp(lastSubmitSyncPoint);

    // Reset allocator.
    renderDevice.FreeCommandAllocator(lastSubmitSyncPoint, allocator);
    allocator = renderDevice.AcquireCommandAllocator();

    // Reset command list.
    commandList->Reset(allocator, nullptr);

    return lastSubmitSyncPoint;
}

auto YaGE::CommandBuffer::Reset() -> void {
    commandList->Close();
    tempBufferAllocator.CleanUp(lastSubmitSyncPoint);

    if (allocator == nullptr)
        allocator = renderDevice.AcquireCommandAllocator();
    else
        allocator->Reset();

    commandList->Reset(allocator, nullptr);
}
