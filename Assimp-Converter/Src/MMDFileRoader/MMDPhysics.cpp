
#include"MMDPhysics.h"

#include<bullet3\btBulletCollisionCommon.h>
#include<bullet3\btBulletDynamicsCommon.h>

#pragma comment(lib, "bullet3\BulletDynamics.lib")
#pragma comment(lib, "bullet3\BulletCollision.lib")
#pragma comment(lib, "bullet3\LinearMath.lib")

class MMDMotionState : public btMotionState
{
public:
	virtual void Reset() = 0;
	virtual void ReflectGlobalTransform() = 0;
};

MMDRigidBody::MMDRigidBody(void){}

MMDRigidBody::~MMDRigidBody(void) = default;

bool MMDRigidBody::Create(const PMXRigidbody & pmx_rigid_body, MMDModel * model, MMDNode * node)
{
	return true;
}

void MMDRigidBody::Destroy(void)
{
	if(shape_)
		shape_.reset();
	if(active_motion_state_)
		active_motion_state_.reset();
	if(kinematic_motion_state_)
		kinematic_motion_state_.reset();
	if(rigid_body_)
		rigid_body_.reset();

	if (node_)
	{
		node_ = nullptr;
	}
}

btRigidBody* MMDRigidBody::GetRigidBody(void) const
{
	return nullptr;
}

unsigned short MMDRigidBody::GetGroup(void) const
{
	return 0;
}

unsigned short MMDRigidBody::GetGroupMask(void) const
{
	return 0;
}

void MMDRigidBody::SetActivation(bool activation)
{
}

void MMDRigidBody::ResetTransform(void)
{
}

void MMDRigidBody::Reset(MMDPhysics * physics)
{
}

void MMDRigidBody::ReflectGlobalTransform(void)
{
}

void MMDRigidBody::CalcLocalTransform(void)
{
}

DirectX::SimpleMath::Matrix MMDRigidBody::GetTransform(void)
{
	return DirectX::SimpleMath::Matrix();
}

MMDJoint::MMDJoint(void)
{
}

MMDJoint::~MMDJoint(void)
{
}

bool MMDJoint::CreateJoint(const PMXJoint & pmx_joint, MMDRigidBody * rigid_body_A, MMDRigidBody * rigid_body_B)
{
	return true;
}

void MMDJoint::Destroy(void)
{
}

btTypedConstraint* MMDJoint::GetConstraint(void) const
{
	return nullptr;
}
