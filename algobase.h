//algobase.h

#ifndef ALGOBASE_H
#define ALGOBASE_H

#include "configure.h"

STL_BEGIN_NAMESPACE

template<class Tp>
inline const Tp& min(const Tp &a,const Tp &b)
{
    return b < a ? b : a;
}

template<class Tp>
inline const Tp& max(const Tp& a,const Tp& b)
{
    return a < b ? b : a;
}

//fill()和fill_n()函数
//将first到last的元素设置为value
template<class ForwardIter,class Tp>
inline void fill(ForwardIter first,ForwardIter last,const Tp& value)
{
    for(;first != last;++first)
        *first = value;
}

//将first开始的n个元素赋值为value
template<class OutputIter,class Size,class Tp>
inline OutputIter fill_n(OutputIter first,Size n,const Tp& value)
{
    for(;n > 0;--n,++first)
        *first = value;
    return first;
}

//将first到last元素以result开始向后复制
template<class InputIter,class OutputIter>
inline OutputIter copy(InputIter first,InputIter last,OutputIter result)
{
    for(;first != last;++first,++result)
        *result = *first;
    return result;
}

//将first到last的元素以result开始向前复制
template<class InputIter,class OutputIter>
inline OutputIter copy_backward(InputIter first,InputIter last,OutputIter result)
{
    for(;first != last;)
        *--result = *--last;
    return result;
}

template<class InputIter,class Distance>
inline void distance(InputIter first,InputIter last,Distance& n)
{
    n = 0;
    while(first != last)
    {
        ++first;
        ++n;
    }
}


STL_END_NAMESPACE

#endif // ALGOBASE_H
