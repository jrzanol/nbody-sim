// by jrzanol
// 

#include <cstdio>
#include <cstdlib>
#include <clocale>

#include "rpc/server.h"

int main(int argc, const char* argv[])
{
    setlocale(LC_ALL, "");

    int objn = 100;

    printf("Defina a quantidade de objetos: ");
    (void)scanf("%d", &objn);

    printf("\nIniciando o servidor RPC... ");

	// Creating a server that listens on port 8080
	rpc::server srv(rpc::constants::DEFAULT_PORT);

    printf("Ok\nDefinindo as funções... ");

	/*
    srv.bind("get_mandelbrot", [&](int width, int height) {
        pixel_data data;
        for (int x = 0; x < width; x++) {
            for (int y = 0; y < height; y++) {
                double cr = to_real(x, width, minR, maxR);
                double ci = to_im(y, height, minI, maxI);
                int n = mandelbrot(cr, ci, maxN);

                unsigned char r = ((int)(fabs(n * cosf(n))) % 256);
                unsigned char g = ((n * 3) % 256);
                unsigned char b = (n % 256);

                data.push_back({r, g, b});
            }
        }

        return data;
    });
	*/

    printf("Ok\nEsperando clientes...\n");

    do
    {
        // Run the server loop.
        srv.async_run();

        Sleep(100);
    } while (true);

	return EXIT_SUCCESS;
}

