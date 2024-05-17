
#include "GA.hpp"
#include "Connectors/Player.hpp"
#include "Model/ResourceAllocator.hpp"

#include <vector>
#include <chrono>
#include <cstdlib>
#include <ctime>

using namespace Connectors;

int main(int argc, char *argv[])
{
    // ARGUMENT PARSING
    std::string ip, color, timeout;

    ip = "127.0.0.1";
    color = "WHITE";
    timeout = "60";

    std::cout << "USAGE: main [color=\"WHITE\"] [timeout=\"60\"] [ip=\"127.0.0.1\"] " << std::endl;

    if (argc > 1)
    {
        color = std::string(argv[1]);
    }

    if (argc > 2)
    {
        timeout = std::string(argv[2]);
    }

    if (argc > 3)
    {
        ip = std::string(argv[3]);
    }

    std::cout << "TOTAL ARGS GIVEN " << argc - 1 << ", DEFAULTED " << 4 - argc << " ARGS" << std::endl;

    std::cout << std::endl;
    std::cout << "-CONFIGURATION LOADED----------------------" << std::endl;
    std::cout << "\tcolor: " << color << std::endl;
    std::cout << "\ttimeout: " << timeout << std::endl;
    std::cout << "\tip: " << ip << std::endl;
    std::cout << "-------------------------------------------" << std::endl;
    std::cout << std::endl;

    ResourceAllocator allocator{};
    allocator.increaseStackSize();

    Player p{Player::of(ip, color, timeout)};

    p.play();

    return EXIT_SUCCESS;
}
