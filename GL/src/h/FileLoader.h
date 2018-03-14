// Obj File loader
#pragma once

#include "glHeader.h"
#include <fstream>
#include <vector>
#include <string>

using namespace std;

class FileLoader
{
	public:
		FileLoader();
		~FileLoader();

		bool openFile(string path, bool tesselate = false);

		uint getFaceCount();
		uint getVertexCount();
		uint getNormalCount();

		GLfloat * getObjectData();
		GLfloat * getNormals();

	private:
		vector <ifstream> fileRefrences;

		vector <glm::vec2> uvs;
		
		vector <float> normals;
		vector <float> verticies;

		GLfloat * vertexData;
		GLfloat * uvsData;

		uint faceCount;

		glm::vec3 calcNormal(glm::vec3 one, glm::vec3 two, glm::vec3 three);
		glm::vec3 normalize(glm::vec3 vec);

};