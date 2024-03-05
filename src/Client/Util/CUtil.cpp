// by jrzanol
//

#include "stdafx.h"
#include "CWindow.h"

GLFWwindow* g_Window = NULL;

unsigned int g_FPS = 0;

float g_DeltaTime = 0;
float g_LastTime = 0;

int CUtil::g_EnemyCount = 0;

const char* CUtil::m_VertexShader = R"glsl(
    #version 430

    layout(location = 0) in vec3 a_position;
    layout(location = 1) in vec3 a_normals;
    layout(location = 2) in vec2 a_texcoords;
    layout(location = 3) in vec3 a_tangent;
    layout(location = 4) in vec3 a_bitangent;
    
    out vec2 TexCoords;
    out vec3 FragPos;  
    out vec3 Normal;
    
    uniform mat4 u_model;
    uniform mat4 u_vp;
    
    void main() {
        TexCoords = a_texcoords;
        FragPos = vec3(u_model * vec4(a_position, 1.0));
        Normal = a_normals;

        gl_Position = (u_vp * u_model * vec4(a_position, 1.0));
    }
)glsl";

const char* CUtil::m_FragmentShader = R"glsl(
    #version 430
    
    out vec4 FragColor;
    in vec2 TexCoords;
    in vec3 FragPos;
    in vec3 Normal;

    uniform sampler2D texture_diffuse1;
    uniform sampler2D texture_specular1;
    uniform sampler2D texture_normal1;
    uniform int u_star;

    uniform vec3 lightPos;
    uniform vec3 lightPos2;
    uniform vec3 lightPos3;
    uniform vec3 lightColor;
    uniform vec3 lightColor2;
    uniform vec3 lightColor3;
    
    void main() {
        if (u_star != 0) {
            FragColor = vec4(1, 1, 1, 0);
        }
        else {
            // Object Color:
            vec2 newTexCoords;
            newTexCoords.x = (TexCoords.x);
            newTexCoords.y = (TexCoords.y);
            
            vec4 objectColor = mix(texture(texture_diffuse1, newTexCoords), texture(texture_specular1, newTexCoords), texture(texture_normal1, newTexCoords));
            
            // Ambient Color:
            FragColor = vec4(1, 1, 1, 1) * objectColor;
        }
    }
)glsl";

