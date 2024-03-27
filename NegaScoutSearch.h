#ifndef NEGASCOUT
#define NEGASCOUT

#include "Tablut.h"
#include "Heuristic.h"

#include <limits>
#include <vector>

const int BOTTOM_SCORE(std::numeric_limits<int>::min());
const int TOP_SCORE(std::numeric_limits<int>::max());

class NegaScoutSearch
{
private:
public:
    Heuristic heuristic;
    int NegaScout(Tablut t, int depth, int alpha, int beta);

    NegaScoutSearch();
    ~NegaScoutSearch();

    NegaScoutSearch(Heuristic _heuristic)
    {
        heuristic = _heuristic;
    }
};

#endif