
#include "GA.hpp"
#include "Tablut.h"
#include "MoveGenerator.hpp"
#include "Heuristic.hpp"
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
        std::cout << (t.checkWinState() == GAME_STATE::BLACKWIN ? " BLACK WON " : (t.checkWinState() == GAME_STATE::WHITEWIN ? " WHITE WON " : " DRAW ")) << std::endl;
        std::cout << "########################" << std::endl;

        return true;
    }
    return false;
}

void customMatch()
{
    Tablut t = Tablut::getStartingPosition();
    Tablut other;
    std::vector<Tablut> moves{};

    int best_score;
    const int max_depth = 5;

    const int iterations = 300;

    std::chrono::steady_clock::time_point begin, end;

    int64_t totalTime = 0;
    int64_t totalTime2 = 0;

    Weights w1 = {-7, -68, -26, -12, -10, 103, -20, -77, -136, 33, 58, 61, -28, -20, -7, 42, 1, 18, -135, 15, 6, -3, -12, 34, 7, -10, -42, 86, 78, -65, -10, -58, -69, -12, -38, 39, 40, -79};
    Weights w2 = {-7, -68, -26, -12, -10, 103, -20, -77, -136, 33, 58, 61, -28, -20, -7, 42, 1, 18, -135, 15, 6, -3, -12, 34, 7, -10, -42, 86, 78, -65, -10, -58, -69, -12, -38, 39, 40, -79};

    SearchEngine searchEngine = SearchEngine(Heuristic(w1));
    SearchEngine searchEngine2 = SearchEngine(Heuristic(w2));

    std::srand(std::time(nullptr));

    for (int i = 0; i < iterations; i++)
    {

        // NEGASCOUT --------------------
        begin = std::chrono::steady_clock::now();
        t = searchEngine.NegaScoutSearch(t, max_depth);
        end = std::chrono::steady_clock::now();

        // GAME CICLE
        t.print();

        std::cout << " --> NEGASCOUT SCORE = " << searchEngine._bestScore << std::endl;
        // PERFORMANCE _______________
        std::cout << searchEngine._transpositionTable << std::endl;

        std::cout << "TOTAL MOVES CHECKED: " << searchEngine.getTotalMoves() << std::endl;
        totalTime += std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
        std::cout << "PERFORMANCE TIME-> difference = " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "[ms]" << std::endl;
        std::cout << "PERFORMANCE TIME MEDIUM-> avg = " << float(totalTime) / (i + 1) << "[ms]" << std::endl;

        searchEngine._transpositionTable.resetStat();

        // CHECK GAME_STATE
        if (checkWin(t))
            break;

        // NEGASCOUT --------------------
        begin = std::chrono::steady_clock::now();
        t = searchEngine2.NegaScoutSearch(t, max_depth);
        end = std::chrono::steady_clock::now();

        // GAME CICLE
        t.print();

        std::cout << " --> NEGASCOUT SCORE = " << searchEngine2._bestScore << std::endl;
        // PERFORMANCE _______________
        std::cout << searchEngine2._transpositionTable << std::endl;

        std::cout << "TOTAL MOVES CHECKED: " << searchEngine2.getTotalMoves() << std::endl;
        totalTime2 += std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
        std::cout << "PERFORMANCE TIME-> difference = " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "[ms]" << std::endl;
        std::cout << "PERFORMANCE TIME MEDIUM-> avg = " << float(totalTime2) / (i + 1) << "[ms]" << std::endl;

        searchEngine2._transpositionTable.resetStat();

        // CHECK GAME_STATE
        if (checkWin(t))
            break;

        /*
        // USER DEFINED PLAY ---------
        Pos fromX, fromY, toX, toY;

        std::cout << "Insert [from X] [from Y] [to X] [to Y]: ";
        std::cin >> fromX;
        std::cin >> fromY;
        std::cin >> toX;
        std::cin >> toY;

        t = t.next(fromX, fromY, toX, toY);

        // GAME CICLE
        t.print();

        // CHECK GAME_STATE
        if (checkWin(t))
            break;
        */

        /*
         // RFIRST BY HEURISTIC
         moveGene.generateLegalMoves(t, moves);
         _heuristic.sortMoves(moves);

         t = moves[0];
         moves = {};

         // GAME CICLE
         t.print();

         // CHECK GAME_STATE
         if (checkWin(t))
             break;
             */

        /*
        // NEGASCOUT --------------------
        begin = std::chrono::steady_clock::now();
        t = searchEngine.NegaScoutSearch(t, 7);
        end = std::chrono::steady_clock::now();

        // GAME CICLE
        t.print();

        std::cout << " --> NEGASCOUT SCORE = " << searchEngine._bestScore << std::endl;
        // PERFORMANCE _______________
        std::cout << searchEngine._transpositionTable << std::endl;

        std::cout << "TOTAL MOVES CHECKED: " << searchEngine.getTotalMoves() << std::endl;
        totalTime += std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
        std::cout << "PERFORMANCE TIME-> difference = " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "[ms]" << std::endl;
        std::cout << "PERFORMANCE TIME MEDIUM-> avg = " << float(totalTime) / (i + 1) << "[ms]" << std::endl;

        searchEngine._transpositionTable.resetStat();

        // CHECK GAME_STATE
        if (checkWin(t))
            break;

        std::cout << "-----------------------------------" << std::endl;
        std::cout << "-----------------------------------" << std::endl;
        */

        std::cout << " NEW ROUND ------------------------" << std::endl;
        std::cout << "-----------------------------------" << std::endl;
    }
}

int main(int argc, char *argv[])
{
    customMatch();
    /*
    while (true)
    {
        GA geneticAlgorithm = GA(7);
        geneticAlgorithm.start();
    }
    */

    return EXIT_SUCCESS;
}
