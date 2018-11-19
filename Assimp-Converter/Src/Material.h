#pragma once

#include<iostream>
#include<string>
#include<filesystem>
#include"DirectXTK\SimpleMath.h"
#include"assimp\scene.h"
#include"Common.h"

struct Material
{
	DirectX::SimpleMath::Color diffuse;
	DirectX::SimpleMath::Color ambient;
	DirectX::SimpleMath::Color specular;
	float shininess;
	DirectX::SimpleMath::Color emissive;

	bool has_tex;
	std::string tex_name;
};

// material infomation load for assimp model data
std::shared_ptr<Material> CreateMaterilal(const aiMaterial* const mat)
{
	Material temp_mat;

	{// diffuse
		aiColor3D color;
		mat->Get(AI_MATKEY_COLOR_DIFFUSE, color);
		temp_mat.diffuse = FromAssimp(color);
	}

	{// ambient
		aiColor3D color;
		mat->Get(AI_MATKEY_COLOR_AMBIENT, color);
		temp_mat.ambient = FromAssimp(color);
	}

	{// specular
		aiColor3D color;
		mat->Get(AI_MATKEY_COLOR_SPECULAR, color);
		temp_mat.specular = FromAssimp(color);
	}

	{// shininess
		temp_mat.shininess = 80.0f;
		mat->Get(AI_MATKEY_SHININESS, temp_mat.shininess);
	}

	{// emissive
		aiColor3D color;
		mat->Get(AI_MATKEY_COLOR_EMISSIVE, color);
		temp_mat.emissive = FromAssimp(color);
	}

	{// texture name
		aiString tex_path;
		if (mat->Get(AI_MATKEY_TEXTURE(aiTextureType_DIFFUSE, 0), tex_path) == AI_SUCCESS)
		{
			temp_mat.has_tex = true;

			std::string file_path(tex_path.C_Str());
			size_t find = file_path.find_last_of("\\");
			if (find == std::string::npos)
				size_t find = file_path.find_last_of("//");

			file_path = file_path.substr(find + 1);

			temp_mat.tex_name = file_path;
		}
	}

	if (temp_mat.has_tex)
		std::cout << "Texture:" << temp_mat.tex_name << std::endl;

	std::cout << "Diffuse : " << temp_mat.diffuse << std::endl;
	std::cout << "Ambient : " << temp_mat.ambient << std::endl;
	std::cout << "Specular : " << temp_mat.specular << std::endl;
	std::cout << "Shininess : " << temp_mat.shininess << std::endl;
	std::cout << "Emission : " << temp_mat.emissive << std::endl;

	std::cout << std::endl;

	return std::make_shared<Material>(temp_mat);
}