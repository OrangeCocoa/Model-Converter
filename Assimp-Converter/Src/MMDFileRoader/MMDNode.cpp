
#include"MMDNode.h"

MMDNode::MMDNode(void)
	: enable_IK_(false)
	, parent_(nullptr)
	, child_(nullptr)
	, next_(nullptr)
	, prev_(nullptr)
	, translate_(DirectX::SimpleMath::Vector3::Zero)
	, rotate_(DirectX::SimpleMath::Quaternion(1, 0, 0, 0))
	, scale_(DirectX::SimpleMath::Vector3(1))
	, anim_translate_(DirectX::SimpleMath::Vector3::Zero)
	, anim_rotate_(DirectX::SimpleMath::Quaternion(1, 0, 0, 0))
	, base_anim_translate_(DirectX::SimpleMath::Vector3::Zero)
	, base_anim_rotate_(DirectX::SimpleMath::Quaternion(1, 0, 0, 0))
	, ik_rotate_(DirectX::SimpleMath::Quaternion(1, 0, 0, 0))
	, local_(DirectX::SimpleMath::Matrix::Identity)
	, global_(DirectX::SimpleMath::Matrix::Identity)
	, pre_inverse_(DirectX::SimpleMath::Matrix::Identity)
	, pre_translate_(DirectX::SimpleMath::Vector3::Zero)
	, pre_rotate_(DirectX::SimpleMath::Quaternion(1, 0, 0, 0))
	, pre_scale_(DirectX::SimpleMath::Vector3(1))
{
}

void MMDNode::AddChild(MMDNode * child)
{
	assert(child);
	assert(!child->parent_);
	assert(!child->prev_);
	assert(!child->next_);

	child->parent_ = this;

	if (!child_)
	{
		child_ = child;
		child->prev_ = child_;
		child->next_ = nullptr;
	}
	else
	{
		auto& prev_node = child_->prev_;
		prev_node->next_ = child;
		child->prev_ = prev_node;

		child_->prev_ = child;
	}
}

void MMDNode::BeginUpdateTransform(void)
{
}

void MMDNode::EndUpdateTransform(void)
{
}

void MMDNode::UpdateLocalTransform(void)
{
}

void MMDNode::UpdateGlobalTransform(void)
{
}

void MMDNode::UpdateChildTransform(void)
{
}

void MMDNode::CalculateInverseInitTransform(void)
{
}

void MMDNode::OnBeginUpdateTransform(void)
{
}

void MMDNode::OnEndUpdateTransfrom(void)
{
}

void MMDNode::OnUpdateLocalTransform(void)
{
}
