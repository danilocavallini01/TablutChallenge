#include "MoveGenerator.h"

MoveGenerator::MoveGenerator() {}
MoveGenerator::~MoveGenerator() {}

void MoveGenerator::generateLegalMoves(Tablut &t, std::vector<Tablut> &nextTabluts)
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
                    getLegalMovesFrom(t, x, y, nextTabluts);
                }
            }
        }

        // GENERATE MOVES FOR KING
        // getLegalMovesFrom(t, t.kingX, t.kingY, nextTabluts);
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
                    getLegalMovesFrom(t, x, y, nextTabluts);
                }
            }
        }
    }

    ZobristKey hash;

    for (auto &nextTablut : nextTabluts)
    {
        hash = zobrist.hash(nextTablut);

        nextTablut.hash = hash;
        nextTablut.pastHashes[nextTablut.pastHashesIndex++] = hash;

        if (nextTablut.pastHashesIndex == MAX_DRAW_LOG)
        {
            nextTablut.pastHashesIndex = 0;
        }

        nextTablut.checkWinState();
    }
};

void MoveGenerator::getLegalMovesFrom(Tablut &t, Pos from_x, const Pos from_y, std::vector<Tablut> &nextTabluts)
{
    // ------------ WHITE LEGAL MOVES ------------
    if (t.isWhiteTurn)
    {
        Pos epsilon = 1;

        // Left solutions
        while ((from_y - epsilon) != -1 && canWhiteContinue(t, from_x, from_y - epsilon))
        {
            nextTabluts.push_back(t.next(from_x, from_y, from_x, from_y - epsilon));
            epsilon++;
        }

        epsilon = 1;
        // Right solutions
        while ((from_y + epsilon) != 9 && canWhiteContinue(t, from_x, from_y + epsilon))
        {
            nextTabluts.push_back(t.next(from_x, from_y, from_x, from_y + epsilon));
            epsilon++;
        }

        epsilon = 1;
        // Up solutions
        while ((from_x - epsilon) != -1 && canWhiteContinue(t, from_x - epsilon, from_y))
        {
            nextTabluts.push_back(t.next(from_x, from_y, from_x - epsilon, from_y));
            epsilon++;
        }

        epsilon = 1;
        // Down solutions
        while ((from_x + epsilon) != 9 && canWhiteContinue(t, from_x + epsilon, from_y))
        {
            nextTabluts.push_back(t.next(from_x, from_y, from_x + epsilon, from_y));
            epsilon++;
        }

        // ------------ BLACK LEGAL MOVES ------------
    }
    else
    {
        Pos epsilon = 1;

        // Left solutions
        while ((from_y - epsilon) != -1 && canBlackContinue(t, from_x, from_y, from_x, from_y - epsilon))
        {
            nextTabluts.push_back(t.next(from_x, from_y, from_x, from_y - epsilon));
            epsilon++;
        }
        epsilon = 1;

        // Right solutions
        while ((from_y + epsilon) != 9 && canBlackContinue(t, from_x, from_y, from_x, from_y + epsilon))
        {
            nextTabluts.push_back(t.next(from_x, from_y, from_x, from_y + epsilon));
            epsilon++;
        }

        epsilon = 1;
        // Up solutions
        while ((from_x - epsilon) != -1 && canBlackContinue(t, from_x, from_y, from_x - epsilon, from_y))
        {
            nextTabluts.push_back(t.next(from_x, from_y, from_x - epsilon, from_y));
            epsilon++;
        }

        epsilon = 1;
        // Down solutions
        while ((from_x + epsilon) != 9 && canBlackContinue(t, from_x, from_y, from_x + epsilon, from_y))
        {
            nextTabluts.push_back(t.next(from_x, from_y, from_x + epsilon, from_y));
            epsilon++;
        }
    }
}