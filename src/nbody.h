// by jrzanol
// 

#pragma once
#include "rpc/msgpack.hpp"

#include <vector>

struct BODY
{
	float X, Y, Z;
	MSGPACK_DEFINE_ARRAY(X, Y, Z)
};

using NBODY = std::vector<BODY>;

