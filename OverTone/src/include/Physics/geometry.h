#include "raylib.h"
#include "raymath.h"

class Shape
{
public:
  enum ShapeType
  {
    SPHERE,
  };

  virtual ShapeType GetType() const = 0;
  virtual Vector3 GetCenterOfMass() const { return centerOfMass; }
  virtual Matrix GetInertiaTensor() const = 0;

protected:
  Vector3 centerOfMass;
};

class Sphere : public Shape
{
public:
  Sphere(float radius) : radius{radius}
  {
    centerOfMass = Vector3Zero();
  }

  ShapeType GetType() const override { return SPHERE; }
  Matrix GetInertiaTensor() const override
  {
    // Moment of inertia for a solid sphere: I = (2/5) * m * r^2
    // Return a per-unit-mass inertia tensor (i.e. scale by mass later where needed):
    // I_per_mass = (2/5) * r^2
    Matrix m = {0};
    float I = 0.4f * radius * radius;
    m.m0 = I;   // xx
    m.m5 = I;   // yy
    m.m10 = I;  // zz
    m.m15 = 1.0f;
    return m;
  }
  float radius;
};
