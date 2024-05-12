#ifndef TABLUT_H
#define TABLUT_H

#include <iostream>
#include <cstdint>
#include <array>
#include <map>
#include <stdio.h>
#include <cstring>
#include <chrono>
#include <vector>
#include <deque>
#include <Json/json.hpp>

using json = nlohmann::json;

typedef int16_t Pos;
typedef uint64_t ZobristKey;

// Table dimensions -> always 9
const Pos DIM(9);
const Pos LAST_ROW(DIM - 2);
const Pos LAST_COL(DIM - 2);
const Pos FIRST_ROW(1);
const Pos FIRST_COL(1);

// Dead king position value
const Pos KDEADPOSITION(20);
// Max _hash log extensions, used to check if position is draw(game state position reached twice)
const int MAX_DRAW_LOG(250);
// Max position log used to check where checkers are ( no king )
const int MAX_POS_LOG(24);

// Enum for Checker values
enum C : int16_t
{
    EMPTY = 0,
    BLACK = 1,
    WHITE = 2,
    KING = 3
};

// ENUM FOR _board structures VALUES
enum S : int16_t
{
    NOTHING = 0,
    ESCAPE = 1,
    CAMPS = 2,
    CASTLE = 3
};

// ENUM For _gameState win or lose
enum GAME_STATE : int16_t
{
    NONE = 0,
    WHITEWIN = 1,
    BLACKWIN = 2,
    WHITEDRAW = 3,
    BLACKDRAW = 4
};

// Matrix representing all tiles types
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

const std::array<std::array<S, DIM>, DIM> tablutStructure{{
    {S::NOTHING, S::ESCAPE, S::ESCAPE, S::CAMPS, S::CAMPS, S::CAMPS, S::ESCAPE, S::ESCAPE, S::NOTHING},
    {S::ESCAPE, S::NOTHING, S::NOTHING, S::NOTHING, S::CAMPS, S::NOTHING, S::NOTHING, S::NOTHING, S::ESCAPE},
    {S::ESCAPE, S::NOTHING, S::NOTHING, S::NOTHING, S::NOTHING, S::NOTHING, S::NOTHING, S::NOTHING, S::ESCAPE},
    {S::CAMPS, S::NOTHING, S::NOTHING, S::NOTHING, S::NOTHING, S::NOTHING, S::NOTHING, S::NOTHING, S::CAMPS},
    {S::CAMPS, S::CAMPS, S::NOTHING, S::NOTHING, S::CASTLE, S::NOTHING, S::NOTHING, S::CAMPS, S::CAMPS},
    {S::CAMPS, S::NOTHING, S::NOTHING, S::NOTHING, S::NOTHING, S::NOTHING, S::NOTHING, S::NOTHING, S::CAMPS},
    {S::ESCAPE, S::NOTHING, S::NOTHING, S::NOTHING, S::NOTHING, S::NOTHING, S::NOTHING, S::NOTHING, S::ESCAPE},
    {S::ESCAPE, S::NOTHING, S::NOTHING, S::NOTHING, S::CAMPS, S::NOTHING, S::NOTHING, S::NOTHING, S::ESCAPE},
    {S::NOTHING, S::ESCAPE, S::ESCAPE, S::CAMPS, S::CAMPS, S::CAMPS, S::ESCAPE, S::ESCAPE, S::NOTHING},
}};

class Tablut
{
public:
    // Tells if is White or Black _turn
    bool _isWhiteTurn;

    // New position of the moved piece
    Pos _x;
    Pos _y;

    // Old position of the moved piece
    Pos _oldX;
    Pos _oldY;

    // Total number of white and black checkers alive on _board
    int16_t _whiteCount;
    int16_t _blackCount;

    // King position during game
    Pos _kingX;
    Pos _kingY;

    // Kills recap in this round
    int _kills;

    // std::array<std::array<C, DIM>, DIM> game
    std::array<std::array<C, DIM>, DIM> _board;

    // Turn count
    int _turn;

    // Current table hash used for transpositionTable lookup
    ZobristKey _hash;

    // Past turn hashes, used to check if same game state is reached twice
    std::array<ZobristKey, MAX_DRAW_LOG> _pastHashes;
    int _pastHashesIndex;

    // How Much Movements can the king make;
    int _kingMovements;

    // Vector of Checkers representing all checkers position of the board ( No king )
    std::array<std::pair<Pos, Pos>, MAX_POS_LOG> _checkerPositions;
    int _checkerPositionIndex;

    // Current Tablut score
    int _score;

    // Tell if game is in win or draw state
    GAME_STATE _gameState;

    Tablut() {}
    ~Tablut() {}

    /*
        Copy constructor
    */
    Tablut(const Tablut &__startFrom)
    {
        std::memcpy(this, &__startFrom, sizeof(Tablut));
    }

    /*
        Load a Tablut from the given Json string
    */
    static Tablut fromJson(const std::string &__json)
    {
        Tablut t = Tablut();

        json jsonValue = json::parse(__json);

        // -- TURN READ --------------
        const std::string turn = jsonValue["turn"];

        if (turn == "BLACKWIN" || turn == "WHITEWIN" || turn == "DRAW")
        {
            std::cout << "------- GAME ENDED -------" << std::endl;
            exit(EXIT_SUCCESS);
        }

        if (turn.at(0) == 'W')
        {
            t._isWhiteTurn = true;
        }
        else
        {
            t._isWhiteTurn = false;
        }

        // -- TURN READ -------------- END
        // -- BOARD READ -------------

        Pos rowIndex = 0;
        Pos colIndex = 0;

        t._whiteCount = 0;
        t._blackCount = 0;

        t._x = -1;
        t._y = -1;

        t._oldX = -1;
        t._oldY = -1;

        t._kills = 0;

        t._gameState = GAME_STATE::NONE;

        t._kingMovements = 0;
        t._turn = 0;

        t._score = 0;

        t._checkerPositions = {};
        t._checkerPositionIndex = 0;

        t._hash = 0;
        t._pastHashes = {};
        t._pastHashesIndex = 0;

        for (auto &row : jsonValue["board"])
        {
            for (std::string cell : row)
            {
                char cellStart = cell.at(0);

                if (cellStart == 'W')
                {
                    t._whiteCount++;
                    t._checkerPositions[t._checkerPositionIndex++] = {rowIndex, colIndex};
                    t._board[rowIndex][colIndex] = C::WHITE;
                }
                else if (cellStart == 'B')
                {
                    t._blackCount++;
                    t._checkerPositions[t._checkerPositionIndex++] = {rowIndex, colIndex};
                    t._board[rowIndex][colIndex] = C::BLACK;
                }
                else if (cellStart == 'K')
                {
                    t._kingX = rowIndex;
                    t._kingY = colIndex;
                    t._board[rowIndex][colIndex] = C::KING;
                }
                else
                {
                    t._board[rowIndex][colIndex] = C::EMPTY;
                }

                colIndex++;
            }

            colIndex = 0;
            rowIndex++;
        }

        return t;
    }

    /*
        Get a Tablut which has the default board setup
    */
    static Tablut getStartingPosition()
    {
        Tablut t = Tablut();

        for (Pos x = 0; x < 9; x++)
        {
            for (Pos y = 0; y < 9; y++)
            {
                t._board[x][y] = C::EMPTY;
            }
        }

        t._isWhiteTurn = true;

        t._whiteCount = 8;
        t._blackCount = 16;

        t._x = 0;
        t._y = 0;

        t._oldX = 0;
        t._oldY = 0;

        t._kingX = 4;
        t._kingY = 4;

        t._kills = 0;

        t._gameState = GAME_STATE::NONE;

        t._hash = 0;
        t._pastHashesIndex = 0;

        t._kingMovements = 0;
        t._turn = 0;

        t._score = 0;

        // Setting all black defenders
        t._board[0][3] = C::BLACK;
        t._board[0][4] = C::BLACK;
        t._board[0][5] = C::BLACK;
        t._board[1][4] = C::BLACK;
        t._board[3][0] = C::BLACK;
        t._board[4][0] = C::BLACK;
        t._board[5][0] = C::BLACK;
        t._board[4][1] = C::BLACK;
        t._board[8][3] = C::BLACK;
        t._board[8][4] = C::BLACK;
        t._board[8][5] = C::BLACK;
        t._board[7][4] = C::BLACK;
        t._board[3][8] = C::BLACK;
        t._board[4][8] = C::BLACK;
        t._board[5][8] = C::BLACK;
        t._board[4][7] = C::BLACK;

        // Setting up white attackers
        t._board[4][2] = C::WHITE;
        t._board[4][3] = C::WHITE;
        t._board[4][5] = C::WHITE;
        t._board[4][6] = C::WHITE;
        t._board[2][4] = C::WHITE;
        t._board[3][4] = C::WHITE;
        t._board[5][4] = C::WHITE;
        t._board[6][4] = C::WHITE;

        // Setting Throne
        t._board[4][4] = C::KING;

        t._checkerPositionIndex = 0;
        for (int i = 0; i < DIM; i++)
        {
            for (int j = 0; j < DIM; j++)
            {
                if (t._board[i][j] == C::BLACK || t._board[i][j] == C::WHITE)
                {
                    t._checkerPositions[t._checkerPositionIndex++] = {i, j};
                }
            }
        }

        return t;
    }

    /*
        Get the new Tablut from the previous one by moving one checker from the specified position
        to the specified position, using the standard Tablut rules
    */
    Tablut next(const Pos __fromX, const Pos __fromY, const Pos __toX, const Pos __toY)
    {

        Tablut _next = Tablut(*this);

        // UPDATE C POSITION
        _next._board[__toX][__toY] = _next._board[__fromX][__fromY];

        // REMOVE C FROM ITS PAST POSITION
        _next._board[__fromX][__fromY] = C::EMPTY;

        // REPLACE THE OLD C POSITION INSIDE THE C POSITION ARRAY WITH THE NEW ONE
        auto mustFind = std::make_pair(__fromX, __fromY);
        for (int i = 0; i < _next._checkerPositionIndex; i++)
        {
            if (_next._checkerPositions[i] == mustFind)
            {
                _next._checkerPositions[i] = std::make_pair(__toX, __toY);
                break;
            }
        }

        // UPDATE OLD AND NEW POSITION OF THE C
        _next._oldX = __fromX;
        _next._oldY = __fromY;

        _next._x = __toX;
        _next._y = __toY;

        // RESET KILL FEED FROM PREVIOUS ROUND
        _next._kills = 0;

        C leftChecker = _next.getLeftChecker();
        C rightChecker = _next.getRightChecker();
        C upChecker = _next.getUpChecker();
        C downChecker = _next.getDownChecker();

        // WHITE TURN
        if (_next._isWhiteTurn)
        {
            // UPDATE KING POSITION
            if (_next._board[__toX][__toY] == C::KING)
            {
                _next._kingX = __toX;
                _next._kingY = __toY;
            }

            // NORMAL WHITE EATS

            // Opposite structure to check if black checker are surrounded by a structure in any direction
            // Kill can occur with white or king checker

            // LEFT eat
            if (_next._y > FIRST_COL && leftChecker == C::BLACK)
            {
                if (_next.getLeftChecker(2) > 1 || (_next.getLeftStructure(2) > 1 && _next.getLeftStructure() != S::CAMPS))
                {
                    _next.killLeft();
                }
            }

            // RIGHT eat
            if (_next._y < LAST_COL && rightChecker == C::BLACK)
            {
                if (_next.getRightChecker(2) > 1 || (_next.getRightStructure(2) > 1 && _next.getRightStructure() != S::CAMPS))
                {
                    _next.killRight();
                }
            }

            // UP eat
            if (_next._x > FIRST_ROW && upChecker == C::BLACK)
            {
                if (_next.getUpChecker(2) > 1 || (_next.getUpStructure(2) > 1 && _next.getUpStructure() != S::CAMPS))
                {
                    _next.killUp();
                }
            }

            // DOWN eat
            if (_next._x < LAST_ROW && downChecker == C::BLACK)
            {
                if (_next.getDownChecker(2) > 1 || (_next.getDownStructure(2) > 1 && _next.getDownStructure() != S::CAMPS))
                {
                    _next.killDown();
                }
            }
        }

        // BLACK TURN
        else
        {
            // NORMAL EATS

            // Opposite structure to check if white checker are surrounded by a structure in any direction
            // LEFT eat ( KING OR SOLDIER ( > 1))
            if (_next._y > FIRST_COL && (leftChecker == C::WHITE || leftChecker == C::KING))
            {
                // Special eat, if king is in throne or near throne kills
                if (leftChecker == C::KING && _next.checkIfKingThreatened())
                {
                    if (_next.checkIfKingDead())
                    {
                        _next.killLeft();
                    }
                }
                else
                {
                    // eat if normall eat (black on opposite side) or theres a camp or castle on the opposite side  @see S
                    if (_next.getLeftStructure() != S::CASTLE && (_next.getLeftChecker(2) == C::BLACK || _next.getLeftStructure(2) > 1))
                    {
                        _next.killLeft();
                    }
                }
            }

            // RIGHT eat
            if (_next._y < LAST_COL && (rightChecker == C::WHITE || rightChecker == C::KING))
            {
                // Special eat, if king is in throne or near throne kills
                if (rightChecker == C::KING && _next.checkIfKingThreatened())
                {
                    if (_next.checkIfKingDead())
                    {
                        _next.killRight();
                    }
                }
                else
                {
                    // eat if normal eat (black on opposite side) or theres a camp or castle on the opposite side  @see S
                    if (_next.getRightStructure() != S::CASTLE && (_next.getRightChecker(2) == C::BLACK || _next.getRightStructure(2) > 1))
                    {
                        _next.killRight();
                    }
                }
            }

            // UP eat
            if (_next._x > FIRST_ROW && (upChecker == C::WHITE || upChecker == C::KING))
            {
                // Special eat, if king is in throne or near throne kills
                if (upChecker == C::KING && _next.checkIfKingThreatened())
                {
                    if (_next.checkIfKingDead())
                    {
                        _next.killUp();
                    }
                }
                else
                {
                    // eat if normal eat (black on opposite side) or theres a camp or castle on the opposite side  @see S
                    if (_next.getUpStructure() != S::CASTLE && (_next.getUpChecker(2) == C::BLACK || _next.getUpStructure(2) > 1))
                    {
                        _next.killUp();
                    }
                }
            }

            // DOWN eat
            if (_next._x < LAST_ROW && (downChecker == C::WHITE || downChecker == C::KING))
            {
                // Special eat, if king is in throne or near throne kills
                if (downChecker == C::KING && _next.checkIfKingThreatened())
                {
                    if (_next.checkIfKingDead())
                    {
                        _next.killDown();
                    }
                }
                else
                {
                    // eat if normal eat (black on opposite side) or theres a camp or castle on the opposite side  @see S
                    if (_next.getDownStructure() != S::CASTLE && (_next.getDownChecker(2) == C::BLACK || _next.getDownStructure(2) > 1))
                    {
                        _next.killDown();
                    }
                }
            }
        }

        _next._turn++;
        _next.checkWinState();

        _next.switchTurn();

        return _next;
    }

    // Tell if game is Over
    bool isGameOver() const
    {
        return _gameState != GAME_STATE::NONE;
    }

    // Tell if a move is non quiet ( move get kills or win/lose a game)
    bool isNonQuiet()
    {
        return _kills > 0 || isGameOver();
    }

    std::tuple<Pos, Pos, Pos, Pos> getMove()
    {
        return {_oldX, _oldY, _x, _y};
    }

    std::array<ZobristKey, MAX_DRAW_LOG> getPastMoves()
    {
        return _pastHashes;
    }

    int getPastMovesIndex()
    {
        return _pastHashesIndex;
    }

    // Tell if someone won, lost or drawed
    GAME_STATE checkWinState()
    {
        if (_gameState != GAME_STATE::NONE)
        {
            return _gameState;
        }

        if (checkDraw())
        {
            _gameState = _isWhiteTurn ? GAME_STATE::WHITEDRAW : GAME_STATE::BLACKDRAW;
        }
        else if (_kingX == KDEADPOSITION)
        {
            _gameState = GAME_STATE::BLACKWIN;
        }
        else if (tablutStructure[_kingX][_kingY] == S::ESCAPE)
        {
            _gameState = GAME_STATE::WHITEWIN;
        }

        return _gameState;
    }

    bool checkDraw() const
    {
        for (int i = 0; i < _pastHashesIndex - 1; i++)
        {
            if (_hash == _pastHashes[i])
            {
                return true;
            }
        }
        return false;
    }

    bool kingIsInThrone() const
    {
        return _kingX == 4 && _kingY == 4;
    }

    bool isKingSurrounded() const
    {
        return _board[4][3] == C::BLACK && _board[4][5] == C::BLACK && _board[3][4] == C::BLACK && _board[5][4] == C::BLACK;
    }

    bool isKingNearThrone() const
    {
        return _board[4][3] == C::KING || _board[4][5] == C::KING || _board[3][4] == C::KING || _board[5][4] == C::KING;
    }

    bool isKingSurroundedNearThrone() const
    {
        // King near throne so can be killed by 3 black checkers surounding him
        int16_t surroundCount = 0;  // Number of black defenders surrounding king
        bool castleCounted = false; // Tell if castle as been counted or not

        // Left CHECK
        if (_board[_kingX][_kingY - 1] == C::BLACK)
        {
            surroundCount++;
        }
        else if (tablutStructure[_kingX][_kingY - 1] == S::CASTLE)
        {
            castleCounted = true;
        }
        else
        {
            return false;
        }

        // Right CHECK
        if (_board[_kingX][_kingY + 1] == C::BLACK)
        {
            surroundCount++;
        }
        else if (!castleCounted && tablutStructure[_kingX][_kingY + 1] == S::CASTLE)
        {
            castleCounted = true;
        }
        else
        {
            return false;
        }

        // Up CHECK
        if (_board[_kingX - 1][_kingY] == C::BLACK)
        {
            surroundCount++;
        }
        else if (!castleCounted && tablutStructure[_kingX - 1][_kingY] == S::CASTLE)
        {
            castleCounted = true;
        }
        else
        {
            return false;
        }

        // Down CHECK
        if (_board[_kingX + 1][_kingY] == C::BLACK)
        {
            surroundCount++;
        }
        else if (!castleCounted && tablutStructure[_kingX + 1][_kingY] == S::CASTLE)
        {
            castleCounted = true;
        }
        else
        {
            return false;
        }

        return surroundCount == 3 && castleCounted;
    }

    // Kill checker by checking which type of checker is on the specified point, update checkersCount and killFeed variables
    void killChecker(Pos __x, Pos __y)
    {
        C &target = _board[__x][__y];

        if (target == C::EMPTY)
        {
            return;
        }

        if (target == C::WHITE)
        {
            target = C::EMPTY;
            _whiteCount = _whiteCount - 1U;
            _kills++;
        }
        else if (target == C::BLACK)
        {
            target = C::EMPTY;
            _blackCount = _blackCount - 1U;
            _kills++;
        }
        else if (target == C::KING)
        {
            target = C::EMPTY;
            _kingX = KDEADPOSITION;
            _kingY = KDEADPOSITION;
            _kills++;
            return;
        }

        int i = 0;
        auto mustFind = std::make_pair(__x, __y);

        for (i = 0; i < _checkerPositionIndex; i++)
        {
            if (_checkerPositions[i] == mustFind)
            {
                break;
            }
        }

        for (; i < _checkerPositionIndex - 1; i++)
        {
            _checkerPositions[i] = _checkerPositions[i + 1];
        }

        _checkerPositionIndex--;
    }

    bool checkIfKingDead() const
    {
        // KING IN THRONE OR NEAR TRONE EAT

        // King in throne and black surounded throne -> WHITE LOSE
        if (kingIsInThrone() && isKingSurrounded())
        {
            return true;
        }

        if (isKingNearThrone() && isKingSurroundedNearThrone())
        {
            return true;
        }

        return false;
    }

    bool checkIfKingThreatened() const
    {
        return kingIsInThrone() || isKingNearThrone();
    }

    void print()
    {
        std::cout << *this << std::endl;
    }

    friend std::ostream &operator<<(std::ostream &out, const Tablut &__tablut)
    {
        out << std::endl
            << "   ";
        for (int y = 0; y < 9; y++)
        {
            out << " " << y << " ";
        }
        out << std::endl
            << "   ";

        for (int y = 0; y < 9; y++)
        {
            out << "___";
        }
        out << std::endl;

        for (int x = 0; x < 9; x++)
        {
            out << " " << x << "|";
            for (int y = 0; y < 9; y++)
            {
                if (__tablut._board[x][y] == C::KING)
                {
                    if (x == __tablut._x && y == __tablut._y)
                    {
                        out << "\033[1;42m K \033[0m";
                    }
                    else
                    {
                        out << "\033[1;93m K \033[0m";
                    }
                }
                else if (__tablut._board[x][y] == C::BLACK)
                {
                    if (x == __tablut._x && y == __tablut._y)
                    {
                        out << "\033[1;42m B \033[0m";
                    }
                    else if (tablutStructure[x][y] == S::CAMPS)
                    {
                        out << "\033[1;100m B \033[0m";
                    }
                    else if (tablutStructure[x][y] == S::ESCAPE)
                    {
                        out << "\033[1;106m B \033[0m";
                    }
                    else
                    {
                        out << "\033[1;97m B \033[0m";
                    }
                }
                else if (__tablut._board[x][y] == C::WHITE)
                {
                    if (x == __tablut._x && y == __tablut._y)
                    {
                        out << "\033[1;42m W \033[0m";
                    }
                    else if (tablutStructure[x][y] == S::CAMPS)
                    {
                        out << "\033[1;100m W \033[0m";
                    }
                    else if (tablutStructure[x][y] == S::ESCAPE)
                    {
                        out << "\033[1;106m W \033[0m";
                    }
                    else
                    {
                        out << "\033[1;97m W \033[0m";
                    }
                }
                else
                {
                    if (x == __tablut._oldX && y == __tablut._oldY)
                    {
                        out << "\033[1;41m   \033[0m";
                    }
                    else if (tablutStructure[x][y] == S::CAMPS)
                    {
                        out << "\033[1;100m   \033[0m";
                    }
                    else if (tablutStructure[x][y] == S::ESCAPE)
                    {
                        out << "\033[1;106m   \033[0m";
                    }
                    else if (tablutStructure[x][y] == S::CASTLE)
                    {
                        out << "\033[1;103m   \033[0m";
                    }
                    else
                    {
                        out << "   ";
                    }
                }
            }
            out << std::endl;
        }

        out << std::endl
            << "====================" << std::endl;
        out << (__tablut._isWhiteTurn ? "\033[1;47m| WHITE MOVE NOW (BLACK MOVED) |\033[0m" : "\033[1;40m| BLACK MOVE NOW (WHITE MOVED)|\033[0m") << std::endl;
        out << "====================" << std::endl;

        out << "WINSTATE: " << __tablut._gameState << std::endl;
        out << "TURN: " << __tablut._turn << std::endl;
        out << "whiteCheckers: " << unsigned(__tablut._whiteCount) << std::endl;
        out << "blackCheckers: " << unsigned(__tablut._blackCount) << std::endl;
        out << "kingPosition: " << int(__tablut._kingX) << "-" << int(__tablut._kingY) << std::endl;
        out << "kills: " << __tablut._kills << std::endl;
        if (int(__tablut._x) >= 0)
        {
            out << "checkerMovedTo: " << int(__tablut._x) << "-" << int(__tablut._y) << std::endl;
            out << "checkerMovedFrom: " << int(__tablut._oldX) << "-" << int(__tablut._oldY) << std::endl;
        }

        return out;
    }

    /*
       GET ADIACENT CHECKER HELPERS
   */
    C getLeftChecker(Pos __by = 1) const
    {
        return _board[_x][_y - __by];
    }

    C getRightChecker(Pos __by = 1) const
    {
        return _board[_x][_y + __by];
    }

    C getUpChecker(Pos __by = 1) const
    {
        return _board[_x - __by][_y];
    }

    C getDownChecker(Pos __by = 1) const
    {
        return _board[_x + __by][_y];
    }

    /*
        GET ADIACENT STRUCTURES HELPERS
    */
    S getLeftStructure(Pos __by = 1) const
    {
        return tablutStructure[_x][_y - __by];
    }

    S getRightStructure(Pos __by = 1) const
    {
        return tablutStructure[_x][_y + __by];
    }

    S getUpStructure(Pos __by = 1) const
    {
        return tablutStructure[_x - __by][_y];
    }

    S getDownStructure(Pos __by = 1) const
    {
        return tablutStructure[_x + __by][_y];
    }

    /*
        KILL ADIACENT CHECKER HELPERS
    */
    void killLeft()
    {
        killChecker(_x, _y - 1);
    }

    void killRight()
    {
        killChecker(_x, _y + 1);
    }

    void killUp()
    {
        killChecker(_x - 1, _y);
    }

    void killDown()
    {
        killChecker(_x + 1, _y);
    }

    void switchTurn()
    {
        _isWhiteTurn = !_isWhiteTurn;
    }
};

#endif