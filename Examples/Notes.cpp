btVector3 min, max;
myAABB aabb;
btTransform child_trans;
btVector3 child_min, child_max;
int i, j;
for(i = 0; i < m_collisionShapes.size(); i++)
{
	btCollisionShape* shape = m_collisionShapes[i];
	if(shape->getShapeType() == COMPOUND_SHAPE_PROXYTYPE)
	{
		btCompoundShape* shape = (btCompoundShape*)m_collisionShapes[i];
		for(j = shape->getNumChildShapes()-1; j >= 0; j--)
		{
			const btCollisionShape* col_shape = shape->getChildShape(j);
			child_trans = shape->getChildTransform(j);
			col_shape->getAabb(child_trans, child_min, child_max);
			aabb.Make(enVec3(child_min.getX(), child_min.getY(), child_min.getZ()), enVec3(child_max.getX(), child_max.getY(), child_max.getZ()));
			my_render->DrawAABB(aabb);
		}
	}
}




//general variables
float mass = 1.f;
btVector3 localInertia( 0.f, 0.f, 0.f );
btTransform	transform;

//ground
btCollisionShape* ground_shape = new btBoxShape( btVector3( 1000.f, 0.1f, 1000.f ) );
m_collisionShapes.push_back( ground_shape );

transform.setIdentity();
btDefaultMotionState* ground_motion_state = new btDefaultMotionState( transform );
btRigidBody::btRigidBodyConstructionInfo ground_cInfo( 0.f, ground_motion_state, ground_shape, localInertia );
btRigidBody* ground_body = new btRigidBody( ground_cInfo );
ground_body->setFriction( 1.f );
ground_body->setRollingFriction( 1.f );
m_dynamics_world->addRigidBody( ground_body );

//compound
btCompoundShape* colShape = new btCompoundShape();

//box
btCollisionShape* boxShape = new btBoxShape( btVector3( 0.5f, 0.5f, 0.5f ) );

//add box #1
transform.setIdentity();
transform.setOrigin( btVector3( 0.f, 5.f, 0.f ) );
colShape->addChildShape( transform, boxShape );

//add box #2
transform.setIdentity();
transform.setOrigin( btVector3( 1.f, 5.f, 0.f ) );
colShape->addChildShape( transform, boxShape );

//compute inertia
transform.setIdentity();
transform.setOrigin( btVector3( 0, 5, 0 ) );
colShape->calculateLocalInertia( mass, localInertia );

m_collisionShapes.push_back( colShape );

//create rigid body and add it to the world
btDefaultMotionState* comp_motion_state = new btDefaultMotionState( transform );
btRigidBody::btRigidBodyConstructionInfo comp_cInfo( mass, comp_motion_state, colShape, localInertia );
btRigidBody* comp_body = new btRigidBody( comp_cInfo );
comp_body->setActivationState( DISABLE_DEACTIVATION );
m_dynamics_world->addRigidBody( comp_body );