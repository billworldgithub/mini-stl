//string.h
//实现string
#ifndef STRING_IMP_H
#define STRING_IMP_H

#include "configure.h"
#include "alloc_imp.h"
#include "construct.h"
#include "uninitialized.h"
#include "algobase.h"
#include "algo.h"
#include <string.h>

STL_BEGIN_NAMESPACE

//////////////////////////////////////////
///string内存管理接口
///
template<class Tp,class Alloc>
class string_base
{
public:
    //内存分配器接口
    typedef Alloc allocator_type;
    allocator_type get_allocator() const
    {
        return allocator_type();
    }

protected:
    //分配器成员
    typedef simple_alloc<Tp,Alloc> alloc_type;

    //内存指针
    Tp *start;
    Tp *finish;
    Tp *end_of_storage;

    //分配n个元素空间
    Tp *allocate(size_t n)
    {
        return alloc_type::allocate(n);
    }

    //释放n个元素空间
    void deallocate(Tp *p,size_t n)
    {
        if(p)
            alloc_type::deallocate(p,n);
    }

    //为对象分配n个元素内存
    void allocate_block(size_t n)
    {
        if(n <= max_size())
        {
            start = allocate(n);
            finish = start;
            end_of_storage = start + n;
        }
        else
        {
            throw_length_error();
        }
    }

    //释放对象的内存空间
    void deallocate_block()
    {
        deallocate(start,end_of_storage - start);
    }

    //能够申请的最大元素个数
    size_t max_size() const
    {
        return (size_t(-1) / sizeof(Tp)) - 1;
    }

    string_base(const allocator_type &)
        : start(0),finish(0),end_of_storage(0) {}

    string_base(const allocator_type &,size_t n)
        : start(0),finish(0),end_of_storage(0)
    {
        allocate_block(n);
    }

    ~string_base()
    {
        deallocate_block();
    }

    void throw_length_error() const;
    void throw_out_of_range() const;
};

template<class Tp,class Alloc>
void string_base<Tp,Alloc>::throw_length_error() const
{
}

template<class Tp,class Alloc>
void string_base<Tp,Alloc>::throw_out_of_range() const
{
}

///////////////////////////////////////////////////////
///string实现
///
template<class CharT,class Alloc>
class basic_string : private string_base<CharT,Alloc>
{
public:
    //公共类型
    typedef CharT value_type;
    typedef value_type* pointer;
    typedef const value_type* const_pointer;
    typedef value_type& reference;
    typedef const value_type& const_reference;
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;
    typedef value_type* iterator;
    typedef const value_type* const_iterator;

    static const size_type npos;

    typedef string_base<CharT,Alloc> Base;
    typedef typename Base::allocator_type allocator_type;
    allocator_type get_allocator() const
    {
        return Base::get_allocator();
    }

protected:
    using Base::start;
    using Base::finish;
    using Base::end_of_storage;

    using Base::allocate;
    using Base::deallocate;
    using Base::allocate_block;
    using Base::deallocate_block;
    using Base::throw_length_error;
    using Base::throw_out_of_range;


public:
    //默认构造函数
    explicit basic_string(const allocator_type &a = allocator_type())
        : Base(a,8)
    {
        terminate_string();
    }

//    struct reserve_t {};
//    basic_string(reserve_t,size_t n,const allocator_type &a = allocator_type())
//        : Base(a,n+1)
//    {
//        terminate_string();
//    }

    //默认复制构造函数，这里没有使用explicit
    basic_string(const basic_string &s) : Base(s.get_allocator())
    {
        range_initialize(s.begin(),s.end());
    }

    //使用s从pos的n个字符构造字符串
    basic_string(const basic_string &s,size_type pos,size_type n = npos,
                 const allocator_type &a = allocator_type())
        : Base(a)
    {
        if(pos > s.size())
            throw_out_of_range();
        else
            range_initialize(s.begin()+pos,s.begin()+pos+min(n,s.size()-pos));
    }

    //使用s的n个字符构造字符串
    basic_string(const CharT *s,size_type n,const allocator_type &a = allocator_type())
        : Base(a)
    {
        range_initialize(s,s+n);
    }

    //使用s构造字符串
    basic_string(const CharT* s,const allocator_type& a = allocator_type())
        : Base(a)
    {
        range_initialize(s,s+strlen(s));
    }

    //使用n个c字符构建字符串
    basic_string(size_type n,CharT c,const allocator_type& a = allocator_type())
        : Base(a,n+1)
    {
        finish = uninitialized_fill_n(start,n,c);
        terminate_string();
    }

    //使用f到l构建字符串
    basic_string(const CharT* f,const CharT* l,const allocator_type& a = allocator_type())
        : Base(a)
    {
        range_initialize(f,l);
    }

    //析构函数
    ~basic_string()
    {
        destroy(start,finish+1);
        //内存有Base::~Base()释放
    }

    //赋值操作符
    basic_string& operator =(const basic_string& s)
    {
        if(&s != this)
            assign(s.begin(),s.end());
        return *this;
    }

    basic_string& operator =(CharT* s)
    {
        return assign(s,s+strlen(s));
    }

    basic_string& operator=(CharT c)
    {
        return assign(static_cast<size_type>(1),c);
    }

private:

    //构建一个空字符，并使p指向这个字符
    void construct_null(CharT *p)
    {
        construct(p);
    }

    static CharT null()
    {
        return CharT();
    }

    void terminate_string()
    {
        STL_TRY
        {
            construct_null(finish);
        }
        STL_UNWIND(destroy(start,finish));
    }

    void range_initialize(const CharT *f,const CharT *l)
    {
        ptrdiff_t n = l - f;
        allocate_block(n+1);
        finish = uninitialized_copy(f,l,start);
        terminate_string();
    }



public:
    //获取迭代器
    iterator begin() {return start;}
    iterator end() {return finish;}
    const_iterator begin() const {return start;}
    const_iterator end() const {return finish;}

    //反向迭代器rbegin(),rend()

public:
    size_type size() const {return (finish - start);}
    size_type length() const {return size();}
    size_t max_size() const {return Base::max_size();}

    //改变大小
    void resize(size_type n,CharT c)
    {
        if(n <= size())
            erase(begin()+n,end());
        else
            append(n-size(),c);
    }

    void resize(size_type n)
    {
        resize(n,null());
    }

    void reserve(size_type n = 0);

    //有效空间大小
    size_t capacity() const {return (end_of_storage - start - 1);}

    //清空字符串
    void clear()
    {
        if(!empty())
        {
            *start = null();
            destroy(start+1,finish+1);
            finish = start;
        }
    }

    //判断字符串是否为空
    bool empty()
    {
        return start == finish;
    }

public:

    //根据索引访问元素
    const_reference operator[](size_type n) const
    {
        return *(start+n);
    }

    reference operator[](size_type n)
    {
        return *(start+n);
    }

    const_reference at(size_type n) const
    {
        if(n >= size())
            throw_out_of_range();

        return *(start+n);
    }

    reference at(size_type n)
    {
        if(n >= size())
            throw_out_of_range();

        return *(start+n);
    }

public:
    basic_string& operator +=(const basic_string& s)
    {
        return append(s);
    }

    basic_string& operator +=(const CharT * s)
    {
        return append(s);
    }

    basic_string& operator+=(CharT c)
    {
        push_back(c);
        return *this;
    }

    basic_string& append(const basic_string& s)
    {
        return append(s.begin(),s.end());
    }

    basic_string& append(const basic_string& s,size_type pos,size_type n)
    {
        if(pos > s.size())
            throw_out_of_range();

        return append(s.begin()+pos,s.begin()+pos+min(n,s.size()-pos));
    }

    basic_string& append(const basic_string &s,size_type n)
    {
        return append(s,s+n);
    }

    basic_string& append(const CharT* s)
    {
        return append(s,s+strlen(s));
    }

    basic_string& append(const CharT* first,const CharT* last)
    {
        if(first != last)
        {
            const size_type old_size = size();
            ptrdiff_t n = last - first;

            //n太大，没有足够的空间
            if(n > max_size() || old_size > max_size() - n)
                throw_length_error();

            if(old_size+n > capacity())
            {
                //需要扩充内存空间
                const size_type len = old_size + max(old_size,(size_t)n)+1;
                pointer new_start = allocate(len);
                pointer new_finish = new_start;

                STL_TRY
                {
                    new_finish = uninitialized_copy(start,finish,new_start);
                    new_finish = uninitialized_copy(first,last,new_finish);
                    construct_null(new_finish);
                }
                STL_UNWIND((destroy(new_start,new_finish),deallocate(new_start,len)));

                destroy(start,finish);
                deallocate_block();

                start = new_start;
                finish = new_finish;
                end_of_storage = new_start + len;
            }
            else
            {
                //具有足够的空间
               finish = uninitialized_copy(first,last,finish);
               construct_null(finish);
            }
        }
        return *this;
    }

    basic_string& append(size_type n,CharT c)
    {
        //长度太长
        if(n > max_size() || size() > max_size() - n)
            throw_length_error();

        //内存空间不足，调整内存大小
        if(size() + n > capacity())
            reserve(size() + max(size(),n));

        if(n > 0)
        {
            finish = uninitialized_fill_n(finish,n,c);
            construct_null(finish);
        }

        return *this;
    }

    void push_back(CharT c)
    {
        if(finish + 1 == end_of_storage)
            reserve(size()+max(size(),static_cast<size_type>(1)));

        construct_null(finish+1);
        *finish = c;
        ++finish;
    }

    void pop_back()
    {
        *(finish-1) = null();
        destroy(finish);
        --finish;
    }

public:
    basic_string& assign(const basic_string& s)
    {
        return assign(s.begin(),s.end());
    }

    basic_string& assign(const basic_string &s,size_type pos,size_type n)
    {
        if(pos > s.size())
            throw_out_of_range();

        return assign(s.begin()+pos,s.begin()+pos+min(n,s.size()-pos));
    }

    basic_string& assign(const CharT* s,size_type n)
    {
        return assign(s,s+n);
    }

    basic_string& assign(const CharT* s)
    {
        return assign(s,s+strlen(s));
    }

    basic_string& assign(const CharT* f,const CharT* l)
    {
        const ptrdiff_t n = l - f;
        if(static_cast<size_type>(n) <= size())
        {
            memcpy(start,f,n);
            erase(start+n,finish);
        }
        else
        {
            memcpy(start,f,size());
            append(f+size(),l);
        }
    }

    basic_string& assign(size_type n,CharT c)
    {
        if(n <= size())
        {
            //具有足够的空间
            memset(start,c,n);
            erase(start+n,finish);
        }
        else
        {
            //没有足够的空间
            memset(start,c,size());
            append(n-size(),c);
        }

    }

public: //Insert
    basic_string& insert(size_type pos,const basic_string& s)
    {
        if(pos > size())
            throw_out_of_range();

        if(size() > max_size() - s.length())
            throw_length_error();

        insert(start+pos,s.begin(),s.end());

        return *this;
    }

    basic_string& insert(size_type pos,const basic_string& s,
                         size_type beg,size_type n)
    {
        if(pos > size() || beg > s.size())
            throw_out_of_range();

        size_type len = min(n,s.size()-beg);

        if(size() > max_size() - len)
            throw_length_error();

        insert(start+pos,s.begin()+beg,s.begin()+beg+len);

        return *this;
    }

    basic_string& insert(size_type pos,const CharT* s,size_type n)
    {
        if(pos > size())
            throw_out_of_range();

        if(size() > max_size() - n)
            throw_length_error();

        insert(start+pos,s,s+n);
        return *this;
    }

    basic_string& insert(size_type pos,const CharT* s)
    {
        if(pos > size())
            throw_out_of_range();

        size_type len = strlen(s);

        if(size() > max_size() - len)
            throw_length_error();

        insert(start+pos,s,s+len);
        return *this;
    }

    basic_string& insert(size_type pos,size_type n,CharT c)
    {
        if(pos > size())
            throw_out_of_range();

        if(size() > max_size() - n)
            throw_length_error();

        insert(start+pos,n,c);
    }

    iterator insert(iterator p,CharT c)
    {
        if(p == finish)
        {
            push_back(c);
            return finish - 1;
        }
        else
            insert_aux(p,c);
    }

    iterator insert_aux(iterator,CharT);

    void copy(const CharT* first,const CharT* last,CharT* result)
    {
        memcpy(result,first,last - first);
    }

    void insert(iterator p,size_t n,CharT c);
    void insert(iterator p,const CharT* first,const CharT* last);



public: //erase

    basic_string&  erase(size_type pos = 0,size_type n = npos)
    {
        if(pos > size())
            throw_out_of_range();

        erase(start+pos,start+pos+min(n,size()-pos));
        return *this;
    }

    iterator erase(iterator position)
    {
        memmove(position,position+1,finish-position);
        destroy(finish);
        --finish;
        return position;
    }

    iterator erase(iterator first,iterator last)
    {
        if(first != last)
        {
            //复制last后面的字符串包括'\0'
            memcpy(first,last,(finish-last)+1);
            const iterator new_finish = finish - (last - first);
            destroy(new_finish + 1,finish + 1);
            finish = new_finish;
        }

        return first;
    }

public: //replace

    basic_string& replace(size_type pos,size_type n,const basic_string& s)
    {
        if(pos > size())
            throw_out_of_range();

        const size_type len = min(n,size()-pos);

        if(size() - len >= max_size() - s.size())
            throw_length_error();

        return replace(start+pos,start+pos+len,s.begin(),s.end());
    }

    basic_string& replace(size_type pos1,size_type n1,
                          const basic_string& s,
                          size_type pos2,size_type n2)
    {
        if(pos1 > size() || pos2 > s.size())
            throw_out_of_range();

        const size_type len1 = min(n1,size()-pos1);
        const size_type len2 = min(n2,s.size()-pos2);

        if(size() - len1 >= max_size() - len2)
            throw_length_error();

        return replace(start+pos1,start+pos1+len1,
                       s.start+pos2,s.start+pos2+len2);

    }

    basic_string& replace(size_type pos,size_type n1,
                          const CharT* s,size_type n2)
    {
        if(pos > size())
            throw_out_of_range();

        const size_type len = min(n1,size() - pos);

        if(n2 > max_size() || size() >= max_size()-n2)
            throw_length_error();

        return replace(start+pos,start+pos+len,s,s+n2);
    }

    basic_string& replace(size_type pos,size_type n1,
                          const CharT* s)
    {
        if(pos > size())
            throw_out_of_range();

        const size_type len = min(n1,size()-pos);
        const size_type n2 = strlen(s);

        if(n2 > max_size() || size()-len >= max_size() - n2)
            throw_length_error();

        return replace(start+pos,start+pos+len,s,s+n2);
    }

    basic_string& replace(size_type pos,size_type n1,
                          size_type n2,CharT c)
    {
        if(pos > size())
            throw_out_of_range();

        const size_type len = min(n1,size()-pos);

        if(n2 > max_size() || size() - len >= max_size() - n2)
            throw_length_error();

        return replace(start+pos,start+pos+len,n2,c);
    }

    basic_string& replace(iterator first,iterator last,
                          const basic_string& s)
    {
        return replace(first,last,s.begin(),s.end());
    }

    basic_string& replace(iterator first,iterator last,
                          const CharT* s,size_type n)
    {
        return replace(first,last,s,s+n);
    }

    basic_string& replace(iterator first,iterator last,
                          const CharT* s)
    {
        return replace(first,last,s,s+strlen(s));
    }


    basic_string& replace(iterator first,iterator last,
                          size_type n,CharT c);

    basic_string& replace(iterator first,iterator last,
                          const CharT* f,const CharT* l);

public:
    const CharT* c_str() const
    {
        return start;
    }

    const CharT* data() const
    {
        return start;
    }

public: //find

    size_type find(const basic_string& s,size_type pos = 0) const
    {
        return find(s.begin(),pos,s.size());
    }

    size_type find(const CharT* s,size_type pos = 0)
    {
        return find(s,pos,strlen(s));
    }

    size_type find(const CharT* s,size_type pos,size_type n) const;
    size_type find(CharT c,size_type pos) const;

public: //rfind找到最后一个匹配项目
    size_type rfind(const basic_string& s,size_type pos = npos) const
    {
        return rfind(s.begin(),pos,s.size());
    }

    size_type rfind(const CharT* s,size_type pos = npos) const
    {
        return rfind(s,pos,strlen(s));
    }

    size_type rfind(const CharT* s,size_type pos,size_type n) const;
    size_type rfind(CharT c,size_type pos = npos) const;

public: //find_first_of找到第一个在s中任意一个元素匹配的数据
    size_type find_first_of(const basic_string& s,size_type pos = 0) const
    {
        return find_first_of(s.begin(),pos,s.size());
    }

    size_type find_first_of(const CharT* s,size_type pos = 0) const
    {
        return find_first_of(s,pos,strlen(s));
    }

    size_type find_first_of(const CharT* s,size_type pos,size_type n) const;
    size_type find_first_of(CharT c,size_type pos = 0) const
    {
        return find(c,pos);
    }

public: //find_last_of
#if 0
    size_type find_last_of(const basic_string& s,size_type pos = npos) const
    {
        return find_last_of(s.begin(),pos,s.size());
    }

    size_type find_last_of(const CharT* s,size_type pos = npos) const
    {
        return find_last_of(s,pos,strlen(s));
    }

    size_type find_last_of(const CharT* s,size_type pos,size_type n) const;
    size_type find_last_of(const CharT c,size_type pos = npos) const
    {
        return rfind(c,pos);
    }
#endif

public: //find_first_not_of 返回字符串中首次出现的不匹配str中的任何一个字符的首字符索引
    size_type find_first_not_of(const basic_string& s,
                                size_type pos = 0) const
    {
        return find_first_not_of(s.begin(),pos,s.size());
    }

    size_type find_first_not_of(const CharT* s,size_type pos = 0) const
    {
        return find_first_not_of(s,pos,strlen(s));
    }

    size_type find_first_not_of(const CharT* s,size_type pos,
                                size_type n) const;
    size_type find_first_not_of(const CharT c,size_type pos = 0) const;

public://find_last_not_of

public:
    basic_string substr(size_type pos = 0,size_type n = npos) const
    {
        if(pos > size())
            throw_out_of_range();

        return basic_string(start+pos,start+pos+min(size()-pos,n));
    }

public://compare

    int compare(const basic_string& s) const
    {
        return compare(start,finish,s.begin(),s.end());
    }

    int compare(size_type pos1,size_type n1,
                const basic_string& s) const
    {
        if(pos1 > size())
            throw_out_of_range();
        return compare(start+pos1,start+pos1+min(n1,size()-pos1),
                       s.begin(),s.end());
    }

    int compare(size_type pos1,size_type n1,
                const basic_string& s,
                size_type pos2,size_type n2) const
    {
        if(pos1 > size())
            throw_out_of_range();
        return compare(start+pos1,start+pos1+min(n1,size()-pos1),
                       s.begin()+pos2,s.begin()+pos2+min(n2,size()-pos2));
    }

    int compare(const CharT* s) const
    {
        return compare(start,finish,s,s+strlen(s));
    }

    int compare(size_type pos1,size_type n1,const CharT* s) const
    {
        if(pos1 > size())
            throw_out_of_range();
        return compare(start+pos1,start+pos1+min(n1,size()-pos1),
                       s,s+strlen(s));
    }

    int compare(size_type pos1,size_type n1,
                const CharT*s,size_type n2)
    {
        if(pos1 > size())
            throw_out_of_range();

        return compare(start+pos1,start+pos1+min(n1,size()-pos1),
                       s,s+n2);
    }

    static int compare(const CharT* f1,const CharT* l1,
                       const CharT* f2,const CharT* l2)
    {
        const ptrdiff_t n1 = l1 - f1;
        const ptrdiff_t n2 = l2 - f2;
        const int cmp = strncmp(f1,f2,min(n1,n2));
        return cmp != 0 ? cmp : (n1 < n2 ? -1 : (n1 > n2 ? 1 : 0));
    }


};

template<class CharT,class Alloc>
const typename basic_string<CharT,Alloc>::size_type
basic_string<CharT,Alloc>::npos =
(basic_string<CharT,Alloc>::size_type)-1;


template<class Tp,class Alloc>
void basic_string<Tp,Alloc>::reserve(size_type res_arg)
{
    //检查参数
    if(res_arg > max_size())
        throw_length_error();

    size_type n = max(res_arg,size()) + 1;
    pointer new_start = allocate(n);
    pointer new_finish = new_start;

    STL_TRY
    {
        new_finish = uninitialized_copy(start,finish,new_start);
        construct_null(new_finish);
    }
    STL_UNWIND((destroy(new_start,new_finish),deallocate(new_start,n)));

    destroy(start,finish);
    deallocate_block();

    start = new_start;
    finish = new_finish;
    end_of_storage = start + n;
}

template<class InputIter,class Distance>
inline void advance(InputIter& i,Distance n)
{
    while(n--)
        ++i;
}

template<class CharT,class Alloc>
void basic_string<CharT,Alloc>::insert(basic_string<CharT,Alloc>::iterator position, const CharT *first, const CharT *last)
{
    if(first != last)
    {
        const ptrdiff_t n = last - first;

        if(end_of_storage - finish >= n+1)
        {
            //具有足够的空间
            const ptrdiff_t elems_after = finish - position;
            iterator old_finish = finish;

            if(elems_after >= n)
            {
                //添加的数据不多
                uninitialized_copy((finish-n)+1,finish+1,finish+1);
                finish += n;
                memcpy(position+n,position,(elems_after-n)+1);
                copy(first,last,position);
            }
            else
            {
                //添加的数据很多
                const CharT* mid = first;
                advance(mid,elems_after + 1);
                uninitialized_copy(mid,last,finish+1);
                finish += (n - elems_after);

                STL_TRY
                {
                    uninitialized_copy(position,old_finish+1,finish);
                    finish += elems_after;
                }
                STL_UNWIND((destroy(old_finish+1,finish),finish = old_finish));
                copy(first,mid,position);
            }
        }
        else
        {
            //没有空间
            const size_type old_size = size();
            const size_type len = old_size + max(old_size,static_cast<size_type>(n))+1;
            pointer new_start = allocate(len);
            pointer new_finish = new_start;

            STL_TRY
            {
                new_finish = uninitialized_copy(start,position,new_finish);
                new_finish = uninitialized_copy(first,last,new_finish);
                new_finish = uninitialized_copy(position,finish,new_finish);
                construct_null(new_finish);
            }
            STL_UNWIND((destroy(new_start,new_finish),deallocate(new_start,len)));

            destroy(start,finish+1);
            deallocate_block();

            start = new_start;
            finish = new_finish;
            end_of_storage = start + len;
        }
    }
}


template<class CharT,class Alloc>
void basic_string<CharT,Alloc>::insert(basic_string<CharT,Alloc>::iterator position,
                                       size_t n,CharT c)
{
    if(n != 0)
    {
        if(size_type(end_of_storage-finish) >= n+1)
        {
            //具有足够的空间
            const size_type elems_after = finish - position;
            iterator old_finish = finish;

            if(elems_after >= n)
            {
                uninitialized_copy((finish-n)+1,finish+1,finish+1);
                finish += n;
                memmove(position+n,position,(elems_after - n) + 1);
                memset(position,c,n);
            }
            else
            {
                uninitialized_fill_n(finish+1,n-elems_after-1,c);
                finish += n - elems_after;

                STL_TRY
                {
                    uninitialized_copy(position,old_finish+1,finish);
                    finish += elems_after;
                }
                STL_UNWIND((destroy(old_finish+1,finish),finish = old_finish));

                memset(position,c,elems_after + 1);
            }
        }
        else
        {
            //需要扩充空间
            const size_type old_size = size();
            const size_type len = old_size + max(old_size,n)+1;
            iterator new_start = allocate(len);
            iterator new_finish = new_start;

            STL_TRY
            {
                new_finish = uninitialized_copy(start,position,new_start);
                new_finish = uninitialized_fill_n(new_finish,n,c);
                new_finish = uninitialized_copy(position,finish,new_finish);
                construct_null(new_finish);
            }
            STL_UNWIND((destroy(new_start,new_finish),deallocate(new_start,len)));

            destroy(start,finish - 1);
            deallocate_block();
            start = new_start;
            finish = new_finish;
            end_of_storage = start + len;

        }
    }
}


template<class CharT,class Alloc>
typename basic_string<CharT,Alloc>::iterator basic_string<CharT,Alloc>::insert_aux(basic_string<CharT,Alloc>::iterator p,
                                  CharT c)
{
    iterator new_pos = p;
    if(finish + 1 < end_of_storage)
    {
        construct_null(finish+1);
        memmove(p+1,p,finish - p);
        *p = c;
        ++finish;
    }
    else
    {
        const size_type old_len = size();
        const size_type len = old_len + max(old_len,static_cast<size_type>(1))+1;

        iterator new_start = allocate(len);
        iterator new_finish = new_start;

        STL_TRY
        {
            new_pos = uninitialized_copy(start,p,new_start);
            construct(new_pos,c);
            new_finish = new_pos + 1;
            new_finish = uninitialized_copy(p,finish,new_finish);
            construct_null(new_finish);

        }
        STL_UNWIND((destroy(new_start,new_finish),deallocate(new_start,len)));

        destroy(start,finish+1);
        deallocate_block();
        start = new_start;
        finish = new_finish;
        end_of_storage = start + len;

    }

    return new_pos;
}

template<class CharT,class Alloc>
basic_string<CharT,Alloc>& basic_string<CharT,Alloc>::replace(iterator first, iterator last,
                                                              size_type n, CharT c)
{
    const size_type len = static_cast<size_type>(last - first);
    if(len > n)
    {
        memset(first,c,n);
        erase(first+n,last);
    }
    else
    {
        memset(first,c,len);
        insert(last,n-len,c);
    }
    return *this;
}

template<class CharT,class Alloc>
basic_string<CharT,Alloc>& basic_string<CharT,Alloc>::replace(iterator first, iterator last,
                                                              const CharT *f, const CharT *l)
{
    const ptrdiff_t n = l - f;
    const difference_type len = last - first;

    if(len >= n)
    {
        //可以直接复制
        copy(f,l,first);
        //删除空闲的位置
        erase(first+n,last);
    }
    else
    {
        const CharT* m = f + len;
        copy(f,m,first);
        //插入其它空间
        insert(last,m,l);
    }

    return *this;
}

inline bool char_eq(char x,char y)
{
    return x == y;
}

template<class CharT,class Alloc>
typename basic_string<CharT,Alloc>::size_type basic_string<CharT,Alloc>::find(const CharT* s,
                                          size_type pos,size_type n) const
{
    if(pos+n > size())
        return npos;
    else
    {
        const const_iterator result = search(start+pos,finish,s,s+n,char_eq);
        return result != finish ? result-begin() : npos;
    }
}

template<class CharT,class Alloc>
typename basic_string<CharT,Alloc>::size_type
basic_string<CharT,Alloc>::find(CharT c,size_type pos) const
{
    if(pos >= size())
        return npos;
    else
    {
        CharT* first = start + pos;
        while(first != finish && !(char_eq(*first,c)))
            ++first;
        return first != finish ? first - begin() : npos;
    }
}

//从头到pos位置查找[s,s+n]字符串最后出现的位置
template<class CharT,class Alloc>
typename basic_string<CharT,Alloc>::size_type
basic_string<CharT,Alloc>
::rfind(const CharT* s,size_type pos,size_type n) const
{
    const size_t len = size();

    //不可能找到比原始字符串还长的字符串
    if(n > len)
        return npos;

    else if(n == 0)
        return min(len,pos);
    else
    {
        const const_iterator last = begin()+min(len-n,pos)+n;
        const const_iterator result = find_end(begin(),last,s,s+n,char_eq);
        return result != last ? result - begin() : npos;
    }
}


template<class CharT,class Alloc>
typename basic_string<CharT,Alloc>::size_type
basic_string<CharT,Alloc>
::rfind(CharT c,size_type pos) const
{
    const size_type len = size();

    //字符串本身为空
    if(len < 1)
        return npos;
    else
    {
        const_iterator first = begin();
        const_iterator last = begin()+min(len-1,pos)+1;

        const_iterator result = last-1;
        while(!char_eq(*result,c))
        {
            if(result == first)
                return npos;
            --result;
        }

        return result - begin();
    }
}

template<class CharT,class Alloc>
typename basic_string<CharT,Alloc>::size_type
basic_string<CharT,Alloc>
::find_first_of(const CharT* s,size_type pos,size_type n) const
{
    if(pos >= size())
        return npos;
    else
    {
        const_iterator result = __find_first_of(begin()+pos,end(),
                                                s,s+n,char_eq);
        return result != finish ? result - begin() : npos;
    }
}

//template<class CharT,class Alloc>
//typename basic_string<CharT,Alloc>::size_type
//basic_string<CharT,Alloc>
//::find_last_of(const CharT*s size_type pos,size_type n) const
//{}

template<class CharT,class Alloc>
typename basic_string<CharT,Alloc>::size_type
basic_string<CharT,Alloc>
::find_first_not_of(const CharT* s,size_type pos,size_type n) const
{
    if(pos > size())
        return npos;
    else
    {
        const_iterator result = __find_first_not_of(begin()+pos,end(),
                                                    s,s+n,char_eq);
        return result != finish ? result - begin() : npos;
    }
}



///////////////////////////////////////////////////////
///string声明
///
typedef basic_string<char,STL_DEFAULT_ALLOCATOR(char)> string;
//不支持宽字节
//typedef basic_string<wchar_t,STL_DEFAULT_ALLOCATOR(wchar_t)> wstring;

//获取string的char *指针
static const char *get_c_string(const string &);

STL_END_NAMESPACE

#endif // STRING_IMP_H
