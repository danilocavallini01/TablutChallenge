#ifndef HEURISTIC
#define HEURISTIC

#include "../Interfaces/IHeuristic.hpp"
#include "Tablut.hpp"
#include "MoveGenerator.hpp"

#include <array>
#include <algorithm>
#include <iostream>
#include <atomic>
#include <mutex>

// Forward Declaration
class Tablut;
class MoveGenerator;

using namespace AI::Interface;
using namespace AI::Define;

// Matrix representing the distance ( in number of moves ) in every cell from the nearest camp
/*--0 1 2 3 4 5 6 7 8
 |
 0  - E E C C C E E -
 1  E - - - C - - - E
 2  E - - - - - - - E
 3  C - - - - - - - C
 4  C C - - T - - C C
 5  C - - - - - - - C
 6  E - - - - - - - E
 7  E - - - C - - - E
 8  - E E C C C E E -

*/
const int TOTAL_WEIGHTS = 40;

typedef std::array<int, TOTAL_WEIGHTS> Weights;
typedef std::array<std::array<int, DIM>, DIM> BoardWeights;
typedef std::pair<int, int> WeightBounds;

/*
    Negative values mean black advantage
    Otherwise white advantage

    weightsS
    0 : white pieces alive count weight
    1 : black pieces alive count weight
    2 : kills white on this round weight
    3 : kills black on this round weight
    4 : king space weight
    5-12: king position weight
    13-23: white pieces position weight
    24-37: black pieces position weight
    38: white restricting black weight
    39: black restricting white weight
*/

// HEURISTIC WEIGHTS BOUNDS
const int WIN_WEIGHT = 100000;
const int DRAW_WEIGHT = 0;

// WEIGHT BOUND SCALE
const int BIG_BOUND = 350;
const int MEDIUM_BOUND = 170;
const int LITTLE_BOUND = 40;

// KILLER MOVES TOTAL SLOT
const int KILLER_MOVES_SLOT = 2;
const int MAX_KILLER_MOVES_DEPTH = 15;

// KILLER MOVES DATABASE
std::atomic<int> _killerMovesHit;
std::mutex _mtxKillerMoveStore;

class Heuristic : public IHeuristic<Tablut>
{
private:
    Weights _weights;
    BoardWeights _kingPosHeuristic;
    BoardWeights _whitePosHeuristic;
    BoardWeights _blackPosHeuristic;

    // Killer moves set
    std::array<std::array<StandardMove, KILLER_MOVES_SLOT>, MAX_KILLER_MOVES_DEPTH> _killerMoves;
    std::array<int, MAX_KILLER_MOVES_DEPTH> _killerMovesIndex;

    void _killerMoveHit()
    {
        _killerMovesHit++;
    }

public:
    Heuristic(Weights __weights)
    {
        _weights = __weights;
        setupPositionsWeights();
        _killerMovesHit = 0;
    }

    Heuristic()
    {
        for (int i = 0; i < TOTAL_WEIGHTS; i++)
        {
            _weights[i] = 0;
        }
        setupPositionsWeights();
    }

    ~Heuristic() {}

    void setupPositionsWeights()
    {
        _kingPosHeuristic = {{
            {_weights[5], _weights[6], _weights[6], _weights[00], _weights[00], _weights[00], _weights[6], _weights[6], _weights[5]},
            {_weights[6], _weights[7], _weights[7], _weights[8], _weights[00], _weights[8], _weights[7], _weights[7], _weights[6]},
            {_weights[6], _weights[7], _weights[8], _weights[9], _weights[10], _weights[9], _weights[8], _weights[7], _weights[6]},
            {_weights[00], _weights[8], _weights[9], _weights[10], _weights[11], _weights[10], _weights[9], _weights[8], _weights[00]},
            {_weights[00], _weights[00], _weights[10], _weights[11], _weights[12], _weights[11], _weights[10], _weights[00], _weights[00]},
            {_weights[00], _weights[8], _weights[9], _weights[10], _weights[11], _weights[10], _weights[9], _weights[8], _weights[00]},
            {_weights[6], _weights[7], _weights[8], _weights[9], _weights[10], _weights[9], _weights[8], _weights[7], _weights[6]},
            {_weights[6], _weights[7], _weights[7], _weights[8], _weights[00], _weights[8], _weights[7], _weights[7], _weights[6]},
            {_weights[5], _weights[6], _weights[6], _weights[00], _weights[00], _weights[00], _weights[6], _weights[6], _weights[5]},
        }};

        _whitePosHeuristic = {{
            {_weights[13], _weights[14], _weights[26], _weights[00], _weights[00], _weights[00], _weights[16], _weights[14], _weights[13]},
            {_weights[14], _weights[15], _weights[17], _weights[19], _weights[00], _weights[19], _weights[17], _weights[15], _weights[14]},
            {_weights[16], _weights[17], _weights[18], _weights[20], _weights[22], _weights[20], _weights[18], _weights[17], _weights[16]},
            {_weights[00], _weights[19], _weights[20], _weights[21], _weights[23], _weights[21], _weights[20], _weights[19], _weights[00]},
            {_weights[00], _weights[00], _weights[22], _weights[23], _weights[00], _weights[23], _weights[22], _weights[00], _weights[00]},
            {_weights[00], _weights[19], _weights[20], _weights[21], _weights[23], _weights[21], _weights[20], _weights[19], _weights[00]},
            {_weights[16], _weights[17], _weights[18], _weights[20], _weights[22], _weights[20], _weights[18], _weights[17], _weights[16]},
            {_weights[14], _weights[15], _weights[17], _weights[19], _weights[00], _weights[19], _weights[17], _weights[15], _weights[14]},
            {_weights[13], _weights[14], _weights[16], _weights[00], _weights[00], _weights[00], _weights[16], _weights[14], _weights[13]},
        }};

        _blackPosHeuristic = {{
            {_weights[24], _weights[25], _weights[26], _weights[35], _weights[36], _weights[35], _weights[26], _weights[25], _weights[24]},
            {_weights[25], _weights[27], _weights[28], _weights[30], _weights[37], _weights[30], _weights[28], _weights[27], _weights[25]},
            {_weights[26], _weights[28], _weights[29], _weights[31], _weights[33], _weights[31], _weights[29], _weights[28], _weights[26]},
            {_weights[35], _weights[30], _weights[31], _weights[32], _weights[34], _weights[32], _weights[31], _weights[30], _weights[35]},
            {_weights[36], _weights[37], _weights[33], _weights[34], _weights[00], _weights[34], _weights[33], _weights[37], _weights[36]},
            {_weights[35], _weights[30], _weights[31], _weights[32], _weights[34], _weights[32], _weights[31], _weights[30], _weights[35]},
            {_weights[26], _weights[28], _weights[29], _weights[31], _weights[33], _weights[31], _weights[29], _weights[28], _weights[26]},
            {_weights[25], _weights[27], _weights[28], _weights[30], _weights[37], _weights[30], _weights[28], _weights[27], _weights[25]},
            {_weights[24], _weights[25], _weights[26], _weights[35], _weights[36], _weights[35], _weights[26], _weights[25], _weights[24]},
        }};
    }

    /**
     * @brief Heavy evaluation of the state, every possible weight is used to score the game state
     * if end game occur, give a fixed score to the state
     *
     * @param __t game state to evaluate
     * @param __depth current depth of the game state
     * @param __color evaluation color
     * @param __colored tell if evaluation must be done by the perspective of the current player
     * @return int
     */
    int evaluate(Tablut &__t, int __depth, bool __color = true, bool __colored = false)
    {
        int score;
        int depthPenality = 10 - __depth;

        if (__t.isGameOver())
        {
            // DRAW CASE
            if (__t._gameState == GAME_STATE::WHITEDRAW || __t._gameState == GAME_STATE::BLACKDRAW)
            {
                // MUST IMPROVE DRAW WEIGHT
                score = DRAW_WEIGHT;
                score = __color ? score - depthPenality : score + depthPenality;
            }
            // WIN CASE
            else
            {
                score = __t._gameState == GAME_STATE::WHITEWIN ? HEURISTIC::WIN_WEIGHT - depthPenality : -HEURISTIC::WIN_WEIGHT + depthPenality;
            }
        }
        // NO GAME STATE FOUND CASE
        else
        {
            score = _weights[0] * __t._whiteCount + _weights[1] * __t._blackCount +
                    __t._kills * (__color ? _weights[2] : _weights[3]) + _weights[4] * kingMovements(__t) + _kingPosHeuristic[__t._kingX][__t._kingY] + positionsWeightSum(__t) + (__t._isWhiteTurn ? _weights[38] : _weights[39]) * restrictingPosition(__t);

            score = __color ? score - depthPenality : score + depthPenality;
        }

        return __colored ? (__color ? score : -score) : score;
    }

    /**
     * @brief Fast evaluation of the game state, the score is the same as the @see evaluate function
     * but the @see restrictPosition function is not considered
     * KIller moves bonuses are considered in this evaluation
     *
     * @param __t game state to evaluate
     * @param __depth current depth of the game state
     * @param __color evaluation color
     * @param __colored tell if evaluation must be done by the perspective of the current player
     * @return int
     */
    int quickEvaluate(Tablut &__t, int __depth, bool __color = true, bool __colored = false)
    {
        int score;
        int depthPenality = 10 - __depth;

        if (__t.isGameOver())
        {
            // DRAW CASE
            if (__t._gameState == GAME_STATE::WHITEDRAW || __t._gameState == GAME_STATE::BLACKDRAW)
            {
                // MUST IMPROVE DRAW WEIGHT
                score = DRAW_WEIGHT;
                score = __color ? score - depthPenality : score + depthPenality;
            }
            // WIN CASE
            else
            {
                score = __t._gameState == GAME_STATE::WHITEWIN ? HEURISTIC::WIN_WEIGHT - depthPenality : -HEURISTIC::WIN_WEIGHT + depthPenality;
            }
        }
        // NO GAME STATE FOUND CASE
        else
        {
            score = _weights[0] * __t._whiteCount + _weights[1] * __t._blackCount + __t._kills * (__color ? _weights[2] : _weights[3]) + _weights[4] * kingMovements(__t) + _kingPosHeuristic[__t._kingX][__t._kingY] + positionsWeightSum(__t);
            score += computeKillerMovesScore(__t, __depth, __color);
            score = __color ? score - depthPenality : score + depthPenality;
        }

        return __colored ? (__color ? score : -score) : score;
    }

    /**
     * @brief Function used to store a move killer into the killer moves
        When at a given depth the array is not full simply push a killer move into the first slot
        In alternative slide all the old killer moves and put the new one on top of the array
     *
     * @param __kMove killer move to store
     * @param __depth killer move's depth
     */
    void storeKillerMove(StandardMove &__kMove, int __depth)
    {
        _mtxKillerMoveStore.lock();
        if (_killerMovesIndex[__depth] < KILLER_MOVES_SLOT)
        {
            _killerMoves[__depth][_killerMovesIndex[__depth]++] = __kMove;
        }
        else
        {
            for (int i = KILLER_MOVES_SLOT - 2; i >= 0; i--)
            {
                _killerMoves[__depth][i + 1] = _killerMoves[__depth][i];
            }
            _killerMoves[__depth][0] = __kMove;
        }
        _mtxKillerMoveStore.unlock();
    }

    /**
     * @brief Tells if a move is a store killer move of a specified depth
        Return the index of the killer moves if its found, if not returns -1
     *
     * @param __kMove killer move to check
     * @param __depth current depth of evaluation
     * @return int - index of killer moves if found, otherwhise -1
     */
    int isKillerMove(StandardMove &__kMove, int __depth)
    {
        for (int i = 0; i < _killerMovesIndex[__depth]; i++)
        {
            if (__kMove == _killerMoves[__depth][i])
            {
                return i;
            }
        }
        return -1;
    }

    /**
     * @brief  Gives a bonus score based on if a move is a killer move
     *
     * @return int - the bonus score
     */
    int computeKillerMovesScore(Tablut &__t, int __depth, bool __color)
    {
        // KILLER MOVES
        StandardMove move = __t.getMove();
        int killerIndex = isKillerMove(move, __depth);

        if (killerIndex == -1)
        {
            return 0;
        }

        _killerMoveHit();

        if (killerIndex == 0)
        {
            return __color ? 5000 : -5000;
        }

        return __color ? 2000 : -2000;
    }

    /**
     * @brief Get the total stored killer moves hits
     *
     * @return int - total hit
     */
    int getKillerMovesHits() const
    {
        return _killerMovesHit;
    }

    /**
     * @brief Reset all killer moves, dont need to clean all killer moves array,
     * by clearing only the index array, when new killer moves will be added they'll overwrite
     * the old invalid entries
     */
    void resetKillerMoves()
    {
        _killerMovesHit = 0;
        for (int i = 0; i < MAX_KILLER_MOVES_DEPTH; i++)
        {
            _killerMovesIndex[i] = 0;
        }
    }

    /**
     * @brief  Give a score to the king liberty to move, by checking 
     * how much possible moves kings have
     *
     */
    int kingMovements(Tablut &__t)
    {
        return MoveGenerator::countKingLegalMoves(__t);
    }

    /**
     * @brief Give the total number of possible next moves of a specified game state
     *  usually gives a better score if the enemy has less space to move
     * 
     * @param __t the game state
     * @return int 
     */
    int restrictingPosition(Tablut &__t)
    {
        return MoveGenerator::countLegalMoves(__t);
    }

    /**
     * @brief Sum of all position weights used to evaluate a tablut
     * 
     * @param __t the game state
     * @return int 
     */
    int positionsWeightSum(Tablut &__t)
    {
        int score = 0;

        std::pair<Pos, Pos> position;
        Pos x, y;

        for (int i = 0; i < __t._checkerPositionIndex; i++)
        {
            position = __t._checkerPositions[i];
            x = position.first;
            y = position.second;

            if (__t._board[x][y] == C::WHITE)
            {
                score += _whitePosHeuristic[x][y];
            }
            else
            {
                score += _blackPosHeuristic[x][y];
            }
        }
        return score;
    }

    
    /**
     * @brief Compare function between two Tabluts, they're sorted by the respective scores
     * 
     * @param __t1 
     * @param __t2 
     * @return true - the first game state has a greater score than the second
     * @return false - the first game state has less score the second
     */
    bool compare(Tablut &__t1, Tablut &__t2)
    {
        return __t1._score > __t2._score;
    }

    // The inverse of the compare function between two Tabluts
    bool inverseCompare(Tablut &__t1, Tablut &__t2)
    {
        return __t1._score < __t2._score;
    }

    /**
     * @brief Sort moves by using the @see compare function, move ordering used in search algorithm
     * to produce cutoffs as soon as possible to reduce space state search
     *
     * @param __moves moves to order
     * @param __depth current depth, used in evaluation to penalize deeper moves
     * @param __whiteEvaluate current color
     * @param __colored tell if evaluation should be colored or not ( means that result is related to the player current turn )
     */
    void sortMoves(std::vector<Tablut> &__moves, int __depth, bool __whiteEvaluate = true, bool __colored = false)
    {
        for (Tablut &move : __moves)
        {
            move._score = quickEvaluate(move, __depth, __whiteEvaluate, __colored);
        }

        // SORT BY HIGHEST VALUE IF THE EVALUATION IS COLORED, SORT BY MIN OR MAX VALUE (BY @param __whiteEvaluate)
        std::sort(__moves.begin(), __moves.end(), std::bind(!__colored && !__whiteEvaluate ? &Heuristic::inverseCompare : &Heuristic::compare, std::ref(*this), std::placeholders::_1, std::placeholders::_2));
    }

    void print()
    {
        std::cout << "-> KING POS HEURISTIC" << std::endl;
        printBoardWeight(_kingPosHeuristic);
        std::cout << "-> WHITE POS HEURISTIC" << std::endl;
        printBoardWeight(_whitePosHeuristic);
        std::cout << "-> BLACK POS HEURISTIC" << std::endl;
        printBoardWeight(_blackPosHeuristic);
    }

    void printBoardWeight(BoardWeights &_boardWeight)
    {
        for (int i = 0; i < DIM; i++)
        {
            std::cout << "[";
            for (int j = 0; j < DIM; j++)
            {
                std::cout << " " << _boardWeight[i][j];
            }
            std::cout << " ]" << std::endl;
        }
    }

    /*
        Return the minimum and maximum value associated to the specified weight of the weights
    */
    static WeightBounds getWeightBounds(int __index)
    {
        // WHITE PIECES / WHITE KILLS / KING SPACE WEIGHT
        if (__index == 0 || __index == 2 || __index == 4)
        {
            return positiveBound(BIG_BOUND);
        }
        // BLACK PIECES / BLACK KILLS
        else if (__index == 1 || __index == 3)
        {
            return negativeBound(BIG_BOUND);
        }
        // KING POSITION WEIGHT
        else if (__index >= 5 && __index <= 12)
        {
            return neutralBound(BIG_BOUND);
        }
        // RESCTRING WHITE WEIGHT POSITION
        else if (__index == 38)
        {
            return negativeBound(LITTLE_BOUND);
        }
        // RESCTRING WHITE WEIGHT POSITION
        else if (__index == 39)
        {
            return positiveBound(LITTLE_BOUND);
        }
        // POSITIONS WEIGHT
        else
        {
            return neutralBound(MEDIUM_BOUND);
        }
    }

    static WeightBounds positiveBound(int __bound)
    {
        return {0, __bound};
    }

    static WeightBounds negativeBound(int __bound)
    {
        return {-__bound, 0};
    }

    static WeightBounds neutralBound(int __bound)
    {
        return {-__bound / 2, __bound / 2};
    }
};

#endif