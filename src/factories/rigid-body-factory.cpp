#include "rigid-body-factory.hpp"

RigidBodyFactory::RigidBodyFactory(entt::DefaultRegistry& registry, btDiscreteDynamicsWorld& dynamicsWorld)
: m_registry(registry), m_dynamicsWorld(dynamicsWorld)
{}

RigidBodyFactory::~RigidBodyFactory() {
    // Remove the rigidbodies from the dynamics world and delete them
	for (int i = m_dynamicsWorld.getNumCollisionObjects() - 1; i >= 0; i--) {
		btCollisionObject* obj = m_dynamicsWorld.getCollisionObjectArray()[i];
		btRigidBody* body = btRigidBody::upcast(obj);
		if (body && body->getMotionState())
		{
			delete body->getMotionState();
		}
		m_dynamicsWorld.removeCollisionObject(obj);
		delete obj;
	}

    m_registry.view<cmpt::Collision>().each([](auto entity, cmpt::Collision& collision) {
        delete collision.collisionShape;
    });
}

cmpt::RigidBody RigidBodyFactory::createStatic(cmpt::Transform transform, cmpt::Collision collider) {
    btTransform rbTransform;
    rbTransform.setIdentity();
    rbTransform.setOrigin(btVector3(transform.position.x, transform.position.y, transform.position.z));
    btScalar mass(0.);
    btVector3 localInertia(0, 0, 0);
    collider.collisionShape->calculateLocalInertia(mass, localInertia);

    btDefaultMotionState* myMotionState = new btDefaultMotionState(rbTransform);
    btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, collider.collisionShape, localInertia);
    btRigidBody* body = new btRigidBody(rbInfo);

    m_dynamicsWorld.addRigidBody(body);
    cmpt::RigidBody rbComponent;
    rbComponent.rb = body;
    return rbComponent;
}

cmpt::RigidBody RigidBodyFactory::createDynamic(cmpt::Transform transform, cmpt::Collision collider, btScalar mass, btVector3 localInertia) {
    btTransform rbTransform;
    rbTransform.setIdentity();
    rbTransform.setOrigin(btVector3(transform.position.x, transform.position.y, transform.position.z));
    collider.collisionShape->calculateLocalInertia(mass, localInertia);

    btDefaultMotionState* myMotionState = new btDefaultMotionState(rbTransform);
    btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, collider.collisionShape, localInertia);
    btRigidBody* body = new btRigidBody(rbInfo);

    m_dynamicsWorld.addRigidBody(body);
    cmpt::RigidBody rbComponent;
    rbComponent.rb = body;
    return rbComponent;
}
