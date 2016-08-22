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

STL_END_NAMESPACE

#endif // CONSTRUCT_H
