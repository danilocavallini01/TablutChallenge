#ifndef ABSTRACT_ALPHABETA_SEARCH_ENGINE
#define ABSTRACT_ALPHABETA_SEARCH_ENGINE

#include "../AbstractSearchEngine.hpp"
#include "../../Interfaces/Engine/IAlphaBetaEngine.hpp"

using namespace AI::Interface::Engine;
using namespace AI::Abstract;

namespace AI
{
    namespace Abstract
    {
        namespace Engine
        {
            template <typename G, typename H, typename Z, typename TT, typename M = StandardMove>
            class AbstractAlphaBetaEngine : public AbstractSearchEngine<G, H, Z, TT, M>, public IAlphaBetaEngine<G>
            {
            public:
                AbstractAlphaBetaEngine(int __maxDepth, int __quiescenceMaxDepth, H __heuristic, Z __zobrist, TT __table)
                    : AbstractSearchEngine<G, H, Z, TT, M>(__maxDepth, __quiescenceMaxDepth, false, __heuristic, __zobrist, __table){};

                virtual ~AbstractAlphaBetaEngine(){};
            };
        }

    }

}

#endif