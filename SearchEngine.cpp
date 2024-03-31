#include "SearchEngine.h"
#include "MoveGenerator.h"

#include <cstring>
#include <future>

SearchEngine::SearchEngine()
{
    heuristic = Heuristic();
    zobrist = Zobrist();
    transpositionTable = TranspositionTable();
}

SearchEngine::SearchEngine(Heuristic &_heuristic, Zobrist &_zobrist, TranspositionTable &_transpositionTable)
{
    heuristic = _heuristic;
    zobrist = _zobrist;
    transpositionTable = _transpositionTable;
}

SearchEngine::~SearchEngine(){};

Tablut SearchEngine::NegaMaxSearch(Tablut &_startingPosition, const int _maxDepth, const int _color)
{
    maxDepth = _maxDepth;

    std::vector<Tablut> moves;
    Tablut move;
    std::vector<std::future<int>> results;

    int alpha = BOTTOM_SCORE;
    int beta = TOP_SCORE;

    score = BOTTOM_SCORE;

    MoveGenerator::generateLegalMoves(_startingPosition, moves);
    heuristic.sortMoves(moves);

    int maxThread = 16;

    for (int t = 0; t < moves.size(); t += maxThread)
    {
        for (int i = 0; i < maxThread && i + t < moves.size(); i++)
        {
            results.push_back(std::async(std::launch::async, &SearchEngine::NegaMax, std::ref(*this), std::ref(moves[i + t]), _maxDepth - 1, -beta, -alpha, -_color));
        }

        for (int i = 0; i < results.size(); i++)
        {
            score = std::max(score, -results[i].get());
            alpha = std::max(alpha, score);

            if (alpha >= beta)
            {
                return moves[i + t];
            }
        }

        results.clear();
    }

    return moves[0];
}

Tablut SearchEngine::NegaScoutSearch(Tablut &_startingPosition, const int _maxDepth)
{
    maxDepth = _maxDepth;

    std::vector<Tablut> moves;
    Tablut move;
    std::vector<std::future<int>> results;

    int alpha = BOTTOM_SCORE;
    int beta = TOP_SCORE;

    score = BOTTOM_SCORE;
    int b = beta;
    int v;

    MoveGenerator::generateLegalMoves(_startingPosition, moves);
    heuristic.sortMoves(moves);

    int maxThread = 16;

    for (int t = 0; t < moves.size(); t += maxThread)
    {
        for (int i = 0; i < maxThread && i + t < moves.size(); i++)
        {
            move = moves[i + t];
            results.push_back(std::async(std::launch::async, &SearchEngine::NegaScout, std::ref(*this), std::ref(move), _maxDepth - 1, -b, -alpha));
        }

        for (int i = 0; i < results.size(); i++)
        {
            v = -results[i].get();

            if (v > score)
            {
                score = v;
            }

            alpha = std::max(alpha, v);

            if (alpha >= beta)
            {
                return moves[i + t];
            }

            b = alpha + 1;
        }

        results.clear();
    }

    return moves[0];
}

int SearchEngine::NegaScout(Tablut &_prev_move, const int _depth, int _alpha, int _beta)
{
    const int alphaOrigin = _alpha;
    int score;
    int b;
    int v;

    std::vector<Tablut> moves;
    Tablut move;

    // -------- TRANSPOSITION TABLE LOOKUP --------
    ZobristKey hash = zobrist.hash(_prev_move);
    std::optional<Entry> maybe_entry = transpositionTable.get(hash);

    Entry tt_entry;

    if (maybe_entry.has_value())
    {
        tt_entry = maybe_entry.value();
        int tt_depth = std::get<ENTRY::DEPTH_INDEX>(tt_entry);

        if (tt_depth >= _depth)
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

    if (_prev_move.isGameOver() || _depth == 0)
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
        tt_entry = std::make_tuple(score, _depth, FLAG::UPPERBOUND);
    }
    else if (score >= b)
    {
        tt_entry = std::make_tuple(score, _depth, FLAG::LOWERBOUND);
    }
    else
    {
        tt_entry = std::make_tuple(score, _depth, FLAG::EXACT);
    }

    transpositionTable.put(tt_entry, hash);

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
    ZobristKey hash = zobrist.hash(_prev_move);
    std::optional<Entry> maybe_entry = transpositionTable.get(hash);

    Entry tt_entry;

    if (maybe_entry.has_value())
    {
        transpositionTable.cacheHit();
        tt_entry = maybe_entry.value();
        int tt_depth = std::get<ENTRY::DEPTH_INDEX>(tt_entry);

        if (tt_depth >= _depth)
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

    if (_prev_move.isGameOver() || _depth == 0)
    {
        return heuristic.evaluate(_prev_move);
    }

    MoveGenerator::generateLegalMoves(_prev_move, moves);
    heuristic.sortMoves(moves);
    score = BOTTOM_SCORE;

    for (int i = 0; i < moves.size(); i++)
    {
        score = std::max(score, -NegaMax(moves[i], _depth - 1, -_beta, -_alpha, -_color));
        _alpha = std::max(_alpha, score);
        if (_alpha >= _beta)
        {
            break;
        }
    }

    // -------- TRANSPOSITION TABLE PUT --------
    if (score <= alphaOrigin)
    {
        tt_entry = std::make_tuple(score, _depth, FLAG::UPPERBOUND);
    }
    else if (score >= _beta)
    {
        tt_entry = std::make_tuple(score, _depth, FLAG::LOWERBOUND);
    }
    else
    {
        tt_entry = std::make_tuple(score, _depth, FLAG::EXACT);
    }

    transpositionTable.put(tt_entry, hash);
    transpositionTable.cachePut();

    return score;
}
