//alloc_imp.h
//实现内存分配和释放

#ifndef ALLOC_IMP_H
#define ALLOC_IMP_H

#include "configure.h"
#include <stdlib.h>

STL_BEGIN_NAMESPACE

//分配器错误
#ifndef THROW_BAD_ALLOC
#   if defined(STL_NO_BAD_ALLOC) || !defined(STL_USE_EXCEPTIONS)
#       include <stdio.h>
#       include <stdlib.h>
#       define THROW_BAD_ALLOC fprintf(stderr,"out of memory\n");exit(1)
#   else
#       include <new>
#       define THROW_BAD_ALLOC throw std::bad_alloc()
#   endif
#endif

///////////////////////////
//MALLOC分配释放器
template <int inst> //非类型形参
class malloc_alloc_template
{
private:
    //错误处理函数
    static void* s_oom_malloc(size_t);
    static void* s_oom_realloc(void*,size_t);

public:
    //分配内存
    static void* allocate(size_t n)
    {
        void* result = malloc(n);
        if(result == 0)
            result = s_oom_malloc(n);

        return result;
    }

    //调整内存
    static void deallocate(void *p,size_t n)
    {
        free(p);
    }

    //重新分配内存
    static void reallocate(void *p,size_t old_sz,size_t new_sz)
    {
        void* result = realloc(p,new_sz);
        if(result == 0)
            result = s_oom_realloc(p,new_sz);

        return result;
    }
};

template <int inst>
void* malloc_alloc_template<inst>::s_oom_malloc(size_t n)
{
    //这里做了简化
    THROW_BAD_ALLOC;
    return 0;
}

template <int inst>
void* malloc_alloc_template<inst>::s_oom_realloc(void* p,size_t n)
{
    //这里做了简化
    THROW_BAD_ALLOC;
    return 0;
}

//这里没有搞明白为什么一定要使用模板，使用类也可以
typedef malloc_alloc_template<0> malloc_alloc;
typedef malloc_alloc alloc;



//简单地内存分配释放器封装
template<class Tp,class Alloc>
class simple_alloc
{
public:

    //分配n个Tp类型对象的内存空间
    static Tp* allocate(size_t n)
    {
        return n == 0 ? 0 : (Tp*) Alloc::allocate(n * sizeof(Tp));
    }

    //分配1个Tp类型对象的内存空间
    static Tp* allocate(void)
    {
        return (Tp*) Alloc::allocate(sizeof(Tp));
    }

    //释放n个Tp类型对象的内存空间
    static void deallocate(Tp* p,size_t n)
    {
        if(n != 0)
            Alloc::deallocate(p,n * sizeof(Tp));
    }

    //释放1个Tp类型对象的内存空间
    static void deallocate(Tp *p)
    {
        Alloc::deallocate(p,sizeof(Tp));
    }
};

STL_END_NAMESPACE


#endif // ALLOC_IMP_H
