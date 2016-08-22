//vector_imp.h
//实现vector功能

#ifndef VECTOR_IMP_H
#define VECTOR_IMP_H

#include "configure.h"
#include "alloc_imp.h"
#include "construct.h"
#include "uninitialized.h"
#include "algobase.h"
#include "iterator_imp.h"


//定义命名空间
STL_BEGIN_NAMESPACE

////////////////////////////////////
//vector的基类有两个目的：
//1.提供内存的分配和释放
//2.封装不同种类的内存分配和释放器
template <class Tp,class Alloc>
class vector_base
{
public:
    //返回分配器函数
    typedef Alloc allocator_type;
    allocator_type get_allocator() const
    {
        return allocator_type();
    }

    //默认构造函数
    vector_base(const Alloc&)
        : start(0),finish(0),end_of_storage(0) {}

    vector_base(size_t n,const Alloc&)
        : start(0),finish(0),end_of_storage(0)
    {
        start = allocate(n);
        finish = start;
        end_of_storage = start+n;
    }

    ~vector_base()
    {
        deallocate(start,end_of_storage-start);
    }

protected:
    Tp* start;
    Tp* finish;
    Tp* end_of_storage;

    typedef simple_alloc<Tp,Alloc> data_allocator;

    //分配n个元素内存空间
    Tp* allocate(size_t n)
    {
        return data_allocator::allocate(n);
    }

    //释放n个元素内存空间
    void deallocate(Tp* p,size_t n)
    {
        data_allocator::deallocate(p,n);
    }
};


////////////////////////////////////
//vector类实现
template <class Tp,class Alloc = STL_DEFAULT_ALLOCATOR(Tp)>
class vector : protected vector_base<Tp,Alloc>
{
private:
    typedef vector_base<Tp,Alloc> Base;//这里基类的操作函数是static

public:
    typedef Tp value_type;

    typedef value_type* pointer;
    typedef const value_type* const_pointer;

    typedef value_type* iterator;
    typedef const value_type* const_iterator;

    typedef value_type& reference;
    typedef const value_type& const_reference;

    typedef size_t size_type;
    typedef ptrdiff_t difference_type;

    //继承类型
    typedef typename Base::allocator_type allocator_type;
    allocator_type get_allocator() const
    {
        return Base::get_allocator();
    }

    //反向迭代器
    typedef reverse_iterator<const_iterator,value_type,const_reference,
        difference_type> const_reverse_iterator;
    typedef reverse_iterator<iterator,value_type,reference,difference_type>
        reverse_iterator;

protected:

    //这是模板继承，这个很重要
    using Base::finish;
    using Base::start;
    using Base::end_of_storage;
    using Base::allocate;
    using Base::deallocate;


protected:
    void insert_aux(iterator position,const Tp& x);
    void insert_aux(iterator position);

public:

    explicit vector(const allocator_type& a = allocator_type())
        : Base(a)
    {}

    //使用x构造n个元素
    vector(size_type n,const Tp& value,const allocator_type& a = allocator_type())
        : Base(n,a)
    {
        finish = uninitialized_fill_n(start,n,value);
    }

    //使用默认构造函数构造n个元素
    explicit vector(size_type n)
        : Base(n,allocator_type())
    {
        finish = uninitialized_fill_n(start,n,Tp());
    }

    //复制构造函数
    explicit vector(const vector<Tp,Alloc>& x)
        : Base(x.size(),x.get_allocator())
    {
        finish = uninitialized_copy(x.begin(),x.end(),start);
    }

    vector(const Tp* first,const Tp* last,const allocator_type& a = allocator_type())
        : Base(last - first,a)
    {
        finish = uninitialized_copy(first,last,start);
    }

    ~vector()
    {
        destroy(start,finish);
    }

    vector<Tp,Alloc>& operator= (const vector<Tp,Alloc>& x);

    iterator begin()
    {
        return start;
    }

    const_iterator begin() const
    {
        return start;
    }

    iterator end()
    {
        return finish;
    }

    const_iterator end() const
    {
        return finish;
    }

    reverse_iterator rbegin()
    {
        return reverse_iterator(end());
    }

    const_reverse_iterator rbegin() const
    {
        return const_reverse_iterator(end());
    }

    reverse_iterator rend()
    {
        return reverse_iterator(begin());
    }

    const_reverse_iterator rend() const
    {
        return const_reverse_iterator(begin());
    }

    //返回元素个数，不过这里可能有bug
    size_type size() const
    {
        return size_type(end()-begin());
    }

    //能够获取的最大元素个数
    size_type max_size() const
    {
        return size_type(-1)/sizeof(Tp);
    }

    //包容的最大元素个数
    size_type capacity() const
    {
        return size_type(end_of_storage - begin());
    }

    bool empty() const
    {
        return begin() == end();
    }

    reference operator[](size_type n)
    {
        return *(begin()+n);
    }

    const_reference operator [](size_type n) const
    {
        return *(begin()+n);
    }

    //请求空间大小，调用之后并没有创建对象实例
    void reserve(size_type n)
    {
        if(capacity() < n)
        {
            const size_type old_size = size();
            iterator tmp = allocate_and_copy(n,start,finish);
            destroy(start,finish);
            deallocate(start,end_of_storage-start);
            start = tmp;
            finish = tmp + old_size;
            end_of_storage = start + n;
        }
    }

    //将vector设置具有n元素，每个元素值为val
    void assign(size_type n,const Tp& val)
    {
        fill_assign(n,val);
    }

    void fill_assign(size_type n,const Tp& val);

    //返回第一个元素
    reference front()
    {
        return *begin();
    }

    const_reference front() const
    {
        return *begin();
    }

    //返回最后一个元素
    reference back()
    {
        return *(end()-1);
    }

    const_reference back() const
    {
        return *(end()-1);
    }

    //在最后添加一个元素
    void push_back(const Tp& x)
    {
        if(finish != end_of_storage)
        {
            //这里只调用构造函数
            construct(finish,x);
            ++finish;
        }
        else
            insert_aux(end(),x);    //内存不够，需要插入元素，很耗时
    }

    //添加一个元素，但是没有设置该元素数值
    void push_back()
    {
        if(finish != end_of_storage)
        {
            construct(finish);
            ++finish;
        }
        else
            insert_aux(end());
    }

    //与一个相同类型的vector交换空间,交换空间就可以了
    void swap(vector<Tp,Alloc>& x)
    {
        std::swap(start,x.start);
        std::swap(finish,x.finish);
        std::swap(end_of_storage,x.end_of_storage);
    }

    //在指定位置插入一个元素
    iterator insert(iterator position,const Tp& x)
    {
        size_type n = position - begin();

        if(finish != end_of_storage && position == end())
        {
            //在vector尾部添加元素
            construct(finish,x);
            ++finish;
        }
        else
            insert_aux(position,x); //任意位置添加元素

        //返回插入数据的地址
        return begin()+n;
    }

    //插入一个空元素
     iterator insert(iterator position)
     {
         size_type n = position - begin();

         if(finish != end_of_storage && position == end())
         {
             construct(position);
             ++finish;
         }
         else
             insert_aux(position);

         return begin() + n;
     }

     //将first到last数据插入到position位置
     void insert(iterator position,const_iterator first,const_iterator last);

     void insert(iterator position,size_type n,const Tp& x)
     {
         fill_insert(position,n,x);
     }

     //从positon位置开始添加n个新元素，每个元素都使用x初始化
     void fill_insert(iterator pos,size_type n,const Tp& x);

     //释放最后一个元素空间
     void pop_back()
     {
         --finish;
         destroy(finish);
     }

     //删除指定位置元素
     iterator erase(iterator position)
     {
         //将position后面所有元素向前移动一位
         if(position + 1 != end())
             copy(position+1,finish,position);

         //释放最后一个元素
         --finish;
         destroy(finish);
         return position;
     }

     //删除first到last之间的数据
     iterator erase(iterator first,iterator last)
     {
         //将last到finish之间的数据覆盖到first
         iterator i = copy(last,finish,first);

         //删除已经不被使用的数据
         destroy(i,finish);

         //更新finish
         finish = finish - (last - first);
         return first;
     }

     //调整大小
     void resize(size_type new_size,const Tp& x)
     {
         if(new_size < size())
             erase(begin()+new_size,end());
         else
             insert(end(),new_size-size(),x);
     }

     void resize(size_type new_size)
     {
         resize(new_size,Tp());
     }

     //清空vector
     void clear()
     {
         erase(begin(),end());
     }

protected:

     //新建一个n个Tp元素空间，将first到last区域复制到新建空间
     iterator allocate_and_copy(size_type n,const_iterator first,const_iterator last)
     {
         iterator result = allocate(n);

         STL_TRY
         {
             uninitialized_copy(first,last,result);
             return result;
         }
         STL_UNWIND(deallocate(result,n))
     }
};

//重载操作符：==，基础 注意函数参数。
template<class Tp,class Alloc>
inline bool operator==(const vector<Tp,Alloc>& x,const vector<Tp,Alloc>& y)
{
    return x.size() == y.size() && equal(x.begin(),x.end(),y.begin());
}

//重载操作符：< 基础
template<class Tp,class Alloc>
inline bool operator<(const vector<Tp,Alloc>& x,const vector<Tp,Alloc>& y)
{
    return lexicographical_compare(x.begin(),x.end(),y.begin(),y.end());
}

template<class Tp,class Alloc>
inline void swap(vector<Tp,Alloc>& x,vector<Tp,Alloc>& y)
{
    x.swap(y);
}

//重载操作符：!=
template<class Tp,class Alloc>
inline bool operator!=(const vector<Tp,Alloc>& x,const vector<Tp,Alloc>& y)
{
    return !(x == y);
}

//重载操作符：>
template<class Tp,class Alloc>
inline bool operator>(const vector<Tp,Alloc>& x,const vector<Tp,Alloc>& y)
{
    return y < x;
}

//重载操作符：<=
template<class Tp,class Alloc>
inline bool operator<=(const vector<Tp,Alloc>& x,const vector<Tp,Alloc>& y)
{
    return !(y < x);
}

//重载操作符：>=
template<class Tp,class Alloc>
inline bool operator >=(const vector<Tp,Alloc>& x,const vector<Tp,Alloc>& y)
{
    return !(x < y);
}

//vector重载操作符：=,注意返回类型和参数类型
//返回vector本身
template<class Tp,class Alloc>
vector<Tp,Alloc>& vector<Tp,Alloc>::operator=(const vector<Tp,Alloc>& x)
{
    if(&x != this)
    {
        const size_type xlen = x.size();
        if(xlen > capacity())
        {
            //新的vector空间大于目前vector空间
            //创建一个新的连续空间容纳新vector，释放旧的vector
            iterator tmp = allocate_and_copy(xlen,x.begin(),x.end());
            destroy(start,finish);
            deallocate(start,end_of_storage - start);
            start = tmp;
            end_of_storage = start + xlen;
        }
        else if(size() > xlen)
        {
            //原有vector元素多余新的vector元素

            //用新元素覆盖原有元素
            iterator i = copy(x.begin(),x.end(),begin());

            //释放旧数据
            destroy(i,finish);
        }
        else
        {
            //原有vector元素少于新的vector元素
            //直接复制已经初始化后的元素
            copy(x.begin(),x.begin()+size(),start);

            //复制并初始化后面的元素
            uninitialized_copy(x.begin()+size(),x.end(),finish);
        }

        finish = start + xlen;
    }

    return *this;
}

template<class Tp,class Alloc>
void vector<Tp,Alloc>::fill_assign(size_type n,const value_type& val)
{
    if(n > capacity())
    {
        //没有空间容纳数据,tmp结束后会调用析构函数
        vector<Tp,Alloc> tmp(n,val,get_allocator());
        tmp.swap(*this);
    }
    else if(n > size())
    {
        fill(begin(),end(),val);
        finish = uninitialized_fill_n(finish,n-size(),val);
    }
    else
    {
        erase(fill_n(begin(),n,val),end());
    }
}


//插入辅助（auxiliary）函数，在任意元素位置插入一个新的元素
template <class Tp,class Alloc>
void vector<Tp,Alloc>::insert_aux(iterator position,const Tp& x)
{
    if(finish != end_of_storage)
    {
        //还具有内存空间。因为需要插入一个元素，所以需要将position后的所有
        //元素后错位一个元素位置

        //这里将前一个元素复制到finish位置，这里是不是可以直接使用copy_backward
        //复制所有数据。
        construct(finish,*(finish-1));
        finish++;
        Tp x_copy = x;
        copy_backward(position,finish-2,finish-1);
        *position = x_copy;
    }
    else
    {
        //没有位置添加新的元素，需要扩充大小
        const size_type old_size = size();

        //如果还没有建立数据就分配一个元素空间。
        //如果已经存在元素，需要将原有空间扩充一倍。
        const size_type len = old_size != 0 ? 2*old_size : 1;
        iterator new_start = allocate(len);
        iterator new_finish = new_start;

        STL_TRY
        {
            //复制position之前的数据
            new_finish = uninitialized_copy(start,position,new_start);

            //在position建立数据元素
            construct(new_finish,x);
            ++new_finish;

            //复制position后面的元素
            new_finish = uninitialized_copy(position,finish,new_finish);
        }
        STL_UNWIND((destroy(new_start,new_finish),(deallocate(new_start,len))));

        //调用原来对象的析构函数
        //从这里我们看到使用默认复制函数多么危险，而且申请释放内存也很耗时。
        //最好一开始就给足内存
        destroy(begin(),end());
        deallocate(start,end_of_storage-start);
        start = new_start;
        finish = new_finish;
        end_of_storage = new_start+len;
    }
}

template<class Tp,class Alloc>
void vector<Tp,Alloc>::insert_aux(iterator position)
{
    if(finish != end_of_storage)
    {
        //有足够空间
        construct(finish,*(finish-1));
        ++finish;
        copy_backward(position,finish-2,finish-1);
        *position = Tp();
    }
    else
    {
        //没有足够空间
        const size_type old_size = size();
        const size_type len = old_size != 0 ? old_size * 2 : 1;
        iterator new_start = allocate(len);
        iterator new_finish = new_start;

        STL_TRY
        {
            new_finish = uninitialized_copy(start,position,new_start);
            construct(new_finish);
            ++new_finish;
            new_finish = uninitialized_copy(position,finish,new_finish);
        }
        STL_UNWIND((destroy(new_start,new_finish),deallocate(new_start,new_finish)));

        destroy(begin(),end());
        deallocate(start,end_of_storage-start);
        start = new_start;
        finish = new_finish;
        end_of_storage = new_start + len;

    }
}



//从positon位置开始添加n个新元素，每个元素都使用x初始化
template<class Tp,class Alloc>
void vector<Tp,Alloc>::fill_insert(iterator position,size_type n,const Tp& x)
{
    if(n != 0)
    {
        if(size_type(end_of_storage - finish) >= n)
        {
            //有足够的空间容下被插入的数据
            Tp x_copy = x;  //最好具有复制构造函数
            const size_type elems_after = finish - position;
            iterator old_finish = finish;

            if(elems_after > n)
            {
                //postion到finish的数据区大于n，分两步移动数据
                //这里没有一次移动所有数据，是由于这些复制函数没有对
                //重叠区域进行保护

                //先将finish-n和finish数据复制到finish开始地址
                uninitialized_copy(finish-n,finish,finish);
                finish += n;

                //将postion到old_finish-n数据复制到以old_finish结束地址
                copy_backward(position,old_finish-n,old_finish);

                //填充position到potion+n为x
                fill(position,position+n,x_copy);
            }
            else
            {
                //先填充finish后n-elems_after个数据
                uninitialized_fill_n(finish,n-elems_after,x_copy);
                finish += n-elems_after;

                //将原来的数据复制finish+n-elems_after后
                uninitialized_copy(position,old_finish,finish);
                finish += elems_after;

                //填充position到old_finish之间的数据
                fill(position,old_finish,x_copy);
            }
        }
        else
        {
            //空间不够，需要分配新的空间
            const size_type old_size = size();
            const size_type len = old_size + std::max(old_size,n);
            iterator new_start = allocate(len);
            iterator new_finish = new_start;

            //依次安装各个区域
            STL_TRY
            {
                new_finish = uninitialized_copy(start,position,new_start);
                new_finish = uninitialized_fill_n(new_finish,n,x);
                new_finish = uninitialized_copy(position,finish,new_finish);
            }
            STL_UNWIND((destroy(new_start,new_finish),deallocate(new_start,len)));

            //释放原来的数据区间
            destroy(start,finish);
            deallocate(start,end_of_storage-start);
            start = new_start;
            finish = new_finish;
            end_of_storage = new_start+len;

        }
    }
}

//将first到last的元素插入到position位置
template<class Tp,class Alloc>
void vector<Tp,Alloc>::insert(iterator position,
                              const_iterator first,const_iterator last)
{
    if(first != last)
    {
        size_type n = 0;
        distance(first,last,n);

        if(size_type(end_of_storage - finish) >= n)
        {
            //具有容纳插入元素的内存空间
            const size_type elems_after = finish - position;
            iterator old_finish = finish;

            if(elems_after > n)
            {
                //从position到finish足以保持n个数据
                uninitialized_copy(finish-n,finish,finish);
                finish += n;
                copy_backward(position,old_finish - n,old_finish);
                copy(first,last,position);
            }
            else
            {
                //操作未初始化区域必须使用uninitialized_copy
                //finish之前的区域才能使用copy函数。
                uninitialized_copy(first+elems_after,last,finish);
                finish += n - elems_after;
                uninitialized_copy(position,old_finish,finish);
                finish += elems_after;
                copy(first,first+elems_after,position);
            }
        }
        else
        {
            const size_type old_size = size();
            const size_type len = old_size + std::max(old_size,n);
            iterator new_start = allocate(len);
            iterator new_finish = new_start;

            STL_TRY
            {
                new_finish = uninitialized_copy(start,position,new_start);
                new_finish = uninitialized_copy(first,last,new_finish);
                new_finish = uninitialized_copy(position,finish,new_finish);
            }
            STL_UNWIND((destroy(new_start,new_finish),deallocate(new_start,len)));

            destroy(start,finish);
            deallocate(start,end_of_storage-start);
            start = new_start;
            finish = new_finish;
            end_of_storage = new_start + len;

        }
    }

}


STL_END_NAMESPACE

#endif // VECTOR_IMP_H
