#pragma once
#include <string>
#include <stdio.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>

#include "glHeader.h"
#include "defs.h"


#define TIME_WINDOW 10

class Graphics
{
	public:
		Graphics(std::string name);
		~Graphics();

		void openWindow();

		void setupGL();

		void drawTriangle();
	private:
		GLFWwindow* window;
		std::string windowName;
};