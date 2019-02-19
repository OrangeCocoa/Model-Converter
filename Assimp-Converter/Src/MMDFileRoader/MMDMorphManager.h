#pragma once

#include<string>
#include"MMDMorph.h"
#include"MMDCommon.h"

class MMDMorphManager
{
public:
	virtual uint64_t GetMorphCount(void) = 0;
	virtual uint64_t FindMorphIndex(const std::string& name) = 0;
	virtual MMDMorph* GetMMDMorph(uint64_t idx) = 0;

	MMDMorph* GetMMDMorph(const std::string& morph_name)
	{
		auto find_idx = FindMorphIndex(morph_name);
		if (failed(find_idx))
		{
			return nullptr;
		}
		return GetMMDMorph(find_idx);
	}
};