#pragma once

#include<string>
#include"MMDIKSolver.h"
#include"MMDCommon.h"

class MMDIKManager
{
public:
	virtual uint64_t GetIKSolverCount(void) = 0;
	virtual uint64_t FindIKSolverIndex(const std::string& name) = 0;
	virtual MMDIkSolver* GetMMDIKSolver(uint64_t idx) = 0;

	MMDIkSolver* GetMMDIKSolver(const std::string& ik_name)
	{
		auto find_idx = FindIKSolverIndex(ik_name);
		if (failed(find_idx))
		{
			return nullptr;
		}
		return GetMMDIKSolver(find_idx);
	}
};