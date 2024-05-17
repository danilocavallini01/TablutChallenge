#ifndef I_ALPHABETA_ENGINE_HPP
#define I_ALPHABETA_ENGINE_HPP

namespace AI
{
    namespace Interface
    {
        namespace Engine
        {
            /**
             * @brief Engine Interface Typed parameters
             *
             * @tparam G
             */
            template <typename G>

            class IAlphaBetaEngine
            {
            public:
                virtual ~IAlphaBetaEngine(){};

                // NEGASCOUT SEARCH FUNCTION
                virtual int AlphaBeta(G &__currentMove, const int __depth, int __alpha, int __beta, const bool __color) = 0;
                virtual int AlphaBetaTT(G &__currentMove, const int __depth, int __alpha, int __beta, const bool __color) = 0;
                virtual int AlphaBetaTimeLimited(G &__currentMove, const int __depth, int __alpha, int __beta, const bool __color) = 0;
            };

        }
    }
}

#endif
