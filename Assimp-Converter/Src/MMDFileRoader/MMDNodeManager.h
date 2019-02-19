#pragma once

#include<string>
#include"MMDNode.h"
#include"MMDCommon.h"

class MMDNodeManager
{
public:
	virtual uint64_t GetNodeCount(void) = 0;
	virtual uint64_t FindNodeIndex(const std::string& name) = 0;
	virtual MMDNode* GetMMDNode(uint64_t idx) = 0;

	MMDNode* GetMMDNode(const std::string& node_name)
	{
		auto find_idx = FindNodeIndex(node_name);
		if (failed(find_idx))
		{
			return nullptr;
		}
		return GetMMDNode(find_idx);
	}
};