#pragma once

// Bullet
class btCollisionShape;
class btDynamicsWorld;
class btCollisionWorld;
class btCompoundShape;
class btManifoldPoint;

#include "LinearMath/btAlignedObjectArray.h"
#include "BulletDynamics/Dynamics/btRigidBody.h"

// Mine -Austin
class Model;

#include "Model.h"

#define CUSTOM_FRACTURE_TYPE (btRigidBody::CO_USER_TYPE+1)


// Im assuming that this just holds the connections two collision masks
struct btConnection
{
	// Children's collision masks
	btCollisionShape*	m_childShape0;
	btCollisionShape*	m_childShape1;

	// Indexes of the objects in our btAlignedObjectArray
	int	m_childIndex0;
	int	m_childIndex1;

	// Strength of connection
	btScalar	m_strength;
};

class btFractureBody : public btRigidBody
{
	//connections
 public:
	// Refrence to our world
	btDynamicsWorld*	m_world;

	// These are just vectors that bullet uses, they supposedly avoid some errors that
	//  the stl vectors contain. Also supposed to be more portable
	btAlignedObjectArray<btScalar>	m_masses;
	btAlignedObjectArray<btConnection>	m_connections;


	// Constructor - This is the one that should be used
	btFractureBody(	const btRigidBodyConstructionInfo& constructionInfo, btDynamicsWorld*	world) :btRigidBody(constructionInfo), m_world(world)
	{
		m_masses.push_back(constructionInfo.m_mass);
		m_internalType=CUSTOM_FRACTURE_TYPE+CO_RIGID_BODY;
		c_strength = 1.0f;
	}



	///btRigidBody constructor for backwards compatibility. 
	///To specify friction (etc) during rigid body construction, please use the other constructor (using btRigidBodyConstructionInfo)
	btFractureBody(	btScalar mass, 
					btMotionState* motionState,
					btCollisionShape* collisionShape,
					const btVector3& localInertia, 
					btScalar* masses, 
					int numMasses, 
					btDynamicsWorld* world)

		:btRigidBody(mass,motionState,collisionShape,localInertia),
		m_world(world)
	{
		c_strength = 1.0f;

		for (int i=0;i<numMasses;i++)
			m_masses.push_back(masses[i]);

		m_internalType=CUSTOM_FRACTURE_TYPE+CO_RIGID_BODY;
	}

	void setModel(Model * m) {model = m;};
	Model * getModel() {return model;};

	void	recomputeConnectivity(btCollisionWorld* world);
	

	static btCompoundShape* shiftTransform(btCompoundShape* boxCompound,btScalar* masses,btTransform& shift, btVector3& principalInertia);
	
	static btCompoundShape* shiftTransformDistributeMass(btCompoundShape* boxCompound,btScalar mass,btTransform& shift);
	
	static bool collisionCallback(btManifoldPoint& cp,	const btCollisionObject* colObj0,int partId0,int index0,const btCollisionObject* colObj1,int partId1,int index1);

	void setStrength(btScalar newVal) {c_strength = newVal;};

 private:
	btScalar c_strength;
	Model * model;

};


void fractureCallback(btDynamicsWorld* world, btScalar timeStep);
void glueCallback(btDynamicsWorld* world, btScalar timeStep);
