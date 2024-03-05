// by jrzanol
//

#include "stdafx.h"
#include "CWindow.h"
#include "CGame.h"

CGame::CGame() : CEvent()
{
}

CGame::~CGame()
{
}

void CGame::Initialize()
{
    // Load Models.
    //CModel* player = CWindow::CreateModel(0, "Mesh/Player.obj");

    // Pre-Load Enemy Models.
    //CModel::LoadModel("Mesh/Enemy.obj", false);
    //CModel::LoadModel("Mesh/Enemy2.obj", false);
    //CModel::LoadModel("Mesh/Enemy3.obj", false);
}

void CGame::ProcessInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void CGame::ProcessMouseButtonEvent(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT)
    {
        //if (action == GLFW_PRESS)
        //    CWindow::CreateModel(4, "Mesh/Bullet.obj");
    }
}

void CGame::ProcessMiliSecTimer()
{
}

void CGame::ProcessSecTimer()
{
}

