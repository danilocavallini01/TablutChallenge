#include "MoveGenerator.h"

MoveGenerator::MoveGenerator() {}
MoveGenerator::~MoveGenerator() {}

void MoveGenerator::generateLegalMoves(Tablut &__t, std::vector<Tablut> &__nextTabluts)
{
    if (__t._isWhiteTurn)
    {
        // GENERATE MOVES FOR WHITE PIECES IF BLACK CHECKERS ENCOUTERED
        for (Pos x = 0; x < DIM; x++)
        {
            for (Pos y = 0; y < DIM; y++)
            {
                if (__t._board[x][y] == C::WHITE || __t._board[x][y] == C::KING)
                {
                    getLegalMovesFrom(__t, x, y, __nextTabluts);
                }
            }
        }

        // GENERATE MOVES FOR KING
        // getLegalMovesFrom(__t, __t._kingX, __t._kingY, __nextTabluts);
    }
    else
    {
        // GENERATE MOVES FOR BLACK PIECES IF BLACK CHECKERS ENCOTERED
        for (Pos x = 0; x < DIM; x++)
        {
            for (Pos y = 0; y < DIM; y++)
            {
                if (__t._board[x][y] == C::BLACK)
                {
                    getLegalMovesFrom(__t, x, y, __nextTabluts);
                }
            }
        }
    }

    ZobristKey hash;

    for (auto &nextTablut : __nextTabluts)
    {
        hash = _zobrist.hash(nextTablut);

        nextTablut.hash = hash;
        nextTablut._pastHashes[nextTablut._pastHashesIndex++] = hash;

        if (nextTablut._pastHashesIndex == MAX_DRAW_LOG)
        {
            nextTablut._pastHashesIndex = 0;
        }

        nextTablut.checkWinState();
    }
};

void MoveGenerator::getLegalMovesFrom(Tablut &__t, const Pos &__fromX, const Pos &__fromY, std::vector<Tablut> &nextTabluts)
{
    // ------------ WHITE LEGAL MOVES ------------
    if (__t._isWhiteTurn)
    {
        Pos epsilon = 1;

        // Left solutions
        while ((__fromY - epsilon) != -1 && canWhiteContinue(__t, __fromX, __fromY - epsilon))
        {
            nextTabluts.push_back(__t.next(__fromX, __fromY, __fromX, __fromY - epsilon));
            epsilon++;
        }

        epsilon = 1;
        // Right solutions
        while ((__fromY + epsilon) != 9 && canWhiteContinue(__t, __fromX, __fromY + epsilon))
        {
            nextTabluts.push_back(__t.next(__fromX, __fromY, __fromX, __fromY + epsilon));
            epsilon++;
        }

        epsilon = 1;
        // Up solutions
        while ((__fromX - epsilon) != -1 && canWhiteContinue(__t, __fromX - epsilon, __fromY))
        {
            nextTabluts.push_back(__t.next(__fromX, __fromY, __fromX - epsilon, __fromY));
            epsilon++;
        }

        epsilon = 1;
        // Down solutions
        while ((__fromX + epsilon) != 9 && canWhiteContinue(__t, __fromX + epsilon, __fromY))
        {
            nextTabluts.push_back(__t.next(__fromX, __fromY, __fromX + epsilon, __fromY));
            epsilon++;
        }

        // ------------ BLACK LEGAL MOVES ------------
    }
    else
    {
        Pos epsilon = 1;

        // Left solutions
        while ((__fromY - epsilon) != -1 && canBlackContinue(__t, __fromX, __fromY, __fromX, __fromY - epsilon))
        {
            nextTabluts.push_back(__t.next(__fromX, __fromY, __fromX, __fromY - epsilon));
            epsilon++;
        }
        epsilon = 1;

        // Right solutions
        while ((__fromY + epsilon) != 9 && canBlackContinue(__t, __fromX, __fromY, __fromX, __fromY + epsilon))
        {
            nextTabluts.push_back(__t.next(__fromX, __fromY, __fromX, __fromY + epsilon));
            epsilon++;
        }

        epsilon = 1;

        // Up solutions
        while ((__fromX - epsilon) != -1 && canBlackContinue(__t, __fromX, __fromY, __fromX - epsilon, __fromY))
        {
            nextTabluts.push_back(__t.next(__fromX, __fromY, __fromX - epsilon, __fromY));
            epsilon++;
        }

        epsilon = 1;

        // Down solutions
        while ((__fromX + epsilon) != 9 && canBlackContinue(__t, __fromX, __fromY, __fromX + epsilon, __fromY))
        {
            nextTabluts.push_back(__t.next(__fromX, __fromY, __fromX + epsilon, __fromY));
            epsilon++;
        }
    }
}