#ifndef TABLUT_NEGA_SCOUT_ENGINE
#define TABLUT_NEGA_SCOUT_ENGINE

#include "Lib/Model/Engine/NegaScoutEngine.hpp"

#include "Lib/Tablut/Tablut.hpp"
#include "Lib/Tablut/MoveGenerator.hpp"
#include "Lib/Tablut/Heuristic.hpp"
#include "Lib/Tablut/Zobrist.hpp"

// FORWARD DECLARATION
class Tablut;
class Heuristic;
class Zobrist;
class MoveGenerator;

using namespace AI::Engine;

// INSTANCE OF NEGASCOUT CLASS WITH THE TABLUT SPECIFIC CLASSES
class TNegaScoutEngine : public NegaScoutEngine<Tablut, Heuristic, MoveGenerator, Zobrist>
{
public:
    TNegaScoutEngine(int __maxDepth, int __quiescenceMaxDepth, Heuristic __heuristic, Zobrist __zobrist, TranspositionTable<Entry> __table = TranspositionTable<Entry>()) : NegaScoutEngine<Tablut, Heuristic, MoveGenerator, Zobrist>(__maxDepth, __quiescenceMaxDepth, __heuristic, __zobrist, __table){};

    ~TNegaScoutEngine(){};
};

#endif