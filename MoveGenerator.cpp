#include "MoveGenerator.h"

MoveGenerator::MoveGenerator() {}
MoveGenerator::~MoveGenerator() {}

void MoveGenerator::generateLegalMoves(Tablut &t, std::vector<Tablut> &res)
{
    if (t.isWhiteTurn)
    {
        // GENERATE MOVES FOR WHITE PIECES IF BLACK CHECKERS ENCOUTERED
        for (Pos x = 0; x < 9; x++)
        {
            for (Pos y = 0; y < 9; y++)
            {
                if (t.board[x][y] == C::WHITE)
                {
                    MoveGenerator::getLegalMovesFrom(t, x, y, res);
                }
            }
        }

        // GENERATE MOVES FOR KING
        MoveGenerator::getLegalMovesFrom(t, t.kingX, t.kingY, res);
    }
    else
    {
        // GENERATE MOVES FOR BLACK PIECES IF BLACK CHECKERS ENCOTERED
        for (Pos x = 0; x < 9; x++)
        {
            for (Pos y = 0; y < 9; y++)
            {
                if (t.board[x][y] == C::BLACK)
                {
                    MoveGenerator::getLegalMovesFrom(t, x, y, res);
                }
            }
        }
    }
};

void MoveGenerator::getLegalMovesFrom(Tablut &t, Pos from_x, const Pos from_y, std::vector<Tablut> &res)
{
    // ------------ WHITE LEGAL MOVES ------------
    if (t.isWhiteTurn)
    {
        Pos epsilon = 1;

        // Left solutions
        while (MoveGenerator::canWhiteContinue(t, from_x, from_y - epsilon))
        {
            res.push_back(t.next(from_x, from_y, from_x, from_y - epsilon));
            epsilon++;
        }

        epsilon = 1;
        // Right solutions
        while (MoveGenerator::canWhiteContinue(t, from_x, from_y + epsilon))
        {
            res.push_back(t.next(from_x, from_y, from_x, from_y + epsilon));
            epsilon++;
        }

        epsilon = 1;
        // Up solutions
        while (MoveGenerator::canWhiteContinue(t, from_x - epsilon, from_y))
        {
            res.push_back(t.next(from_x, from_y, from_x - epsilon, from_y));
            epsilon++;
        }

        epsilon = 1;
        // Down solutions
        while (MoveGenerator::canWhiteContinue(t, from_x + epsilon, from_y))
        {
            res.push_back(t.next(from_x, from_y, from_x + epsilon, from_y));
            epsilon++;
        }

        // ------------ BLACK LEGAL MOVES ------------
    }
    else
    {
        Pos epsilon = 1;
        
        // Left solutions
        while (MoveGenerator::canBlackContinue(t, from_x, from_y, from_x, from_y - epsilon))
        {
            res.push_back(t.next(from_x, from_y, from_x, from_y - epsilon));
            epsilon++;
        }
        epsilon = 1;

        // Right solutions
        while (MoveGenerator::canBlackContinue(t, from_x, from_y, from_x, from_y + epsilon))
        {
            res.push_back(t.next(from_x, from_y, from_x, from_y + epsilon));
            epsilon++;
        }

        epsilon = 1;
        // Up solutions
        while (MoveGenerator::canBlackContinue(t, from_x, from_y, from_x - epsilon, from_y))
        {
            res.push_back(t.next(from_x, from_y, from_x - epsilon, from_y));
            epsilon++;
        }

        epsilon = 1;
        // Down solutions
        while (MoveGenerator::canBlackContinue(t, from_x, from_y, from_x + epsilon, from_y))
        {
            res.push_back(t.next(from_x, from_y, from_x + epsilon, from_y));
            epsilon++;
        }
    }
}