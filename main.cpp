
#include "Lib/GeneticAlgorithm/GA.hpp"
#include "Lib/Connectors/Player.hpp"
#include "Lib/Model/ResourceAllocator.hpp"

#include <vector>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <emscripten/emscripten.h>

#ifdef __cplusplus
#define EXTERN extern "C"
#else
#define EXTERN
#endif

using namespace Connectors;

const Weights _white = {247, -297, 114, -160, 260, 116, 21, 115, 64, 102, 67, -15, 116, 26, 44, 7, -6, 27, -27, -84, -39, 38, 18, -44, -23, -17, -24, 34, 62, 48, -29, 53, -69, -19, -14, -14, -27, 63, -15, 5};
const Weights _black = {299, -160, 191, -213, 260, 61, 57, 38, 53, -82, 38, 27, 12, 71, -31, -46, 19, -52, 57, -79, 26, -2, 1, 43, 22, -54, -38, 24, 69, 85, 7, -41, -24, -18, 18, -68, -83, -81, -1, 25};

bool _checkWin(Tablut &__position)
{
    __position.checkWinState();
    if (__position.isGameOver())
    {
        std::cout << "########################" << std::endl;
        std::cout << (__position.checkWinState() == GAME_STATE::BLACKWIN ? " BLACK WON " : (__position.checkWinState() == GAME_STATE::WHITEWIN ? " WHITE WON " : " DRAW ")) << std::endl;
        std::cout << "########################" << std::endl;

        return true;
    }
    return false;
}

void printStats(TNegaScoutEngine &__engine, std::chrono::steady_clock::time_point &__timeBegin, std::chrono::steady_clock::time_point &__timeEnd, int __totalTime, int __cicles)
{
    __engine.print();

    std::cout << "PERFORMANCE TIME-> difference = " << std::chrono::duration_cast<std::chrono::milliseconds>(__timeEnd - __timeBegin).count() << "[ms]" << std::endl;
    std::cout << "PERFORMANCE TIME MEDIUM-> avg = " << float(__totalTime) / (__cicles + 1) << "[ms]" << std::endl;
}

int main(int argc, char **argv)
{
    return 0;
}

EXTERN EMSCRIPTEN_KEEPALIVE void computeMove()
{
    printf("ciao\n");
}

extern "C"
{
    void calculateMove(const int *__board)
    {
        Tablut current = Tablut::fromIntArray(__board);
        current.print();
    }
}

/*
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
}*/

/*
int main(int argc, char *argv[])
{

    int _maxDepth = 7;
    int _maxQDepth = 2;
    int _maxIterations = 200;
    int _threads = MAX_THREADS - 1;

    Tablut gameBoard = Tablut::getStartingPosition();

    // Zobrist hash instance
    Zobrist hasher = Zobrist();

    // Setup new search engine with the given weights heuristic
    TNegaScoutEngine searchEngineWhite = TNegaScoutEngine(_maxDepth, _maxQDepth, Heuristic(_white), hasher);
    TNegaScoutEngine searchEngineBlack = TNegaScoutEngine(_maxDepth, _maxQDepth, Heuristic(_black), hasher);

    for (int n : _white)
        std::cout << n << ' ';
    std::cout << std::endl;

    for (int n : _black)
        std::cout << n << ' ';
    std::cout << std::endl;

    int totalWhiteScore = 0;
    int totalBlackScore = 0;

    int64_t totalTimeWhite = 0;
    int64_t totalTimeBlack = 0;

    std::chrono::steady_clock::time_point timeBegin;
    std::chrono::steady_clock::time_point timeEnd;

    StopWatch timerWhite = StopWatch(20000);
    StopWatch timerBlack = StopWatch(20000);

    int i;

    hasher.addHash(gameBoard);

    for (i = 0; i < _maxIterations / 2; i++)
    {
        // WHITE ONE --------------------
        // NEGASCOUT --------------------

        timerWhite.start();
        timeBegin = std::chrono::steady_clock::now();
        gameBoard = searchEngineWhite.IterativeDeepening(gameBoard, timerWhite, 4, _threads);
        timeEnd = std::chrono::steady_clock::now();
        timerWhite.reset();

        // STAT ------------------

        totalWhiteScore += searchEngineWhite.getBestScore();
        totalTimeWhite += std::chrono::duration_cast<std::chrono::milliseconds>(timeEnd - timeBegin).count();

        if (_verbose)
        {
            gameBoard.print();
            printStats(searchEngineWhite, timeBegin, timeEnd, totalTimeWhite, i);
        }

        // CHECK GAME_STATE
        if (_checkWin(gameBoard))
        {
            break;
        }

        // BLACK ONE --------------------
        // NEGASCOUT --------------------

        timerBlack.start();
        timeBegin = std::chrono::steady_clock::now();
        gameBoard = searchEngineBlack.IterativeDeepening(gameBoard, timerBlack, 4, _threads);
        timeEnd = std::chrono::steady_clock::now();
        timerBlack.reset();

        // STAT ------------------

        totalBlackScore += searchEngineBlack.getBestScore();
        totalTimeBlack += std::chrono::duration_cast<std::chrono::milliseconds>(timeEnd - timeBegin).count();

        if (_verbose)
        {
            gameBoard.print();
            printStats(searchEngineBlack, timeBegin, timeEnd, totalTimeBlack, i);
        }

        // CHECK GAME_STATE
        if (_checkWin(gameBoard))
        {
            break;
        }

        if (_verbose)
        {
            std::cout << "- NEW ROUND ------------------------" << std::endl;
            std::cout << "-----------------------------------" << std::endl;
        }
    }
}
*/