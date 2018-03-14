#include <iostream>
#include <cstdlib>

#include "./GL/src/h/World.h"
#include "./GL/src/h/Model.h"
#include "./GL/src/h/defs.h"

int main (int argc, char * argv[])
{
    srand (static_cast <unsigned> (time(0)));

    std::cout << "Thesis" << std::endl;

    // Create the world
    World * world = new World();

    // Load in the shaders that were going to use
    GLuint shader = loadShaders("./GL/src/shaders/lightShader.vertexshader", "./GL/src/shaders/lightShader.fragmentshader");
    GLuint solidShader = loadShaders("./GL/src/shaders/SimpleVertexShader.vertexshader", "./GL/src/shaders/SimpleFragmentShader.fragmentshader");

    // Create the memory for our singular light
    GLuint LightID = glGetUniformLocation(shader, "LightPosition_worldspace");

    vec3 lightPosition = vec3(10.0f, 10.0f, 10.0f);

    // Let there be light
    world->setLight(LightID);
    world->setLightPos(lightPosition);

    // Models in our scene, the plane needs to have its normals calculated
    Model * object = new Model("./GL/src/obj/cube.obj", solidShader, true);
    Model * plane  = new Model("./GL/src/obj/plane.obj",  solidShader, false, true);
    Model * light  = new Model("./GL/src/obj/cube.obj", solidShader, false);

    // Initilize our buffers
    object->initBuffers();
    plane->initBuffers();
    light->initBuffers();

    // Set the plane to a different color
    object->randomizeColor();
    plane->setColor(1.0f, 0.5f, 0.25f);
    light->setColor(1.0f, 1.0f, 1.0f);

    object->setTransform(translate(mat4(), vec3(0.0f, 10.0f, 0.0f)));
    plane->setTransform(scale(mat4(), vec3(30.0f, 0.0f, 30.0f)));
    light->setTransform(translate(mat4(), lightPosition));

    // Add them to the scene
    world->addModel(object);
    world->addModel(plane);
    world->addModel(light);

    // Set Backgrond to black
    world->setBackgroundColor(0.0f, 0.0f, 0.0f);

    // Render
    while(glfwGetKey(world->getWindow(), GLFW_KEY_ESCAPE ) != GLFW_PRESS && glfwWindowShouldClose(world->getWindow()) == 0 )
    {
        world->render();
    }

    // The world object will handle the destruction of all models in its space
    delete world;

    return 0;
}