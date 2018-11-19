#pragma once

#include<string>
#include<DirectXTK\SimpleMath.h>

class MMDNode
{
public:
	MMDNode();

	void AddChild(MMDNode* child);
	// call begin and end
	void BeginUpdateTransform();
	void EndUpdateTransform();

	void UpdateLocalTransform();
	void UpdateGlobalTransform();
	void UpdateChildTransform();

	void SetIndex(uint32_t idx) { index_ = idx; }
	uint32_t GetIndex() const { return index_; }

	void SetName(const std::string& name) { this->name_ = name; }
	const std::string& GetName() const { return name_; }

	void EnableIK(bool enable) { enable_IK_ = enable; }
	bool IsIK() const { return enable_IK_; }

	void SetTranslate(const DirectX::SimpleMath::Vector3& t) { translate_ = t; }
	const DirectX::SimpleMath::Vector3& GetTranslate() const { return translate_; }

	void SetRotate(const DirectX::SimpleMath::Quaternion& r) { rotate_ = r; }
	const DirectX::SimpleMath::Quaternion& GetRotate() const { return rotate_; }

	void SetScale(const DirectX::SimpleMath::Vector3& s) { scale_ = s; }
	const DirectX::SimpleMath::Vector3& GetScale() const { return scale_; }

	void SetAnimationTranslate(const DirectX::SimpleMath::Vector3& t) { anim_translate_ = t; }
	const DirectX::SimpleMath::Vector3& GetAnimationTranslate() const { return anim_translate_; };

	void SetAnimationRotate(const DirectX::SimpleMath::Quaternion& q) { anim_rotate_ = q; }
	const DirectX::SimpleMath::Quaternion& GetAnimationRotate() const { return anim_rotate_; }

	DirectX::SimpleMath::Vector3 AnimateTranslate() const { return anim_translate_ + translate_; }
	DirectX::SimpleMath::Quaternion AnimateRotate() const { return anim_rotate_ * rotate_; }

	void SetIKRotate(const DirectX::SimpleMath::Quaternion& ikr) { ik_rotate_ = ikr; }
	const DirectX::SimpleMath::Quaternion& GetIKRotate() const { return ik_rotate_; }

	MMDNode* GetParent() const { return parent_; }
	MMDNode* GetChild() const { return child_; }
	MMDNode* GetNext() const { return next_; }
	MMDNode* GetPrev() const { return prev_; }

	void SetLocalTransform(const DirectX::SimpleMath::Matrix& m) { local_ = m; }
	const DirectX::SimpleMath::Matrix& GetLocalTransform() const { return local_; }

	void SetGlobalTransform(const DirectX::SimpleMath::Matrix& m) { global_ = m; }
	const DirectX::SimpleMath::Matrix& GetGlobalTransform() const { return global_; }

	void CalculateInverseInitTransform();
	const DirectX::SimpleMath::Matrix& GetInverseInitTransform() const { return pre_inverse_; }

	// call after node initialize
	void SaveInitialTRS()
	{
		pre_translate_ = translate_;
		pre_rotate_ = rotate_;
		pre_scale_ = scale_;
	}
	void LoadInitialTRS()
	{
		translate_ = pre_translate_;
		rotate_ = pre_rotate_;
		scale_ = pre_scale_;
		//anim_translate_ = DirectX::SimpleMath::Vector3(0, 0, 0);
		//anim_rotate_ = DirectX::SimpleMath::Quaternion(1, 0, 0, 0);
	}
	const DirectX::SimpleMath::Vector3& GetInitialTranslate() const { return pre_translate_; }
	const DirectX::SimpleMath::Quaternion& GetInitialRotate() const { return pre_rotate_; }
	const DirectX::SimpleMath::Vector3& GetInitialScale() const { return pre_scale_; }

	void SaveBaseAnimation()
	{
		base_anim_translate_ = anim_translate_;
		base_anim_rotate_ = anim_rotate_;
	}

	void LoadBaseAnimation()
	{
		anim_translate_ = base_anim_translate_;
		anim_rotate_ = base_anim_rotate_;
	}

	void ClearBaseAnimation()
	{
		base_anim_translate_ = DirectX::SimpleMath::Vector3(0, 0, 0);
		base_anim_rotate_ = DirectX::SimpleMath::Quaternion(1, 0, 0, 0);
	}

	const DirectX::SimpleMath::Vector3& GetBaseAnimationTranslate() const { return base_anim_translate_; };
	const DirectX::SimpleMath::Quaternion& GetBaseAnimationRotate() const { return base_anim_rotate_; }

protected:
	virtual void OnBeginUpdateTransform();
	virtual void OnEndUpdateTransfrom();
	virtual void OnUpdateLocalTransform();

protected:
	uint32_t		index_;
	std::string		name_;
	bool			enable_IK_;

	MMDNode*		parent_;
	MMDNode*		child_;
	MMDNode*		next_;
	MMDNode*		prev_;

	DirectX::SimpleMath::Vector3	translate_;
	DirectX::SimpleMath::Quaternion	rotate_;
	DirectX::SimpleMath::Vector3	scale_;

	DirectX::SimpleMath::Vector3	anim_translate_;
	DirectX::SimpleMath::Quaternion	anim_rotate_;

	DirectX::SimpleMath::Vector3	base_anim_translate_;
	DirectX::SimpleMath::Quaternion	base_anim_rotate_;

	DirectX::SimpleMath::Quaternion	ik_rotate_;

	DirectX::SimpleMath::Matrix		local_;
	DirectX::SimpleMath::Matrix		global_;
	DirectX::SimpleMath::Matrix		pre_inverse_;

	DirectX::SimpleMath::Vector3	pre_translate_;
	DirectX::SimpleMath::Quaternion	pre_rotate_;
	DirectX::SimpleMath::Vector3	pre_scale_;
};