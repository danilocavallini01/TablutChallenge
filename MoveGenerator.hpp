#ifndef MOVEGENERATOR_H
#define MOVEGENERATOR_H

#include "Tablut.hpp"
#include "Interfaces/IMoveGenerator.hpp"

#include <vector>
#include <cstdlib>
#include <ranges>

// Forward Declaration
class Tablut;

class MoveGenerator : public IMoveGenerator<Tablut>
{
private:
    // Generate all legal moves for a single Checker by scanning in all directions
    static void _getLegalMovesFrom(Tablut &__t, const Pos &__fromX, const Pos &__fromY, std::vector<Tablut> &nextTabluts)
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

    /*
        Count all possible legal moves from a specified position
    */
    static int _countLegalMovesFrom(Tablut &__t, const Pos &__fromX, const Pos &__fromY)
    {
        // ------------ WHITE LEGAL MOVES ------------
        int totalMoves = 0;
        if (__t._isWhiteTurn)
        {
            Pos epsilon = 1;

            // Left solutions
            while ((__fromY - epsilon) != -1 && _canWhiteContinue(__t, __fromX, __fromY - epsilon))
            {
                totalMoves++;
                epsilon++;
            }

            epsilon = 1;
            // Right solutions
            while ((__fromY + epsilon) != 9 && _canWhiteContinue(__t, __fromX, __fromY + epsilon))
            {
                totalMoves++;
                epsilon++;
            }

            epsilon = 1;
            // Up solutions
            while ((__fromX - epsilon) != -1 && _canWhiteContinue(__t, __fromX - epsilon, __fromY))
            {
                totalMoves++;
                epsilon++;
            }

            epsilon = 1;
            // Down solutions
            while ((__fromX + epsilon) != 9 && _canWhiteContinue(__t, __fromX + epsilon, __fromY))
            {
                totalMoves++;
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
                totalMoves++;
                epsilon++;
            }
            epsilon = 1;

            // Right solutions
            while ((__fromY + epsilon) != 9 && _canBlackContinue(__t, __fromX, __fromY, __fromX, __fromY + epsilon))
            {
                totalMoves++;
                epsilon++;
            }

            epsilon = 1;

            // Up solutions
            while ((__fromX - epsilon) != -1 && _canBlackContinue(__t, __fromX, __fromY, __fromX - epsilon, __fromY))
            {
                totalMoves++;
                epsilon++;
            }

            epsilon = 1;

            // Down solutions
            while ((__fromX + epsilon) != 9 && _canBlackContinue(__t, __fromX, __fromY, __fromX + epsilon, __fromY))
            {
                totalMoves++;
                epsilon++;
            }
        }

        return totalMoves;
    }

    // Tell if a white checker can go in that location
    static bool _canWhiteContinue(Tablut &__t, const Pos &__toX, const Pos &__toY)
    {
        C targetChecker = __t._board[__toX][__toY];
        S targetStructure = tablutStructure[__toX][__toY];

        return targetChecker == C::EMPTY && targetStructure < 2;
    }

    // Tell if a white checker can go in that location
    static bool _canBlackContinue(Tablut &__t, const Pos &__fromX, const Pos &__fromY, const Pos &__toX, const Pos &__toY)
    {
        C targetChecker = __t._board[__toX][__toY];
        S targetStructure = tablutStructure[__toX][__toY];

        // Check if move is made from a camp cell to another camp cells
        if (_isInCamp(__fromX, __fromY) && targetStructure == S::CAMPS)
        {
            return targetChecker == C::EMPTY && _tellIfCanPassCamp(__fromX, __fromY, __toX, __toY);
        }
        return targetChecker == C::EMPTY && targetStructure < 2;
    }

    static bool _isInCamp(const Pos &__x, const Pos &__y)
    {
        return tablutStructure[__x][__y] == S::CAMPS;
    };

    // Dont check if camp is a legal move cause MoveGenerator already delete illegal moves
    static bool _tellIfCanPassCamp(const Pos &__fromX, const Pos &__fromY, const Pos &__toX, const Pos &__toY)
    {
        return abs(__fromX - __toX) <= 2 && abs(__fromY - __toY) <= 2;
    };

public:
    MoveGenerator(){};
    ~MoveGenerator(){};

    // Generate all legal moves for a Tablut depending on the current _turn ( WHITE OR BLACK )
    static void generateLegalMoves(Tablut &__t, std::vector<Tablut> &__nextTabluts)
    {
        Pos x, y;
        std::pair<Pos, Pos> position;

        if (__t._isWhiteTurn)
        {
            // GENERATE MOVES FOR WHITE PIECES IF BLACK CHECKERS ENCOUTERED

            for (int i = 0; i < __t._checkerPositionIndex; i++)
            {
                position = __t._checkerPositions[i];
                x = position.first;
                y = position.second;

                if (__t._board[x][y] == C::WHITE)
                {
                    _getLegalMovesFrom(__t, x, y, __nextTabluts);
                }
            }

            // GENERATE MOVES FOR KING
            _getLegalMovesFrom(__t, __t._kingX, __t._kingY, __nextTabluts);
        }
        else
        {
            // GENERATE MOVES FOR BLACK PIECES IF BLACK CHECKERS ENCOTERED

            for (int i = 0; i < __t._checkerPositionIndex; i++)
            {
                position = __t._checkerPositions[i];
                x = position.first;
                y = position.second;

                if (__t._board[x][y] == C::BLACK)
                {
                    _getLegalMovesFrom(__t, x, y, __nextTabluts);
                }
            }
        }
    };

    // Count all legal moves for a Tablut depending on the current _turn ( WHITE OR BLACK )
    static int countLegalMoves(Tablut &__t)
    {
        int totalMoves = 0;

        Pos x, y;
        std::pair<Pos, Pos> position;

        if (__t._isWhiteTurn)
        {
            // GENERATE MOVES FOR WHITE PIECES IF BLACK CHECKERS ENCOUTERED

            for (int i = 0; i < __t._checkerPositionIndex; i++)
            {
                position = __t._checkerPositions[i];
                x = position.first;
                y = position.second;

                if (__t._board[x][y] == C::WHITE)
                {
                    totalMoves += _countLegalMovesFrom(__t, x, y);
                }
            }

            // GENERATE MOVES FOR KING
            totalMoves += _countLegalMovesFrom(__t, __t._kingX, __t._kingY);
        }
        else
        {
            // GENERATE MOVES FOR BLACK PIECES IF BLACK CHECKERS ENCOTERED

            for (int i = 0; i < __t._checkerPositionIndex; i++)
            {
                position = __t._checkerPositions[i];
                x = position.first;
                y = position.second;

                if (__t._board[x][y] == C::BLACK)
                {
                    totalMoves += _countLegalMovesFrom(__t, x, y);
                }
            }
        }

        return totalMoves;
    };

    // Cout all legal moves for the king in the current tablut
    static int countKingLegalMoves(Tablut &__t)
    {
        return _countLegalMovesFrom(__t, __t._kingX, __t._kingY);
    }
};

#endif