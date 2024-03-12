// by jrzanol
// 

#include <cstdio>
#include <cstdlib>
#include <clocale>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "..\nbody.h"
#include "rpc/server.h"

const float G = 9.80665f;

class CBody
{
public:
    CBody()
    {
        m_Id = 0;
        m_Type = 0;
        m_Mass = 0.f;

        m_Position = glm::vec3();
        m_Velocity = glm::vec3();
    }

    int GetId() const { return m_Id; }
    int GetType() const { return m_Type; }
    float GetMass() const { return m_Mass; }
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
        int type;
        float mass;
        float px, py;
        float vx, vy;

        if (sscanf(line, "%d %f %f %f %f %f", &type, &mass, &px, &py, &vx, &vy) == 6)
        {
            m_Id = g_BodyIdCounter++;
            m_Type = type;
            m_Mass = mass;

            m_Position = glm::vec3(px, py, 0.f);
            m_Velocity = glm::vec3(vx, vy, 0.f);
            return true;
        }
        else
            return false;
    }

private:
    int m_Id;
    int m_Type;
    float m_Mass;

    glm::vec3 m_Position;
    glm::vec3 m_Velocity;

    static int g_BodyIdCounter;
};

int CBody::g_BodyIdCounter = 1;
std::list<CBody> g_Body;
NBODY g_BodyList;

void Simulate()
{
    const float step = 0.02f;

    for (CBody& b : g_Body)
    {
        glm::vec3 force = glm::vec3();

        for (const CBody& otherb : g_Body)
        {
            if (b == otherb)
                continue;

            glm::vec3 attr = b.GetAttraction(otherb);

            force += attr;
        }

        float mass = b.GetMass();
        glm::vec3 vel = force / b.GetMass();

        b.UpdateVelocity(vel * step);
        b.UpdatePosition(b.GetVelocity() * step);
    }

    for (CBody& b : g_Body)
        printf("Body #%d Pos (%.2f,%.2f,%.2f)\n", b.GetId(), b.GetPosition().x, b.GetPosition().y, b.GetPosition().z);
}

int main(int argc, const char* argv[])
{
    setlocale(LC_ALL, "");
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

    printf("Ok\nIniciando o servidor RPC... ");

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

            g_BodyList.push_back(b);
        }

        return g_BodyList;
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

