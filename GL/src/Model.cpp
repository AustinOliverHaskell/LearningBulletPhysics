#include "./h/FileLoader.h"
#include "./h/Controls.h"
#include "./h/PointCloud.h"
#include "./h/Model.h"

#include <cstdlib>
#include <vector>
#include <glm/gtc/type_ptr.hpp>

using namespace glm;

// TODO: Add comments

Model::Model(Model * m)
{
	vertexCount = m->getVertexCount();
	faceCount = m->getFaceCount();

	vector <GLfloat> * sData = new vector<GLfloat>();
	vector <GLfloat> * nData = new vector<GLfloat>();
	vector <GLfloat> * cData = new vector<GLfloat>();

	GLfloat * otherShapeData = m->getShapeData();
	GLfloat * otherNormalData = m->getNormalData();
	GLfloat * otherColorData = m->getColorData();

	for (uint i = 0; i < vertexCount; i++)
	{
		sData->push_back(otherShapeData[i]);
		nData->push_back(otherNormalData[i]);
		cData->push_back(otherColorData[i]);
	}

	shapeData = sData->data();
	colorData = cData->data();
	normalData = nData->data();

	transform = m->getTransform();

	position = m->getPosition();
	m_scale  = m->getScale();
	rotation = m->getRotation();

	shader = m->getShader();		

	
	collisionShape = m->getCollisionShape();
	motionState = new btDefaultMotionState();
	rigidBody = nullptr;

	mass = m->getMass();
	friction = m->getFriction();
	rollingFriction = m->getRollingFriction();
	resititution = m->getRestitution();

	changeColor = false;
	isCopy = true;

	initBuffers();
	configureRigidBody();
}

Model::Model(PointCloud p, GLuint shade)
{
	shapeData  = p.getVertexData();
	colorData  = p.getColorData();
	normalData = p.getNormalData();
	vertexCount = p.getVertexCount();

	faceCount = vertexCount / 9;

	transform = mat4(1);
	mass = 0;
	friction = 0;
	rollingFriction = 0;
	resititution = 0;

	changeColor = false;
	isCopy = false;

	position = vec3(1, 1, 1);
	m_scale  = vec3(1, 1, 1);
	rotation = 0.0f;

	shader = shade;

	motionState = new btDefaultMotionState();
	collisionShape = nullptr;
	rigidBody = nullptr;

	initBuffers();

	// Calls configureRigidBody internally
	calcTriangleCollisionMesh();


}

Model::Model(std::string path, GLuint shade,  bool tessalate)
{
	FileLoader * file = new FileLoader();

	if (!file->openFile(path, tessalate))
	{
		std::cout << "Filed to load file: " << path << std::endl;
	}

	faceCount   = file->getFaceCount();
	shapeData   = file->getObjectData();
	normalData  = file->getNormals();
	vertexCount = file->getVertexCount();

	shader = shade;

	GLfloat * colors = new GLfloat[faceCount * sizeof(vec3) * 3];

	// Define color data for the object
	for (uint i = 0; i < faceCount * sizeof(vec3); i+=3)
	{
		colors[i  ] = 0.5;
		colors[i+1] = 0.5;
		colors[i+2] = 0.5;
	}

	colorData = colors;

	transform = mat4(1);
	position  = vec3(1, 1, 1);
	rotation  = 0.0f;
	m_scale   = vec3(1, 1, 1);

	motionState    = nullptr;
	collisionShape = nullptr;
	rigidBody      = nullptr;

	// Deaults to origin
	motionState = new btDefaultMotionState();
	// Assume Cube shape for collision
	collisionShape = new btBoxShape(btVector3(1, 1, 1));
	mass = 0;
	friction = 0;
	rollingFriction = 0;
	resititution = 0;
	configureRigidBody();

	changeColor = false;
	isCopy = false;
}

Model::~Model()
{
	if (setupComplete)
	{
		glDeleteBuffers(1, &verticies);
		glDeleteBuffers(1, &colors);
	}

	if (collisionShape != nullptr && !isCopy)
	{
		delete collisionShape;
		collisionShape = nullptr;
	}

	if (motionState != nullptr)
	{
		delete motionState;
		motionState = nullptr;
	}

	if (rigidBody != nullptr)
	{
		delete rigidBody;
		rigidBody = nullptr;
	}


	if (shapeData != nullptr)
	{
		delete shapeData;
		shapeData = nullptr;
	}

	if (colorData != nullptr)
	{
		delete colorData;
		colorData = nullptr;
	}

	if (normalData != nullptr)
	{
		delete normalData;
		normalData = nullptr;
	}
}

GLfloat * Model::getShapeData()
{
	return shapeData;
}

GLfloat * Model::getColorData()
{
	return colorData;
}

GLfloat * Model::getNormalData()
{
	return normalData;
}



void Model::setColor(float r, float g, float b)
{
	GLfloat * color = new GLfloat[faceCount * sizeof(vec3) * 3];

	// Define color data for the object
	for (uint i = 0; i < faceCount * sizeof(vec3); i+=3)
	{
		color[i  ] = r;
		color[i+1] = g;
		color[i+2] = b;
	}

	// Remove old memory
	delete colorData;

	colorData = color;

	glGenBuffers(1, &colors);
	glBindBuffer(GL_ARRAY_BUFFER, colors);
	glBufferData(GL_ARRAY_BUFFER, faceCount * sizeof(vec3) * 3, colorData, GL_DYNAMIC_DRAW);
}

void Model::randomizeColor()
{
	GLfloat * color = new GLfloat[faceCount * sizeof(vec3) * 3];

	// Define color data for the object
	for (uint i = 0; i < faceCount * sizeof(vec3); i+=3)
	{
		float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
		float g = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
		float b = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);

		color[i  ] = r;
		color[i+1] = g;
		color[i+2] = b;
	}

	// Remove old memory
	delete colorData;

	colorData = color;

	glGenBuffers(1, &colors);
	glBindBuffer(GL_ARRAY_BUFFER, colors);
	glBufferData(GL_ARRAY_BUFFER, faceCount * sizeof(vec3) * 3, colorData, GL_DYNAMIC_DRAW);
}

uint Model::getFaceCount()
{
	return faceCount;
}

uint Model::getVertexCount()
{
	return vertexCount;
}

GLuint Model::getShader()
{
	return shader;
}


btRigidBody * Model::getRigidBody()
{
	return rigidBody;
}

btCollisionShape * Model::getCollisionShape()
{
	return collisionShape;
}

float Model::getMass()
{
	return mass;
}

float Model::getFriction()
{
	return friction;
}

float Model::getRollingFriction()
{
	return rollingFriction;
}

float Model::getRestitution()
{
	return resititution;
}

vec3  Model::getPosition()
{
	return position;
}
vec3  Model::getScale()
{
	return m_scale;
}
float Model::getRotation()
{
	return rotation;
}

mat4 Model::getTransform()
{
	return transform;
}

void Model::changeColorOnGround(bool c)
{
	changeColor = c;
}

void Model::setVertexData(GLfloat * data)
{
	shapeData = data;
}

void Model::setColorData(GLfloat * data)
{
	colorData = data;
}

void Model::setShader(GLuint shade)
{
	shader = shade;
}

void Model::setTransform(mat4 trans)
{
	transform = trans;

}

void Model::setPosition(vec3 p)
{
	position = p;

	if (motionState != nullptr)
	{
		delete motionState;
	}
	
	motionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(position.x, position.y, position.z)));

	configureRigidBody();
}
void Model::setRotation(float r)
{
	rotation = r;
}
void Model::setScale(vec3 s)
{
	m_scale = s;
}

void Model::setMass(float m)
{
	mass = m;
	configureRigidBody();
}

void Model::setFriction(float f)
{
	friction = f;
}

void Model::setRollingFriction(float f)
{
	rollingFriction = f;
}

void Model::setRestitution(float r)
{
	resititution = r;
}

void Model::setCollisionShape(btCollisionShape * shape)
{
	delete collisionShape;
	collisionShape = shape;

	configureRigidBody();
}

void Model::initBuffers()
{
	// Set up the uniforms
	MatrixID      = glGetUniformLocation(shader, "MVP");
    ViewMatrixID  = glGetUniformLocation(shader,   "V");
    ModelMatrixID = glGetUniformLocation(shader,   "M");

	glGenBuffers(1, &verticies);
	glBindBuffer(GL_ARRAY_BUFFER, verticies);
	glBufferData(GL_ARRAY_BUFFER, faceCount * sizeof(vec3) * 3, shapeData, GL_STATIC_DRAW);

	GLenum err = glGetError();

	if (err != GL_NO_ERROR)
	{
		//cerr << "Error in Model::initBuffers: " << err << endl;
	}

	for (uint i = 0; i < faceCount * sizeof(vec3) * 3; i++)
	{
		//cout << shapeData[i] << endl;
	}

	glGenBuffers(1, &colors);
	glBindBuffer(GL_ARRAY_BUFFER, colors);
	glBufferData(GL_ARRAY_BUFFER, faceCount * sizeof(vec3) * 3, colorData, GL_DYNAMIC_DRAW);

	glGenBuffers(1, &normals);
	glBindBuffer(GL_ARRAY_BUFFER, normals);
	glBufferData(GL_ARRAY_BUFFER, faceCount * sizeof(vec3) * 3, normalData, GL_STATIC_DRAW);

	// Done with setup, this boolean lets the destructor know what it should free up
	setupComplete = true;
}

void Model::draw(Controls * controls)
{
	if (setupComplete)
	{
		mat4 viewMatrix = controls->getViewMatrix();

		btTransform trans;
       	motionState->getWorldTransform(trans);

		mat4 m;

		trans.getOpenGLMatrix(glm::value_ptr(m));

		transform = scale(m, m_scale);

		mat4 modelMatrix = transform;
		
		mat4 projectionMatrix = controls->getProjectionMatrix();

		mat4 MVP = projectionMatrix * viewMatrix * transform;

		// TODO: Make this dynamic, actually check collisions. This is a dirty fix
		if (trans.getOrigin().getY() <= 1 && changeColor)
		{
			setColor(0.0f, 0.0f, 1.0f);
		}

		glUseProgram(shader);

		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
		glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &modelMatrix[0][0]);
		glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &viewMatrix[0][0]);

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, verticies); 
		glVertexAttribPointer(0, 3, GL_FLOAT,GL_FALSE,  0, (void*)0);

		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, colors);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glEnableVertexAttribArray(2);
 		glBindBuffer(GL_ARRAY_BUFFER, normals);
 		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glDrawArrays(GL_TRIANGLES, 0, sizeof(vec3)*faceCount*3);

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);
	}
}

void Model::transformDraw(Controls * controls, btTransform trans)
{
	if (setupComplete)
	{
		mat4 viewMatrix = controls->getViewMatrix();

		mat4 m;

		trans.getOpenGLMatrix(glm::value_ptr(m));

		transform = scale(m, m_scale);

		mat4 modelMatrix = transform;
		
		mat4 projectionMatrix = controls->getProjectionMatrix();

		mat4 MVP = projectionMatrix * viewMatrix * transform;

		glUseProgram(shader);

		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
		glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &modelMatrix[0][0]);
		glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &viewMatrix[0][0]);

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, verticies); 
		glVertexAttribPointer(0, 3, GL_FLOAT,GL_FALSE,  0, (void*)0);

		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, colors);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glEnableVertexAttribArray(2);
 		glBindBuffer(GL_ARRAY_BUFFER, normals);
 		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glDrawArrays(GL_TRIANGLES, 0, sizeof(vec3)*faceCount*3);

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);
	}
}

void Model::configureRigidBody()
{
	btVector3 fallInertia(0, 0, 0);
	collisionShape->calculateLocalInertia(mass, fallInertia);
    btRigidBody::btRigidBodyConstructionInfo fallRigidBodyCI(mass, motionState, collisionShape, fallInertia);
    fallRigidBodyCI.m_friction = friction;
    fallRigidBodyCI.m_restitution = resititution;
    fallRigidBodyCI.m_rollingFriction = rollingFriction;
   	
    if (rigidBody != nullptr)
	{
		delete rigidBody;
		rigidBody = nullptr;
	}

    rigidBody = new btRigidBody(fallRigidBodyCI);
}

void Model::calcTriangleCollisionMesh()
{
	btTriangleMesh * mesh = new btTriangleMesh();

	//cout << " ----- ----- Mesh ----- ----- " << endl;

	for (uint i = 0; i < vertexCount; i+=9)
	{
		btVector3 a(shapeData[i], shapeData[i+1], shapeData[i+2]);
		btVector3 b(shapeData[i+3], shapeData[i+4], shapeData[i+5]);
		btVector3 c(shapeData[i+6], shapeData[i+7], shapeData[i+8]);

		/*cout << "Triangle" << endl;
		cout << " ----- " << endl;
		cout << "[" << shapeData[i] << ", " << shapeData[i+1] << "," << shapeData[i+2] << "]" << endl;
		cout << "[" << shapeData[i+3] << ", " << shapeData[i+4] << "," << shapeData[i+5] << "]" << endl;
		cout << "[" << shapeData[i+6] << ", " << shapeData[i+7] << "," << shapeData[i+8] << "]" << endl;*/


		mesh->addTriangle(a, b, c);
	}

	//cout << " ----- ----- ----- ----- ----- " << endl;

	btConvexTriangleMeshShape * meshCollision = new btConvexTriangleMeshShape(mesh);

	if (collisionShape != nullptr)
	{
		delete collisionShape;
		collisionShape = nullptr;
	}

	collisionShape = meshCollision;

	collisionShape->setLocalScaling(btVector3(m_scale.x, m_scale.y, m_scale.z));

	configureRigidBody();

}