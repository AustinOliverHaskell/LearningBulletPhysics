#include "./h/Cube.h"
#include "./h/glHeader.h"

using namespace glm;

Cube::Cube()
{
	glGenBuffers(1, &verticies);
	glBindBuffer(GL_ARRAY_BUFFER, verticies);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

	glGenBuffers(1, &colors);
	glBindBuffer(GL_ARRAY_BUFFER, colors);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_color_buffer_data), g_color_buffer_data, GL_STATIC_DRAW);
}

Cube::~Cube()
{
	glDeleteBuffers(1, &verticies);
	glDeleteBuffers(1, &colors);
}

int Cube::getIndex()
{
	return index;
}

void Cube::draw()
{
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, verticies);
	glVertexAttribPointer(0, 3, GL_FLOAT,GL_FALSE, 0, (void*)0);

	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, colors);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glDrawArrays(GL_TRIANGLES, 0, 3*12);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
}

mat4 Cube::getMatrix()
{
	return mat4(1.0f);
}