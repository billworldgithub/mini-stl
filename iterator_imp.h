//iterator_imp.h
//实现迭代器

#ifndef ITERATOR_IMP_H
#define ITERATOR_IMP_H

//逆向迭代器
template<class RandomAccessIterator,class Tp,class Reference = Tp&,
         class Distance = ptrdiff_t>
class reverse_iterator
{
    typedef reverse_iterator<RandomAccessIterator,Tp,Reference,Distance> Self;

protected:
    RandomAccessIterator current;

public:
    typedef Tp value_type;
    typedef Distance difference_type;
    typedef Tp* pointer;
    typedef Reference reference;

    reverse_iterator() {}
    explicit reverse_iterator(RandomAccessIterator x)
        : current(x)
    {}

public:
    //用于转化iterator
    RandomAccessIterator base() const
    {
        return current;
    }

public:

    //取地址值
    Reference operator*() const
    {
        return *(current-1);
    }

    pointer operator->() const
    {
        return &(operator *());
    }

    //自加操作符++p
    Self& operator++()
    {
        --current;
        return *this;
    }

    //后置自加p++
    Self operator++(int)
    {
        Self tmp = *this;
        --current;
        return tmp;
    }

    //--p
    Self& operator--()
    {
        ++current;
        return *this;
    }

    //p--
    Self operator--(int)
    {
        Self tmp = *this;
        ++current;
        return tmp;
    }

    //+
    Self operator+(Distance n) const
    {
        return Self(current - n);
    }

    //+=
    Self& operator+=(Distance n)
    {
        current -= n;
        return *this;
    }

    //-
    Self operator-(Distance n) const
    {
        return Self(current+n);
    }

    //-=
    Self& operator-=(Distance n)
    {
        current += n;
        return *this;
    }

    //[]
    Reference operator[](Distance n) const
    {
        //先获取iterator，然后调用*iter获取元素
        return *(*this + n);
    }
};


template<class RandomAccessIterator,class Tp,
         class Reference,class Distance>
inline bool operator==(const reverse_iterator<RandomAccessIterator,Tp,
                                              Reference,Distance>& x,
                       const reverse_iterator<RandomAccessIterator,Tp,
                                              Reference,Distance>& y)
{
    return x.base() == y.base();
}

template<class RandomAccessIterator,class Tp,
         class Reference,class Distance>
inline bool operator<(const reverse_iterator<RandomAccessIterator,Tp,
                                              Reference,Distance>& x,
                       const reverse_iterator<RandomAccessIterator,Tp,
                                              Reference,Distance>& y)
{
    return x.base() < y.base();
}


template<class RandomAccessIterator,class Tp,
         class Reference,class Distance>
inline bool operator!=(const reverse_iterator<RandomAccessIterator,Tp,
                                              Reference,Distance>& x,
                       const reverse_iterator<RandomAccessIterator,Tp,
                                              Reference,Distance>& y)
{
    return !(x == y);
}

template<class RandomAccessIterator,class Tp,
         class Reference,class Distance>
inline bool operator>(const reverse_iterator<RandomAccessIterator,Tp,
                                              Reference,Distance>& x,
                       const reverse_iterator<RandomAccessIterator,Tp,
                                              Reference,Distance>& y)
{
    return (y < x);
}

template<class RandomAccessIterator,class Tp,
         class Reference,class Distance>
inline bool operator<=(const reverse_iterator<RandomAccessIterator,Tp,
                                              Reference,Distance>& x,
                       const reverse_iterator<RandomAccessIterator,Tp,
                                              Reference,Distance>& y)
{
    return !(x > y);
}

template<class RandomAccessIterator,class Tp,
         class Reference,class Distance>
inline bool operator>=(const reverse_iterator<RandomAccessIterator,Tp,
                                              Reference,Distance>& x,
                       const reverse_iterator<RandomAccessIterator,Tp,
                                              Reference,Distance>& y)
{
    return !(x < y);
}


template<class RandomAccessIterator,class Tp,
         class Reference,class Distance>
inline Distance operator-(const reverse_iterator<RandomAccessIterator,Tp,
                          Reference,Distance>& x,
                          const reverse_iterator<RandomAccessIterator,Tp,
                          Reference,Distance>& y)
{
    return (x.base() - y.base());
}

template<class RandomAccessIterator,class Tp,
         class Reference,class Distance>
inline reverse_iterator<RandomAccessIterator,Tp,Reference,Distance>
operator+(Distance n,const reverse_iterator<RandomAccessIterator,Tp,
          Reference,Distance>& x)
{
    return reverse_iterator<RandomAccessIterator,Tp,Reference,Distance>(x.base()-n);
}


#endif // ITERATOR_IMP_H
