#pragma once

#include "Type.h"

struct Variable
{
	Type type;
	int offset;
	int pointerDepth;
	bool used;

	Variable() : type(), offset(0), pointerDepth(0), used(false) {}
	Variable(Type type, int pointerDepth, bool used = false)
		: type(type), offset(0), pointerDepth(pointerDepth), used(used) {}
	Variable(Type type, int offset, int pointerDepth, bool used = false)
		: type(type), offset(offset), pointerDepth(pointerDepth), used(used) {}
};
