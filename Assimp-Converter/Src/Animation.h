#pragma once

#include<vector>
#include<string>
#include<algorithm>
#include"DirectXTK\SimpleMath.h"
#include"assimp\scene.h"
#include"Common.h"

enum AnimBehaviour
{
	DEFAULT,
	CONSTANT,
	LINEAR,
	REPEAT
};

struct VectorKey
{
	double time;
	DirectX::SimpleMath::Vector3 vec_key;
};

struct QuatKey
{
	double time;
	DirectX::SimpleMath::Quaternion quat_key;
};

struct MeshKey
{
	double time;
	unsigned int value;
};

struct MeshMorphKey
{
	double time;
	std::vector<unsigned int> values;
	std::vector<double> weights;
};

VectorKey FromAssimp(const aiVectorKey& key)
{
	VectorKey v;

	v.time = key.mTime;
	v.vec_key = FromAssimp(key.mValue);

	return v;
}

QuatKey FromAssimp(const aiQuatKey& key)
{
	QuatKey q;

	q.time = key.mTime;
	q.quat_key = DirectX::SimpleMath::Quaternion( key.mValue.w, key.mValue.x, key.mValue.y, key.mValue.z );

	return q;
}

MeshKey FromAssimp(const aiMeshKey& key)
{
	MeshKey m{ key.mTime ,key.mValue };
	return m;
}

MeshMorphKey FromAssimp(const aiMeshMorphKey& key)
{
	MeshMorphKey mm;

	mm.time = key.mTime;

	unsigned int vw_num = key.mNumValuesAndWeights;

	for (unsigned int i = 0; i < vw_num; ++i)
	{
		mm.values.emplace_back(key.mValues[i]);
		mm.weights.emplace_back(key.mWeights[i]);
	}

	return mm;
}

struct NodeAnim
{
	std::string name;

	unsigned int translate_num;
	std::vector<VectorKey> translate;

	unsigned int scaling_num;
	std::vector<VectorKey> scaling;

	unsigned int rotation_num;
	std::vector<QuatKey>   rotation;

	AnimBehaviour post_state;
	AnimBehaviour pre_state;

	// node animation infomation load for assimp model data
	bool LoadNodeAnim(const aiNodeAnim* const node_anim)
	{
		name = node_anim->mNodeName.C_Str();

		translate_num = node_anim->mNumPositionKeys;
		scaling_num = node_anim->mNumScalingKeys;
		rotation_num = node_anim->mNumRotationKeys;

		for (unsigned int i = 0; i < translate_num; ++i)
		{// position info ( = transrate )
			translate.emplace_back(FromAssimp(node_anim->mPositionKeys[i]));
		}

		for (unsigned int i = 0; i < scaling_num; ++i)
		{// scale info
			scaling.emplace_back(FromAssimp(node_anim->mScalingKeys[i]));
		}

		for (unsigned int i = 0; i < rotation_num; ++i)
		{// rotate info
			rotation.emplace_back(FromAssimp(node_anim->mRotationKeys[i]));
		}

		post_state = static_cast<AnimBehaviour>(node_anim->mPostState);
		pre_state = static_cast<AnimBehaviour>(node_anim->mPreState);

		return true;
	}
};

struct MeshAnim
{
	std::string name;

	unsigned int key_num;
	std::vector<MeshKey> mesh_keys;

	// mesh animation infomation load for assimp model data
	bool LoadMeshAnim(const aiMeshAnim* const mesh_anim)
	{
		name = mesh_anim->mName.C_Str();

		key_num = mesh_anim->mNumKeys;

		for (unsigned int i = 0; i < key_num; ++i)
		{
			mesh_keys.emplace_back(FromAssimp(mesh_anim->mKeys[i]));
		}

		return true;
	}
};

struct MeshMorphAnim
{
	std::string name;

	unsigned int key_num;
	std::vector<MeshMorphKey> morph_keys;

	// mesh morph animation infomation load for assimp model data
	bool LoadMeshMorphAnim(const aiMeshMorphAnim* const mesh_morph_anim)
	{
		name = mesh_morph_anim->mName.C_Str();

		key_num = mesh_morph_anim->mNumKeys;

		for (unsigned int i = 0; i < key_num; ++i)
		{
			morph_keys.emplace_back(FromAssimp(mesh_morph_anim->mKeys[i]));
		}

		return true;
	}
};

struct Animation
{
	double duration;
	double ticks_per_second;

	unsigned int node_anim_num;
	std::vector<NodeAnim> node_anims;

	unsigned int mesh_anim_num;
	std::vector<MeshAnim> mesh_anims;

	unsigned int mesh_morph_anim_num;
	std::vector<MeshMorphAnim> mesh_morph_anims;
};

// animation infomation load for assimp model data
std::shared_ptr<Animation> CreateAnimation(const aiAnimation* const anim)
{
	Animation temp_anim;

	temp_anim.duration = anim->mDuration;
	temp_anim.ticks_per_second = anim->mTicksPerSecond;

	temp_anim.node_anim_num = anim->mNumChannels;
	temp_anim.mesh_anim_num = anim->mNumMeshChannels;
	temp_anim.mesh_morph_anim_num = anim->mNumMorphMeshChannels;

	for (unsigned int i = 0; i < temp_anim.node_anim_num; ++i)
	{// node animation
		temp_anim.node_anims.emplace_back();
		temp_anim.node_anims.back().LoadNodeAnim(anim->mChannels[i]);
	}

	for (unsigned int i = 0; i < temp_anim.mesh_anim_num; ++i)
	{// mesh animation
		temp_anim.mesh_anims.emplace_back();
		temp_anim.mesh_anims.back().LoadMeshAnim(anim->mMeshChannels[i]);
	}

	for (unsigned int i = 0; i < temp_anim.mesh_morph_anim_num; ++i)
	{// mesh morph animation (face morph ? )
		temp_anim.mesh_morph_anims.emplace_back();
		temp_anim.mesh_morph_anims.back().LoadMeshMorphAnim(anim->mMorphMeshChannels[i]);
	}

	return std::make_shared<Animation>(temp_anim);
}

template <typename T>
struct Compare
{
	bool operator()(const double lhs, const T& rhs) { return lhs < rhs.time; }
};

// Load straight lerp value for keyframe
DirectX::SimpleMath::Vector3 GetLerpValue(const double time, const std::vector<VectorKey>& values)
{
	// look for key position
	auto result = std::upper_bound(values.begin(), values.end(),
									time, Compare<VectorKey>());

	DirectX::SimpleMath::Vector3 value;

	if (result == values.begin())
	{// less than front time
		value = result->vec_key;
	}
	else if (result == values.end())
	{// more than back time
		value = (result - 1)->vec_key;
	}
	else
	{// straight lerp
		double dt = result->time - (result - 1)->time;
		double t = time - (result - 1)->time;
		value = DirectX::SimpleMath::Vector3::Lerp((result - 1)->vec_key, result->vec_key, static_cast<float>(t / dt));
	}

	return value;
}

// Load sphere lerp value for keyframe
DirectX::SimpleMath::Quaternion GetLerpValue(const double time, const std::vector<QuatKey>& values)
{
	// look for key position
	auto result = std::upper_bound(values.begin(), values.end(),
									time, Compare<QuatKey>());

	DirectX::SimpleMath::Quaternion value;

	if (result == values.begin())
	{// less than front time
		value = result->quat_key;
	}
	else if (result == values.end())
	{// more than back time
		value = (result - 1)->quat_key;
	}
	else
	{// sphere lerp
		double dt = result->time - (result - 1)->time;
		double t = time - (result - 1)->time;
		value = DirectX::SimpleMath::Quaternion::Slerp((result - 1)->quat_key, result->quat_key, static_cast<float>(t / dt));
	}

	return value;
}