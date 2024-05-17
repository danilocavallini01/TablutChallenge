#ifndef I_NEGASCOUTENGINE_HPP
#define I_NEGASCOUTENGINE_HPP

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

            class INegaScoutEngine
            {
            public:
                virtual ~INegaScoutEngine(){};

                // NEGASCOUT SEARCH FUNCTION
                virtual int NegaScout(G &__currentMove, const int __depth, int __alpha, int __beta, const bool __color) = 0;
                virtual int NegaScoutTT(G &__currentMove, const int __depth, int __alpha, int __beta, const bool __color) = 0;
                virtual int NegaScoutTimeLimited(G &__currentMove, const int __depth, int __alpha, int __beta, const bool __color) = 0;
            };

        }
    }
}

#endif
