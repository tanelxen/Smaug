#pragma once
#include <glm/vec3.hpp>
#include "mesh.h"

struct ray_t
{
	glm::vec3 origin;
	glm::vec3 delta;
};

// If hit is false, no data after it is guaranteed to be valid!
struct test_t
{
	bool hit = false;
};

struct testRayPlane_t : public test_t
{
	glm::vec3 hitPos;
	
	// Normal of the plane
	glm::vec3 normal;

	// Dot of the line and tri
	float approach;

	// Parametric point of intersection
	float t = FLT_MAX;

};

// Test if a line hits any geo in the world
testRayPlane_t testRay(ray_t ray);

bool testPointInAABB(glm::vec3 point, aabb_t aabb);
// sizes up the aabb by aabbBloat units before testing
bool testPointInAABB(glm::vec3 point, aabb_t aabb, float aabbBloat);