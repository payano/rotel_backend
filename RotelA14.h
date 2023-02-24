#pragma once

#include "RotelBase.h"

namespace rotel {

class RotelA14 : public RotelBase {
public:
	RotelA14();
	virtual ~RotelA14();

	const std::map<COMMAND_TYPE, std::vector<int>>& getFeatures() override;

private:
	std::map<COMMAND_TYPE, std::vector<int>> features;
};


} /* namespace rotel */

