#ifndef MOVEGENERATOR_H
#define MOVEGENERATOR_H

#include "Tablut.h"
#include "Zobrist.h"
#include <vector>
#include <cstdlib>

// Forward Declaration
class Tablut;
class Zobrist;

class MoveGenerator
{
private:
    Zobrist _zobrist = Zobrist();

public:
    MoveGenerator();
    ~MoveGenerator();

    // Generate all legal moves for a Tablut depending on the current _turn ( WHITE OR BLACK )
    void generateLegalMoves(Tablut &__t, std::vector<Tablut> &nextTabluts);

    // Generate all legal moves for a single Checker by scanning in all directions
    void getLegalMovesFrom(Tablut &__t, const Pos &__fromX, const Pos &__fromY, std::vector<Tablut> &nextTabluts);

    // Tell if a white checker can go in that location
    inline bool canWhiteContinue(Tablut &__t, const Pos &__toX, const Pos &__toY)
    {
        C targetChecker = __t._board[__toX][__toY];
        S targetStructure = tablutStructure[__toX][__toY];

        return targetChecker == C::EMPTY && targetStructure < 2U;
    }

    // Tell if a white checker can go in that location
    inline bool canBlackContinue(Tablut &__t, const Pos &__fromX, const Pos &__fromY, const Pos &__toX, const Pos &__toY)
    {
        C targetChecker = __t._board[__toX][__toY];
        S targetStructure = tablutStructure[__toX][__toY];

        // Check if move is made from a camp cell to another camp cells
        if (isInCamp(__fromX, __fromY) && targetStructure == S::CAMPS)
        {
            return targetChecker == C::EMPTY && tellIfCanPassCamp(__fromX, __fromY, __toX, __toY);
        }
        return targetChecker == C::EMPTY && targetStructure < 2U;
    }

    inline bool isInCamp(const Pos &__x, const Pos &__y)
    {
        return tablutStructure[__x][__y] == S::CAMPS;
    };

    // Dont check if camp is a legal move cause MoveGenerator already delete illegal moves
    inline bool tellIfCanPassCamp(const Pos &__fromX, const Pos &__fromY, const Pos &__toX, const Pos &__toY)
    {
        return abs(__fromX - __toX) <= 2 && abs(__fromY - __toY) <= 2;
    };
};

#endif