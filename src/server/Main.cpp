// by jrzanol
// 

#include <omp.h>
#include <cstdio>
#include <cstdlib>
#include <clocale>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "..\nbody.h"
#include "rpc/server.h"

const float G = 9.80665f;
float g_Step = DEFAULT_STEP;
bool g_Init = false;

class CBody
{
public:
    CBody()
    {
        m_Id = 0;
        m_Type = 0;
        m_Mass = 0.f;
        m_Name[0] = 0;

        m_Position = glm::vec3();
        m_Velocity = glm::vec3();
    }

    int GetId() const { return m_Id; }
    int GetType() const { return m_Type; }
    float GetMass() const { return m_Mass; }
    void SetMass(float newm) { m_Mass = newm; }
    const char* GetName() const { return m_Name; }
    glm::vec3 GetPosition() const { return m_Position; }
    glm::vec3 GetVelocity() const { return m_Velocity; }
    bool operator==(const CBody& b) { return m_Position == b.GetPosition(); }

    void UpdatePosition(glm::vec3 pos) { m_Position -= pos; }
    void UpdateVelocity(glm::vec3 vel) { m_Velocity += vel; }

    glm::vec3 GetAttraction(const CBody& otherBody) const
    {
        // Calcula a distância:
        glm::vec3 distance = m_Position - otherBody.GetPosition();
        double d = std::sqrt(std::pow(distance.x, 2) + std::pow(distance.y, 2));

        // Calcula a Força de Atração:
        double force = G * m_Mass * otherBody.GetMass() / std::pow(d, 2);

        // Calcula a Força de Direção:
        double theta = std::atan2(distance.y, distance.x);
        return glm::vec3(std::cos(theta) * force, std::sin(theta) * force, 0.f);
    }

    bool ParseLine(const char* line)
    {
        char name[12];
        int type;
        float mass;
        float px, py;
        float vx, vy;

        if (sscanf(line, "%11s %d %f %f %f %f %f", name, &type, &mass, &px, &py, &vx, &vy) == 7)
        {
            m_Id = g_BodyIdCounter++;
            m_Type = type;
            m_Mass = mass;
            strcpy(m_Name, name);

            m_Position = glm::vec3(px, py, 0.f);
            m_Velocity = glm::vec3(vx, vy, 0.f);
            return true;
        }
        else
            return false;
    }

    static int g_BodyIdCounter;

private:
    int m_Id;
    int m_Type;
    float m_Mass;
    char m_Name[12];

    glm::vec3 m_Position;
    glm::vec3 m_Velocity;
};

int CBody::g_BodyIdCounter = 1;
std::vector<CBody> g_Body;
NBODY g_BodyList;

void Simulate()
{
    if (!g_Init)
        return;

    for (CBody& b : g_Body)
    {
        float x = 0.f;
        float y = 0.f;

        unsigned int i;
        const unsigned int CurId = b.GetId();

#pragma omp parallel for private(i) reduction(+:x, y)
        for (i = 0; i < g_Body.size(); ++i)
        {
            if (CurId == g_Body[i].GetId())
                continue;

            glm::vec3 attr = b.GetAttraction(g_Body[i]);

            x += attr.x;
            y += attr.y;
        }

        glm::vec3 force = glm::vec3(x, y, 0.f);

        float mass = b.GetMass();
        glm::vec3 vel = force / b.GetMass();

        b.UpdateVelocity(vel * g_Step);
        b.UpdatePosition(b.GetVelocity() * g_Step);
    }

    //for (CBody& b : g_Body)
    //    printf("Body #%d Pos (%.2f,%.2f,%.2f)\n", b.GetId(), b.GetPosition().x, b.GetPosition().y, b.GetPosition().z);
}
void Init(bool reload = false)
{
    g_Step = DEFAULT_STEP;
    g_Body.clear();

    CBody::g_BodyIdCounter = 1;

    system("cls");
    printf("Lendo o arquivo... ");

    FILE* in = fopen("Server.csv", "rt");
    if (in)
    {
        char line[64];

        while (fgets(line, sizeof(line), in))
        {
            char* p;
            while ((p = strchr(line, ',')) != NULL)
                *p = ' ';

            CBody b;

            if (b.ParseLine(line))
                g_Body.push_back(b);
        }

        fclose(in);
    }

    printf("Ok\n");

    if (reload)
        printf("RPC online. Servidor reiniciado... \n");
}

int main(int argc, const char* argv[])
{
    setlocale(LC_ALL, "");
    
    Init();
    printf("Iniciando o servidor RPC... ");

	// Creating a server that listens on port 8080
	rpc::server srv(rpc::constants::DEFAULT_PORT);

    printf("Ok\nDefinindo as funções... ");

    srv.bind("get", [&]() {
        g_BodyList.clear();

        for (const CBody& body : g_Body)
        {
            BODY b;
            b.Id = body.GetId();
            b.Type = body.GetType();
            b.PosX = body.GetPosition().x;
            b.PosY = body.GetPosition().y;
            b.PosZ = body.GetPosition().z;
            b.Mass = body.GetMass();
            strcpy(b.Name, body.GetName());

            g_BodyList.push_back(b);
        }

        return g_BodyList;
    });

    srv.bind("init", [&]() { g_Init = true; });
    srv.bind("reload", [&]() { Init(true); });
    srv.bind("getStep", [&]() { return g_Step; });
    srv.bind("setStep", [&](float newStep) { g_Step = newStep; });
    srv.bind("setMass", [&](int Id, float newMass) {
        for (CBody& body : g_Body)
        {
            if (body.GetId() == Id)
            {
                body.SetMass(newMass);
                break;
            }
        }
    });

    printf("Ok\nEsperando clientes...\n");

    // Run the server loop.
    srv.async_run();

    do
    {
        Simulate();
        Sleep(10);
    } while (true);

	return EXIT_SUCCESS;
}

