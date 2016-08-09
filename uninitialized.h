//stl_uninitialized.h

#ifndef STL_UINITIALIZED_H
#define STL_UINITIALIZED_H

#include "configure.h"

STL_BEGIN_NAMESPACE

#include "construct.h"

//这个函数会将first到last的数据区域复制到result开始区域，
//返回被复制区域的结束位置。
//注意:这个函数是不安全的，没有对数据重叠区进行保护
//这个函数是操作finish之后未初始化的区域
template<class InputIter,class ForwardIter>
inline ForwardIter uninitialized_copy(InputIter first,InputIter last,
                                      ForwardIter result)
{
    ForwardIter cur = result;
    STL_TRY
    {
        for(;first != last;++first,++cur)
            construct(&*cur,*first);    //这里为什么是&*cur,而不是cur
        return cur;
    }
    STL_UNWIND(destroy(result,cur));
}

//这个函数运行在finish之前已经初始化的区域
//template<class InputIter,class ForwardIter>
//inline ForwardIter copy(InputIter first,InputIter last,ForwardIter result)
//{

//}

//从first迭代器开始，将n元素使用x进行初始化。
template<class ForwardIter,class Size,class Tp>
inline ForwardIter uninitialized_fill_n(ForwardIter first,Size n,const Tp& x)
{
    ForwardIter cur = first;

    STL_TRY
    {
        for(;n > 0;--n,++cur)
            construct(&*cur,x);
        return cur;
    }
    STL_UNWIND(destroy(first,cur));
}

STL_END_NAMESPACE

#endif // STL_UINITIALIZED_H
