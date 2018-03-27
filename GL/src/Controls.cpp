#include "./h/Controls.h"
#include "./h/glHeader.h"
#include "./h/defs.h"

#include "glm/ext.hpp"

#include <iostream>
#include <btBulletDynamicsCommon.h>

using namespace glm;

Controls::Controls(GLFWwindow* w)
{
	window = w;

	// Set them to identity for right now
	projectionMatrix = mat4(1.0f);
	viewMatrix       = mat4(1.0f);

	FOV = 45;

	hAngle = 8.35f;
	vAngle = -5.533e-8f;

	speed = 6.0f;
	mouseSpeed = 0.01f;

	position = glm::vec3( -22.6f, 3.99f, 13.89f );

	wireframeEnabled = false;
	cameraLocked = true;

}

Controls::~Controls()
{

}

mat4 Controls::getProjectionMatrix()
{
	return projectionMatrix;
}

mat4 Controls::getViewMatrix()
{
	return viewMatrix;
}

btRigidBody * Controls::grabObject(btDynamicsWorld * world)
{
	btRigidBody * retVal = nullptr;
	vec3 out_origin, out_direction;

	double mouseX, mouseY;

	glfwGetCursorPos(window, &mouseX, &mouseY);

	glm::vec4 lRayStart_NDC(
	((float)mouseX/(float)WINDOW_WIDTH  - 0.5f) * 2.0f, // [0,1024] -> [-1,1]
	((float)mouseY/(float)WINDOW_HEIGHT - 0.5f) * 2.0f, // [0, 768] -> [-1,1]
	-1.0, // The near plane maps to Z=-1 in Normalized Device Coordinates
	1.0f
	);

	glm::vec4 lRayEnd_NDC(
	((float)mouseX/(float)WINDOW_WIDTH  - 0.5f) * 2.0f,
	((float)mouseY/(float)WINDOW_HEIGHT - 0.5f) * 2.0f,
	0.0,
	1.0f
	);

	// The Projection matrix goes from Camera Space to NDC.
	// So inverse(ProjectionMatrix) goes from NDC to Camera Space.
	glm::mat4 InverseProjectionMatrix = glm::inverse(projectionMatrix);

	// The View Matrix goes from World Space to Camera Space.
	// So inverse(ViewMatrix) goes from Camera Space to World Space.
	glm::mat4 InverseViewMatrix = glm::inverse(viewMatrix);

	glm::vec4 lRayStart_camera = InverseProjectionMatrix * lRayStart_NDC;    
	lRayStart_camera/=lRayStart_camera.w;

	glm::vec4 lRayStart_world  = InverseViewMatrix       * lRayStart_camera; 
	lRayStart_world /=lRayStart_world .w;

	glm::vec4 lRayEnd_camera   = InverseProjectionMatrix * lRayEnd_NDC;   
	lRayEnd_camera  /=lRayEnd_camera  .w;

	glm::vec4 lRayEnd_world    = InverseViewMatrix       * lRayEnd_camera;  
	lRayEnd_world   /=lRayEnd_world   .w;

	glm::vec3 lRayDir_world(lRayEnd_world - lRayStart_world);
	lRayDir_world = glm::normalize(lRayDir_world);

	out_origin = glm::vec3(lRayStart_world);
	out_direction = glm::normalize(lRayDir_world);

	glm::vec3 out_end = out_origin + out_direction*1000.0f;

	btCollisionWorld::ClosestRayResultCallback RayCallback(
		btVector3(out_origin.x, out_origin.y, out_origin.z), 
		btVector3(out_end.x, out_end.y, out_end.z)
	);
	world->rayTest(
		btVector3(out_origin.x, out_origin.y, out_origin.z), 
		btVector3(out_end.x, out_end.y, out_end.z), 
		RayCallback
	);

	if(RayCallback.hasHit())
	{
		btRigidBody* body = (btRigidBody*)btRigidBody::upcast(RayCallback.m_collisionObject);
		if (body)
		{
			retVal = body;
			//std::cout << "HIT" << std::endl;
		}
	}
	/*else
	{
		std::cout << "No hit" << std::endl; 
	}*/

	return retVal;

}

void Controls::computeMatrices()
{
	// Get the time when this function is called,
	//  this should only be called once due to the 
	//  static typing
	static double prevFrameTime = glfwGetTime();
	double currentTime = glfwGetTime();

	// Time difference
	float deltaTime = float(currentTime - prevFrameTime);

	// Get mouse position
	double mouseX;
	double mouseY;

	//std::cout << vAngle << std::endl;

	// Compute the direction of the camera
	//  Spherical coordinates to Cartesian coordinates conversion
	vec3 direction(
		cos(vAngle) * sin(hAngle),
		sin(vAngle),
		cos(vAngle) * cos(hAngle)
		);

	vec3 right(
		sin(hAngle - PI/2.0f),
		0,
		cos(hAngle - PI/2.0f)
		);

	glm::vec3 up = glm::cross( right, direction );

	if (!cameraLocked)
	{
		glfwGetCursorPos(window, &mouseX, &mouseY);


		// Set the mouse position back to center so that
		//  the cursor doesnt go too far off screen
		// TODO: Make this dynamic so the screen can be resized
		glfwSetCursorPos(window, WINDOW_WIDTH/2.0f, WINDOW_HEIGHT/2.0f);


		// Compute Orientation

		// Explanation: This works by taking the amount of distance 
		//  the user moved the mouse times the speed of the change
		//  that we defined above (mouseSpeed). The variable mouseSpeed
		//  we defined can be changed to suit the user
		hAngle += mouseSpeed * (float)(WINDOW_WIDTH/2.0f - mouseX);
		vAngle += mouseSpeed * (float)(WINDOW_HEIGHT/2.0f - mouseY);

		if (vAngle > PI/2)
		{
			vAngle = PI/2;
		}
		else if (vAngle < -PI/2)
		{
			vAngle = -PI/2;
		}

		// Move forward
		if (glfwGetKey( window, GLFW_KEY_W ) == GLFW_PRESS){
		    position += direction * deltaTime * speed;
		}
		// Move backward
		if (glfwGetKey( window, GLFW_KEY_S ) == GLFW_PRESS){
		    position -= direction * deltaTime * speed;
		}
		// Strafe right
		if (glfwGetKey( window, GLFW_KEY_D ) == GLFW_PRESS){
		    position += right * deltaTime * speed;
		}
		// Strafe left
		if (glfwGetKey( window, GLFW_KEY_A ) == GLFW_PRESS){
		    position -= right * deltaTime * speed;
		}
	
		// Zoom, I added this - Austin :P
		if (glfwGetKey(window, GLFW_KEY_RIGHT_BRACKET) == GLFW_PRESS)
		{
			if (FOV <= 60)
			{
				FOV++;
			}
		}

		if (glfwGetKey(window, GLFW_KEY_LEFT_BRACKET) == GLFW_PRESS)
		{
			if (FOV >= 30)
			{
				FOV--;
			}
		}

		if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
		{
			position -= vec3(0.0f, 0.1f, 0.0f);
		}

		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		{
			position -= vec3(0.0f, -0.1f, 0.0f);
		}
	}	




		// Strafe left
	if (glfwGetKey( window, GLFW_KEY_P ) == GLFW_PRESS){
	    // Dump campera info
	    std::cout << "Camera Pos:(" << position.x << ", " << position.y << ", " << position.z<< ")" << std::endl;
	    std::cout << "Camera Direction:(" << direction.x << ", " << direction.y << ", " << direction.z<< ")" << std::endl;
	    std::cout << "FOV:" << FOV << std::endl;
	    std::cout << "hAngle:" << hAngle << std::endl;
	    std::cout << "vAngle:" << vAngle << std::endl; 
	}

	static int oldState = GLFW_RELEASE;
	int newState = glfwGetKey(window, GLFW_KEY_F);

	if (newState == GLFW_RELEASE && oldState == GLFW_PRESS)
	{
		wireframeEnabled = !wireframeEnabled;

		if (wireframeEnabled)
		{
			glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
		}
		else
		{
			glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
		}
	}
	oldState = newState;

	static int lockOldState = GLFW_RELEASE;
	int lockNewState = glfwGetKey(window, GLFW_KEY_L);

	if (lockNewState == GLFW_RELEASE && lockOldState == GLFW_PRESS)
	{
		cameraLocked = !cameraLocked;
		std::cout << "Camera Locked?: " << (cameraLocked ? "Yes" : "No") << std::endl;
		glfwSetCursorPos(window, WINDOW_WIDTH/2.0f, WINDOW_HEIGHT/2.0f);

	}
	lockOldState = lockNewState;

	projectionMatrix = glm::perspective(glm::radians(FOV), ASPECT_RATIO, 0.1f, 100.0f);

	viewMatrix = glm::lookAt(
		position,           // Camera is here
		position+direction, // and looks here : at the same position, plus "direction"
		up                  // Head is up (set to 0,-1,0 to look upside-down)
	);

	prevFrameTime = currentTime;
}