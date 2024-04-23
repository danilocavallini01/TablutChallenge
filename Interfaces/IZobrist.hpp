#ifndef ZOBRIST_H_I
#define ZOBRIST_H_I

template <class Key>
class IZobrist
{
public:
    virtual ~IZobrist() {}
    virtual Key hash(const IGameBoard &__game) = 0;

};

#endif