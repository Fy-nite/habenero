#pragma once
#include <raylib.h>

namespace Hotones { namespace Physics {

// Initialize the physics subsystem. Returns true on success.
bool InitPhysics();
void ShutdownPhysics();

// Register a static (non-moving) collision mesh built from a raylib `Model`.
// Returns a positive handle id on success, or -1 if registration failed / not available.
int RegisterStaticMeshFromModel(const Model& model, const Vector3& position);
void UnregisterStaticMesh(int handle);

// Optional: perform a sphere sweep against a registered static mesh. Returns true if hit.
bool SweepSphereAgainstStatic(int handle, const Vector3& start, const Vector3& end, float radius, Vector3& hitPos, Vector3& hitNormal, float& t);

}} // namespace Hotones::Physics
