#include "Tablut.h"
#include "MoveGenerator.h"
#include "Heuristic.h"
#include "SearchEngine.h"
#include <vector>
#include <chrono>

int main(int argc, char *argv[])
{
    Tablut t = Tablut::newGame();
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

    for (int i = 0; i < iterations; i++)
    {
        // NEGASCOUT --------------------
        std::cout << "########################" << std::endl;

        begin = std::chrono::steady_clock::now();
        t = searchEngine.NegaScoutSearch(t, max_depth);
        end = std::chrono::steady_clock::now();

        std::cout << "NEGASCOUT SCORE = " << searchEngine.score << std::endl;
        // PERFORMANCE _______________
        std::cout << searchEngine.transpositionTable << std::endl;

        totalTime += std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
        std::cout << "PERFORMANCE TIME-> difference = " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "[ms]" << std::endl;
        std::cout << "PERFORMANCE TIME MEDIUM-> avg = " << float(totalTime) / (i + 1) << "[ms]" << std::endl;

        searchEngine.transpositionTable.resetStat();

        t.print();

        if (t.isGameOver())
        {
            std::cout << "########################" << std::endl;
            std::cout << (t.isWinState() == WIN::BLACKWIN ? " BLACK WON " : " WHITE WON ") << std::endl;
            std::cout << "########################" << std::endl;
            break;
        }




        // NEGAMAX --------------------
        std::cout << "########################" << std::endl;

        begin = std::chrono::steady_clock::now();
        t = searchEngine2.NegaMaxSearch(t, max_depth, 1);
        end = std::chrono::steady_clock::now();

        std::cout << "NEGAMAX SCORE = " << searchEngine2.score << std::endl;
        // PERFORMANCE _______________
        std::cout << searchEngine2.transpositionTable << std::endl;

        totalTime2 += std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
        std::cout << "PERFORMANCE TIME-> difference = " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "[ms]" << std::endl;
        std::cout << "PERFORMANCE TIME MEDIUM-> avg = " << float(totalTime2) / (i + 1) << "[ms]" << std::endl;

        searchEngine2.transpositionTable.resetStat();

        t.print();

        if (t.isGameOver())
        {
            std::cout << "########################" << std::endl;
            std::cout << (t.isWinState() == WIN::BLACKWIN ? " BLACK WON " : " WHITE WON ") << std::endl;
            std::cout << "########################" << std::endl;
            break;
        }

        /*searchEngine.bestmove.print();

        begin = std::chrono::steady_clock::now();
        std::cout << "NEGAMAX SCORE: " << searchEngine2.NegaMax(t, max_depth, BOTTOM_SCORE, TOP_SCORE, 1) << std::endl;

        // PERFORMANCE _______________
        std::cout << "TOTAL WORKER: " << searchEngine2.totalThreads << std::endl;
        std::cout << "PERFORMANCE TT-> HITS:" << searchEngine2.transpositionTable._cacheHit << " ,PUTS:" << searchEngine2.transpositionTable._cachePut << ", MISS: " << searchEngine2.transpositionTable.cacheMiss() << std::endl;
        end = std::chrono::steady_clock::now();
        std::cout << "PERFORMANCE TIME-> difference = " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "[ms]" << std::endl;
        searchEngine2.bestmove.print();



        t = searchEngine.bestmove;

        std::cout << std::endl
                  << std::endl
                  << std::endl;

        std::cout << "-----------------------------------" << std::endl;
        t.print();
        std::cout << "-----------------------------------" << std::endl;*/
    }

    return EXIT_SUCCESS;
}