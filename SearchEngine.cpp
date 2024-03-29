#include <cstring>

#include "SearchEngine.h"
#include "Heuristic.h"
#include "MoveGenerator.h"

SearchEngine::SearchEngine()
{
    heuristic = Heuristic();
    zobrist = Zobrist();
    transpositionTable = TranspositionTable();
}

SearchEngine::~SearchEngine(){};

int SearchEngine::NegaScout(Tablut &_prev_move, const int _depth, int _alpha, int _beta)
{
    const int alphaOrigin = _alpha;
    int score;
    int b;
    int v;
    std::vector<Tablut> moves;
    Tablut move;

    // -------- TRANSPOSITION TABLE LOOKUP --------
    const ZobristKey hash = zobrist.hash(_prev_move);
    const std::optional<Entry> maybe_entry = transpositionTable.get(hash);

    Entry tt_entry;

    if (maybe_entry.has_value())
    {
        transpositionTable.cacheHit();
        tt_entry = maybe_entry.value();

        if (std::get<ENTRY::DEPTH_INDEX>(tt_entry) >= _depth)
        {
            FLAG tt_entry_flag = std::get<ENTRY::FLAG_INDEX>(tt_entry);
            int tt_score = std::get<ENTRY::SCORE_INDEX>(tt_entry);

            if (tt_entry_flag == FLAG::EXACT)
            {
                return tt_score;
            }
            else if (tt_entry_flag == FLAG::LOWERBOUND)
            {
                _alpha = std::max(_alpha, tt_score);
            }
            else
            {
                _beta = std::min(_beta, tt_score);
            }

            if (_alpha >= _beta)
            {
                return tt_score;
            }
        }
    }
    // --------TRANSPOSITION TABLE LOOKUP -------- END

    if (_depth == 0 || _prev_move.isGameOver())
    {
        return heuristic.evaluate(_prev_move);
    }

    score = BOTTOM_SCORE;
    b = _beta;

    MoveGenerator::generateLegalMoves(_prev_move, moves);
    heuristic.sortMoves(moves);

    for (int i = 0; i < moves.size(); i++)
    {
        move = moves[i];
        v = -SearchEngine::NegaScout(move, _depth - 1, -b, -_alpha);

        if (v > _alpha && v < _beta && i > 0)
        {
            v = -SearchEngine::NegaScout(move, _depth - 1, -_beta, -v);
        }

        if (v > score)
        {
            score = v;
            bestmove[_depth] = move;
        }

        _alpha = std::max(_alpha, v);

        if (_alpha >= _beta)
        {
            break;
        }

        b = _alpha + 1;
    }

    // -------- TRANSPOSITION TABLE PUT --------
    if (score <= alphaOrigin)
    {
        tt_entry = std::make_tuple(score, _depth, FLAG::UPPERBOUND, bestmove[_depth]);
    }
    else if (score >= b)
    {
        tt_entry = std::make_tuple(score, _depth, FLAG::LOWERBOUND, bestmove[_depth]);
    }
    else
    {
        tt_entry = std::make_tuple(score, _depth, FLAG::EXACT, bestmove[_depth]);
    }

    transpositionTable.put(tt_entry, hash);
    transpositionTable.cachePut();

    // -------- TRANSPOSITION TABLE PUT -------- END

    return score;
}

int SearchEngine::NegaMax(Tablut &_prev_move, const int _depth, int _alpha, int _beta, int _color)
{
    const int alphaOrigin = _alpha;
    int score;
    std::vector<Tablut> moves;
    Tablut move;

    // -------- TRANSPOSITION TABLE LOOKUP --------
    const ZobristKey hash = zobrist.hash(_prev_move);
    const std::optional<Entry> maybe_entry = transpositionTable.get(hash);

    Entry tt_entry;

    if (maybe_entry.has_value())
    {
        transpositionTable.cacheHit();
        tt_entry = maybe_entry.value();

        if (std::get<ENTRY::DEPTH_INDEX>(tt_entry) >= _depth)
        {
            FLAG tt_entry_flag = std::get<ENTRY::FLAG_INDEX>(tt_entry);
            int tt_score = std::get<ENTRY::SCORE_INDEX>(tt_entry);

            if (tt_entry_flag == FLAG::EXACT)
            {
                return tt_score;
            }
            else if (tt_entry_flag == FLAG::LOWERBOUND)
            {
                _alpha = std::max(_alpha, tt_score);
            }
            else
            {
                _beta = std::min(_beta, tt_score);
            }

            if (_alpha >= _beta)
            {
                return tt_score;
            }
        }
    }
    // --------TRANSPOSITION TABLE LOOKUP -------- END

    if (_depth == 0 || _prev_move.isGameOver())
    {
        return heuristic.evaluate(_prev_move);
    }

    MoveGenerator::generateLegalMoves(_prev_move, moves);
    heuristic.sortMoves(moves);
    score = BOTTOM_SCORE;

    bestmove[_depth] = moves[0];

    for (int i = 0; i < moves.size(); i++)
    {
        score = std::max(score, -NegaMax(moves[i], _depth - 1, -_beta, -_alpha, -_color));
        _alpha = std::max(_alpha, score);
        if (_alpha >= _beta)
        {
            bestmove[_depth] = moves[i];
            break;
        }
    }

    // -------- TRANSPOSITION TABLE PUT --------
    if (score <= alphaOrigin)
    {
        tt_entry = std::make_tuple(score, _depth, FLAG::UPPERBOUND, bestmove[_depth]);
    }
    else if (score >= _beta)
    {
        tt_entry = std::make_tuple(score, _depth, FLAG::LOWERBOUND, bestmove[_depth]);
    }
    else
    {
        tt_entry = std::make_tuple(score, _depth, FLAG::EXACT, bestmove[_depth]);
    }

    transpositionTable.put(tt_entry, hash);
    transpositionTable.cachePut();

    return score;
}
