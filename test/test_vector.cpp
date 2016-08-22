//test_vector.cpp

#include <gtest/gtest.h>
#include "../vector_imp.h"

using namespace mini_stl;

TEST(TestVector,Construct)
{
    vector<int> vect0;
    EXPECT_EQ((int)(vect0.begin()),0);
    EXPECT_EQ((int)(vect0.end()),0);
    EXPECT_EQ((int)(vect0.size()),0);

    vector<int> vect1(3,1);
    EXPECT_EQ((int)(vect1.size()),3);
    for(vector<int>::iterator iter = vect1.begin();iter != vect1.end();++iter)
        EXPECT_EQ(*iter,1);
    EXPECT_EQ(vect1.size(),vect1.capacity());

    vector<int> vect2(2);
    EXPECT_EQ(vect2.size(),2);

    //vector<int> vect3 = vect1; //error:explicit
    vector<int>vect3(vect1);
    EXPECT_EQ(vect3.size(),3);
    for(vector<int>::iterator iter = vect3.begin();iter != vect3.end();++iter)
        EXPECT_EQ(*iter,1);
    vect3[0] = 0;
    vect3[1] = 1;
    vect3[2] = 2;

    EXPECT_EQ(vect3[0],0);
    EXPECT_EQ(vect3[1],1);
    EXPECT_EQ(vect3[2],2);

    vector<int>::iterator iter = vect3.begin();
    vector<int> vect4(iter,iter+2); //不包括iter+2元素
    EXPECT_EQ(vect4[0],0);
    EXPECT_EQ(vect4[1],1);
}

TEST(TestVector,Operation)
{
    //赋值之前必须申请空间
    vector<int> vect(3);


    vect[0] = 0;
    vect[1] = 1;
    vect[2] = 2;
    EXPECT_EQ(vect[0],0);
    EXPECT_EQ(vect[1],1);
    EXPECT_EQ(vect[2],2);

    //扩展空间reserve()
    EXPECT_EQ(vect.size(),3);
    EXPECT_EQ(vect.capacity(),3);
    vect.reserve(5);
    EXPECT_EQ(vect.size(),3);
    EXPECT_EQ(vect.capacity(),5);
    EXPECT_EQ(vect[0],0);
    EXPECT_EQ(vect[1],1);
    EXPECT_EQ(vect[2],2);

    //设置元素值assign()
    vect.assign(2,6);
    EXPECT_EQ(vect[0],6);
    EXPECT_EQ(vect[1],6);
    EXPECT_EQ(vect.size(),2);
    vect.assign(3,5);
    EXPECT_EQ(vect.size(),3);
    EXPECT_EQ(vect[0],5);
    EXPECT_EQ(vect[1],5);
    EXPECT_EQ(vect[2],5);
    vect.assign(4,4);
    EXPECT_EQ(vect.size(),4);
    EXPECT_EQ(vect.capacity(),5);
    EXPECT_EQ(vect[0],4);
    EXPECT_EQ(vect[1],4);
    EXPECT_EQ(vect[2],4);

    //front(),back()
    vect[0] = 0;
    vect[1] = 1;
    vect[2] = 2;
    EXPECT_EQ(vect.front(),0);
    EXPECT_EQ(vect.back(),4);

    //在元素后面添加元素push_back()
    //0,1,2,4,5,6
    vect.push_back(5);
    vect.push_back(6);
    EXPECT_EQ(vect.size(),6);
    EXPECT_EQ(vect[4],5);
    EXPECT_EQ(vect[5],6);


    //插入元素
    //0,1,2,3,4,5,6
    vect.insert(vect.begin()+3,3);
    EXPECT_EQ(vect[2],2);
    EXPECT_EQ(vect[3],3);
    EXPECT_EQ(vect[4],4);

    vector<int> vect2(3,55);
    vector<int>::iterator iter = vect2.begin();
    vect.insert(vect.end(),iter,iter+3);
    EXPECT_EQ(vect[6],6);
    EXPECT_EQ(vect[7],55);
    EXPECT_EQ(vect[8],55);
    EXPECT_EQ(vect[9],55);

    vect.insert(vect.begin()+1,2,77);
    EXPECT_EQ(vect[0],0);
    EXPECT_EQ(vect[1],77);
    EXPECT_EQ(vect[2],77);
    EXPECT_EQ(vect[3],1);

    //弹出元素pop_back()
    vect.pop_back();
    EXPECT_EQ(vect.size(),11);

    //删除元素erase()
    vect.erase(vect.begin()+1);
    EXPECT_EQ(vect.size(),10);
    EXPECT_EQ(vect[0],0);
    EXPECT_EQ(vect[1],77);
    EXPECT_EQ(vect[2],1);
    EXPECT_EQ(vect[3],2);

    vect.erase(vect.begin()+1,vect.begin()+4);
    EXPECT_EQ(vect[0],0);
    EXPECT_EQ(vect[1],3);
    EXPECT_EQ(vect[2],4);
    EXPECT_EQ(vect[3],5);

    //调整大小resize()
    vect.resize(10,22);
    EXPECT_EQ(vect.size(),10);
    EXPECT_EQ(vect[8],22);
    EXPECT_EQ(vect[9],22);

    //清空clear()
    vect.clear();
    EXPECT_EQ(vect.size(),0);
}

TEST(TestVector,ReverseIterator)
{
    vector<int> vect;
    vect.push_back(1);
    vect.push_back(2);
    vect.push_back(3);

    vector<int>::reverse_iterator iter = vect.rbegin();
    EXPECT_EQ(3,*iter);
    ++iter;
    EXPECT_EQ(2,*iter);
    ++iter;
    EXPECT_EQ(1,*iter);
    ++iter;
    --iter;
    EXPECT_EQ(1,*iter);
    --iter;
    EXPECT_EQ(2,*iter);
    --iter;
    EXPECT_EQ(3,*iter);

    //
    iter = vect.rbegin();
    EXPECT_EQ(1,*(iter+2));
}
