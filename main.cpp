#include "Tablut.h"
#include "MoveGenerator.h"
#include "Heuristic.h"
#include "NegaScoutSearch.h"
#include <vector>

int main(int argc, char *argv[])
{
    Tablut t = Tablut::newGame();
    std::vector<Tablut> moves{};
    NegaScoutSearch searchEngine;
    Heuristic hh = Heuristic();
    int best_score;
    int max_depth = 7;

    for (int i = 0; i < 200; i++)
    {
        searchEngine = NegaScoutSearch();
        best_score = searchEngine.NegaScout(t, max_depth, BOTTOM_SCORE, TOP_SCORE);
        std::cout << "targetScore = " << best_score << std::endl;

        MoveGenerator::generateLegalMoves(t, moves);

        t = moves[0];

        for (int i = 0; i < moves.size(); i++)
        {
            std::cout << i << "->" << hh.evaluate(moves[i])<<"|";
            if (hh.evaluate(moves[i]) == best_score)
            {
                t = moves[i];
            }
        }

        std::cout << std::endl << "########################" << std::endl;
        t.print();
        std::cout << "########################" << std::endl;

        if (t.isGameOver())
        {
            std::cout << "########################" << std::endl;
            std::cout << (t.isWinState() == WIN::BLACKWIN ? " BLACK WON " : " WHITE WON ") << std::endl;
            std::cout << "########################" << std::endl;
            break;
        }

        moves = {};
    }

    return EXIT_SUCCESS;
}