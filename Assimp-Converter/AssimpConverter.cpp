
#include<string>
#include<iostream>
#include<fstream>
#include<memory>

#include<assimp\Importer.hpp>
#include<assimp\scene.h>
#include<assimp\postprocess.h>
#include<assimp\vector3.h>
#include<assimp\matrix3x3.h>
#include<assimp\matrix4x4.h>

#pragma comment(lib, "assimp.lib")
#pragma comment(lib, "DirectXTK.lib")

#include"AssimpConverter.h"
#include"Model.h"

class AssimpConverter::Impl
{
public:
	Assimp::Importer importer_;
	aiScene* scene_;

	std::shared_ptr<Model> model_;

	Impl(){}
};

AssimpConverter::AssimpConverter() : impl_(std::make_unique<Impl>()) {}
AssimpConverter::~AssimpConverter() = default;

bool AssimpConverter::Load(std::string file_path)
{
	auto flag = aiProcess_Triangulate | aiProcess_ConvertToLeftHanded;

	// scene
	impl_->scene_ = const_cast<aiScene*>(impl_->importer_.ReadFile(file_path.c_str(), flag));

	if (!impl_->scene_)
	{
		std::cout << impl_->importer_.GetErrorString() << std::endl;
		return false;
	}

	// model
	if (impl_->model_) impl_->model_.reset();

	impl_->model_ = std::make_shared<Model>();

	impl_->model_->LoadModel(impl_->scene_);

	return true;
}

bool AssimpConverter::Save(std::string file_path)
{
	std::ofstream file;
	file.open(file_path, std::ios::out | std::ios::binary | std::ios::trunc);

	impl_->model_;

	file.close();

	return true;
}

bool AssimpConverter::Read(std::string file_path)
{
	std::ifstream file;
	file.open(file_path, std::ios::in | std::ios::binary);

	

	file.close();

	return true;
}
