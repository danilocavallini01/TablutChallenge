#ifndef MOVEGENERATOR_H
#define MOVEGENERATOR_H

#include "Tablut.h"
#include <vector>
#include <cstdlib>

// Forward Declaration
class Tablut;

class MoveGenerator
{
private:
    // Generate all legal moves for a single Checker by scanning in all directions
    void _getLegalMovesFrom(Tablut &__t, const Pos &__fromX, const Pos &__fromY, std::vector<Tablut> &nextTabluts)
    {
        // ------------ WHITE LEGAL MOVES ------------
        if (__t._isWhiteTurn)
        {
            Pos epsilon = 1;

            // Left solutions
            while ((__fromY - epsilon) != -1 && _canWhiteContinue(__t, __fromX, __fromY - epsilon))
            {
                nextTabluts.push_back(__t.next(__fromX, __fromY, __fromX, __fromY - epsilon));
                epsilon++;
            }

            epsilon = 1;
            // Right solutions
            while ((__fromY + epsilon) != 9 && _canWhiteContinue(__t, __fromX, __fromY + epsilon))
            {
                nextTabluts.push_back(__t.next(__fromX, __fromY, __fromX, __fromY + epsilon));
                epsilon++;
            }

            epsilon = 1;
            // Up solutions
            while ((__fromX - epsilon) != -1 && _canWhiteContinue(__t, __fromX - epsilon, __fromY))
            {
                nextTabluts.push_back(__t.next(__fromX, __fromY, __fromX - epsilon, __fromY));
                epsilon++;
            }

            epsilon = 1;
            // Down solutions
            while ((__fromX + epsilon) != 9 && _canWhiteContinue(__t, __fromX + epsilon, __fromY))
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
            while ((__fromY - epsilon) != -1 && _canBlackContinue(__t, __fromX, __fromY, __fromX, __fromY - epsilon))
            {
                nextTabluts.push_back(__t.next(__fromX, __fromY, __fromX, __fromY - epsilon));
                epsilon++;
            }
            epsilon = 1;

            // Right solutions
            while ((__fromY + epsilon) != 9 && _canBlackContinue(__t, __fromX, __fromY, __fromX, __fromY + epsilon))
            {
                nextTabluts.push_back(__t.next(__fromX, __fromY, __fromX, __fromY + epsilon));
                epsilon++;
            }

            epsilon = 1;

            // Up solutions
            while ((__fromX - epsilon) != -1 && _canBlackContinue(__t, __fromX, __fromY, __fromX - epsilon, __fromY))
            {
                nextTabluts.push_back(__t.next(__fromX, __fromY, __fromX - epsilon, __fromY));
                epsilon++;
            }

            epsilon = 1;

            // Down solutions
            while ((__fromX + epsilon) != 9 && _canBlackContinue(__t, __fromX, __fromY, __fromX + epsilon, __fromY))
            {
                nextTabluts.push_back(__t.next(__fromX, __fromY, __fromX + epsilon, __fromY));
                epsilon++;
            }
        }
    }

    // Tell if a white checker can go in that location
    bool _canWhiteContinue(Tablut &__t, const Pos &__toX, const Pos &__toY)
    {
        C targetChecker = __t._board[__toX][__toY];
        S targetStructure = tablutStructure[__toX][__toY];

        return targetChecker == C::EMPTY && targetStructure < 2U;
    }

    // Tell if a white checker can go in that location
    bool _canBlackContinue(Tablut &__t, const Pos &__fromX, const Pos &__fromY, const Pos &__toX, const Pos &__toY)
    {
        C targetChecker = __t._board[__toX][__toY];
        S targetStructure = tablutStructure[__toX][__toY];

        // Check if move is made from a camp cell to another camp cells
        if (_isInCamp(__fromX, __fromY) && targetStructure == S::CAMPS)
        {
            return targetChecker == C::EMPTY && _tellIfCanPassCamp(__fromX, __fromY, __toX, __toY);
        }
        return targetChecker == C::EMPTY && targetStructure < 2U;
    }

    bool _isInCamp(const Pos &__x, const Pos &__y)
    {
        return tablutStructure[__x][__y] == S::CAMPS;
    };

    // Dont check if camp is a legal move cause MoveGenerator already delete illegal moves
    bool _tellIfCanPassCamp(const Pos &__fromX, const Pos &__fromY, const Pos &__toX, const Pos &__toY)
    {
        return abs(__fromX - __toX) <= 2 && abs(__fromY - __toY) <= 2;
    };

public:
    MoveGenerator(){};
    ~MoveGenerator(){};

    // Generate all legal moves for a Tablut depending on the current _turn ( WHITE OR BLACK )
    void generateLegalMoves(Tablut &__t, std::vector<Tablut> &__nextTabluts)
    {
        if (__t._isWhiteTurn)
        {
            // GENERATE MOVES FOR WHITE PIECES IF BLACK CHECKERS ENCOUTERED
            for (Pos x = 0; x < DIM; x++)
            {
                for (Pos y = 0; y < DIM; y++)
                {
                    if (__t._board[x][y] == C::WHITE)
                    {
                        _getLegalMovesFrom(__t, x, y, __nextTabluts);
                    }
                }
            }

            // GENERATE MOVES FOR KING
            int moveDifferences = __nextTabluts.size();
            _getLegalMovesFrom(__t, __t._kingX, __t._kingY, __nextTabluts);
            __t._kingMovements = moveDifferences - __nextTabluts.size();
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
                        _getLegalMovesFrom(__t, x, y, __nextTabluts);
                    }
                }
            }
        }
    };
};

#endif