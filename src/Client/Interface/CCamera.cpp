// by jrzanol
//

#include "stdafx.h"

#include "CUtil.h"
#include "CModel.h"
#include "CCamera.h"

CCamera::CCamera()
{
    m_Position = glm::vec3(3.50f, 60.f, 7.5f);
    m_WorldUp = glm::vec3(0.0f, 1.0f, 0.0f);
    m_Yaw = -90.f;
    m_Pitch = -71.4f;
    m_Zoom = 45.f;

    UpdateCameraVectors();
}

CCamera::~CCamera()
{
}

void CCamera::ProcessSecTimer()
{
#ifdef _DEBUG
    static time_t s_LastShowCamPosition = 0;
    time_t rawnow = time(0);

    if (s_LastShowCamPosition < rawnow)
    {
        printf("Pos %.2f,%.2f,%.2f\n", m_Position.x, m_Position.y, m_Position.z);
        s_LastShowCamPosition = rawnow + 5;
    }

    //printf("Pos %.2f,%.2f,%.2f WorldUp %.2f,%.2f,%.2f Yan %.2f Pitch %.2f Zoom %.2f\n", m_Position.x, m_Position.y, m_Position.z, m_WorldUp.x, m_WorldUp.y, m_WorldUp.z, m_Yaw, m_Pitch, m_Zoom);
#endif
}

void CCamera::ProcessMouseDragEvent(GLFWwindow* window, float xoffset, float yoffset)
{
    xoffset *= 0.1f;
    yoffset *= 0.1f;

    m_Yaw += xoffset;
    m_Pitch += yoffset;

    // make sure that when pitch is out of bounds, screen doesn't get flipped
    if (m_Pitch > 89.0f)
        m_Pitch = 89.0f;
    else if (m_Pitch < -89.0f)
        m_Pitch = -89.0f;

    // update Front, Right and Up Vectors using the updated Euler angles
    UpdateCameraVectors();
}

void CCamera::ProcessMouseScroll(GLFWwindow* window, double _xoffset, double _yoffset)
{
    float yoffset = static_cast<float>(_yoffset);

    m_Zoom -= (float)yoffset;

    if (m_Zoom < 1.0f)
        m_Zoom = 1.0f;
    else if (m_Zoom > 45.0f)
        m_Zoom = 45.0f;
}

void CCamera::ProcessInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        ProcessKeyboard(Camera_Movement::FORWARD, g_DeltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        ProcessKeyboard(Camera_Movement::BACKWARD, g_DeltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        ProcessKeyboard(Camera_Movement::LEFT, g_DeltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        ProcessKeyboard(Camera_Movement::RIGHT, g_DeltaTime);
}

glm::mat4 CCamera::GetViewMatrix()
{
    return glm::lookAt(m_Position, m_Position + m_Front, m_Up);
}

void CCamera::ProcessKeyboard(Camera_Movement direction, float deltaTime)
{
    float velocity = (7.5f * deltaTime);

    if (direction == Camera_Movement::FORWARD)
        m_Position += m_Front * velocity;
    else if (direction == Camera_Movement::BACKWARD)
        m_Position -= m_Front * velocity;
    else if (direction == Camera_Movement::LEFT)
        m_Position -= m_Right * velocity;
    else if (direction == Camera_Movement::RIGHT)
        m_Position += m_Right * velocity;
}

void CCamera::UpdateCameraVectors()
{
    // Calculate the new Front vector.
    glm::vec3 front;
    front.x = cos(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
    front.y = sin(glm::radians(m_Pitch));
    front.z = sin(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));

    m_Front = glm::normalize(front);
    float angle = 180.f/atan2f(m_Front.y, m_Front.x);

    // also re-calculate the Right and Up vector
    m_Right = glm::normalize(glm::cross(m_Front, m_WorldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
    m_Up = glm::normalize(glm::cross(m_Right, m_Front));
}

