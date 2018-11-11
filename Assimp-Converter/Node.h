#pragma once

#include<string>
#include<vector>
#include"assimp\scene.h"
#include"DirectXTK\SimpleMath.h"
#include"Mesh.h"
#include"Common.h"

struct Node
{
	std::string name;

	// update matrix
	DirectX::SimpleMath::Matrix transform;
	// default matrix
	DirectX::SimpleMath::Matrix transform_orig;

	DirectX::SimpleMath::Matrix global;
	DirectX::SimpleMath::Matrix inverse;

	// node parent look for node list after CreateNodeInfo ( key is node name )
	std::string parent_name;
	std::shared_ptr<Node> parent;

	unsigned int mesh_num;
	std::vector<std::shared_ptr<Mesh>> meshes;

	unsigned int children_num;
	std::vector<std::shared_ptr<Node>> childrens;
};

// node infomation load for assimp model data
std::shared_ptr<Node> CreateNode(const aiNode* const node, aiMesh** const mesh)
{
	Node temp_node;

	temp_node.name = node->mName.C_Str();

	if(node->mParent != 0)
		temp_node.parent_name = node->mParent->mName.C_Str();

	temp_node.transform = FromAssimp(node->mTransformation);
	temp_node.transform_orig = temp_node.transform;

	temp_node.mesh_num = node->mNumMeshes;
	temp_node.children_num = node->mNumChildren;

	// create mesh
	for (unsigned int i = 0; i < temp_node.mesh_num; ++i)
	{
		temp_node.meshes.emplace_back(LoadMesh(mesh[node->mMeshes[i]]));
	}

	// create child node
	for (unsigned int i = 0; i < temp_node.children_num; ++i)
	{
		temp_node.childrens.emplace_back(CreateNode(node->mChildren[i], mesh));
	}

	return std::make_shared<Node>(temp_node);
}

// create node list and map
void CreateNodeInfo(const std::shared_ptr<Node> node,
	std::vector<std::shared_ptr<Node>> node_list,
	std::unordered_map<std::string, std::shared_ptr<Node>> node_index)
{
	node_list.emplace_back(node);
	node_index.insert(std::make_pair(node->name, node));

	for (auto child : node->childrens)
	{
		CreateNodeInfo(child, node_list, node_index);
	}
}

// child matrix calculate for parent matrix
// use for mesh animation
void UpdateNodeDerivedMatrix(const std::shared_ptr<Node> node, const DirectX::SimpleMath::Matrix parent_matrix)
{
	node->global = node->transform * parent_matrix;
	node->inverse = node->global.Invert();

	for (auto child : node->childrens)
	{
		UpdateNodeDerivedMatrix(child, node->global);
	}
}