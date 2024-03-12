// by jrzanol
// 

#pragma once
#include "rpc/msgpack.hpp"

#include <vector>

struct BODY
{
	int Id, Type;
	float PosX, PosY, PosZ;
	float Mass;
	char Name[12];

	MSGPACK_DEFINE_ARRAY(Id, Type, PosX, PosY, PosZ, Mass, Name)
};

using NBODY = std::vector<BODY>;

