// by jrzanol
// 

#pragma once
#include "rpc/msgpack.hpp"

#include <vector>

struct BODY
{
	int Type;
	float PosX, PosY, PosZ;

	MSGPACK_DEFINE_ARRAY(Type, PosX, PosY, PosZ)
};

using NBODY = std::vector<BODY>;

