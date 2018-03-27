#pragma once
// Controls object
#include "glHeader.h"

#include <btBulletDynamicsCommon.h>

using namespace glm;

class Controls
{
public:
	Controls(GLFWwindow* w);
	~Controls();

	mat4 getProjectionMatrix();
	mat4 getViewMatrix();

	void computeMatrices();

	btRigidBody * grabObject(btDynamicsWorld * world);

private:
	mat4 viewMatrix;
	mat4 projectionMatrix;
	GLFWwindow* window;

	// Field of view
	float FOV;

	// Horizontal and Verticle angle respectivly
	float hAngle;
	float vAngle;

	float speed;
	float mouseSpeed;

	vec3 position;

	bool wireframeEnabled;
	bool cameraLocked;
};