#ifndef BASE_TRANSPOSITION_TABLE
#define BASE_TRANSPOSITION_TABLE

#include "Interfaces/ITranspositionTable.hpp"
#include "../Model/Zobrist.hpp"

#include <unordered_map>
#include <mutex>
#include <iostream>

#include <boost/unordered/concurrent_flat_map.hpp>

// Forward definition
class Zobrist;

namespace AI
{
    namespace Abstract
    {
        // Entry Flag enum for values EXACT,LOWEBOUND,PPERBOUND
        enum FLAG : uint8_t
        {
            EXACT = 0U,
            LOWERBOUND = 1U,
            UPPERBOUND = 2U,
        };

        /*
            Entry node of transposition table defined as follow:
                <SCORE,DEPTH,TT_FLAG,COLOR>
            Index for every element of the entry tuple
        */
        enum ENTRY : int
        {
            SCORE_INDEX = 0,
            DEPTH_INDEX = 1,
            FLAG_INDEX = 2,
            COLOR_INDEX = 3,
        };

        typedef std::tuple<int, int, FLAG, bool> Entry;

        template <typename TTEntry = Entry>
        class TranspositionTable : public ITranspositionTable<TTEntry, ZobristKey>
        {
        private:
            boost::unordered::concurrent_flat_map<ZobristKey, TTEntry> _map;
            unsigned int _cacheHit{0};
            unsigned int _cachePut{0};

        public:
            TranspositionTable() : _cacheHit(0), _cachePut(0)
            {
                _map = boost::unordered::concurrent_flat_map<ZobristKey, TTEntry>();
            }

            ~TranspositionTable() {}

            friend std::ostream &operator<<(std::ostream &out, const TranspositionTable &p)
            {
                out << "PERFORMANCE TT-> HITS: " << p._cacheHit << " ,PUTS: " << p._cachePut << ", TOTAL: " << p.cacheTotalAccess();
                return out;
            }

            /**
             * @brief Put the given entry in the underlying transposition table and associate it
             * to the given Zobrist Key
             *
             * @param t
             * @param __k
             */
            void put(TTEntry &t, ZobristKey &__k)
            {
                _map.try_emplace(__k, t);
            }

            /**
             * @brief Return the entry paired to the given Zobrist Key if present, if not
             * present only return an empty optional
             *
             * @param __k - the zobrist key
             * @return std::optional<TTEntry>
             */
            std::optional<TTEntry> get(const ZobristKey &__k)
            {
                std::optional<TTEntry> entry;
                _map.visit(__k, [&](const auto &__entry)
                           { entry = __entry.second; });
                return entry;
            }

            void clear()
            {
                _map.clear();
                resetStat();
            }

            void cacheHit()
            {
                _cacheHit++;
            }

            void cachePut()
            {
                _cachePut++;
            }

            int cacheTotalAccess() const
            {
                return _cachePut + _cacheHit;
            }

            void resetStat()
            {
                _cacheHit = 0;
                _cachePut = 0;
            }
        };

    }
}

#endif
