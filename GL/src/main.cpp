#include "./h/glHeader.h"
#include "./h/Controls.h"
#include "./h/World.h"

using namespace std;

int main(int argc, char* argv[])
{
	World * w = new World();

	cout << endl <<"Running Learning" << endl;

	Model * sphere = new Model("./obj/sphere.obj");

	w->addModel(sphere);

	while(glfwGetKey(w->getWindow(), GLFW_KEY_ESCAPE ) != GLFW_PRESS && glfwWindowShouldClose(w->getWindow()) == 0 )
	{
		w->render();
	}

	delete w;

	return 0;
}
