//algo.h
#ifndef ALGO_H
#define ALGO_H

//在[first1,last1]中寻找第一次出现[first2,last2]的位置
template<class ForwardIter1,class ForwardIter2,class BinaryPred>
ForwardIter1 search(ForwardIter1 first1,ForwardIter1 last1,
                    ForwardIter2 first2,ForwardIter2 last2,
                    BinaryPred predicate)
{
    //检查参数
    if(first1 == last1 || first2 == last2)
        return first1;

    //被搜索的对象只有一个元素
    ForwardIter2 tmp(first2);
    ++tmp;
    if(tmp == last2)
    {
        while(first1 != last1 && !predicate(*first1,*first2))
        {
            ++first1;
        }
        return first1;
    }

    //一般情况
    ForwardIter2 p1,p;
    p1 = first2;
    ++p1;

    ForwardIter1 current = first1;

    while(first1 != last1)
    {
        //定位第1个相等的元素
        while(first1 != last1)
        {
            if(predicate(*first1,*first2))
                break;
            ++first1;
        }

        //这与上面重复
        while(first1 != last1 && !predicate(*first1,*first2))
            ++first1;

        //每个找到第1个匹配的元素
        if(first1 == last1)
            return last1;

        p = p1;
        current = first1;

        //不可能找到目标元素
        if(++current == last1)
            return last1;

        while(predicate(*current,*p))
        {
            //找到匹配的元素
            if(++p == last2)
                return first1;

            //匹配的元素个数不同
            if(++current == last1)
                return last1;
        }

        ++first1;
    }

    return first1;
}

//在first1到last1中寻找first2到last2最后出现的位置
template<class ForwardIter1,class ForwardIter2,
         class BinaryPredicate>
ForwardIter1 find_end(ForwardIter1 first1,ForwardIter1 last1,
                      ForwardIter2 first2,ForwardIter2 last2,
                      BinaryPredicate comp)
{
    //被查找的字符串为空
    if(first2 == last2)
    {
        return last1;
    }
    else
    {
        ForwardIter1 result = last1;
        while(1)
        {
            ForwardIter1 new_result = search(first1,last1,first2,last2,comp);

            //找到最后一个目标数据或者没有找到目标数据
            if(new_result == last1)
                return result;
            else
            {
                //保留本次搜索结果
                result = new_result;
                first1 = new_result;
                ++first1;
            }
        }
    }
}

//在[first1,last1]中找到第一个在[first2,last2]中任意一个字符
template<class InputIter,class ForwardIter,class BinaryPredicate>
InputIter __find_first_of(InputIter first1,InputIter last1,
                        ForwardIter first2,ForwardIter last2,
                        BinaryPredicate comp)
{
    for(;first1 != last1;++first1)
        for(ForwardIter iter = first2;iter != last2;++iter)
            if(comp(*first1,*iter))
                return first1;

    return last1;
}

template<class InputIter,class ForwardIter,class BinaryPredicate>
InputIter __find_first_not_of(InputIter first1,InputIter last1,
                        ForwardIter first2,ForwardIter last2,
                        BinaryPredicate comp)
{
    for(;first1 != last1;++first1)
    {
        ForwardIter iter = first2;
        for(;iter != last2;++iter)
            if(comp(*first1,*iter))
                break;
        if(iter == last2)
            return first1;
    }

    return last1;
}

#endif // ALGO_H
