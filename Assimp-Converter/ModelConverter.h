#pragma once

#include<memory>
#include<string>

class ModelConverter
{
private:
	class Impl;
	std::unique_ptr<Impl> impl_;

public:
	ModelConverter();
	~ModelConverter();

	bool Load(std::string file_path);
	bool Save(std::string file_path);
	// model read sample
	bool Read(std::string file_path);
};