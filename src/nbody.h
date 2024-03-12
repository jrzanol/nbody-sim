// by jrzanol
// 

#pragma once
#include "rpc/msgpack.hpp"

#include <vector>

struct BODY
{
	int Id, Type;
	float PosX, PosY, PosZ;

	MSGPACK_DEFINE_ARRAY(Id, Type, PosX, PosY, PosZ)
};

using NBODY = std::vector<BODY>;

