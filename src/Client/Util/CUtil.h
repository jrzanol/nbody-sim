// by jrzanol
//

#pragma once

class CUtil
{
public:
	static int g_EnemyCount;

	static const char* m_VertexShader;
	static const char* m_FragmentShader;
};

extern unsigned int g_FPS;

extern float g_DeltaTime;
extern float g_LastTime;

const int g_WindowMaxX = 1024;
const int g_WindowMaxY = 768;

extern GLFWwindow* g_Window;
