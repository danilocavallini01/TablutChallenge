#ifndef ABSTRACT_NEGASCOUT_SEARCH_ENGINE
#define ABSTRACT_NEGASCOUT_SEARCH_ENGINE

#include "../AbstractSearchEngine.hpp"
#include "../../Interfaces/Engine/INegaScoutEngine.hpp"

using namespace AI::Interface::Engine;
using namespace AI::Abstract;

namespace AI
{
    namespace Abstract
    {
        namespace Engine
        {
            template <typename G, typename H, typename MG, typename Z, typename TT, typename M = StandardMove>
            class AbstractNegaScoutEngine : public AbstractSearchEngine<G, H, MG, Z, TT, M>, public INegaScoutEngine<G>
            {
            public:
                AbstractNegaScoutEngine(int __maxDepth, int __quiescenceMaxDepth, H __heuristic, Z __zobrist, TT __table)
                    : AbstractSearchEngine<G, H, MG, Z, TT, M>(__maxDepth, __quiescenceMaxDepth, true, __heuristic, __zobrist, __table){};

                virtual ~AbstractNegaScoutEngine(){};
            };
        }

    }

}

#endif