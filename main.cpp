#include "Tablut.h"
#include "MoveGenerator.h"
#include "Heuristic.h"
#include "SearchEngine.h"
#include <vector>
#include <chrono>
#include <cstdlib>
#include <ctime>

bool checkWin(Tablut &t)
{
    if (t.isGameOver())
    {
        std::cout << "########################" << std::endl;
        std::cout << (t.checkWinState() == WIN::BLACKWIN ? " BLACK WON " : (t.checkWinState() == WIN::WHITEWIN ? " WHITE WON " : " DRAW ")) << std::endl;
        std::cout << "########################" << std::endl;

        return true;
    }
    return false;
}

int main(int argc, char *argv[])
{
    Tablut t = Tablut::newGame();
    Tablut other;
    std::vector<Tablut> moves{};

    int best_score;
    const int max_depth = 7;

    const int iterations = 300;

    std::chrono::steady_clock::time_point begin;
    std::chrono::steady_clock::time_point end;

    int64_t totalTime = 0;
    int64_t totalTime2 = 0;

    SearchEngine searchEngine = SearchEngine();
    SearchEngine searchEngine2 = SearchEngine();

    MoveGenerator moveGene = MoveGenerator();
    Heuristic heuristic = Heuristic();

    std::srand(std::time(nullptr));

    for (int i = 0; i < iterations; i++)
    {
        /*
        // RFIRST BY HEURISTIC
        moveGene.generateLegalMoves(t, moves);
        heuristic.sortMoves(moves);

        t = moves[0];
        moves = {};

        // GAME CICLE
        t.print();

        // CHECK WIN
        if (checkWin(t))
            break;
            */

        // NEGASCOUT --------------------
        begin = std::chrono::steady_clock::now();
        t = searchEngine.NegaScoutSearch(t, max_depth);
        end = std::chrono::steady_clock::now();

        // GAME CICLE
        t.print();

        std::cout << " --> NEGASCOUT SCORE = " << searchEngine.score << std::endl;
        // PERFORMANCE _______________
        std::cout << searchEngine.transpositionTable << std::endl;

        totalTime += std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
        std::cout << "PERFORMANCE TIME-> difference = " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "[ms]" << std::endl;
        std::cout << "PERFORMANCE TIME MEDIUM-> avg = " << float(totalTime) / (i + 1) << "[ms]" << std::endl;

        searchEngine.transpositionTable.resetStat();

        // CHECK WIN
        if (checkWin(t))
            break;

        std::cout << "-----------------------------------" << std::endl;
        std::cout << "-----------------------------------" << std::endl;


        // NEGAMAX --------------------
        begin = std::chrono::steady_clock::now();
        t = searchEngine2.NegaMaxSearch(t, 3);
        end = std::chrono::steady_clock::now();

        // GAME CICLE
        t.print();

        std::cout << " --> NEGAMAX SCORE = " << searchEngine2.score << std::endl;
        // PERFORMANCE _______________
        std::cout << searchEngine2.transpositionTable << std::endl;

        totalTime2 += std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
        std::cout << "PERFORMANCE TIME-> difference = " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "[ms]" << std::endl;
        std::cout << "PERFORMANCE TIME MEDIUM-> avg = " << float(totalTime2) / (i + 1) << "[ms]" << std::endl;

        searchEngine2.transpositionTable.resetStat();

         // CHECK WIN
        if (checkWin(t))
            break;

        std::cout << "-----------------------------------" << std::endl;
        std::cout << "-----------------------------------" << std::endl;
    }

    return EXIT_SUCCESS;
}