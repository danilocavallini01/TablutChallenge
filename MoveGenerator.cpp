#include "MoveGenerator.h"

MoveGenerator::MoveGenerator() {}
MoveGenerator::~MoveGenerator() {}

void MoveGenerator::generateLegalMoves(Tablut &t, std::vector<Tablut> &res)
{
    if (t.isWhiteTurn)
    {
        // GENERATE MOVES FOR WHITE PIECES IF BLACK CHECKERS ENCOUTERED
        for (Pos x = 0; x < DIM; x++)
        {
            for (Pos y = 0; y < DIM; y++)
            {
                if (t.board[x][y] == C::WHITE || t.board[x][y] == C::KING)
                {
                    MoveGenerator::getLegalMovesFrom(t, x, y, res);
                }
            }
        }

        // GENERATE MOVES FOR KING
        // MoveGenerator::getLegalMovesFrom(t, t.kingX, t.kingY, res);
    }
    else
    {
        // GENERATE MOVES FOR BLACK PIECES IF BLACK CHECKERS ENCOTERED
        for (Pos x = 0; x < DIM; x++)
        {
            for (Pos y = 0; y < DIM; y++)
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
        while ((from_y - epsilon) != -1 && MoveGenerator::canWhiteContinue(t, from_x, from_y - epsilon))
        {
            res.push_back(t.next(from_x, from_y, from_x, from_y - epsilon));
            epsilon++;
        }

        epsilon = 1;
        // Right solutions
        while ((from_y + epsilon) != 9 && MoveGenerator::canWhiteContinue(t, from_x, from_y + epsilon))
        {
            res.push_back(t.next(from_x, from_y, from_x, from_y + epsilon));
            epsilon++;
        }

        epsilon = 1;
        // Up solutions
        while ((from_x - epsilon) != -1 && MoveGenerator::canWhiteContinue(t, from_x - epsilon, from_y))
        {
            res.push_back(t.next(from_x, from_y, from_x - epsilon, from_y));
            epsilon++;
        }

        epsilon = 1;
        // Down solutions
        while ((from_x + epsilon) != 9 && MoveGenerator::canWhiteContinue(t, from_x + epsilon, from_y))
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
        while ((from_y - epsilon) != -1 && MoveGenerator::canBlackContinue(t, from_x, from_y, from_x, from_y - epsilon))
        {
            res.push_back(t.next(from_x, from_y, from_x, from_y - epsilon));
            epsilon++;
        }
        epsilon = 1;

        // Right solutions
        while ((from_y + epsilon) != 9 && MoveGenerator::canBlackContinue(t, from_x, from_y, from_x, from_y + epsilon))
        {
            res.push_back(t.next(from_x, from_y, from_x, from_y + epsilon));
            epsilon++;
        }

        epsilon = 1;
        // Up solutions
        while ((from_x - epsilon) != -1 && MoveGenerator::canBlackContinue(t, from_x, from_y, from_x - epsilon, from_y))
        {
            res.push_back(t.next(from_x, from_y, from_x - epsilon, from_y));
            epsilon++;
        }

        epsilon = 1;
        // Down solutions
        while ((from_x + epsilon) != 9 && MoveGenerator::canBlackContinue(t, from_x, from_y, from_x + epsilon, from_y))
        {
            res.push_back(t.next(from_x, from_y, from_x + epsilon, from_y));
            epsilon++;
        }
    }
}