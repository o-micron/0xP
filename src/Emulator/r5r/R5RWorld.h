#pragma once

#include "R5RThirdParty.h"

#include <list>

struct R5RWorld
{
    btDiscreteDynamicsWorld*             world;
    btDefaultCollisionConfiguration*     collisionConfig;
    btCollisionDispatcher*               dispatcher;
    btDbvtBroadphase*                    overlappingPairCache;
    btSequentialImpulseConstraintSolver* solver;
    std::list<btRigidBody*>              rigidBodies;
};