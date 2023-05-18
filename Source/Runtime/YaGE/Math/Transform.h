#pragma once

#include "Matrix4.h"
#include "Quaternion.h"

namespace YaGE {

class Transform {
public:
    /// @brief
    ///   Create an identity transform object.
    YAGE_FORCEINLINE Transform() noexcept
        : translate(),
          rotation(1.0f),
          scale(1.0f),
          localMatrix(1.0f),
          parentMatrix(1.0f),
          worldMatrix(1.0f),
          isDirty(false) {}

    /// @brief
    ///   Create a transform object from the given transform matrix.
    ///
    /// @param transform    The transform matrix to be decomposed.
    YAGE_FORCEINLINE Transform(Matrix4 transform) noexcept
        : translate(),
          rotation(),
          scale(),
          localMatrix(transform),
          parentMatrix(1.0f),
          worldMatrix(transform),
          isDirty(false) {
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
          parentMatrix(1.0f),
          worldMatrix(1.0f),
          isDirty(false) {
        localMatrix.Scale(scale).Rotate(rotation).Translate(translate);
        worldMatrix = localMatrix;
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
          parentMatrix(1.0f),
          worldMatrix(1.0f),
          isDirty(false) {
        localMatrix.Scale(this->scale).Rotate(rotation).Translate(this->translate);
        worldMatrix = localMatrix;
    }

    /// @brief
    ///   Update local transform components from the given matrix.
    ///
    /// @param transform    The transform matrix to be decomposed.
    YAGE_FORCEINLINE auto SetLocalTransform(Matrix4 transform) noexcept -> void {
        localMatrix = transform;
        DirectX::XMMatrixDecompose(&scale._vec, &rotation._vec, &translate._vec, transform._xmmat);
        worldMatrix = localMatrix * parentMatrix;
        isDirty     = false;
    }

    /// @brief
    ///   Set a new translate position for local transform.
    ///
    /// @param trans    The new local translate position.
    YAGE_FORCEINLINE auto SetLocalTranslate(Vector3 trans) noexcept -> void {
        translate = Vector4(trans, 0.0f);
        isDirty   = true;
    }

    /// @brief
    ///   Set a new translate position for local transform.
    ///
    /// @param trans    The new local translate position.
    YAGE_FORCEINLINE auto SetLocalTranslate(Vector4 trans) noexcept -> void {
        translate = trans;
        isDirty   = true;
    }

    /// @brief
    ///   Set a new translate position for local transform.
    ///
    /// @param x    The new local translate position on X axis.
    /// @param y    The new local translate position on Y axis.
    /// @param z    The new local translate position on Z axis.
    YAGE_FORCEINLINE auto SetLocalTranslate(float x, float y, float z) noexcept -> void {
        translate = Vector4(x, y, z, 0.0f);
        isDirty   = true;
    }

    /// @brief
    ///   Set a new rotation for local transform.
    ///
    /// @param rot    The new local rotation.
    YAGE_FORCEINLINE auto SetLocalRotation(Quaternion rot) noexcept -> void {
        rotation = rot;
        isDirty  = true;
    }

    /// @brief
    ///   Set a new rotation for local transform.
    ///
    /// @param axis    The axis to be rotated around.
    /// @param radius  The radius of rotation.
    YAGE_FORCEINLINE auto SetLocalRotation(Vector3 axis, float radius) noexcept -> void {
        rotation = Quaternion(axis, radius);
        isDirty  = true;
    }

    /// @brief
    ///   Set a new rotation for local transform.
    ///
    /// @param axis    The axis to be rotated around.
    /// @param radius  The radius of rotation.
    YAGE_FORCEINLINE auto SetLocalRotation(Vector4 axis, float radius) noexcept -> void {
        rotation = Quaternion(axis, radius);
        isDirty  = true;
    }

    /// @brief
    ///   Set a new Euler-angle rotation for local transform.
    ///
    /// @param pitch    The pitch angle in radians.
    /// @param yaw      The yaw angle in radians.
    /// @param roll     The roll angle in radians.
    YAGE_FORCEINLINE auto SetLocalRotation(float pitch, float yaw, float roll) noexcept -> void {
        rotation = Quaternion(pitch, yaw, roll);
        isDirty  = true;
    }

    /// @brief
    ///   Set new scale factors for local transform.
    ///
    /// @param factor    The new local scale factors.
    YAGE_FORCEINLINE auto SetLocalScale(Vector3 factor) noexcept -> void {
        scale   = Vector4(factor, 0.0f);
        isDirty = true;
    }

    /// @brief
    ///   Set new scale factors for local transform.
    ///
    /// @param factor    The new local scale factors.
    YAGE_FORCEINLINE auto SetLocalScale(Vector4 factor) noexcept -> void {
        scale   = factor;
        isDirty = true;
    }

    /// @brief
    ///   Set new scale factors for local transform.
    ///
    /// @param x    The new local scale factor on X axis.
    /// @param y    The new local scale factor on Y axis.
    /// @param z    The new local scale factor on Z axis.
    YAGE_FORCEINLINE auto SetLocalScale(float x, float y, float z) noexcept -> void {
        scale   = Vector4(x, y, z, 0.0f);
        isDirty = true;
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
        if (isDirty) {
            localMatrix = Matrix4(1.0f).Scale(scale).Rotate(rotation).Translate(translate);
            worldMatrix = localMatrix * parentMatrix;
            isDirty     = false;
        }

        return localMatrix;
    }

    /// @brief
    ///   Get world transform matrix.
    ///
    /// @return Matrix4
    ///   Return a matrix that represents world transform.
    YAGE_NODISCARD YAGE_FORCEINLINE auto WorldMatrix() const noexcept -> Matrix4 {
        if (isDirty) {
            localMatrix = Matrix4(1.0f).Scale(scale).Rotate(rotation).Translate(translate);
            worldMatrix = localMatrix * parentMatrix;
            isDirty     = false;
        }

        return worldMatrix;
    }

    /// @brief
    ///   Set parent transform for this transform.
    ///
    /// @param parent    The parent transform to be set.
    YAGE_FORCEINLINE auto SetParent(Matrix4 parent) noexcept -> void {
        parentMatrix = parent;
        isDirty      = true;
    }

    /// @brief
    ///   Set parent transform for this transform.
    /// @note
    ///   Reference of the parent transform object will not be cached in this object. Parent matrix must be updated manually every time when the parent transform is updated.
    ///
    /// @param parent    The parent transform to be set.
    YAGE_FORCEINLINE auto SetParent(const Transform &parent) noexcept -> void {
        parentMatrix = parent.WorldMatrix();
        isDirty      = true;
    }

    /// @brief
    ///   Get parent transform matrix.
    ///
    /// @return Matrix4
    ///   Return a matrix that represents parent transform.
    YAGE_NODISCARD auto ParentMatrix() const noexcept -> Matrix4 { return parentMatrix; }

private:
    // Local coordinate transform.
    Vector4    translate;
    Quaternion rotation;
    Vector4    scale;

    // Cached transform matrix.
    mutable Matrix4 localMatrix;
    mutable Matrix4 parentMatrix;
    mutable Matrix4 worldMatrix;

    mutable bool isDirty;
};

} // namespace YaGE
