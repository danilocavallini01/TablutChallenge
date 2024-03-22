#ifndef TABLUT_H
#define TABLUT_H

#include <iostream>

typedef int_fast16_t CheckerValue;
typedef int_fast16_t StructuresValue;

// Table dimensions -> always 9
const int DIM(9);

// ENUM FOR CHECKERS VALUES
enum CHECKER : CheckerValue
{
    EMPTY = 0,
    WHITE = 1,
    KING = 8,
    BLACK = -1
};

enum Structure : StructuresValue {
    NOTHING = 0,
    ESCAPE = 1,
    CAMPS = 2,
    CASTLE = 3
};

const Structure TablutStructure[DIM][DIM]
{
    {Structure.NOTHING,Structure.ESCAPE,Structure.ESCAPE,Structure.CAMPS,Structure.CAMPS,Structure.CAMPS,Structure.ESCAPE,Structure.ESCAPE,Structure.NOTHING},
    {} // CONTINUARE QDA QUA
}

// Matrix representing KING ESCAOE TILES -> white win
/*--0 1 2 3 4 5 6 7 8
 |
 0  - X X - - - X X -
 1  X - - - - - - - X
 2  X - - - - - - - X
 3  - - - - - - - - -
 4  - - - - - - - - -
 5  - - - - - - - - -
 6  X - - - - - - - X
 7  X - - - - - - - X
 8  - X X - - - X X -

*/
const TablutValue kingEscapePosition[16]
{
    1,2,6,7,10,20,60,70,18,28,68,78,81,82,86,87
};


// Matrix representing CAMP TILES -> starter positions of black
/*--0 1 2 3 4 5 6 7 8
 |
 0  - - - X X X - - -
 1  - - - - X - - - -
 2  - - - - - - - - -
 3  X - - - - - - - X
 4  X X - - - - - X X
 5  X - - - - - - - X
 6  - - - - - - - - -
 7  - - - - X - - - -
 8  - - - X X X - - -

*/
const TablutValue campsPosition[16]
{
    3,4,5,14,30,40,41,50,38,47,48,58,74,83,84,85
};

// Matrix representing of THRONE TILE -> starter positions of white king
/*--0 1 2 3 4 5 6 7 8
 |
 0  - - - - - - - - -
 1  - - - - - - - - -
 2  - - - - - - - - -
 3  - - - - - - - - -
 4  - - - - X - - - -
 5  - - - - - - - - -
 6  - - - - - - - - -
 7  - - - - - - - - -
 8  - - - - - - - - -

*/
const TablutValue thronePosition[1]
{
    44
};



class Tablut
{
    public:
        Tablut();
        ~Tablut();

        // Board game
        TablutValue board[DIM][DIM];

        void print();

        // Update table by one checker
        static Tablut update(Tablut &t, const TablutValue from_x, const TablutValue from_y, const TablutValue to_x, const TablutValue to_y);

        // Constructor from json
        static Tablut fromJson(const std::string &json);

        // Tablut with starting position set
        static Tablut newGame();
};


#endif