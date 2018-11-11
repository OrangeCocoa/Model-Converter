#pragma once

#include<memory>

class AssimpConverter
{
private:
	class Impl;
	std::unique_ptr<Impl> impl_;

public:
	AssimpConverter();
	~AssimpConverter();

	bool Load(std::string);
	bool Save(std::string);
	bool Read(std::string);
};