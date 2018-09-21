
#include<iostream>
#include<fstream>
#include<vector>
#include<map>
#include<filesystem>

#include"ModelConverter.h"

#include<assimp\Importer.hpp>
#include<assimp\scene.h>
#include<assimp\postprocess.h>
#include<assimp\vector3.h>
#include<assimp\matrix3x3.h>
#include<assimp\matrix4x4.h>

#include <DirectXMath.h>

#pragma comment(lib, "assimp.lib")

class ModelConverter::Impl
{
public:
	Assimp::Importer importer_;
	aiScene* scene_;

	struct Vertex
	{
		aiVector3D position;
		aiVector3D normal;
		aiVector2D texcoord;
	};

	struct Mesh
	{
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
		std::string texture;
		std::string name;
	};

	struct Weight
	{
		int bone_num;
		float weight;
	};

	struct VertexWeight
	{
		std::vector<Weight> weights;
		aiVector3D vertex_pos;
		int numbones;
	};

	struct Surface
	{
		std::vector<VertexWeight> vertex_weights;
	};

	struct AnimationKey
	{
		float time;
		aiVector3D position;
		aiQuaternion rotation;
	};

	struct Bone
	{
		std::string name;
		std::string parent_name;
		aiVector3D position;
		aiQuaternion rotation;
		int num_pos;
		int num_rot;
		std::vector<AnimationKey> keys;
	};

	std::map<std::string, int> bone_map_; // name, index
	int bone_num_;
	std::vector<Mesh> meshes_;
	std::vector<Bone> bones_;
	std::vector<Surface> anim_surf_;

	void ProcessNodeHierarchey(const aiNode* node)
	{
		std::string node_name(node->mName.data);
		std::string parent_name = "none";

		if (node->mParent)
		{
			parent_name = std::string(node->mParent->mName.data);
		}

		// mesh find
		for (unsigned i = 0; i < node->mNumMeshes; ++i)
		{
			aiMesh* mesh = scene_->mMeshes[node->mMeshes[i]];
			meshes_.emplace_back(ProcessMesh(mesh));
			LoadBones(i, mesh);
			bones_.emplace_back(ProcessBone(node, node_name, parent_name));
		}


		for (unsigned int i = 0; i < node->mNumChildren; ++i)
		{
			ProcessNodeHierarchey(node->mChildren[i]);
		}
	}

	Mesh ProcessMesh(const aiMesh* mesh)
	{
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
		std::string texture;

		for (unsigned int n = 0; n < mesh->mNumVertices; ++n)
		{
			std::string name = mesh->mName.C_Str();

			Vertex vertex;

			vertex.position.x = mesh->mVertices[n].x;
			vertex.position.y = mesh->mVertices[n].y;
			vertex.position.z = mesh->mVertices[n].z;

			if (mesh->mNormals)
			{
				vertex.normal.x = (float)mesh->mNormals[n].x;
				vertex.normal.y = (float)mesh->mNormals[n].y;
				vertex.normal.z = (float)mesh->mNormals[n].z;
			}

			if (mesh->mTextureCoords[0])
			{
				vertex.texcoord.x = (float)mesh->mTextureCoords[0][n].x;
				vertex.texcoord.y = (float)mesh->mTextureCoords[0][n].y;
			}

			vertices.emplace_back(vertex);
		}

		for (unsigned int n = 0; n < mesh->mNumFaces; ++n)
		{
			aiFace face = mesh->mFaces[n];

			for (unsigned int i = 0; i < face.mNumIndices; ++i)
				indices.emplace_back(face.mIndices[i]);
		}

		if (mesh->mMaterialIndex >= 0)
		{
			aiMaterial * mat = scene_->mMaterials[mesh->mMaterialIndex];
			aiString str;
			mat->GetTexture(aiTextureType_DIFFUSE, 0, &str);

			std::string filepath(str.C_Str());
			size_t find = filepath.find_last_of("\\");
			if(find == std::string::npos)
				size_t find = filepath.find_last_of("//");

			filepath = filepath.substr(find + 1);
			texture = filepath.c_str();
		}

		Mesh ret;

		ret.vertices = vertices;
		ret.indices = indices;
		ret.texture = texture;

		return ret;
	}

	const aiNodeAnim* ProcessNodeAnim(const aiAnimation* animation, const std::string node_name)
	{
		for (unsigned int i = 0; i < animation->mNumChannels; ++i)
		{
			const aiNodeAnim* node_anim = animation->mChannels[i];

			if (std::string(node_anim->mNodeName.data) == node_name)
			{
				return node_anim;
			}
		}

		return nullptr;
	}

	Bone ProcessBone(const aiNode* node, std::string node_name, std::string parent_name)
	{
		Bone bone;
		bone.name = node_name;
		bone.parent_name = parent_name;
		bone.num_pos = 0;
		bone.num_rot = 0;

		node->mTransformation.DecomposeNoScaling(bone.rotation, bone.position);

		if (!scene_->mAnimations)
		{
			return Bone();
		}

		const aiAnimation* animation = scene_->mAnimations[0];
		const aiNodeAnim* node_anim = ProcessNodeAnim(animation, node_name);


		if (bone_map_.find(node_name) != bone_map_.end())
		{
			int bone_index = bone_map_[node_name];

			unsigned int num_keys = node_anim->mNumRotationKeys;

			bone.num_pos = num_keys;
			bone.num_rot = num_keys;

			unsigned int curr_key_idx;
			aiVectorKey curr_pos_key;
			aiQuatKey curr_rot_key;

			for (curr_key_idx = 0; curr_key_idx < num_keys; ++curr_key_idx)
			{
				curr_pos_key = node_anim->mPositionKeys[curr_key_idx];
				curr_rot_key = node_anim->mRotationKeys[curr_key_idx];
				float frameTime = static_cast<float>(curr_rot_key.mTime);

				aiVector3D key_pos;

				if (curr_key_idx < node_anim->mNumPositionKeys)
				{
					key_pos = curr_pos_key.mValue;

				}
				else {
					key_pos = bone.position;

				}

				AnimationKey key;
				key.position = key_pos;
				key.rotation = curr_rot_key.mValue;
				key.time = frameTime;
				bone.keys.emplace_back(key);
			}
		}

		return bone;
	}

	void LoadBones(int index, const aiMesh* mesh)
	{
		for (unsigned int i = 0; i < mesh->mNumBones; i++)
		{
			int bone_index = 0;
			std::string bone_name(mesh->mBones[i]->mName.data);

			if (bone_map_.find(bone_name) == bone_map_.end())
			{
				bone_index = bone_num_;
				bone_num_++;
				bone_map_[bone_name] = bone_index;
			}
			else {
				bone_index = bone_map_[bone_name];
			}
		}
	}
};

ModelConverter::ModelConverter() : impl_(std::make_unique<Impl>()){}

ModelConverter::~ModelConverter() = default;

bool ModelConverter::Load(std::string file_path)
{
	auto flag = aiProcess_Triangulate | aiProcess_ConvertToLeftHanded;

	impl_->scene_ = const_cast<aiScene*>(impl_->importer_.ReadFile(file_path.c_str(), flag));

	if (!impl_->scene_)
	{
		std::cout << impl_->importer_.GetErrorString() << std::endl;
		return false;
	}

	impl_->ProcessNodeHierarchey(impl_->scene_->mRootNode);

	return true;
}

bool ModelConverter::Save(std::string file_path)
{
	std::ofstream file;
	file.open(file_path, std::ios::out | std::ios::binary);

	unsigned int mesh_cnt = impl_->meshes_.size();

	file.write(reinterpret_cast<char*>(&mesh_cnt), sizeof(unsigned int));

	for (unsigned int m = 0; m < mesh_cnt; ++m)
	{
		auto & mesh = impl_->meshes_[m];

		unsigned int vtx_cnt = mesh.vertices.size();

		file.write(reinterpret_cast<char*>(&vtx_cnt), sizeof(unsigned int));

		for (unsigned int v = 0; v < vtx_cnt; ++v)
		{
			auto & vtx = mesh.vertices[v];

			file.write(reinterpret_cast<char*>(&vtx), sizeof(Impl::Vertex));
		}

		unsigned int index_cnt = mesh.indices.size();

		file.write(reinterpret_cast<char*>(&index_cnt), sizeof(unsigned int));

		for (unsigned int i = 0; i < index_cnt; ++i)
		{
			auto & index = mesh.indices[i];

			file.write(reinterpret_cast<char*>(&index), sizeof(unsigned int));
		}

		unsigned texture_str_cnt = mesh.texture.size();

		file.write(reinterpret_cast<char*>(&texture_str_cnt), sizeof(unsigned int));

		if (texture_str_cnt > 0)
		{
			std::cout << mesh.name.c_str() << " texture " << mesh.texture.c_str() << std::endl;
			file.write(mesh.texture.c_str(), sizeof(char) * texture_str_cnt);
		}
	}

	file.close();

	std::cout << "モデルファイルを .pzm 形式に変換しました" << std::endl;
	return true;
}