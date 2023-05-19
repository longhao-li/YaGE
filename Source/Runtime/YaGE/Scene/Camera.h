#pragma once

#include "../Math/Numbers.h"
#include "../Math/Quaternion.h"

namespace YaGE {

class Camera {
public:
    /// @brief
    ///   Create a camera with default parameters.
    /// @remarks
    ///   Default parameters are:
    ///   - zNear: 0.1f
    ///   - zFar: 1000.0f
    ///   - fovY: Pi / 4.0f
    ///   - aspectRatio: 16.0f / 9.0f
    YAGE_FORCEINLINE Camera() noexcept
        : zNear(0.1f),
          zFar(1000.0f),
          fovY(Numbers::Pi<float> * 0.25f),
          aspectRatio(16.0f / 9.0f),
          pitch(0.0f),
          yaw(0.0f),
          viewIsDirty(true),
          projectionIsDirty(true),
          position(0.0f, 0.0f, 0.0f, 1.0f),
          viewMatrix(1.0f),
          projectionMatrix(1.0f) {}

    /// @brief
    ///   Initialize a camera with the given parameters.
    ///
    /// @param zNear        Near plane distance for this perspective camera.
    /// @param zFar         Far plane distance for this perspective camera.
    /// @param fovY         Field of view in Y direction for this perspective camera.
    /// @param aspectRatio  Aspect ratio for this perspective camera.
    YAGE_FORCEINLINE Camera(float zNear, float zFar, float fovY, float aspectRatio) noexcept
        : zNear(zNear),
          zFar(zFar),
          fovY(fovY),
          aspectRatio(aspectRatio),
          pitch(0.0f),
          yaw(0.0f),
          viewIsDirty(true),
          projectionIsDirty(true),
          position(0.0f, 0.0f, 0.0f, 1.0f),
          viewMatrix(1.0f),
          projectionMatrix(1.0f) {}

    /// @brief
    ///   Get distance to the near plane of this camera.
    ///
    /// @return float
    ///   Return distance to the near plane of this camera.
    YAGE_NODISCARD YAGE_FORCEINLINE auto ZNear() const noexcept -> float { return zNear; }

    /// @brief
    ///   Set a new distance to the near plane of this camera.
    ///
    /// @param z    New distance to the near plane of this camera.
    YAGE_FORCEINLINE auto SetZNear(float z) noexcept -> void {
        zNear             = z;
        projectionIsDirty = true;
    }

    /// @brief
    ///   Get distance to the far plane of this camera.
    ///
    /// @return float
    ///   Return distance to the far plane of this camera.
    YAGE_NODISCARD YAGE_FORCEINLINE auto ZFar() const noexcept -> float { return zFar; }

    /// @brief
    ///   Set a new distance to the far plane of this camera.
    ///
    /// @param z    New distance to the far plane of this camera.
    YAGE_FORCEINLINE auto SetZFar(float z) noexcept -> void {
        zFar              = z;
        projectionIsDirty = true;
    }

    /// @brief
    ///   Get field of view in Y direction of this camera.
    ///
    /// @return float
    ///   Return field of view in Y direction of this camera.
    YAGE_NODISCARD YAGE_FORCEINLINE auto FieldOfView() const noexcept -> float { return fovY; }

    /// @brief
    ///   Set a new field of view in Y direction of this camera.
    ///
    /// @param fov  New field of view in Y direction of this camera.
    YAGE_FORCEINLINE auto SetFieldOfView(float fov) noexcept -> void {
        fovY              = fov;
        projectionIsDirty = true;
    }

    /// @brief
    ///   Get aspect ratio of this camera.
    ///
    /// @return float
    ///   Return aspect ratio of this camera.
    YAGE_NODISCARD YAGE_FORCEINLINE auto AspectRatio() const noexcept -> float { return aspectRatio; }

    /// @brief
    ///   Set a new aspect ratio of this camera.
    ///
    /// @param ratio    New aspect ratio of this camera.
    YAGE_FORCEINLINE auto SetAspectRatio(float ratio) noexcept -> void {
        aspectRatio       = ratio;
        projectionIsDirty = true;
    }

    /// @brief
    ///   Get rotation radius around X axis of this camera.
    ///
    /// @return float
    ///   Return rotation radius around X axis of this camera.
    YAGE_NODISCARD YAGE_FORCEINLINE auto Pitch() const noexcept -> float { return pitch; }

    /// @brief
    ///   Set a new rotation radius around X axis of this camera.
    ///
    /// @param radius   New rotation radius around X axis of this camera.
    YAGE_FORCEINLINE auto SetPitch(float radius) noexcept -> void {
        pitch       = radius;
        viewIsDirty = true;
    }

    /// @brief
    ///   Get rotation radius around Y axis of this camera.
    ///
    /// @return float
    ///   Return rotation radius around Y axis of this camera.
    YAGE_NODISCARD YAGE_FORCEINLINE auto Yaw() const noexcept -> float { return yaw; }

    /// @brief
    ///   Set a new rotation radius around Y axis of this camera.
    ///
    /// @param radius   New rotation radius around Y axis of this camera.
    YAGE_FORCEINLINE auto SetYaw(float radius) noexcept -> void {
        yaw         = radius;
        viewIsDirty = true;
    }

    /// @brief
    ///   Get rotation of this camera. This rotation is equal to creating quaternion from pitch and yaw.
    /// @note
    ///   This method will create a new quaternion every time it is called. For performance consideration, you should cache the result if you need to use it for multiple times.
    ///
    /// @return Quaternion
    ///   Return a quaternion that represents rotation of this camera.
    YAGE_NODISCARD YAGE_FORCEINLINE auto Rotation() const noexcept -> Quaternion {
        return Quaternion(pitch, yaw, 0.0f);
    }

    /// @brief
    ///   Get position of this camera.
    ///
    /// @return Vector4
    ///   Return position of this camera.
    YAGE_NODISCARD YAGE_FORCEINLINE auto Position() const noexcept -> Vector4 { return position; }

    /// @brief
    ///   Set a new position of this camera.
    ///
    /// @param pos  New position of this camera.
    YAGE_FORCEINLINE auto SetPosition(Vector3 pos) noexcept -> void {
        position    = Vector4(pos, 1.0f);
        viewIsDirty = true;
    }

    /// @brief
    ///   Set a new position of this camera.
    ///
    /// @param x    X component of new position of this camera.
    /// @param y    Y component of new position of this camera.
    /// @param z    Z component of new position of this camera.
    YAGE_FORCEINLINE auto SetPosition(float x, float y, float z) noexcept -> void {
        position    = Vector4(x, y, z, 1.0f);
        viewIsDirty = true;
    }

    /// @brief
    ///   Set a new position of this camera.
    ///
    /// @param pos  New position of this camera. Must be a valid homogeneous coordinate point.
    YAGE_FORCEINLINE auto SetPosition(Vector4 pos) noexcept -> void {
        position    = pos / pos.w;
        viewIsDirty = true;
    }

    /// @brief
    ///   Get front direction of this camera.
    ///
    /// @return Vector4
    ///   Return a homogeneous coordinate vector that represents front direction of this camera.
    YAGE_NODISCARD YAGE_FORCEINLINE auto Front() const noexcept -> Vector4 {
        return Vector4(0.0f, 0.0f, 1.0f, 0.0f) * Matrix4(Rotation());
    }

    /// @brief
    ///   Get up direction of this camera.
    ///
    /// @return Vector4
    ///   Return a homogeneous coordinate vector that represents up direction of this camera.
    YAGE_NODISCARD YAGE_FORCEINLINE auto Up() const noexcept -> Vector4 {
        return Vector4(0.0f, 1.0f, 0.0f, 0.0f) * Matrix4(Rotation());
    }

    /// @brief
    ///   Get right direction of this camera.
    ///
    /// @return Vector4
    ///   Return a homogeneous coordinate vector that represents right direction of this camera.
    YAGE_NODISCARD YAGE_FORCEINLINE auto Right() const noexcept -> Vector4 {
        return Vector4(1.0f, 0.0f, 0.0f, 0.0f) * Matrix4(Rotation());
    }

    /// @brief
    ///   Get view transform matrix of this camera.
    ///
    /// @return Matrix4
    ///   Return a 4x4 matrix that represents view transform of this camera.
    YAGE_NODISCARD YAGE_FORCEINLINE auto ViewMatrix() const noexcept -> Matrix4 {
        if (viewIsDirty) {
            Matrix4 rotation(Rotation());
            Vector4 front = Vector4(0.0f, 0.0f, 1.0f, 0.0f) * rotation;
            Vector4 up    = Vector4(0.0f, 1.0f, 0.0f, 0.0f) * rotation;
            viewMatrix    = Matrix4(1.0f).LookTo(position, front, up);
            viewIsDirty   = false;
        }

        return viewMatrix;
    }

    /// @brief
    ///   Get projection transform matrix of this camera.
    ///
    /// @return Matrix4
    ///   Return a 4x4 matrix that represents projection transform of this camera.
    YAGE_NODISCARD YAGE_FORCEINLINE auto ProjectionMatrix() const noexcept -> Matrix4 {
        if (projectionIsDirty) {
            projectionMatrix  = Matrix4(1.0f).Perspective(fovY, aspectRatio, zNear, zFar);
            projectionIsDirty = false;
        }

        return projectionMatrix;
    }

private:
    /// @brief  Near plane distance.
    float zNear;

    /// @brief  Far plane distance.
    float zFar;

    /// @brief  Field of view in Y direction.
    float fovY;

    /// @brief  Aspect ratio of this camera.
    float aspectRatio;

    /// @brief  Pitch rotation radius of this camera.
    float pitch;

    /// @brief  Yaw rotation radius of this camera.
    float yaw;

    /// @brief  Used to mark whether view matrix is dirty.
    mutable bool viewIsDirty;

    /// @brief  Used to mark whether projection matrix is dirty.
    mutable bool projectionIsDirty;

    /// @brief  Position of this camera.
    Vector4 position;

    /// @brief  Cached view matrix of this camera.
    mutable Matrix4 viewMatrix;

    /// @brief  Cached projection matrix of this camera.
    mutable Matrix4 projectionMatrix;
};

} // namespace YaGE
