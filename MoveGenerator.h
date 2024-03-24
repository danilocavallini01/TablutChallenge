#ifndef MOVEGENERATOR_H
#define MOVEGENERATOR_H

#include "Tablut.h"
#include <vector>
#include <cstdlib>

class MoveGenerator
{
public:
    MoveGenerator();
    ~MoveGenerator();

    // Generate all legal moves for a Tablut depending on the current turn ( WHITE OR BLACK )
    static void generateLegalMoves(Tablut &t, std::vector<Tablut> &res);

    // Generate all legal moves for a single Checker by scanning in all directions
    static void getLegalMovesFrom(Tablut &t, const Pos from_x, const Pos from_y, std::vector<Tablut> &res);

    static inline bool canWhiteContinue(Tablut &t, const Pos to_x, const Pos to_y)
    {

        if (to_x < 0 || to_x > 8 || to_y < 0 || to_y > 8)
        {
            return false;
        }

        C targetChecker;
        S targetStructure;

        targetChecker = t.board[to_x][to_y];
        targetStructure = tablutStructure[to_x][to_y];

        return targetChecker == 0U && targetStructure < 2U;
    }

    static inline bool canBlackContinue(Tablut &t, const Pos from_x, const Pos from_y, const Pos to_x, const Pos to_y)
    {

        if (to_x < 0 || to_x > 8 || to_y < 0 || to_y > 8)
        {
            return false;
        }

        C targetChecker;
        S targetStructure;

        targetChecker = t.board[to_x][to_y];
        targetStructure = tablutStructure[to_x][to_y];

        // Check if move is made from a camp cell to another camp cells
        if (isInCamp(from_x, from_y) && targetStructure == S::CAMPS)
        {
            return targetChecker == 0U && tellIfCanPassCamp(from_x, from_y, to_x, to_y);
        }
        return targetChecker == 0U && targetStructure < 2U;
    }

    static inline bool isInCamp(const Pos x, const Pos y)
    {
        return tablutStructure[x][y] == S::CAMPS;
    };

    // Dont check if camp is a legal move cause MoveGenerator already delete illegal moves
    static inline bool tellIfCanPassCamp(const Pos from_x, const Pos from_y, const Pos to_x, const Pos to_y)
    {
        return abs(from_x - to_x) <= 2 && abs(from_y - to_y) <= 2;
    };
};

#endif

/*

    static inline void getNearCampCells(const Pos from_x, const Pos from_y, std::array<std::array<Pos, 2>, 3> &nearCampCells)
    {

        u_int8_t index = 0;

        // CHECKING 2 CAMPS TOP
        if (from_x != 0U && tablutStructure[from_x - 1][from_y] == S::CAMPS)
        {
            nearCampCells[index++] = {from_x - 1, from_y};

             *
             *  SITUATION so C3 can be ignored cause black checker can't reach that one
             *  C2
             *  C1 C3
             *  C(starting)
             *
            if (from_x > 1U && tablutStructure[from_x - 2][from_y] == S::CAMPS)
            {
                nearCampCells[index++] = {from_x - 2, from_y};
                return;
            }
        }

        // CHECKING 2 CAMPS BOT
        if (from_x != 8U && tablutStructure[from_x + 1][from_y] == S::CAMPS)
        {
            nearCampCells[index++] = {from_x + 1, from_y};

             *
             *  SITUATION so C3 can be ignored cause black checker can't reach that one
             *  C(starting)
             *  C1 C3
             *  C2
             *
            if (from_x < 7U && tablutStructure[from_x + 2][from_y] == S::CAMPS)
            {
                nearCampCells[index++] = {from_x + 2, from_y};
                return;
            }
        }
    }
    */