#pragma once

#include<vector>
#include<string>
#include<memory>
#include"DirectXTK\SimpleMath.h"
#include"assimp\scene.h"
#include"Common.h"

#pragma comment(lib, "assimp.lib")

struct MeshInfo
{
	std::vector<DirectX::SimpleMath::Vector3> vertices;
	std::vector<DirectX::SimpleMath::Vector3> normals;
	std::vector<DirectX::SimpleMath::Vector3> tangents;
	std::vector<DirectX::SimpleMath::Vector3> bitangents;
	std::vector<DirectX::SimpleMath::Vector3> texcoords;
	std::vector<DirectX::SimpleMath::Color>	  colors;
};

struct Weight
{
	unsigned int id;
	float value;
};

struct Bone
{
	std::string name;
	// update matrix
	DirectX::SimpleMath::Matrix offset;
	// default matrix
	DirectX::SimpleMath::Matrix offset_orig;

	unsigned int weight_num;
	std::vector<Weight> weights;

	// bone infomation load for assimp model data
	bool LoadBone(const aiBone* const bone)
	{
		name = bone->mName.C_Str();
		offset = FromAssimp(bone->mOffsetMatrix);

		weight_num = bone->mNumWeights;

		for (unsigned int i = 0; i < weight_num; ++i)
		{
			Weight weight{ bone->mWeights[i].mVertexId, bone->mWeights[i].mWeight };
			weights.emplace_back(weight);
		}

		return true;
	}
};

struct Face
{
	unsigned int indice_num;
	std::vector<unsigned int> indieces;

	bool LoadFace(const aiFace* const face)
	{
		indice_num = face->mNumIndices;

		for (unsigned int i = 0; i < indice_num; ++i)
		{
			indieces.emplace_back(face->mIndices[i]);
		}

		return true;
	}
};

struct AnimMesh
{
	unsigned int vertice_num;
	float weight;
	MeshInfo mesh_info;

	// animation mesh infomation load for assimp model data
	bool LoadAnimMesh(const aiAnimMesh* const anim_mesh)
	{
		vertice_num = anim_mesh->mNumVertices;
		
		aiVector3D* texcoord_temp = anim_mesh->mTextureCoords[0];
		aiColor4D* color_temp = anim_mesh->mColors[0];

		for (unsigned int i = 0; i < vertice_num; ++i)
		{
			mesh_info.vertices.emplace_back(FromAssimp(anim_mesh->mVertices[i]));
			mesh_info.normals.emplace_back(FromAssimp(anim_mesh->mNormals[i]));
			mesh_info.tangents.emplace_back(FromAssimp(anim_mesh->mTangents[i]));
			mesh_info.bitangents.emplace_back(FromAssimp(anim_mesh->mBitangents[i]));
			mesh_info.texcoords.emplace_back(FromAssimp(texcoord_temp[i]));
			mesh_info.colors.emplace_back(FromAssimp(color_temp[i]));
		}

		return true;
	}
};

struct Mesh
{
	std::string name;

	unsigned int primitive_types;

	bool has_vertices;
	bool has_normals;
	bool has_tangents_and_bitangents;
	bool has_texcoord;
	bool has_color;
	bool has_bone;
	bool has_face;

	unsigned int vertice_num;
	MeshInfo mesh_info;
	MeshInfo mesh_info_orig;

	unsigned int bone_num;
	std::vector<Bone> bones;

	unsigned int face_num;
	std::vector<Face> faces;

	unsigned int anim_mesh_num;
	std::vector<AnimMesh> anim_mesh;

	unsigned int uv_num;
	std::vector<unsigned int> uv_channels;

	unsigned int material_index;
};

// mesh infomation load for assimp model data
std::shared_ptr<Mesh> LoadMesh(const aiMesh* const mesh)
{
	Mesh temp_mesh;
	temp_mesh.name = mesh->mName.C_Str();

	temp_mesh.primitive_types = mesh->mPrimitiveTypes;
	temp_mesh.material_index = mesh->mMaterialIndex;

	temp_mesh.has_vertices = mesh->HasPositions();
	temp_mesh.has_normals = mesh->HasNormals();
	temp_mesh.has_texcoord = mesh->HasTextureCoords(0);
	temp_mesh.has_color = mesh->HasVertexColors(0);
	temp_mesh.has_tangents_and_bitangents = mesh->HasTangentsAndBitangents();

	temp_mesh.has_bone = mesh->HasBones();
	temp_mesh.has_face = mesh->HasFaces();

	temp_mesh.anim_mesh_num = mesh->mNumAnimMeshes;

	// uv channel 
	temp_mesh.uv_num = mesh->GetNumUVChannels();
	for (unsigned int i = 0; i < temp_mesh.uv_num; ++i)
	{
		temp_mesh.uv_channels.emplace_back(mesh->mNumUVComponents[i]);
	}

	// load vertex info
	if (temp_mesh.has_vertices)
	{
		temp_mesh.vertice_num = mesh->mNumVertices;

		aiVector3D* texcoord_temp = mesh->mTextureCoords[0];
		aiColor4D* color_temp = mesh->mColors[0];

		for (unsigned int i = 0; i < temp_mesh.vertice_num; ++i)
		{
			temp_mesh.mesh_info.vertices.emplace_back(FromAssimp(mesh->mVertices[i]));

			// load normal info
			if (temp_mesh.has_normals)
				temp_mesh.mesh_info.normals.emplace_back(FromAssimp(mesh->mNormals[i]));

			// load tangents and bitangents info
			if (temp_mesh.has_tangents_and_bitangents)
			{
				temp_mesh.mesh_info.tangents.emplace_back(FromAssimp(mesh->mTangents[i]));
				temp_mesh.mesh_info.bitangents.emplace_back(FromAssimp(mesh->mBitangents[i]));
			}

			// load texture coord info
			if (temp_mesh.has_texcoord)
				temp_mesh.mesh_info.texcoords.emplace_back(FromAssimp(texcoord_temp[i]));

			// load vertice color info
			if (temp_mesh.has_color)
				temp_mesh.mesh_info.colors.emplace_back(FromAssimp(color_temp[i]));
		}

		temp_mesh.mesh_info_orig = temp_mesh.mesh_info;
	}

	// load bone info
	if (temp_mesh.has_bone)
	{
		temp_mesh.bone_num = mesh->mNumBones;

		for (unsigned int i = 0; i < temp_mesh.bone_num; ++i)
		{
			temp_mesh.bones.emplace_back();
			temp_mesh.bones.back().LoadBone(mesh->mBones[i]);
		}
	}

	// load face info
	if (temp_mesh.has_face)
	{
		temp_mesh.face_num = mesh->mNumFaces;

		for (unsigned int i = 0; i < temp_mesh.face_num; ++i)
		{
			temp_mesh.faces.emplace_back();
			temp_mesh.faces.back().LoadFace(&mesh->mFaces[i]);
		}
	}

	// load anim mesh info
	temp_mesh.anim_mesh_num = mesh->mNumAnimMeshes;

	for (unsigned int i = 0; i < temp_mesh.anim_mesh_num; ++i)
	{
		temp_mesh.anim_mesh.emplace_back();
		temp_mesh.anim_mesh.back().LoadAnimMesh(mesh->mAnimMeshes[i]);
	}

	return std::make_shared<Mesh>(temp_mesh);
}