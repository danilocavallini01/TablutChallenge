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

    std::chrono::steady_clock::time_point begin;
    std::chrono::steady_clock::time_point end;

    SearchEngine searchEngine = SearchEngine();
    SearchEngine searchEngine2 = SearchEngine();

    for (int i = 0; i < 100; i++)
    {

        std::cout << "########################" << std::endl;

        begin = std::chrono::steady_clock::now();
        searchEngine.search(t, max_depth);
        std::cout << "NEGASCOUT SCORE = " << std::endl;

        // PERFORMANCE _______________
        std::cout << "TOTAL WORKER: " << searchEngine.totalThreads << std::endl;
        std::cout << "PERFORMANCE TT-> HITS:" << searchEngine.transpositionTable._cacheHit << " ,PUTS:" << searchEngine.transpositionTable._cachePut << ", MISS: " << searchEngine.transpositionTable.cacheMiss() << std::endl;
        end = std::chrono::steady_clock::now();
        std::cout << "PERFORMANCE TIME-> difference = " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "[ms]" << std::endl;
        /*searchEngine.bestmove.print();

        begin = std::chrono::steady_clock::now();
        std::cout << "NEGAMAX SCORE: " << searchEngine2.NegaMax(t, max_depth, BOTTOM_SCORE, TOP_SCORE, 1) << std::endl;

        // PERFORMANCE _______________
        std::cout << "TOTAL WORKER: " << searchEngine2.totalThreads << std::endl;
        std::cout << "PERFORMANCE TT-> HITS:" << searchEngine2.transpositionTable._cacheHit << " ,PUTS:" << searchEngine2.transpositionTable._cachePut << ", MISS: " << searchEngine2.transpositionTable.cacheMiss() << std::endl;
        end = std::chrono::steady_clock::now();
        std::cout << "PERFORMANCE TIME-> difference = " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "[ms]" << std::endl;
        searchEngine2.bestmove.print();

        if (t.isGameOver())
        {
            std::cout << "########################" << std::endl;
            std::cout << (t.isWinState() == WIN::BLACKWIN ? " BLACK WON " : " WHITE WON ") << std::endl;
            std::cout << "########################" << std::endl;
            break;
        }

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