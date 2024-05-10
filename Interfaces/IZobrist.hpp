#ifndef ZOBRIST_H_I
#define ZOBRIST_H_I

template <class Key, class GameState>
class IZobrist
{
public:
    virtual ~IZobrist() {}
    virtual Key hash(const GameState &__game) = 0;
};

#endif