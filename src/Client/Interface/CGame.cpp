// by jrzanol
//

#include "stdafx.h"
#include "CWindow.h"
#include "CGame.h"

#include "..\nbody.h"

CGame::CGame() : CEvent()
{
    m_Rpc = NULL;
}

CGame::~CGame()
{
}

void CGame::Initialize()
{
    m_Rpc = new rpc::client("127.0.0.1", 8080);

    // Pre-Load Enemy Models.
    //CModel::LoadModel("Mesh/Enemy.obj", false);

    //CModel* earth = CModel::LoadModel("Mesh/Earth.obj", true);
    //*earth->GetPosition() = glm::vec3(0.f, 5.f, 0.f);
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
    if (m_Rpc)
    {
        NBODY result = m_Rpc->call("get").as<NBODY>();

        for (const BODY& b : result)
        {
            printf("Spawn Id=%d Type=%d Pos=(%.2f,0.f,%.2f)\n", b.Id, b.Type, b.PosX, b.PosY);

            CModel* m = CModel::GetModelFromId(b.Id);
            if (m == NULL)
            {
                const char* modelStr = NULL;

                //if (b.Type == 0)
                modelStr = "Mesh/Earth.obj";

                m = CModel::LoadModel(modelStr, true);
            }

            if (m)
            {
                m->m_Id = b.Id;

                *m->GetPosition() = glm::vec3(b.PosX, 0.f, b.PosY);
            }
        }
    }
}

void CGame::ProcessSecTimer()
{
}

