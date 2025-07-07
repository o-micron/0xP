/// --------------------------------------------------------------------------------------
/// Copyright 2025 Omar Sherif Fathy
///
/// Licensed under the Apache License, Version 2.0 (the "License");
/// you may not use this file except in compliance with the License.
/// You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
///
/// Unless required by applicable law or agreed to in writing, software
/// distributed under the License is distributed on an "AS IS" BASIS,
/// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
/// See the License for the specific language governing permissions and
/// limitations under the License.
/// --------------------------------------------------------------------------------------

#include <Physics/Jolt/XPJoltPhysics.h>

#include <DataPipeline/XPDataPipelineStore.h>
#include <DataPipeline/XPFile.h>
#include <DataPipeline/XPMeshAsset.h>
#include <DataPipeline/XPMeshBuffer.h>
#include <Engine/XPRegistry.h>
#include <Renderer/Interface/XPIRenderer.h>
#include <SceneDescriptor/XPAttachments.h>
#include <SceneDescriptor/XPLayer.h>
#include <SceneDescriptor/XPNode.h>
#include <SceneDescriptor/XPScene.h>
#include <Utilities/XPLogger.h>

#include <tuple>
#include <unordered_map>

#ifdef __clang__
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wall"
    #pragma clang diagnostic ignored "-Weverything"
#endif

// Always keep it before including others
#include <Jolt/Jolt.h>
// ------------------------------------------
#include <Jolt/Core/Factory.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Physics/Body/BodyActivationListener.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/CapsuleShape.h>
#include <Jolt/Physics/Collision/Shape/ConvexHullShape.h>
#include <Jolt/Physics/Collision/Shape/CylinderShape.h>
#include <Jolt/Physics/Collision/Shape/MeshShape.h>
#include <Jolt/Physics/Collision/Shape/PlaneShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/Collision/Shape/StaticCompoundShape.h>
#include <Jolt/Physics/PhysicsSettings.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/RegisterTypes.h>
#ifdef __clang__
    #pragma clang diagnostic pop
#endif

#include <cstdarg>
#include <thread>

// Disable common warnings triggered by Jolt, you can use JPH_SUPPRESS_WARNING_PUSH/POP to store/restore warning state
JPH_SUPPRESS_WARNINGS

// All Jolt symbols are in the JPH namespace
using namespace JPH;

// If you want your code to compile using single/double precision write 0.0_r to get a Real value that compiles to float
// or double depending if JPH_DOUBLE_PRECISION is set or not.
using namespace JPH::literals;

// Callback for traces, connect this to your own trace function if you have one
static void
TraceImpl(const char* inFMT, ...)
{
    va_list list;
    va_start(list, inFMT);
    char buffer[1024];
    vsnprintf(buffer, sizeof(buffer), inFMT, list);
    va_end(list);

    // Print to the TTY
    XP_LOGV(XPLoggerSeverityTrace, "%s", buffer);
}

#ifdef JPH_ENABLE_ASSERTS

// Callback for asserts, connect this to your own assert handler if you have one
static bool
AssertFailedImpl(const char* inExpression, const char* inMessage, const char* inFile, uint inLine)
{
    // Print to the TTY
    std::stringstream ss;
    ss << inFile << ":" << inLine << ": (" << inExpression << ") " << (inMessage != nullptr ? inMessage : "");

    XP_LOGV(XPLoggerSeverityError, "Jolt Assertion failure\n%s\n", ss.str().c_str());

    // Breakpoint
    return true;
}
#endif

// Layer that objects can be in, determines which other objects it can collide with
// Typically you at least want to have 1 layer for moving bodies and 1 layer for static bodies,
// but you can have more layers if you want. E.g. you could have a layer for high detail collision
// (which is not used by the physics simulation but only if you do collision testing).
namespace Layers {
static constexpr ObjectLayer NON_MOVING = 0;
static constexpr ObjectLayer MOVING     = 1;
static constexpr ObjectLayer NUM_LAYERS = 2;
};

// Class that determines if two object layers can collide
class ObjectLayerPairFilterImpl : public ObjectLayerPairFilter
{
  public:
    virtual bool ShouldCollide(ObjectLayer inObject1, ObjectLayer inObject2) const override
    {
        switch (inObject1) {
            case Layers::NON_MOVING: return inObject2 == Layers::MOVING; // non moving only collides with moving
            case Layers::MOVING: return true;                            // moving collides with everything
            default: JPH_ASSERT(false); return false;
        }
    }
};

// Each broadphase layer results in a separate bounding volume hierarchy tree in the broadphase.
// You at least want to have a layer for non-moving and moving objects to avoid having to update
// a tree full of static objects every frame.
// You can have a 1-on-1 mapping between object layers and broadphase layers (like in this case)
// but if you have many object layers you'll be creating many broadphase trees, which is inefficient.
// If you want to fine tune your broadphase layers define JPH_BROADPHASE_STATS and look at the stats
// reported on the TTY
namespace BroadPhaseLayers {
static constexpr BroadPhaseLayer NON_MOVING(0);
static constexpr BroadPhaseLayer MOVING(1);
static constexpr uint            NUM_LAYERS(2);
};

// BroadPhaseLayerInterface implementation
// This defines a mapping between object and broadphase layers.
class BPLayerInterfaceImpl final : public BroadPhaseLayerInterface
{
  public:
    BPLayerInterfaceImpl()
    {
        // Create a mapping table from object to broadphase layer
        mObjectToBroadPhase[Layers::NON_MOVING] = BroadPhaseLayers::NON_MOVING;
        mObjectToBroadPhase[Layers::MOVING]     = BroadPhaseLayers::MOVING;
    }
    virtual uint            GetNumBroadPhaseLayers() const override { return BroadPhaseLayers::NUM_LAYERS; }
    virtual BroadPhaseLayer GetBroadPhaseLayer(ObjectLayer inLayer) const override
    {
        JPH_ASSERT(inLayer < Layers::NUM_LAYERS);
        return mObjectToBroadPhase[inLayer];
    }
#if defined(JPH_EXTERNAL_PROFILE) || defined(JPH_PROFILE_ENABLED)
    virtual const char* GetBroadPhaseLayerName(BroadPhaseLayer inLayer) const override
    {
        switch ((BroadPhaseLayer::Type)inLayer) {
            case (BroadPhaseLayer::Type)BroadPhaseLayers::NON_MOVING: return "NON_MOVING";
            case (BroadPhaseLayer::Type)BroadPhaseLayers::MOVING: return "MOVING";
            default: JPH_ASSERT(false); return "INVALID";
        }
    }
#endif

  private:
    BroadPhaseLayer mObjectToBroadPhase[Layers::NUM_LAYERS];
};

// class that determines if an object layer can collide with a broadphase layer
class ObjectVsBroadPhaseLayerFilterImpl : public ObjectVsBroadPhaseLayerFilter
{
  public:
    virtual bool ShouldCollide(ObjectLayer inLayer1, BroadPhaseLayer inLayer2) const override
    {
        switch (inLayer1) {
            case Layers::NON_MOVING: return inLayer2 == BroadPhaseLayers::MOVING;
            case Layers::MOVING: return true;
            default: JPH_ASSERT(false); return false;
        }
    }
};

// Contact listener
class XPPhysicsContactListener : public ContactListener
{
  public:
    virtual ValidateResult OnContactValidate(const Body&               inBody1,
                                             const Body&               inBody2,
                                             RVec3Arg                  inBaseOffset,
                                             const CollideShapeResult& inCollisionResult) override
    {
        XP_LOG(XPLoggerSeverityInfo, "Contact validate callback");

        // Allows you to ignore a contact before it is created (using layers to not make objects collide is cheaper)
        return ValidateResult::AcceptAllContactsForThisBodyPair;
    }

    virtual void OnContactAdded(const Body&            inBody1,
                                const Body&            inBody2,
                                const ContactManifold& inManifold,
                                ContactSettings&       ioSettings) override
    {
        XP_LOG(XPLoggerSeverityInfo, "A Contact was added");
    }

    virtual void OnContactPersisted(const Body&            inBody1,
                                    const Body&            inBody2,
                                    const ContactManifold& inManifold,
                                    ContactSettings&       ioSettings) override
    {
        XP_LOG(XPLoggerSeverityInfo, "A Contact was presisted");
    }

    virtual void OnContactRemoved(const SubShapeIDPair& inSubShapePair) override
    {
        XP_LOG(XPLoggerSeverityInfo, "A Contact was removed");
    }
};

// Activation listener
class XPPhysicsBodyActivationListener : public BodyActivationListener
{
  public:
    virtual void OnBodyActivated(const BodyID& inBodyID, uint64 inBodyUserData) override
    {
        XP_LOG(XPLoggerSeverityInfo, "A body got activated");
        bodies_to_update.insert({ (Collider*)inBodyUserData, inBodyID });
    }
    virtual void OnBodyDeactivated(const BodyID& inBodyID, uint64 inBodyUserData) override
    {
        XP_LOG(XPLoggerSeverityInfo, "A body went to sleep");
        bodies_to_update.erase((Collider*)inBodyUserData);
    }

    std::unordered_map<Collider*, BodyID> bodies_to_update;
};

XPJoltPhysics::XPJoltPhysics(XPRegistry* const registry)
  : XPIPhysics(registry)
  , _registry(registry)
  , _isPlaying(true)
{
}

XPJoltPhysics::~XPJoltPhysics() {}

void
XPJoltPhysics::initialize()
{
    // Register allocation hook. In this example we'll just let Jolt use malloc / free but you can override these
    // if you want (see memory.h). This needs to be done before any other Jolt function is called
    RegisterDefaultAllocator();

    // Install trace and assert callbacks
    Trace = TraceImpl;
    JPH_IF_ENABLE_ASSERTS(AssertFailed = AssertFailedImpl;)

    // Create a factory, this class is responsible for creating instances of classes based on their name or hash
    // and is mainly used for deserialization of saved data.
    // It is not directly used in this example but still required.
    Factory::sInstance = new Factory();

    // Register all physics types with the factory and install their collision handlers with the CollisionDispatch
    // class. If you have your own custom shape types you probably need to register their handlers with the
    // collisionDispatch before calling this function.
    // If you implement your own default material (PhysicsMaterial::sDefault) make sure to initialize it before this
    // function or else this function will create one for you.
    RegisterTypes();

    // We need a temp allocator for temporary allocations during the physics update.
    // We're pre-allocating 80 MB to avoid having to do allocations during the physics simulation update.
    // If you don't want to pre-allocate you can also use TempAllocatorMalloc to fallback to malloc/free.
    _temp_allocator = XP_NEW JPH::TempAllocatorImpl(100 * 1024 * 1024);

    // We need a job system that will execute physics jobs on multiple threads.
    // Typically you would implement the JobSystem interface yourself and let Jolt Physics run on top of your own job
    // scheduler. JobSystemThreadPool is an example implementation.
    _job_system = XP_NEW JPH::JobSystemThreadPool(cMaxPhysicsJobs, cMaxPhysicsBarriers, 4);

    // This is the max amount of rigidbodies that you can add to the physics system.
    // If you try to add more you'll get an error.
    const uint cMaxBodies = 65536;

    // This determines how many mutexes to allocate to protect rigidbodies from concurrent access.
    // Set it to 0 for the default settings.
    const uint cNumBodyMutexes = 0;

    // This is the max amount of body pairs that can be queued at any time (the broadphase will detect overlapping
    // body pairs based on their bounding boxes and will insert them into a queue for the narrowphase).
    // If you make this buffer too small the queue will fill up and the broadphase jobs will start to do narrow
    // phase work. This is slightly less efficient.
    const uint cMaxBodyPairs = 65536;

    // This is the maximum size of the contact constraint buffer. If more contacts (collisions between bodies) are
    // detected than this number then these contacts will be ignored and bodies will start interpenetrating / fall
    // through the world.
    const uint cMaxContactConstraints = 20480;

    // Create mapping table from object layer to broadphase layer
    // Note: As this is an interface, PhysicsSystem will take a reference to this so this instance needs to stay alive!
    // Also have a look at BroadPhaseLayerInterfaceTable or BroadPhaseLayerInterfaceMask for a simpler interface.
    _broad_phase_layer_interface = XP_NEW BPLayerInterfaceImpl;

    // Create class that filters object vs broadphase layers
    // Note: As this is an interface, PhysicsSystem will take a reference to this so this instance needs to stay alive!
    // Also have a look at ObjectVsBroadPhaseLayerFilterTable or ObjectVsBroadPhaseLayerFilterMask for a simpler
    // interface.
    _object_vs_broadphase_layer_filter = XP_NEW ObjectVsBroadPhaseLayerFilterImpl;

    // Create class that filters object vs object layers
    // Note: As this is an interface, PhysicsSystem will take a reference to this so this instance needs to stay alive!
    // Also have a look at ObjectLayerPairFilterTable or ObjectLayerPairFilterMask for a simpler interface.
    _object_vs_object_layer_filter = XP_NEW ObjectLayerPairFilterImpl;

    // We simulate the physics world in discrete time steps. 60 Hz is a good rate to update the physics system.
    // const float cDeltaTime = 1.0f / 60.0f;
}

void
XPJoltPhysics::finalize()
{
    for (auto& scenePair : _scenes) {
        destroyScene(scenePair.first);
        // we are erasing the scenePair.first from the scenes when destroyScene is called ..
        // add dummy to keep size and iteration from crashing ..
        _scenes[scenePair.first];
    }
    _scenes.clear();
    XP_DELETE _object_vs_object_layer_filter;
    XP_DELETE _object_vs_broadphase_layer_filter;
    XP_DELETE _broad_phase_layer_interface;
    XP_DELETE _job_system;
    XP_DELETE _temp_allocator;

    // Unregisters all types with the factory and cleans up the default material
    UnregisterTypes();

    delete JPH::Factory::sInstance;
    JPH::Factory::sInstance = nullptr;
}

XPProfilable void
XPJoltPhysics::update()
{
    if (_isPlaying) {
        float               deltaTime      = _registry->getRenderer()->getDeltaTime();
        const uint          collisionSteps = 1;
        XPPhysicsSceneData& sceneData      = getOrCreateScene();
        JPH::BodyInterface& bodyInterface  = sceneData._physics_system->GetBodyInterface();
        sceneData._physics_system->Update(deltaTime, collisionSteps, _temp_allocator, _job_system);

        JPH::RMat44 mat;
        JPH::RVec3  position;
        JPH::Quat   rotation;
        bool        hasTransformChanges = false;
        for (const auto& bodyPair : sceneData._body_activation_listener->bodies_to_update) {
            Collider* collider = bodyPair.first;
            XPNode*   node     = collider->owner;
            if (node->hasTransformAttachment()) {
                bodyInterface.GetPositionAndRotation(bodyPair.second, position, rotation);
                JPH::Vec3 eulerAngles            = rotation.GetEulerAngles();
                node->getTransform()->location.x = position.GetX();
                node->getTransform()->location.y = position.GetY();
                node->getTransform()->location.z = position.GetZ();
                node->getTransform()->euler.x    = eulerAngles.GetX();
                node->getTransform()->euler.y    = eulerAngles.GetY();
                node->getTransform()->euler.z    = eulerAngles.GetZ();
                hasTransformChanges              = true;
            }
        }
        _registry->getScene()->addAttachmentChanges(XPEInteractionHasTransformChanges, false);
    }
}

XPProfilable void
XPJoltPhysics::onSceneTraitsChanged()
{
    XPScene* scene = _registry->getScene();
    if (scene->hasAnyAttachmentChanges(XPEInteractionHasColliderChanges | XPEInteractionHasRigidbodyChanges)) {
        for (auto& layer : scene->getLayers()) {
            for (auto& node : layer->getNodes()) {
                if (node->hasAnyAttachmentChanges(XPEInteractionHasColliderChanges |
                                                  XPEInteractionHasRigidbodyChanges)) {
                    destroyColliders(node);
                    if (node->hasColliderAttachment() && node->hasRigidbodyAttachment()) { createColliders(node); }
                }
            }
        }
        scene->removeAttachmentChanges(XPEInteractionHasColliderChanges | XPEInteractionHasRigidbodyChanges);
    }
}

void
XPJoltPhysics::play()
{
    _isPlaying = true;
}

void
XPJoltPhysics::pause()
{
    _isPlaying = false;
}

void
XPJoltPhysics::fetchModelMatrix(void* rigidbody, void* shape, XPMat4<float>& model)
{
}

XPProfilable void
XPJoltPhysics::createColliders(XPNode* node)
{
    XPPhysicsSceneData& physicsSceneData = getOrCreateScene();

    if (physicsSceneData._bodies.find(node->getId()) != physicsSceneData._bodies.end()) {
        // no need to create anything, just return
        return;
    }

    Collider* collider = node->getCollider();

    if (collider->info.size() == 1) {
        switch (collider->info[0].shape) {
            case XPEColliderShapePlane: {
                createPlaneCollider(node);
            } break;
            case XPEColliderShapeBox: {
                createBoxCollider(node);
            } break;
            case XPEColliderShapeSphere: {
                createSphereCollider(node);
            } break;
            case XPEColliderShapeCapsule: {
                createCapsuleCollider(node);
            } break;
            case XPEColliderShapeCylinder: {
                createCylinderCollider(node);
            } break;
            case XPEColliderShapeConvexMesh: {
                createConvexMeshCollider(node);
            } break;
            case XPEColliderShapeTriangleMesh: {
                createTriMeshCollider(node);
            } break;

            default:
                assert(false);
                XP_LOG(XPLoggerSeverityFatal, "Unreachable");
                break;
        }
    } else {
        // compound shapes
        createCompoundCollider(node);
    }
}

XPProfilable void
XPJoltPhysics::destroyColliders(XPNode* node)
{
    XPScene* scene = node->getAbsoluteScene();
    if (_scenes.find(scene->getId()) == _scenes.end()) { return; }

    XPPhysicsSceneData& sceneData = _scenes[scene->getId()];
    if (sceneData._bodies.find(node->getId()) != sceneData._bodies.end()) {
        JPH::Body* body = sceneData._bodies[node->getId()];

        auto& bodyInterface = sceneData._physics_system->GetBodyInterface();
        bodyInterface.DeactivateBody(body->GetID());
        bodyInterface.RemoveBody(body->GetID());
        bodyInterface.DestroyBody(body->GetID());

        sceneData._bodies.erase(node->getId());
        Collider* collider = node->getCollider();
        for (size_t i = 0; i < collider->info.size(); ++i) {
            XPColliderInfo& colliderInfo = collider->info[i];
            sceneData._body_activation_listener->bodies_to_update.erase(colliderInfo.owner);
        }
    }
}

XPProfilable JPH::ShapeRefC
             createPlaneShape(XPColliderInfo& colliderInfo)
{
    BoxShapeSettings shapeSettings(
      Vec3(colliderInfo.parameters.boxWidth <= 0.1f ? 0.1f : colliderInfo.parameters.boxWidth,
           0.1f,
           colliderInfo.parameters.boxDepth <= 0.1f ? 0.1f : colliderInfo.parameters.boxDepth));
    shapeSettings.SetEmbedded();
    ShapeSettings::ShapeResult shapeResult = shapeSettings.Create();
    assert(shapeResult.IsValid());
    ShapeRefC shape       = shapeResult.Get();
    colliderInfo.shapeRef = (void*)shape.GetPtr();
    return shape;
}

XPProfilable JPH::ShapeRefC
             createBoxShape(XPColliderInfo& colliderInfo)
{
    BoxShapeSettings shapeSettings(
      Vec3(colliderInfo.parameters.boxWidth <= 0.1f ? 0.1f : colliderInfo.parameters.boxWidth,
           colliderInfo.parameters.boxHeight <= 0.1f ? 0.1f : colliderInfo.parameters.boxHeight,
           colliderInfo.parameters.boxDepth <= 0.1f ? 0.1f : colliderInfo.parameters.boxDepth));
    shapeSettings.SetEmbedded();
    ShapeSettings::ShapeResult shapeResult = shapeSettings.Create();
    assert(shapeResult.IsValid());
    ShapeRefC shape       = shapeResult.Get();
    colliderInfo.shapeRef = (void*)shape.GetPtr();
    return shape;
}

XPProfilable JPH::ShapeRefC
             createSphereShape(XPColliderInfo& colliderInfo)
{
    SphereShapeSettings shapeSettings(colliderInfo.parameters.sphereRadius);
    shapeSettings.SetEmbedded();
    ShapeSettings::ShapeResult shapeResult = shapeSettings.Create();
    assert(shapeResult.IsValid());
    ShapeRefC shape       = shapeResult.Get();
    colliderInfo.shapeRef = (void*)shape.GetPtr();
    return shape;
}

XPProfilable JPH::ShapeRefC
             createCapsuleShape(XPColliderInfo& colliderInfo)
{
    CapsuleShapeSettings shapeSettings(colliderInfo.parameters.capsuleHeight / 2.0f,
                                       colliderInfo.parameters.capsuleRadius);
    shapeSettings.SetEmbedded();
    ShapeSettings::ShapeResult shapeResult = shapeSettings.Create();
    assert(shapeResult.IsValid());
    ShapeRefC shape       = shapeResult.Get();
    colliderInfo.shapeRef = (void*)shape.GetPtr();
    return shape;
}

XPProfilable JPH::ShapeRefC
             createCylinderShape(XPColliderInfo& colliderInfo)
{
    CylinderShapeSettings shapeSettings(colliderInfo.parameters.cylinderHeight / 2.0f,
                                        colliderInfo.parameters.cylinderRadius);
    shapeSettings.SetEmbedded();
    ShapeSettings::ShapeResult shapeResult = shapeSettings.Create();
    assert(shapeResult.IsValid());
    ShapeRefC shape       = shapeResult.Get();
    colliderInfo.shapeRef = (void*)shape.GetPtr();
    return shape;
}

XPProfilable JPH::ShapeRefC
             createConvexMeshShape(XPColliderInfo&     colliderInfo,
                                   XPJoltShapesCache&  shapesCache,
                                   const XPMeshBuffer* mb,
                                   XPJoltPhysics*      physics,
                                   void (XPJoltPhysics::*uploadMeshAsset)(XPMeshAsset*))
{
    if (shapesCache.find(mb->getMeshAsset()->getId()) == shapesCache.end()) {
        (physics->*uploadMeshAsset)(mb->getMeshAsset());
    } else {
        XP_LOGV(XPLoggerSeverityInfo,
                "Using cached collision shape for <%s>",
                mb->getMeshAsset()->getFile()->getPath().c_str());
    }

    ShapeRefC shape       = shapesCache[mb->getMeshAsset()->getId()][colliderInfo.meshBufferObjectIndex];
    colliderInfo.shapeRef = (void*)shape.GetPtr();
    return shape;
}

XPProfilable JPH::ShapeRefC
             createTriMeshShape(XPColliderInfo&     colliderInfo,
                                XPJoltShapesCache&  shapesCache,
                                const XPMeshBuffer* mb,
                                XPJoltPhysics*      physics,
                                void (XPJoltPhysics::*uploadMeshAsset)(XPMeshAsset*))
{
    if (shapesCache.find(mb->getMeshAsset()->getId()) == shapesCache.end()) {
        (physics->*uploadMeshAsset)(mb->getMeshAsset());
    } else {
        XP_LOGV(XPLoggerSeverityInfo,
                "Using cached collision shape for <%s>",
                mb->getMeshAsset()->getFile()->getPath().c_str());
    }

    ShapeRefC shape       = shapesCache[mb->getMeshAsset()->getId()][colliderInfo.meshBufferObjectIndex];
    colliderInfo.shapeRef = (void*)shape.GetPtr();
    return shape;
}

XPProfilable void
XPJoltPhysics::createPlaneCollider(XPNode* node)
{
    Collider*  collider = node->getCollider();
    Rigidbody* rb       = node->getRigidbody();
    Transform* tr       = node->getTransform();

    XPColliderInfo& colliderInfo = collider->info[0];

    XPPhysicsSceneData&  physicsSceneData = getOrCreateScene();
    BodyInterface&       bodyInterface    = physicsSceneData._physics_system->GetBodyInterface();
    ShapeRefC            shape            = createPlaneShape(colliderInfo);
    BodyCreationSettings settings(shape,
                                  RVec3(tr->location.x, tr->location.y, tr->location.z),
                                  Quat::sEulerAngles(RVec3(tr->euler.x, tr->euler.y, tr->euler.z)),
                                  rb->isStatic ? EMotionType::Static : EMotionType::Dynamic,
                                  rb->isStatic ? Layers::NON_MOVING : Layers::MOVING);
    settings.mUserData = (uint64_t)collider;

    Body* body = bodyInterface.CreateBody(settings);
    assert(body != nullptr);

    physicsSceneData._bodies[node->getId()]                                = body;
    physicsSceneData._body_activation_listener->bodies_to_update[collider] = body->GetID();
    rb->simRef                                                             = (void*)body;

    bodyInterface.AddBody(body->GetID(), EActivation::Activate);
}

XPProfilable void
XPJoltPhysics::createBoxCollider(XPNode* node)
{
    Collider*  collider = node->getCollider();
    Rigidbody* rb       = node->getRigidbody();
    Transform* tr       = node->getTransform();

    XPColliderInfo& colliderInfo = collider->info[0];

    XPPhysicsSceneData&  physicsSceneData = getOrCreateScene();
    BodyInterface&       bodyInterface    = physicsSceneData._physics_system->GetBodyInterface();
    ShapeRefC            shape            = createBoxShape(colliderInfo);
    BodyCreationSettings settings(shape,
                                  RVec3(tr->location.x, tr->location.y, tr->location.z),
                                  Quat::sEulerAngles(RVec3(tr->euler.x, tr->euler.y, tr->euler.z)),
                                  rb->isStatic ? EMotionType::Static : EMotionType::Dynamic,
                                  rb->isStatic ? Layers::NON_MOVING : Layers::MOVING);
    settings.mUserData = (uint64_t)collider;

    Body* body = bodyInterface.CreateBody(settings);
    assert(body != nullptr);

    physicsSceneData._bodies[node->getId()]                                = body;
    physicsSceneData._body_activation_listener->bodies_to_update[collider] = body->GetID();
    rb->simRef                                                             = (void*)body;

    bodyInterface.AddBody(body->GetID(), EActivation::Activate);
}

XPProfilable void
XPJoltPhysics::createSphereCollider(XPNode* node)
{
    Collider*  collider = node->getCollider();
    Rigidbody* rb       = node->getRigidbody();
    Transform* tr       = node->getTransform();

    XPColliderInfo& colliderInfo = collider->info[0];

    XPPhysicsSceneData&  physicsSceneData = getOrCreateScene();
    BodyInterface&       bodyInterface    = physicsSceneData._physics_system->GetBodyInterface();
    ShapeRefC            shape            = createSphereShape(colliderInfo);
    BodyCreationSettings settings(shape,
                                  RVec3(tr->location.x, tr->location.y, tr->location.z),
                                  Quat::sEulerAngles(RVec3(tr->euler.x, tr->euler.y, tr->euler.z)),
                                  rb->isStatic ? EMotionType::Static : EMotionType::Dynamic,
                                  rb->isStatic ? Layers::NON_MOVING : Layers::MOVING);
    settings.mUserData = (uint64_t)collider;

    Body* body = bodyInterface.CreateBody(settings);
    assert(body != nullptr);

    physicsSceneData._bodies[node->getId()]                                = body;
    physicsSceneData._body_activation_listener->bodies_to_update[collider] = body->GetID();
    rb->simRef                                                             = (void*)body;

    bodyInterface.AddBody(body->GetID(), EActivation::Activate);
}

XPProfilable void
XPJoltPhysics::createCapsuleCollider(XPNode* node)
{
    Collider*  collider = node->getCollider();
    Rigidbody* rb       = node->getRigidbody();
    Transform* tr       = node->getTransform();

    XPColliderInfo& colliderInfo = collider->info[0];

    XPPhysicsSceneData&  physicsSceneData = getOrCreateScene();
    BodyInterface&       bodyInterface    = physicsSceneData._physics_system->GetBodyInterface();
    ShapeRefC            shape            = createCapsuleShape(colliderInfo);
    BodyCreationSettings settings(shape,
                                  RVec3(tr->location.x, tr->location.y, tr->location.z),
                                  Quat::sEulerAngles(RVec3(tr->euler.x, tr->euler.y, tr->euler.z)),
                                  rb->isStatic ? EMotionType::Static : EMotionType::Dynamic,
                                  rb->isStatic ? Layers::NON_MOVING : Layers::MOVING);
    settings.mUserData = (uint64_t)collider;

    Body* body = bodyInterface.CreateBody(settings);
    assert(body != nullptr);

    physicsSceneData._bodies[node->getId()]                                = body;
    physicsSceneData._body_activation_listener->bodies_to_update[collider] = body->GetID();
    rb->simRef                                                             = (void*)body;

    bodyInterface.AddBody(body->GetID(), EActivation::Activate);
}

XPProfilable void
XPJoltPhysics::createCylinderCollider(XPNode* node)
{
    Collider*  collider = node->getCollider();
    Rigidbody* rb       = node->getRigidbody();
    Transform* tr       = node->getTransform();

    XPColliderInfo& colliderInfo = collider->info[0];

    XPPhysicsSceneData&  physicsSceneData = getOrCreateScene();
    BodyInterface&       bodyInterface    = physicsSceneData._physics_system->GetBodyInterface();
    ShapeRefC            shape            = createCylinderShape(colliderInfo);
    BodyCreationSettings settings(shape,
                                  RVec3(tr->location.x, tr->location.y, tr->location.z),
                                  Quat::sEulerAngles(RVec3(tr->euler.x, tr->euler.y, tr->euler.z)),
                                  rb->isStatic ? EMotionType::Static : EMotionType::Dynamic,
                                  rb->isStatic ? Layers::NON_MOVING : Layers::MOVING);
    settings.mUserData = (uint64_t)collider;

    Body* body = bodyInterface.CreateBody(settings);
    assert(body != nullptr);

    physicsSceneData._bodies[node->getId()]                                = body;
    physicsSceneData._body_activation_listener->bodies_to_update[collider] = body->GetID();
    rb->simRef                                                             = (void*)body;

    bodyInterface.AddBody(body->GetID(), EActivation::Activate);
}

XPProfilable void
XPJoltPhysics::createConvexMeshCollider(XPNode* node)
{
    Collider*     collider = node->getCollider();
    Rigidbody*    rb       = node->getRigidbody();
    Transform*    tr       = node->getTransform();
    MeshRenderer* mr       = node->getMeshRenderer();

    XPColliderInfo& colliderInfo = collider->info[0];

    XPDataPipelineStore* dataPipelineStore = node->getAbsoluteScene()->getRegistry()->getDataPipelineStore();
    const auto&          meshBuffers       = dataPipelineStore->getMeshBuffers();
    const XPMeshBuffer*  mb                = nullptr;
    for (const auto& pair : meshBuffers) {
        if (mr->info[0].meshBuffer->getId() == pair.second->getId()) {
            mb = mr->info[0].meshBuffer;
            break;
        }
    }

    if (mb == nullptr) { return; }

    XPPhysicsSceneData& physicsSceneData = getOrCreateScene();
    BodyInterface&      bodyInterface    = physicsSceneData._physics_system->GetBodyInterface();

    ShapeRefC shape = createConvexMeshShape(colliderInfo, _shapesCache, mb, this, &XPJoltPhysics::uploadMeshAsset);

    BodyCreationSettings settings(shape,
                                  RVec3(tr->location.x, tr->location.y, tr->location.z),
                                  Quat::sEulerAngles(RVec3(tr->euler.x, tr->euler.y, tr->euler.z)),
                                  rb->isStatic ? EMotionType::Static : EMotionType::Dynamic,
                                  rb->isStatic ? Layers::NON_MOVING : Layers::MOVING);
    settings.mUserData = (uint64_t)collider;

    Body* body = bodyInterface.CreateBody(settings);
    assert(body != nullptr);

    physicsSceneData._bodies[node->getId()]                                = body;
    physicsSceneData._body_activation_listener->bodies_to_update[collider] = body->GetID();
    rb->simRef                                                             = (void*)body;

    bodyInterface.AddBody(body->GetID(), EActivation::Activate);
}

XPProfilable void
XPJoltPhysics::createTriMeshCollider(XPNode* node)
{
    Collider*     collider = node->getCollider();
    Rigidbody*    rb       = node->getRigidbody();
    Transform*    tr       = node->getTransform();
    MeshRenderer* mr       = node->getMeshRenderer();

    XPColliderInfo& colliderInfo = collider->info[0];

    XPDataPipelineStore* dataPipelineStore = node->getAbsoluteScene()->getRegistry()->getDataPipelineStore();
    const auto&          meshBuffers       = dataPipelineStore->getMeshBuffers();
    const XPMeshBuffer*  mb                = nullptr;
    for (const auto& pair : meshBuffers) {
        // Note that it doesn't matter if we're only accessing index 0 of info
        // We just want to point to the main mesh buffer which contains the whole main mesh buffer
        if (mr->info[0].meshBuffer->getId() == pair.second->getId()) {
            mb = mr->info[0].meshBuffer;
            break;
        }
    }

    if (mb == nullptr) { return; }

    XPPhysicsSceneData& physicsSceneData = getOrCreateScene();
    BodyInterface&      bodyInterface    = physicsSceneData._physics_system->GetBodyInterface();

    ShapeRefC shape = createTriMeshShape(colliderInfo, _shapesCache, mb, this, &XPJoltPhysics::uploadMeshAsset);
    BodyCreationSettings settings(_shapesCache[mb->getMeshAsset()->getId()][colliderInfo.meshBufferObjectIndex],
                                  RVec3(tr->location.x, tr->location.y, tr->location.z),
                                  Quat::sEulerAngles(RVec3(tr->euler.x, tr->euler.y, tr->euler.z)),
                                  EMotionType::Static,
                                  Layers::NON_MOVING);
    settings.mUserData = (uint64_t)collider;

    Body* body = bodyInterface.CreateBody(settings);
    assert(body != nullptr);

    physicsSceneData._bodies[node->getId()]                                = body;
    physicsSceneData._body_activation_listener->bodies_to_update[collider] = body->GetID();
    rb->simRef                                                             = (void*)body;

    bodyInterface.AddBody(body->GetID(), EActivation::Activate);
}

XPProfilable void
XPJoltPhysics::createCompoundCollider(XPNode* node)
{
    Collider*     collider = node->getCollider();
    Rigidbody*    rb       = node->getRigidbody();
    Transform*    tr       = node->getTransform();
    MeshRenderer* mr       = node->getMeshRenderer();

    XPPhysicsSceneData& physicsSceneData = getOrCreateScene();
    BodyInterface&      bodyInterface    = physicsSceneData._physics_system->GetBodyInterface();

    XPDataPipelineStore* dataPipelineStore = node->getAbsoluteScene()->getRegistry()->getDataPipelineStore();
    const auto&          meshBuffers       = dataPipelineStore->getMeshBuffers();

    // Create compound shape settings
    JPH::StaticCompoundShapeSettings compoundSettings;

    for (size_t i = 0; i < collider->info.size(); ++i) {
        XPColliderInfo& colliderInfo = collider->info[i];
        switch (colliderInfo.shape) {
            case XPEColliderShapePlane: {
                compoundSettings.AddShape(Vec3::sZero(), Quat::sIdentity(), createPlaneShape(colliderInfo));
            } break;
            case XPEColliderShapeBox: {
                compoundSettings.AddShape(Vec3::sZero(), Quat::sIdentity(), createBoxShape(colliderInfo));
            } break;
            case XPEColliderShapeSphere: {
                compoundSettings.AddShape(Vec3::sZero(), Quat::sIdentity(), createSphereShape(colliderInfo));
            } break;
            case XPEColliderShapeCapsule: {
                compoundSettings.AddShape(Vec3::sZero(), Quat::sIdentity(), createCapsuleShape(colliderInfo));
            } break;
            case XPEColliderShapeCylinder: {
                compoundSettings.AddShape(Vec3::sZero(), Quat::sIdentity(), createCylinderShape(colliderInfo));
            } break;
            case XPEColliderShapeConvexMesh: {
                XPMeshRendererInfo& mrInfo = mr->info[i];
                const XPMeshBuffer* mb     = nullptr;
                for (const auto& pair : meshBuffers) {
                    // Note that it doesn't matter if we're only accessing index 0 of info
                    // We just want to point to the main mesh buffer which contains the whole main mesh buffer
                    if (mrInfo.meshBuffer->getId() == pair.second->getId()) {
                        mb = mrInfo.meshBuffer;
                        break;
                    }
                }
                if (mb == nullptr) {
                    XP_LOGV(XPLoggerSeverityError, "Mesh renderer info %zu seems to be incorrect, skipping ..", i);
                    continue;
                }
                compoundSettings.AddShape(
                  Vec3::sZero(),
                  Quat::sIdentity(),
                  createConvexMeshShape(colliderInfo, _shapesCache, mb, this, &XPJoltPhysics::uploadMeshAsset));
            } break;
            case XPEColliderShapeTriangleMesh: {
                XPMeshRendererInfo& mrInfo = mr->info[i];
                const XPMeshBuffer* mb     = nullptr;
                for (const auto& pair : meshBuffers) {
                    // Note that it doesn't matter if we're only accessing index 0 of info
                    // We just want to point to the main mesh buffer which contains the whole main mesh buffer
                    if (mrInfo.meshBuffer->getId() == pair.second->getId()) {
                        mb = mrInfo.meshBuffer;
                        break;
                    }
                }
                if (mb == nullptr) {
                    XP_LOGV(XPLoggerSeverityError, "Mesh renderer info %zu seems to be incorrect, skipping ..", i);
                    continue;
                }
                compoundSettings.AddShape(
                  Vec3::sZero(),
                  Quat::sIdentity(),
                  createTriMeshShape(colliderInfo, _shapesCache, mb, this, &XPJoltPhysics::uploadMeshAsset));
            } break;

            default:
                assert(false);
                XP_LOG(XPLoggerSeverityFatal, "Unreachable");
                break;
        }
    }

    // Create the compound shape
    RefConst<StaticCompoundShape> compoundShape = StaticCast<StaticCompoundShape>(compoundSettings.Create().Get());

    BodyCreationSettings settings(compoundShape,
                                  RVec3(tr->location.x, tr->location.y, tr->location.z),
                                  Quat::sEulerAngles(RVec3(tr->euler.x, tr->euler.y, tr->euler.z)),
                                  EMotionType::Static,
                                  Layers::NON_MOVING);
    settings.mUserData = (uint64_t)collider;

    Body* body = bodyInterface.CreateBody(settings);
    assert(body != nullptr);

    physicsSceneData._bodies[node->getId()]                                = body;
    physicsSceneData._body_activation_listener->bodies_to_update[collider] = body->GetID();
    rb->simRef                                                             = (void*)body;

    bodyInterface.AddBody(body->GetID(), EActivation::Activate);
}

XPProfilable void
XPJoltPhysics::setRigidbodyPositionAndRotation(XPNode* node)
{
    if (node->hasRigidbodyAttachment() && node->hasTransformAttachment()) {
        Rigidbody*          rb = node->getRigidbody();
        Transform*          tr = node->getTransform();
        JPH::RVec3          position(tr->location.x, tr->location.y, tr->location.z);
        JPH::Quat           rotation = JPH::Quat::sEulerAngles(JPH::RVec3(tr->euler.x, tr->euler.y, tr->euler.z));
        XPPhysicsSceneData& physicsSceneData = getOrCreateScene();
        BodyInterface&      bodyInterface    = physicsSceneData._physics_system->GetBodyInterface();
        bodyInterface.SetPositionAndRotation(
          ((JPH::Body*)rb->simRef)->GetID(), position, rotation, EActivation::Activate);
    }
}

XPProfilable void
XPJoltPhysics::setRigidbodyLinearVelocity(XPNode* node)
{
    if (node->hasRigidbodyAttachment()) {
        Rigidbody*          rb               = node->getRigidbody();
        XPPhysicsSceneData& physicsSceneData = getOrCreateScene();
        BodyInterface&      bodyInterface    = physicsSceneData._physics_system->GetBodyInterface();
        bodyInterface.SetLinearVelocity(((JPH::Body*)rb->simRef)->GetID(),
                                        JPH::Vec3Arg(rb->linearVelocity.x, rb->linearVelocity.y, rb->linearVelocity.z));
    }
}

XPProfilable void
XPJoltPhysics::setRigidbodyAngularVelocity(XPNode* node)
{
    if (node->hasRigidbodyAttachment()) {
        Rigidbody*          rb               = node->getRigidbody();
        XPPhysicsSceneData& physicsSceneData = getOrCreateScene();
        BodyInterface&      bodyInterface    = physicsSceneData._physics_system->GetBodyInterface();
        bodyInterface.SetAngularVelocity(
          ((JPH::Body*)rb->simRef)->GetID(),
          JPH::Vec3Arg(rb->angularVelocity.x, rb->angularVelocity.y, rb->angularVelocity.z));
    }
}

XPProfilable void
XPJoltPhysics::setRigidbodyStatic(XPNode* node)
{
    if (node->hasRigidbodyAttachment()) {
        Rigidbody*          rb               = node->getRigidbody();
        XPPhysicsSceneData& physicsSceneData = getOrCreateScene();
        BodyInterface&      bodyInterface    = physicsSceneData._physics_system->GetBodyInterface();
        bodyInterface.DeactivateBody(((JPH::Body*)rb->simRef)->GetID());
        bodyInterface.SetMotionType(((JPH::Body*)rb->simRef)->GetID(),
                                    rb->isStatic ? JPH::EMotionType::Static : JPH::EMotionType::Dynamic,
                                    rb->isStatic ? JPH::EActivation::DontActivate : JPH::EActivation::Activate);
        bodyInterface.SetObjectLayer(((JPH::Body*)rb->simRef)->GetID(),
                                     rb->isStatic ? Layers::NON_MOVING : Layers::MOVING);
    }
}

XPProfilable void
XPJoltPhysics::updateColliderShape(XPNode* node)
{
    if (node->hasColliderAttachment() && node->hasRigidbodyAttachment() && node->hasTransformAttachment()) {
        Rigidbody*          rb               = node->getRigidbody();
        XPPhysicsSceneData& physicsSceneData = getOrCreateScene();
        BodyInterface&      bodyInterface    = physicsSceneData._physics_system->GetBodyInterface();
        std::tuple<JPH::Vec3, JPH::Vec3, JPH::Vec3> old;

        // Get current body properties
        old = std::make_tuple(((JPH::Body*)rb->simRef)->GetPosition(),
                              ((JPH::Body*)rb->simRef)->GetLinearVelocity(),
                              ((JPH::Body*)rb->simRef)->GetAngularVelocity());

        // Remove the old body
        bodyInterface.DeactivateBody(((JPH::Body*)rb->simRef)->GetID());
        bodyInterface.RemoveBody(((JPH::Body*)rb->simRef)->GetID());
        bodyInterface.DestroyBody(((JPH::Body*)rb->simRef)->GetID());

        physicsSceneData._bodies.erase(node->getId());

        // create new collision shapes and rigidbodies
        createColliders(node);

        // set rigidbody values back
        bodyInterface.SetPosition(((JPH::Body*)rb->simRef)->GetID(), std::get<0>(old), JPH::EActivation::Activate);
        bodyInterface.SetLinearVelocity(((JPH::Body*)rb->simRef)->GetID(), std::get<1>(old));
        bodyInterface.SetAngularVelocity(((JPH::Body*)rb->simRef)->GetID(), std::get<1>(old));
    }
}

XPRegistry*
XPJoltPhysics::getRegistry()
{
    return _registry;
}

void
XPJoltPhysics::beginUploadMeshAssets()
{
}

void
XPJoltPhysics::endUploadMeshAssets()
{
}

XPProfilable void
XPJoltPhysics::uploadMeshAsset(XPMeshAsset* meshAsset)
{
    // avoid uploading an already uploaded asset
    if (_shapesCache.find(meshAsset->getId()) != _shapesCache.end()) { return; }

    // get mesh buffer from mesh asset
    XPMeshBuffer* mb = meshAsset->getMeshBuffer();
    // get mesh buffer objects
    for (size_t i = 0; i < mb->getObjectsCount(); ++i) {
        // for each mesh buffer object, get the indices ranges that are indexing from main mesh buffer
        XPMeshBufferObject& mbo = mb->getObjects()[i];

        JPH::TriangleList triangleList;
        triangleList.resize(mbo.numIndices / 3);

        for (size_t i = 0; i < triangleList.size(); ++i) {
            auto          i0 = mb->getIndices()[(i * 3) + mbo.indexOffset];
            XPVec4<float> p0 = mb->getPositions()[i0 + mbo.vertexOffset];
            auto          v0 = JPH::Float3(p0.x, p0.y, p0.z);

            auto          i1 = mb->getIndices()[(i * 3) + 1 + mbo.indexOffset];
            XPVec4<float> p1 = mb->getPositions()[i1 + mbo.vertexOffset];
            auto          v1 = JPH::Float3(p1.x, p1.y, p1.z);

            auto          i2 = mb->getIndices()[(i * 3) + 2 + mbo.indexOffset];
            XPVec4<float> p2 = mb->getPositions()[i2 + mbo.vertexOffset];
            auto          v2 = JPH::Float3(p2.x, p2.y, p2.z);

            triangleList[i] = JPH::Triangle(v0, v1, v2);
        }

        MeshShapeSettings shapeSettings(triangleList);
        shapeSettings.SetEmbedded(); // A ref counted object on the stack (base class RefTarget) should be marked as
                                     // such to prevent it from being freed when its reference count goes to 0.

        // Create the shape
        ShapeSettings::ShapeResult shapeResult = shapeSettings.Create();

        // assert no errors
        assert(shapeResult.IsValid());

        // create a phycics triangle mesh shape and store it in scene as cache
        // this is a ref counted object, keep it alive by copying
        _shapesCache[meshAsset->getId()][i] = shapeResult.Get();
    }
}

void
XPJoltPhysics::beginReUploadMeshAssets()
{
}

void
XPJoltPhysics::endReUploadMeshAssets()
{
}

void
XPJoltPhysics::reUploadMeshAsset(XPMeshAsset* meshAsset)
{
    // TODO:
    // I need to do very similar to what upload already does
    // the problem here is what if I need to remove from the _shapesCache then add new mesh data
    // Also don't forget to update all the already created colliders in the scene as they will need to update
    // from the newly created shape data !
}

XPProfilable XPPhysicsSceneData&
XPJoltPhysics::getOrCreateScene()
{
    XPScene* scene = _registry->getScene();
    assert(scene != nullptr);
    if (_scenes.find(scene->getId()) != _scenes.end()) {
        // scene is already created previously
        return _scenes[scene->getId()];
    }

    createScene(scene);

    assert(_scenes.find(scene->getId()) != _scenes.end());
    return _scenes[scene->getId()];
}

XPProfilable void
XPJoltPhysics::createScene(XPScene* scene)
{
    assert(_scenes.find(scene->getId()) == _scenes.end());
    auto result = _scenes.insert({ scene->getId(), XPPhysicsSceneData() });
    assert(result.second == true);

    XPPhysicsSceneData& sceneData = result.first->second;

    {
        // This is the max amount of rigidbodies that you can add to the physics system.
        // If you try to add more you'll get an error.
        const uint cMaxBodies = 65536 * 100;

        // This determines how many mutexes to allocate to protect rigidbodies from concurrent access.
        // Set it to 0 for the default settings.
        const uint cNumBodyMutexes = 0;

        // This is the max amount of body pairs that can be queued at any time (the broadphase will detect
        // overlapping body pairs based on their bounding boxes and will insert them into a queue for the
        // narrowphase). If you make this buffer too small the queue will fill up and the broadphase jobs will start
        // to do narrow phase work. This is slightly less efficient.
        const uint cMaxBodyPairs = 65536 * 100;

        // This is the maximum size of the contact constraint buffer. If more contacts (collisions between bodies)
        // are detected than this number then these contacts will be ignored and bodies will start interpenetrating
        // / fall through the world.
        const uint cMaxContactConstraints = 102400;

        // Now we can create the actual physics system.
        sceneData._physics_system = XP_NEW PhysicsSystem;
        sceneData._physics_system->Init(cMaxBodies,
                                        cNumBodyMutexes,
                                        cMaxBodyPairs,
                                        cMaxContactConstraints,
                                        *_broad_phase_layer_interface,
                                        *_object_vs_broadphase_layer_filter,
                                        *_object_vs_object_layer_filter);

        // A body activation listener gets notified when bodies activate and go to sleep
        // Note that this is called from a job so whatever you do here needs to be thread safe.
        // Registering one is entirely optional.
        sceneData._body_activation_listener = XP_NEW XPPhysicsBodyActivationListener;
        sceneData._physics_system->SetBodyActivationListener(sceneData._body_activation_listener);

        // A contact listener gets notified when bodies (are about to) collide, and when they separate again.
        // Note that this is called from a job so whatever you do here needs to be thread safe.
        // Registering one is entirely optional.
        sceneData._contact_listener = XP_NEW XPPhysicsContactListener;
        sceneData._physics_system->SetContactListener(sceneData._contact_listener);

        // // now load all nodes traits
        // auto& physicsNodes =
        //   scene->getNodes(ColliderAttachmentDescriptor | TransformAttachmentDescriptor |
        //   RigidbodyAttachmentDescriptor);
        // for (auto& physicsNode : physicsNodes) { createColliders(physicsNode); }

        // Optional step:
        // Before starting the physics simulation you can optimize the broad phase. This improves collision
        // detection performance (it's pointless here because we only have 2 bodies). You should definitely not call
        // this every frame or when e.g. streaming in a new level section as it is an expensive operation. Instead
        // insert all new objects in batches instead of 1 at a time to keep the broad phase efficient.
        // sceneData._physics_system->OptimizeBroadPhase();
    }
}

XPProfilable void
XPJoltPhysics::destroyScene(uint32_t sceneId)
{
    XPPhysicsSceneData& sceneData     = _scenes[sceneId];
    JPH::BodyInterface& bodyInterface = sceneData._physics_system->GetBodyInterface();
    for (auto& bodyPair : sceneData._bodies) {
        bodyInterface.DeactivateBody(bodyPair.second->GetID());
        bodyInterface.RemoveBody(bodyPair.second->GetID());
        bodyInterface.DestroyBody(bodyPair.second->GetID());
    }
    sceneData._bodies.clear();

    XP_DELETE sceneData._contact_listener;
    XP_DELETE sceneData._body_activation_listener;
    XP_DELETE sceneData._physics_system;

    _scenes.erase(sceneId);
}

void
XPJoltPhysics::reCreateScene(XPScene* scene)
{
    const uint32_t sceneId = scene->getId();
    auto           it      = _scenes.find(sceneId);
    if (it != _scenes.end()) { destroyScene(scene->getId()); }

    createScene(scene);
}
