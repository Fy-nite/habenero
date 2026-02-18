// Minimal physics abstraction layer.
// If Jolt is added later, implement the functions here to forward to Jolt.

#include "../include/Physics/PhysicsSystem.hpp"
#include <iostream>

namespace Hotones { namespace Physics {

bool InitPhysics() {
    // No-op stub for now. Replace with Jolt initialization when available.
    std::cerr << "Physics: InitPhysics() - stub (no backend)" << std::endl;
    return true;
}

void ShutdownPhysics() {
    std::cerr << "Physics: ShutdownPhysics() - stub" << std::endl;
}

int RegisterStaticMeshFromModel(const Model& model, const Vector3& position) {
    // We don't have a physics backend yet; return -1 as 'not registered'.
    (void)model; (void)position;
    return -1;
}

void UnregisterStaticMesh(int handle) {
    (void)handle;
}

bool SweepSphereAgainstStatic(int handle, const Vector3& start, const Vector3& end, float radius, Vector3& hitPos, Vector3& hitNormal, float& t) {
    (void)handle; (void)start; (void)end; (void)radius;
    (void)hitPos; (void)hitNormal; (void)t;
    return false;
}

}} // namespace Hotones::Physics
