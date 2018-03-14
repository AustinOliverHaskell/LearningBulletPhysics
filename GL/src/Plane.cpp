#include "./h/Plane.h"
#include "./h/glHeader.h"

using namespace glm;

Plane::Plane()
{
	glGenBuffers(1, &verticies);
	glBindBuffer(GL_ARRAY_BUFFER, verticies);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data_plane), g_vertex_buffer_data_plane, GL_STATIC_DRAW);

	glGenBuffers(1, &colors);
	glBindBuffer(GL_ARRAY_BUFFER, colors);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_color_buffer_data_plane), g_color_buffer_data_plane, GL_STATIC_DRAW);
}

Plane::~Plane()
{
	glDeleteBuffers(1, &verticies);
	glDeleteBuffers(1, &colors);
}

int Plane::getIndex()
{
	return index;
}

void Plane::draw()
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

mat4 Plane::getMatrix()
{
	return mat4(1.0f);
}