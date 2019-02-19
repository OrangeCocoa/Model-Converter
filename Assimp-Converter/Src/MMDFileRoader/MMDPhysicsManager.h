#pragma once

#include<string>
#include"MMDPhysics.h"
#include"MMDCommon.h"

class MMDPhysicsManager
{
public:
	virtual uint64_t GetPhysicsCount(void) = 0;
	virtual uint64_t FindPhysicsIndex(const std::string& name) = 0;
	virtual MMDPhysics* GetMMDPhysics(uint64_t idx) = 0;

	MMDPhysics* GetMMDPhysics(const std::string& physics_name)
	{
		auto find_idx = FindPhysicsIndex(physics_name);
		if (failed(find_idx))
		{
			return nullptr;
		}
		return GetMMDPhysics(find_idx);
	}
};