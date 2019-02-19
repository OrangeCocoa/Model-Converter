#pragma once

#include<string>
#include<DirectXTK\SimpleMath.h>

class MMDNode
{
public:
	MMDNode(void);

	void AddChild(MMDNode* child);
	// call begin and end
	void BeginUpdateTransform(void);
	void EndUpdateTransform(void);

	void UpdateLocalTransform(void);
	void UpdateGlobalTransform(void);
	void UpdateChildTransform(void);

	void SetIndex(uint32_t idx) { index_ = idx; }
	uint32_t GetIndex(void) const { return index_; }

	void SetName(const std::string& name) { this->name_ = name; }
	const std::string& GetName(void) const { return name_; }

	void EnableIK(bool enable) { enable_IK_ = enable; }
	bool IsIK(void) const { return enable_IK_; }

	void SetTranslate(const DirectX::SimpleMath::Vector3& t) { translate_ = t; }
	const DirectX::SimpleMath::Vector3& GetTranslate(void) const { return translate_; }

	void SetRotate(const DirectX::SimpleMath::Quaternion& r) { rotate_ = r; }
	const DirectX::SimpleMath::Quaternion& GetRotate(void) const { return rotate_; }

	void SetScale(const DirectX::SimpleMath::Vector3& s) { scale_ = s; }
	const DirectX::SimpleMath::Vector3& GetScale(void) const { return scale_; }

	void SetAnimationTranslate(const DirectX::SimpleMath::Vector3& t) { anim_translate_ = t; }
	const DirectX::SimpleMath::Vector3& GetAnimationTranslate(void) const { return anim_translate_; };

	void SetAnimationRotate(const DirectX::SimpleMath::Quaternion& q) { anim_rotate_ = q; }
	const DirectX::SimpleMath::Quaternion& GetAnimationRotate(void) const { return anim_rotate_; }

	DirectX::SimpleMath::Vector3 AnimateTranslate(void) const { return anim_translate_ + translate_; }
	DirectX::SimpleMath::Quaternion AnimateRotate(void) const { return anim_rotate_ * rotate_; }

	void SetIKRotate(const DirectX::SimpleMath::Quaternion& ikr) { ik_rotate_ = ikr; }
	const DirectX::SimpleMath::Quaternion& GetIKRotate(void) const { return ik_rotate_; }

	MMDNode* GetParent(void) const { return parent_; }
	MMDNode* GetChild(void) const { return child_; }
	MMDNode* GetNext(void) const { return next_; }
	MMDNode* GetPrev(void) const { return prev_; }

	void SetLocalTransform(const DirectX::SimpleMath::Matrix& m) { local_ = m; }
	const DirectX::SimpleMath::Matrix& GetLocalTransform(void) const { return local_; }

	void SetGlobalTransform(const DirectX::SimpleMath::Matrix& m) { global_ = m; }
	const DirectX::SimpleMath::Matrix& GetGlobalTransform(void) const { return global_; }

	void CalculateInverseInitTransform(void);
	const DirectX::SimpleMath::Matrix& GetInverseInitTransform(void) const { return pre_inverse_; }

	// call after node initialize
	void SaveInitialTRS(void)
	{
		pre_translate_ = translate_;
		pre_rotate_ = rotate_;
		pre_scale_ = scale_;
	}
	void LoadInitialTRS(void)
	{
		translate_ = pre_translate_;
		rotate_ = pre_rotate_;
		scale_ = pre_scale_;
		//anim_translate_ = DirectX::SimpleMath::Vector3(0, 0, 0);
		//anim_rotate_ = DirectX::SimpleMath::Quaternion(1, 0, 0, 0);
	}
	const DirectX::SimpleMath::Vector3& GetInitialTranslate(void) const { return pre_translate_; }
	const DirectX::SimpleMath::Quaternion& GetInitialRotate(void) const { return pre_rotate_; }
	const DirectX::SimpleMath::Vector3& GetInitialScale(void) const { return pre_scale_; }

	void SaveBaseAnimation(void)
	{
		base_anim_translate_ = anim_translate_;
		base_anim_rotate_ = anim_rotate_;
	}

	void LoadBaseAnimation(void)
	{
		anim_translate_ = base_anim_translate_;
		anim_rotate_ = base_anim_rotate_;
	}

	void ClearBaseAnimation(void)
	{
		base_anim_translate_ = DirectX::SimpleMath::Vector3(0, 0, 0);
		base_anim_rotate_ = DirectX::SimpleMath::Quaternion(1, 0, 0, 0);
	}

	const DirectX::SimpleMath::Vector3& GetBaseAnimationTranslate(void) const { return base_anim_translate_; };
	const DirectX::SimpleMath::Quaternion& GetBaseAnimationRotate(void) const { return base_anim_rotate_; }

protected:
	virtual void OnBeginUpdateTransform(void);
	virtual void OnEndUpdateTransfrom(void);
	virtual void OnUpdateLocalTransform(void);

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