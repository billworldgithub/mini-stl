#ifndef STL_CONF_H
#define STL_CONF_H

//库空间
#define STL_BEGIN_NAMESPACE namespace mini_stl{
#define STL_END_NAMESPACE   }

using std::cout;
using std::endl;

//内存分配器
//#define STL_USE_STD_ALLOCATORS

#ifndef STL_DEFAULT_ALLOCATOR
#   ifdef STL_USE_STD_ALLOCATORS
#       define STL_DEFAULT_ALLOCATOR(T) allocator<T>
#   else
#       define STL_DEFAULT_ALLOCATOR(T) alloc
#   endif
#endif

//异常处理
#define STL_USE_EXCEPTIONS
#ifdef STL_USE_EXCEPTIONS
#   define STL_TRY try
#   define STL_CATCH_ALL catch(...)
#   define STL_THROW(x) throw x
#   define STL_RETHROW throw
#   define STL_NOTHROW throw()
#   define STL_UNWIND(action) catch(...) {action;throw;}
#else
#   define STL_TRY
#   define STL_CATCH_ALL if(false)
#   define STL_THROW(x)
#   define STL_RETHROW
#   define STL_NOTHROW
#   define STL_UNWIND(action)
#endif

#define STL_CLASS_PARTIAL_SPECIALIZATION

#if defined(STL_CLASS_PARTIAL_SPECIALIZATION) || defined(STL_PARTIAL_SPECIALIZATION_SYNTAX)
#   define STL_TEMPLATE_NULL template<>
#else
#   define STL_TEMPLATE_NULL
#endif


#endif // STL_CONF_H
