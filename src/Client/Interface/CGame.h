// by jrzanol
//

#pragma once

#include "CUtil.h"
#include "CModel.h"

#include "rpc/client.h"

class CGame : CEvent
{
public:
	CGame();
	~CGame();

	void Initialize();

	void ProcessInput(GLFWwindow*);
	void ProcessMouseButtonEvent(GLFWwindow*, int, int, int);
	void ProcessMiliSecTimer();
	void ProcessSecTimer();

	static rpc::client* m_Rpc;

	static int m_BodyNameSize;
	static char* m_BodyName[32];
};

