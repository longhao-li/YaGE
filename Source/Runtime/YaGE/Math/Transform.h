#pragma once

#include "Matrix4.h"
#include "Quaternion.h"

namespace YaGE {

class Transform {
public:
    /// @brief
    ///   Create an identity transform object.
    YAGE_FORCEINLINE Transform() noexcept
        : translate(), rotation(1.0f), scale(1.0f), localMatrix(1.0f), parent(nullptr), isLocalDirty(false) {}

    /// @brief
    ///   Create a transform object from the given transform matrix.
    ///
    /// @param transform    The transform matrix to be decomposed.
    YAGE_FORCEINLINE Transform(Matrix4 transform) noexcept
        : translate(), rotation(), scale(), localMatrix(transform), parent(nullptr), isLocalDirty(false) {
        DirectX::XMMatrixDecompose(&scale._vec, &rotation._vec, &translate._vec, transform._xmmat);
    }

    /// @brief
    ///   Create a transform object from the given translation, rotation and scale.
    ///
    /// @param scale        A vector that represents scale factors.
    /// @param rotation     A quaternion that represents rotation.
    /// @param translate    A vector that represents translation.
    YAGE_FORCEINLINE Transform(Vector4 scale, Quaternion rotation, Vector4 translate) noexcept
        : translate(translate),
          rotation(rotation),
          scale(scale),
          localMatrix(1.0f),
          parent(nullptr),
          isLocalDirty(false) {
        localMatrix.Scale(scale).Rotate(rotation).Translate(translate);
    }

    /// @brief
    ///   Create a transform object from the given translation, rotation and scale.
    ///
    /// @param scale        A vector that represents scale factors.
    /// @param rotation     A quaternion that represents rotation.
    /// @param translate    A vector that represents translation.
    YAGE_FORCEINLINE Transform(Vector3 scale, Quaternion rotation, Vector3 translate) noexcept
        : translate(translate, 0.0f),
          rotation(rotation),
          scale(scale, 0.0f),
          localMatrix(1.0f),
          parent(nullptr),
          isLocalDirty(false) {
        localMatrix.Scale(this->scale).Rotate(rotation).Translate(this->translate);
    }

    /// @brief
    ///   Update local transform components from the given matrix.
    ///
    /// @param transform    The transform matrix to be decomposed.
    YAGE_FORCEINLINE auto SetLocalTransform(Matrix4 transform) noexcept -> void {
        localMatrix = transform;
        DirectX::XMMatrixDecompose(&scale._vec, &rotation._vec, &translate._vec, transform._xmmat);
        isLocalDirty = false;
    }

    /// @brief
    ///   Set a new translate position for local transform.
    ///
    /// @param trans    The new local translate position.
    YAGE_FORCEINLINE auto SetLocalTranslate(Vector3 trans) noexcept -> void {
        translate    = Vector4(trans, 0.0f);
        isLocalDirty = true;
    }

    /// @brief
    ///   Set a new translate position for local transform.
    ///
    /// @param trans    The new local translate position.
    YAGE_FORCEINLINE auto SetLocalTranslate(Vector4 trans) noexcept -> void {
        translate    = trans;
        isLocalDirty = true;
    }

    /// @brief
    ///   Set a new translate position for local transform.
    ///
    /// @param x    The new local translate position on X axis.
    /// @param y    The new local translate position on Y axis.
    /// @param z    The new local translate position on Z axis.
    YAGE_FORCEINLINE auto SetLocalTranslate(float x, float y, float z) noexcept -> void {
        translate    = Vector4(x, y, z, 0.0f);
        isLocalDirty = true;
    }

    /// @brief
    ///   Set a new rotation for local transform.
    ///
    /// @param rot    The new local rotation.
    YAGE_FORCEINLINE auto SetLocalRotation(Quaternion rot) noexcept -> void {
        rotation     = rot;
        isLocalDirty = true;
    }

    /// @brief
    ///   Set a new rotation for local transform.
    ///
    /// @param axis    The axis to be rotated around.
    /// @param radius  The radius of rotation.
    YAGE_FORCEINLINE auto SetLocalRotation(Vector3 axis, float radius) noexcept -> void {
        rotation     = Quaternion(axis, radius);
        isLocalDirty = true;
    }

    /// @brief
    ///   Set a new rotation for local transform.
    ///
    /// @param axis    The axis to be rotated around.
    /// @param radius  The radius of rotation.
    YAGE_FORCEINLINE auto SetLocalRotation(Vector4 axis, float radius) noexcept -> void {
        rotation     = Quaternion(axis, radius);
        isLocalDirty = true;
    }

    /// @brief
    ///   Set a new Euler-angle rotation for local transform.
    ///
    /// @param pitch    The pitch angle in radians.
    /// @param yaw      The yaw angle in radians.
    /// @param roll     The roll angle in radians.
    YAGE_FORCEINLINE auto SetLocalRotation(float pitch, float yaw, float roll) noexcept -> void {
        rotation     = Quaternion(pitch, yaw, roll);
        isLocalDirty = true;
    }

    /// @brief
    ///   Set new scale factors for local transform.
    ///
    /// @param factor    The new local scale factors.
    YAGE_FORCEINLINE auto SetLocalScale(Vector3 factor) noexcept -> void {
        scale        = Vector4(factor, 0.0f);
        isLocalDirty = true;
    }

    /// @brief
    ///   Set new scale factors for local transform.
    ///
    /// @param factor    The new local scale factors.
    YAGE_FORCEINLINE auto SetLocalScale(Vector4 factor) noexcept -> void {
        scale        = factor;
        isLocalDirty = true;
    }

    /// @brief
    ///   Set new scale factors for local transform.
    ///
    /// @param x    The new local scale factor on X axis.
    /// @param y    The new local scale factor on Y axis.
    /// @param z    The new local scale factor on Z axis.
    YAGE_FORCEINLINE auto SetLocalScale(float x, float y, float z) noexcept -> void {
        scale        = Vector4(x, y, z, 0.0f);
        isLocalDirty = true;
    }

    /// @brief
    ///   Get local translate position.
    ///
    /// @return Vector4
    ///   Return the local translate position.
    YAGE_NODISCARD YAGE_FORCEINLINE auto LocalTranslate() const noexcept -> Vector4 { return translate; }

    /// @brief
    ///   Get local rotation.
    ///
    /// @return Quaternion
    ///   Return a quaternion that represents local rotation.
    YAGE_NODISCARD YAGE_FORCEINLINE auto LocalRotation() const noexcept -> Quaternion { return rotation; }

    /// @brief
    ///   Get local scale factors.
    ///
    /// @return Vector4
    ///   Return a vector that represents local scale factors.
    YAGE_NODISCARD YAGE_FORCEINLINE auto LocalScale() const noexcept -> Vector4 { return scale; }

    /// @brief
    ///   Get local transform matrix.
    ///
    /// @return Matrix4
    ///   Return a matrix that represents local transform.
    YAGE_NODISCARD YAGE_FORCEINLINE auto LocalMatrix() const noexcept -> Matrix4 {
        if (isLocalDirty) {
            localMatrix  = Matrix4(1.0f).Scale(scale).Rotate(rotation).Translate(translate);
            isLocalDirty = false;
        }

        return localMatrix;
    }

    /// @brief
    ///   Get transform matrix.
    /// @note
    ///   This function will recursively calculate world transform matrix from parent transform. Therefore, for performace consideration, please cache the result if you need to use it multiple times.
    ///
    /// @return Matrix4
    ///   Return a matrix that represents world transform.
    YAGE_NODISCARD YAGE_FORCEINLINE auto TransformMatrix() const noexcept -> Matrix4 {
        Matrix4 parentMatrix(1.0f);
        if (parent != nullptr) {
            Transform *current = parent;
            while (current != nullptr) {
                parentMatrix = current->LocalMatrix() * parentMatrix;
                current      = current->parent;
            }
        }

        return parentMatrix * LocalMatrix();
    }

    /// @brief
    ///   Set parent transform for this transform.
    /// @note
    ///   Child transforms will cache pointer to parent transform. Therefore, parent transforms must be pinned.
    ///
    /// @param newParent    The parent transform to be set.
    YAGE_FORCEINLINE auto SetParent(Transform &newParent) noexcept -> void { parent = &newParent; }

    /// @brief
    ///   Remove parent transform for this transform.
    YAGE_FORCEINLINE auto RemoveParent() noexcept -> void { parent = nullptr; }

    /// @brief
    ///   Get parent transform object.
    ///
    /// @return Transform *
    ///   Return a transform object that represents parent transform.
    YAGE_NODISCARD YAGE_FORCEINLINE auto Parent() const noexcept -> Transform * { return parent; }

    /// @brief
    ///   Checks if this transform has a parent transform.
    ///
    /// @return bool
    /// @retval true    This transform has a parent transform.
    /// @retval false   This transform does not have a parent transform.
    YAGE_NODISCARD YAGE_FORCEINLINE auto HasParent() const noexcept -> bool { return parent != nullptr; }

private:
    // Local coordinate transform.
    Vector4    translate;
    Quaternion rotation;
    Vector4    scale;

    // Cached transform matrix.
    mutable Matrix4 localMatrix;

    Transform   *parent;
    mutable bool isLocalDirty;
};

} // namespace YaGE
