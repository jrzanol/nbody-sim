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
        m_Position = glm::vec3();
        m_Velocity = glm::vec3();

        m_Mass = 0.f;
    }

    int GetType() const { return m_Type; }
    float GetMass() const { return m_Mass; }
    glm::vec3 GetPosition() const { return m_Position; }
    glm::vec3 GetVelocity() const { return m_Velocity; }
    bool operator==(const CBody& b) { return m_Position == b.GetPosition(); }

    void UpdatePosition(glm::vec3 pos) { m_Position = pos; }
    void UpdateVelocity(glm::vec3 vel) { m_Velocity = vel; }

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
        float px, py, pz;
        float vx, vy, vz;

        if (sscanf(line, "%d %f %f %f %f %f %f %f", &type, &mass, &px, &py, &pz, &vx, &vy, &vz) == 8)
        {
            m_Type = type;
            m_Mass = mass;

            m_Position = glm::vec3(px, py, pz);
            m_Velocity = glm::vec3(vx, vy, vz);
            return true;
        }
        else
            return false;
    }

private:
    int m_Type;
    float m_Mass;

    glm::vec3 m_Position;
    glm::vec3 m_Velocity;
};

std::list<CBody> g_Body;

void Simulate()
{
    const float step = 1.f;

    for (CBody& b : g_Body)
    {
        glm::vec3 force = glm::vec3();

        for (const CBody& otherb : g_Body)
        {
            if (b == otherb)
                continue;

            force += b.GetAttraction(otherb);
        }

        b.UpdateVelocity(force / b.GetMass() * step);
        b.UpdatePosition(b.GetVelocity() * step);
    }
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
        NBODY bb;

        for (const CBody& body : g_Body)
        {
            BODY b;
            b.Type = body.GetType();
            b.PosX = body.GetPosition().x;
            b.PosY = body.GetPosition().y;
            b.PosZ = body.GetPosition().z;

            bb.push_back(b);
        }

        return bb;
    });

    printf("Ok\nEsperando clientes...\n");

    do
    {
        // Run the server loop.
        srv.async_run();

        Simulate();
        Sleep(100);
    } while (true);

	return EXIT_SUCCESS;
}

