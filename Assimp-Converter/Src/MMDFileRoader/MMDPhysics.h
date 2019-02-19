#pragma once

#include<memory>
#include<string>
#include<DirectXTK\SimpleMath.h>

#include"PMXFile.h"

class btRigidBody;
class btCollisionShape;
class btTypedConstraint;
class btDiscreteDynamicsWorld;
class btBroadphaseInterface;
class btDefaultCollisionConfiguration;
class btCollisionDispatcher;
class btSequentialImpulseConstraintSolver;
class btMotionState;
struct btOverlapFilterCallback;

class MMDPhysics;
class MMDModel;
class MMDNode;
class MMDMotionState;

class MMDRigidBody
{
private:
	enum class RigidBodyType
	{
		Kinematic,
		Dynamic,
		Aligned,
	};

	std::unique_ptr<btCollisionShape>	shape_;
	std::unique_ptr<MMDMotionState>		active_motion_state_;
	std::unique_ptr<MMDMotionState>		kinematic_motion_state_;
	std::unique_ptr<btRigidBody>		rigid_body_;

	RigidBodyType	rigid_body_type_;
	unsigned short	group_;
	unsigned short	group_mask_;

	MMDNode*                    node_;
	DirectX::SimpleMath::Matrix offset_mtx_;

	std::string                 name_;

public:
	MMDRigidBody(void);
	~MMDRigidBody(void);
	MMDRigidBody(const MMDRigidBody& rhs) = delete;
	MMDRigidBody& operator = (const MMDRigidBody& rhs) = delete;

	//bool Create(const PMDRigidBodyExt& pmd_rigid_body, MMDModel* model, MMDNode* node);
	bool Create(const PMXRigidbody& pmx_rigid_body, MMDModel* model, MMDNode* node);
	void Destroy(void);

	btRigidBody* GetRigidBody(void) const;
	unsigned short GetGroup(void) const;
	unsigned short GetGroupMask(void) const;

	void SetActivation(bool activation);
	void ResetTransform(void);
	void Reset(MMDPhysics* physics);

	void ReflectGlobalTransform(void);
	void CalcLocalTransform(void);

	DirectX::SimpleMath::Matrix GetTransform(void);
};

class MMDJoint
{
private:
	std::unique_ptr<btTypedConstraint>	constraint_;

public:
	MMDJoint(void);
	~MMDJoint(void);
	MMDJoint(const MMDJoint& rhs) = delete;
	MMDJoint& operator = (const MMDJoint& rhs) = delete;

	//bool CreateJoint(const PMDJointExt& pmd_joint, MMDRigidBody* rigid_body_A, MMDRigidBody* rigid_body_B);
	bool CreateJoint(const PMXJoint& pmx_joint, MMDRigidBody* rigid_body_A, MMDRigidBody* rigid_body_B);
	void Destroy(void);

	btTypedConstraint* GetConstraint(void) const;
};


class MMDPhysics
{
private:

public:
};