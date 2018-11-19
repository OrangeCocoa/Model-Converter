#pragma once

#include<vector>
#include<memory>
#include<unordered_map>
#include<limits>

#include<DirectXMath.h>
#include<DirectXCollision.h>

#include"assimp\scene.h"
#include<DirectXTK\SimpleMath.h>

#include"Node.h"
#include"Mesh.h"
#include"Material.h"
#include"Animation.h"

struct Model
{
	std::vector<std::shared_ptr<Material>> materials;

	// parent and child node
	std::shared_ptr<Node> node;
	// node list ( matrix all update )
	std::vector<std::shared_ptr<Node>> node_list;
	// node index ( animation update )
	std::unordered_map<std::string, std::shared_ptr<Node>> node_index;

	// animation
	bool has_anim;
	std::vector<std::shared_ptr<Animation>> animations;

	// default collider ( should be custom )
	std::unordered_map<std::string, DirectX::BoundingOrientedBox> obb_index;

	// create model
	void LoadModel(const aiScene* const scene)
	{
		// create material
		if (scene->HasMaterials())
		{
			aiMaterial** mat = scene->mMaterials;
			unsigned int mat_num = scene->mNumMaterials;

			for (unsigned int i = 0; i < mat_num; ++i)
			{
				materials.emplace_back(CreateMaterilal(mat[i]));
				
				if (!materials.back()->has_tex) continue;
				
				// texture load
			}
		}

		// create node
		node = std::make_shared<Node>();
		node = CreateNode(scene->mRootNode, scene->mMeshes);

		CreateNodeInfo(node, node_list, node_index);

		// create animation
		if (scene->HasAnimations())
		{
			aiAnimation** anim = scene->mAnimations;
			unsigned int anim_num = scene->mNumAnimations;

			for (unsigned int i = 0; i < anim_num; ++i)
			{
				animations.emplace_back(CreateAnimation(anim[i]));
			}
		}
	}

private:
	// reset mesh infomation
	void ResetMesh(void)
	{
		for (const auto&[node_name, node] : node_index)
		{
			for (auto mesh : node->meshes)
			{
				if (!mesh->has_bone) continue;

				mesh->mesh_info = mesh->mesh_info_orig;
			}
		}
	}

public:
	// reset node world matrix
	void ResetNode(void)
	{
		for (const auto& [node_name, node] : node_index)
		{
			if (node)
				node->transform = node->transform_orig;
		}

		ResetMesh();
	}

	// mesh matrix calculate for current time
	void UpdateNodeMatrix(const double time, const std::shared_ptr<Animation> animation)
	{
		for (const auto& node_anim : animation->node_anims)
		{
			DirectX::SimpleMath::Matrix matrix;
			
			DirectX::SimpleMath::Vector3 trans = GetLerpValue(time, node_anim.translate);
			matrix.CreateTranslation(trans);

			DirectX::SimpleMath::Vector3 scale = GetLerpValue(time, node_anim.scaling);
			matrix.CreateScale(scale);

			DirectX::SimpleMath::Quaternion rotate = GetLerpValue(time, node_anim.rotation);
			matrix.CreateFromQuaternion(rotate);

			node_index.at(node_anim.name)->transform = matrix;
		}
	}

	void UpdateMesh(void)
	{
		for (const auto& node : node_list)
		{
			for (const auto& mesh : node->meshes)
			{
				if (!mesh->has_bone) continue;

				std::vector<DirectX::SimpleMath::Matrix> bone_mtx;
				bone_mtx.reserve(mesh->bones.size());

				for (auto bone : mesh->bones)
				{
					auto local_node = node_index.at(bone.name);
					bone_mtx.emplace_back(node->inverse * local_node->global * bone.offset);
				}

				// destination mesh infomation
				decltype(mesh->mesh_info.vertices) mesh_vtx;
				mesh_vtx.reserve(mesh->mesh_info.vertices.size());

				decltype(mesh->mesh_info.normals) mesh_nml;
				mesh_nml.reserve(mesh->mesh_info.normals.size());

				std::fill(mesh_vtx.begin(), mesh_vtx.end(), DirectX::SimpleMath::Vector3::Zero);

				if (mesh->has_normals)
				{
					std::fill(mesh_nml.begin(), mesh_nml.end(), DirectX::SimpleMath::Vector3::Zero);
				}

				// original mesh infomation
				const auto& orig_vtx = mesh->mesh_info_orig.vertices;
				const auto& orig_nml = mesh->mesh_info_orig.normals;

				// all vertice
				for (unsigned int i = 0; i < mesh->bones.size(); ++i)
				{
					const auto& bone = mesh->bones[i];
					const auto& m = bone_mtx[i];

					for (const auto& weight : bone.weights)
					{
						mesh_vtx[weight.id] += weight.value * DirectX::SimpleMath::Vector3::Transform(orig_vtx[weight.id], m);
					}

					if (mesh->has_normals)
					{
						for (const auto& weight : bone.weights)
						{
							mesh_nml[weight.id] += weight.value * DirectX::SimpleMath::Vector3::Transform(orig_nml[weight.id], m);
						}
					}
				}

				// calc obb orientation ( rotate ) and set
				DirectX::SimpleMath::Quaternion rotate = DirectX::SimpleMath::Quaternion::CreateFromRotationMatrix(node->transform);

				obb_index[mesh->name].Orientation = rotate;
			}
		}
	}

	void UpdateModel(const double time, unsigned int index)
	{
		if (!has_anim) return;

		double curr_time = std::fmod(time, animations[index]->duration);

		UpdateNodeMatrix(curr_time, animations[index]);

		UpdateNodeDerivedMatrix(node, DirectX::SimpleMath::Matrix::Identity);

		UpdateMesh();
	}

	// initialize collider
	bool CreateOBB(void)
	{
		UpdateNodeDerivedMatrix(node, DirectX::SimpleMath::Matrix::Identity);

		UpdateModel(0.0, 0);

		float max_value = (std::numeric_limits<float>::max)();
		DirectX::SimpleMath::Vector3 min_vtx{ max_value, max_value, max_value };

		float min_value = (std::numeric_limits<float>::min)();
		DirectX::SimpleMath::Vector3 max_vtx{ min_value, min_value, min_value };

		// calc OBB vertice position
		for (const auto& node : node_list)
		{
			for (const auto& mesh : node->meshes)
			{
				const auto& verticies = mesh->mesh_info.vertices;

				for (const auto vtx : verticies)
				{
					// affine convert node matrix
					DirectX::SimpleMath::Vector3 temp_vtx = DirectX::SimpleMath::Vector3::Transform(vtx, node->global);

					min_vtx.x = (std::min)(temp_vtx.x, min_vtx.x);
					min_vtx.y = (std::min)(temp_vtx.y, min_vtx.y);
					min_vtx.z = (std::min)(temp_vtx.z, min_vtx.z);
					
					max_vtx.x = (std::max)(temp_vtx.x, max_vtx.x);
					max_vtx.y = (std::max)(temp_vtx.y, max_vtx.y);
					max_vtx.z = (std::max)(temp_vtx.z, max_vtx.z);
				}

				DirectX::SimpleMath::Vector3 center = 0.5f * (min_vtx + max_vtx);
				DirectX::SimpleMath::Vector3 extents = 
					DirectX::SimpleMath::Vector3(std::abs(max_vtx.x - center.x), std::abs(max_vtx.y - center.y), std::abs(max_vtx.z - center.z));

				// create obb ( not rotation is AABB )
				obb_index[mesh->name] = DirectX::BoundingOrientedBox(center, extents, DirectX::SimpleMath::Quaternion::Identity);
			}
		}

		return true;
	}

	void Draw(void)
	{
		// obb wireframe is line list for GetCorners vertex ( right under begin inverse clock rotete )

	}

	// draw reverse
	void ReverseModelNode(void)
	{
		for (const auto& node : node_list)
		{
			std::reverse(std::begin(node->meshes), std::end(node->meshes));
		}

		std::reverse(std::begin(node_list), std::end(node_list));

	}
};