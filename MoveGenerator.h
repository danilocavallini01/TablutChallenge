#pragma once

#ifndef MOVEGENERATOR_H
#define MOVEGENERATOR_H

#include "Tablut.h"
#include "Zobrist.h"
#include <vector>
#include <cstdlib>

class MoveGenerator
{
private:
    Zobrist zobrist = Zobrist();
public:
    MoveGenerator();
    ~MoveGenerator();

    // Generate all legal moves for a Tablut depending on the current turn ( WHITE OR BLACK )
    void generateLegalMoves(Tablut &t, std::vector<Tablut> &nextTabluts);

    // Generate all legal moves for a single Checker by scanning in all directions
    void getLegalMovesFrom(Tablut &t, const Pos from_x, const Pos from_y, std::vector<Tablut> &nextTabluts);

    inline bool canWhiteContinue(Tablut &t, const Pos to_x, const Pos to_y)
    {
        C targetChecker;
        S targetStructure;

        targetChecker = t.board[to_x][to_y];
        targetStructure = tablutStructure[to_x][to_y];

        return targetChecker == 0U && targetStructure < 2U;
    }

    inline bool canBlackContinue(Tablut &t, const Pos from_x, const Pos from_y, const Pos to_x, const Pos to_y)
    {
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

    inline bool isInCamp(const Pos x, const Pos y)
    {
        return tablutStructure[x][y] == S::CAMPS;
    };

    // Dont check if camp is a legal move cause MoveGenerator already delete illegal moves
    inline bool tellIfCanPassCamp(const Pos from_x, const Pos from_y, const Pos to_x, const Pos to_y)
    {
        return abs(from_x - to_x) <= 2 && abs(from_y - to_y) <= 2;
    };
};

#endif