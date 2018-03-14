#pragma once

#include "glHeader.h"
#include <iostream> 

class GraphicDebugger
{
public:
	// Setup
	GraphicDebugger()
	{
		fps = 0;
		lastTime = glfwGetTime();
	}

	// Calculates framecount from last sec
	void showFPS()
	{
		double currentTime = glfwGetTime();
		fps++;

		// Its been one second
		if (currentTime - lastTime >= 1.0)
		{
			std::cout << "FPS: " << fps << " - " << "each frame is " << 1000.0/(double)fps << "ms "<<std::endl;
			fps = 0;
			lastTime = currentTime;
		}
	}


private:
	int fps;
	double lastTime;
};
