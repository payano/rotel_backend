#include <stdio.h>
#include "RotelBase.h"

int main(void) {
	std::unique_ptr<rotel::RotelBase> rb = rotel::RotelBase::get("10.10.20.124");
	rb->setFeature(rotel::COMMAND_TYPE::SOURCE_SELECTION_COMMANDS, static_cast<int>(rotel::SOURCE_SELECTION_COMMANDS::OPT1));
	rb->getSettings();
	return 0;
}
