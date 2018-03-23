
#include "./h/btFractureBody.h"
#include "BulletCollision/CollisionDispatch/btCollisionWorld.h"
#include "BulletCollision/CollisionShapes/btCompoundShape.h"
#include "BulletDynamics/Dynamics/btDynamicsWorld.h"


// This function will compute which children are connected to eachother
void btFractureBody::recomputeConnectivity(btCollisionWorld* world)
{
	// Clear all connections, were recomuting them afterall
	m_connections.clear();

	// Check if the collision shape is compound, because this is a rigidbody
	//  extention we want to make sure we dont do this to a normal rigidbody
	if (getCollisionShape()->isCompound())
	{

		// Get refrence to the collision shape of this object
		btCompoundShape* compound = (btCompoundShape*)getCollisionShape();


		// Compare all child nodes to eachother to deturmine if they are connected
		for (int i=0;i<compound->getNumChildShapes();i++)
		{
			for (int j=i+1;j<compound->getNumChildShapes();j++)
			{

				struct   MyContactResultCallback : public btCollisionWorld::ContactResultCallback
				{
					bool m_connected;
					btScalar m_margin;
					MyContactResultCallback() :m_connected(false),m_margin(0.05)
					{
					}
					virtual   btScalar   addSingleResult(
						btManifoldPoint& cp,   
						const btCollisionObjectWrapper* colObj0Wrap,
						int partId0,
						int index0,
						const btCollisionObjectWrapper* colObj1Wrap,
						int partId1,
						int index1)
					{
						if (cp.getDistance()<=m_margin)
							m_connected = true;
						return 1.f;
					}
			   };

				MyContactResultCallback result;

				// Object One
				btCollisionObject obA;
				obA.setWorldTransform(compound->getChildTransform(i));
				obA.setCollisionShape(compound->getChildShape(i));

				// Object Two
				btCollisionObject obB;
				obB.setWorldTransform(compound->getChildTransform(j));
				obB.setCollisionShape(compound->getChildShape(j));

				// Fills in contact info into result from the two objects in this cycle
				world->contactPairTest(&obA,&obB,result);

				// If theyre connected then we need to create a connection object
				//  (Defined in the header)
				if (result.m_connected)
				{
					btConnection tmp;

					tmp.m_childIndex0 = i;
					tmp.m_childIndex1 = j;

					tmp.m_childShape0 = compound->getChildShape(i);
					tmp.m_childShape1 = compound->getChildShape(j);

					tmp.m_strength = 1.f;

					// Add to connection list
					m_connections.push_back(tmp);
				}
			}
		}
	}
	

}

// Need to figure out what this is for
btCompoundShape* btFractureBody::shiftTransformDistributeMass(btCompoundShape* boxCompound,btScalar mass,btTransform& shift)
{

	btVector3 principalInertia;

	btScalar* masses = new btScalar[boxCompound->getNumChildShapes()];
	for (int j=0;j<boxCompound->getNumChildShapes();j++)
	{
		//evenly distribute mass
		masses[j]=mass/boxCompound->getNumChildShapes();
	}

	return shiftTransform(boxCompound,masses,shift,principalInertia);

}

// Need to Figure out what this is for
btCompoundShape* btFractureBody::shiftTransform(btCompoundShape* boxCompound,btScalar* masses,btTransform& shift, btVector3& principalInertia)
{
	btTransform principal;

	boxCompound->calculatePrincipalAxisTransform(masses,principal,principalInertia);


	///create a new compound with world transform/center of mass properly aligned with the principal axis

	///non-recursive compound shapes perform better
	
		/*#ifdef USE_RECURSIVE_COMPOUND

		btCompoundShape* newCompound = new btCompoundShape();
		newCompound->addChildShape(principal.inverse(),boxCompound);
		newBoxCompound = newCompound;
		//m_collisionShapes.push_back(newCompound);

		//btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
		//btRigidBody::btRigidBodyConstructionInfo rbInfo(mass,myMotionState,newCompound,principalInertia);

		#else

		#ifdef CHANGE_COMPOUND_INPLACE
			newBoxCompound = boxCompound;
			for (int i=0;i<boxCompound->getNumChildShapes();i++)
			{
				btTransform newChildTransform = principal.inverse()*boxCompound->getChildTransform(i);
				///updateChildTransform is really slow, because it re-calculates the AABB each time. todo: add option to disable this update
				boxCompound->updateChildTransform(i,newChildTransform);
			}
			bool isDynamic = (mass != 0.f);
			btVector3 localInertia(0,0,0);
			if (isDynamic)
				boxCompound->calculateLocalInertia(mass,localInertia);
			
		#else*/
		///creation is faster using a new compound to store the shifted children
	btCompoundShape* newBoxCompound = new btCompoundShape();
	for (int i=0;i<boxCompound->getNumChildShapes();i++)
	{
		btTransform newChildTransform = principal.inverse()*boxCompound->getChildTransform(i);
		///updateChildTransform is really slow, because it re-calculates the AABB each time. todo: add option to disable this update
		newBoxCompound->addChildShape(newChildTransform,boxCompound->getChildShape(i));
	}
	//#endif

	//#endif//USE_RECURSIVE_COMPOUND

	shift = principal;
	return newBoxCompound;
}





