#include "app.h"
#include <iostream>
#include <string>

void usage()
{
    std::cout << "rogosynth - SDL2 ImGui Synth.\n";
    std::cout << "          - by Roger Allen (rallen@gmail.com)\n";
    std::cout << "          - https://github.com/rogerallen/rogosynth\n";
    std::cout << "options:\n";
    std::cout << "  -h      - this message.\n";
}

int main(int argc, char *argv[])
{
    for (int i = 1; i < argc; ++i) {
        if (argv[i][0] == '-') {
            if (argv[i][1] == 'h') {
                usage();
                return 0;
            }
            // else if (argv[i][1] == 'd') {
            //    cudaDevice = std::stoi(argv[++i]);
            //}
            else {
                std::cerr << "ERROR: unknown option -" << argv[i][1]
                          << std::endl;
                usage();
                return 1;
            }
        }
    }
    App app;
    app.run(/* send options here */);
    return 0;
}
