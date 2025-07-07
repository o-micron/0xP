#include <SceneDescriptor/Attachments/XPTransform.h>

#include <Engine/XPRegistry.h>
#include <Physics/Interface/XPIPhysics.h>
#include <SceneDescriptor/Attachments/XPCollider.h>
#include <SceneDescriptor/Attachments/XPRigidbody.h>
#include <SceneDescriptor/XPNode.h>
#include <SceneDescriptor/XPScene.h>
#include <Utilities/XPMaths.h>

void
Transform::onChanged_location()
{
    XPMat4<float>::buildModelMatrix(
      modelMatrix,
      location,
      euler,
      scale,
      static_cast<XPMat4<float>::ModelMatrixOperations>(XPMat4<float>::ModelMatrixOperation_Translation |
                                                        XPMat4<float>::ModelMatrixOperation_Rotation |
                                                        XPMat4<float>::ModelMatrixOperation_Scale));
    if (owner->hasColliderAttachment() && owner->hasRigidbodyAttachment()) {
        XPScene*    scene    = owner->getAbsoluteScene();
        XPRegistry* registry = scene->getRegistry();
        XPIPhysics* physics  = registry->getPhysics();
        physics->setRigidbodyPositionAndRotation(owner);
    }
}

void
Transform::onChanged_euler()
{
    XPMat4<float>::buildModelMatrix(
      modelMatrix,
      location,
      euler,
      scale,
      static_cast<XPMat4<float>::ModelMatrixOperations>(XPMat4<float>::ModelMatrixOperation_Translation |
                                                        XPMat4<float>::ModelMatrixOperation_Rotation |
                                                        XPMat4<float>::ModelMatrixOperation_Scale));
    if (owner->hasColliderAttachment() && owner->hasRigidbodyAttachment()) {
        XPScene*    scene    = owner->getAbsoluteScene();
        XPRegistry* registry = scene->getRegistry();
        XPIPhysics* physics  = registry->getPhysics();
        physics->setRigidbodyPositionAndRotation(owner);
    }
}

void
Transform::onChanged_scale()
{
    XPMat4<float>::buildModelMatrix(
      modelMatrix,
      location,
      euler,
      scale,
      static_cast<XPMat4<float>::ModelMatrixOperations>(XPMat4<float>::ModelMatrixOperation_Translation |
                                                        XPMat4<float>::ModelMatrixOperation_Rotation |
                                                        XPMat4<float>::ModelMatrixOperation_Scale));
}

void
Transform::onChanged_modelMatrix()
{
    XPMat4<float>::decomposeModelMatrix(modelMatrix, location, euler, scale);
    if (owner->hasColliderAttachment() && owner->hasRigidbodyAttachment()) {
        XPScene*    scene    = owner->getAbsoluteScene();
        XPRegistry* registry = scene->getRegistry();
        XPIPhysics* physics  = registry->getPhysics();
        physics->setRigidbodyPositionAndRotation(owner);
    }
}
