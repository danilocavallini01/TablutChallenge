#ifndef MOVE_GENERATOR
#define MOVE_GENERATOR

#include <vector>

template <class GameState>
class IMoveGenerator
{
public:
    virtual ~IMoveGenerator(){};
    static void generateLegalMoves(GameState &__g, std::vector<GameState> &__nexts) {};
};

#endif