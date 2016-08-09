//construct.h

#ifndef CONSTRUCT_H
#define CONSTRUCT_H

#include "configure.h"
#include <new>

STL_BEGIN_NAMESPACE

template <class T1,class T2>
inline void construct(T1 *p,const T2& value)
{
    //placement new：在给定地址调用对象的构造函数，并不申请内存
    new((void*)p) T1(value);
}

template <class T>
inline void construct(T* p)
{
     new ((void*)p) T();
}

template <class T>
inline void destroy(T* pointer)
{
    pointer->~T();
}

template <class ForwardIterator>
inline void destroy(ForwardIterator first,ForwardIterator last)
{
    for ( ; first != last; ++first)
      destroy(&*first);
}

inline void destroy(char *,char*) {}
inline void destroy(int *,int *) {}
inline void destroy(long *,long *) {}
inline void destroy(float *,float *) {}
inline void destroy(double*,double*) {}

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

#endif // CONSTRUCT_H
