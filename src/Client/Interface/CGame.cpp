// by jrzanol
//

#include "stdafx.h"
#include "CWindow.h"
#include "CGame.h"

#include "..\nbody.h"

rpc::client* CGame::m_Rpc = NULL;

int CGame::m_BodyNameSize = 0;
char* CGame::m_BodyName[32] = { NULL };

CGame::CGame() : CEvent()
{
}

CGame::~CGame()
{
}

void CGame::Initialize()
{
    // Load Stars.
    for (int i = 0; i < 256; ++i)
    {
        CModel* m = CModel::LoadModel("Mesh/Star.obj", true);
        if (m)
        {
            float x = (float)((rand() % 100) - 50);
            float z = (float)((rand() % 100) - 50);

            m->m_Scale = glm::vec3(0.03f, 0.f, 0.05f);
            *m->GetPosition() = glm::vec3(x, -(rand() % 10) - 1.f, z);
        }
    }

    m_Rpc = new rpc::client("127.0.0.1", 8080);
    CWindow::m_Step = m_Rpc->call("getStep").as<float>();

    ProcessMiliSecTimer();

    for (int Id = 1; Id < 32; ++Id)
    {
        CModel* m = CModel::GetModelFromId(Id);
        if (m == NULL)
            break;

        m_BodyName[m_BodyNameSize] = m->m_Name;
        m_BodyNameSize++;
    }
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
            CModel* m = CModel::GetModelFromId(b.Id);
            if (m == NULL)
            {
                const char* modelStr = NULL;

                if (b.Type == 1)
                    modelStr = "Mesh/Sun.obj";
                else if (b.Type == 2)
                    modelStr = "Mesh/Mercurio.obj";
                else if (b.Type == 3)
                    modelStr = "Mesh/Earth.obj";
                else
                    modelStr = "Mesh/Mars.obj";

                m = CModel::LoadModel(modelStr, true);

                if (m)
                {
                    strcpy(m->m_Name, b.Name);
                    m->m_Mass = b.Mass;

                    if (b.Type == 2)
                        m->m_Scale *= 0.5f;
                    else if (b.Type == 3)
                        m->m_Scale *= 0.7f;
                    else if (b.Type == 4)
                        m->m_Scale *= 0.8f;
                }
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
    CWindow::m_Step = m_Rpc->call("getStep").as<float>();
}

