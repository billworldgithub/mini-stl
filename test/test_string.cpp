//test_string.cpp

#include "../string_imp.h"
#include <iostream>
#include <gtest/gtest.h>

using namespace mini_stl;

TEST(TestString,Construct)
{
    //
    string str;
    EXPECT_EQ(str.size(),0);
    EXPECT_EQ(str.capacity(),7);

    string str1("0123456",5);
    EXPECT_EQ(str1.size(),5);

    string str2(str1);
    EXPECT_EQ(str2.size(),5);

    string str3(str1,1,3);
    EXPECT_EQ(str3.size(),3);

    string str4("12345");
    EXPECT_EQ(str4.size(),5);

    string str5(5,'1');
    EXPECT_EQ(str5.size(),5);

    string::const_iterator iter = str1.begin();
    string str6(iter,iter+3);
    EXPECT_EQ(str6.size(),3);

    //operator=()
    string str7;
    str7 = str4;
    EXPECT_STREQ(str7.begin(),"12345");

    str7 = "abc";
    EXPECT_STREQ(str7.begin(),"abc");

    str7 = 'd';
    EXPECT_STREQ(str7.begin(),"d");

}

TEST(TestString,Size)
{
    string str("123456");
    str.resize(3,'c');
    EXPECT_STREQ(str.begin(),"123");

    str.resize(8,'a');
    EXPECT_EQ(8,str.capacity());
    EXPECT_STREQ(str.begin(),"123aaaaa");

    str.resize(12);
    EXPECT_EQ(16,str.capacity());
    EXPECT_STREQ("123aaaaa",str.begin());

    EXPECT_EQ(false,str.empty());

    str.clear();
    EXPECT_EQ(true,str.empty());
}

TEST(TestString,Index)
{
    string str("123");
    EXPECT_EQ('1',str[0]);
    EXPECT_EQ('2',str.at(1));
}

TEST(TestString,Assign)
{
    //basic_string& assign(const CharT* f,const CharT* l)
    string str1("0123456");
    string str2("abcdef");
    string str3("mmmmmmmmmmmm");

    str2.assign(str1.begin(),str1.end());
    EXPECT_STREQ(str2.begin(),"0123456");
    EXPECT_EQ(str2.size(),7);
    EXPECT_EQ(str2.capacity(),12);

    str3.assign(str1.begin(),str1.end());
    EXPECT_STREQ(str3.begin(),"0123456");
    EXPECT_EQ(str3.size(),7);
    EXPECT_EQ(str3.capacity(),12);

    //basic_string& assign(size_type n,CharT c)
    string str4("123456");
    str4.assign(3,'a');
    EXPECT_EQ(str4.capacity(),6);
    EXPECT_EQ(str4.size(),3);
    EXPECT_STREQ(str4.begin(),"aaa");

    str4.assign(10,'b');
    EXPECT_EQ(str4.capacity(),10);
    EXPECT_EQ(str4.size(),10);
    EXPECT_STREQ(str4.begin(),"bbbbbbbbbb");
}

TEST(TestString,Append)
{
    //basic_string& append(const CharT* first,const CharT* last)
    string str1("123456");
    EXPECT_EQ(str1.capacity(),6);

    string str2("abc");
    str1.append(str2.begin(),str2.end());
    EXPECT_EQ(str1.capacity(),12);
    EXPECT_EQ(str1.size(),9);
    EXPECT_STREQ(str1.begin(),"123456abc");

    string str3("de");
    str1.append(str3.begin(),str3.end());
    EXPECT_EQ(str1.capacity(),12);
    EXPECT_EQ(str1.size(),11);
    EXPECT_STREQ(str1.begin(),"123456abcde");

    //basic_string& append(size_type n,CharT c)
    string str4("1234567");
    str4.append(3,'a');
    EXPECT_EQ(str4.capacity(),14);
    EXPECT_EQ(str4.size(),10);
    EXPECT_STREQ(str4.begin(),"1234567aaa");

    str4.append(2,'b');
    EXPECT_EQ(str4.capacity(),14);
    EXPECT_EQ(str4.size(),12);
    EXPECT_STREQ(str4.begin(),"1234567aaabb");

    string str5("123");
    string str6("abc");
    str5 += str6;
    EXPECT_STREQ(str5.begin(),"123abc");

    str5 += "def";
    EXPECT_STREQ(str5.begin(),"123abcdef");

    str5 += 'g';
    EXPECT_STREQ(str5.begin(),"123abcdefg");


}

TEST(TestString,Insert)
{
    //void insert(iterator p,const CharT* first,const CharT* last);
    string str("123456");
    str.reserve(16);
    EXPECT_EQ(6,str.size());
    EXPECT_EQ(16,str.capacity());

    string str1("abc");
    str.insert(str.begin()+1,str1.begin(),str1.end());
    EXPECT_STREQ("1abc23456",str.begin());

    str.insert(str.begin()+8,str1.begin(),str1.end());
    EXPECT_STREQ("1abc2345abc6",str.begin());
    EXPECT_EQ(16,str.capacity());

    string str2("123456");
    str.insert(str.begin()+10,str2.begin(),str2.end());
    EXPECT_STREQ("1abc2345ab123456c6",str.begin());
    EXPECT_EQ(24,str.capacity());

    //清空
    //basic_string& insert(size_type pos,const basic_string& s)
    str.clear();
    str.assign("123456");
    EXPECT_STREQ("123456",str.begin());
    str.insert(1,str1);
    EXPECT_STREQ("1abc23456",str.begin());

//    basic_string& insert(size_type pos,const basic_string& s,
//                         size_type beg,size_type n)
    str.assign("123456");
    str1.assign("abc");
    str.insert(1,str1,1,1);
    EXPECT_STREQ("1b23456",str.begin());


    str.reserve(10);
    str.assign("123456");
    str.insert(str.begin()+1,2,'a');
    EXPECT_STREQ("1aa23456",str.begin());
    str.assign("123456");
    str.insert(str.begin()+5,3,'a');
    EXPECT_STREQ("12345aaa6",str.begin());
    str.insert(str.begin(),3,'b');
    EXPECT_STREQ("bbb12345aaa6",str.begin());


    str.assign("123456");
    str.insert(str.end(),'a');
    EXPECT_STREQ("123456a",str.begin());
    str.insert(str.begin()+1,'b');
    EXPECT_STREQ("1b23456a",str.begin());
    str.assign("123");
    str.reserve(3);
    EXPECT_EQ(3,str.capacity());
    str.insert(str.begin(),'a');
    EXPECT_STREQ("a123",str.begin());
}


TEST(TestString,Erase)
{
    string str1("123456");
    string::iterator iter = str1.begin();

    str1.erase(iter+1,iter+3);
    EXPECT_EQ(str1.size(),4);
    EXPECT_STREQ(str1.begin(),"1456");

    str1.assign("123456");
    str1.erase(2,2);
    EXPECT_STREQ("1256",str1.begin());
    str1.erase(str1.begin());
    EXPECT_STREQ("256",str1.begin());
}

TEST(TestString,Replace)
{
    string str1("123456");
    string str2("abc");
    string str3("defg");
    str1.replace(str1.begin(),str1.end(),str2.begin(),str2.end());
    EXPECT_STREQ("abc",str1.begin());
    str1.replace(str1.begin()+1,str1.end(),str3.begin(),str3.end());
    EXPECT_STREQ("adefg",str1.begin());

    str1.assign("123456");
    str1.replace(str1.begin(),str1.end(),3,'a');
    EXPECT_STREQ("aaa",str1.begin());
    str1.replace(str1.begin(),str1.end(),6,'b');
    EXPECT_STREQ("bbbbbb",str1.begin());

    str1.assign("123456");
    str1.replace(1,2,str2);
    EXPECT_STREQ("1abc456",str1.begin());

    str1.assign("123456");
    str1.replace(1,2,str2,0,2);
    EXPECT_STREQ("1ab456",str1.begin());

    char * s = "abc";
    str1.assign("123456");
    str1.replace(1,2,s,3);
    EXPECT_STREQ("1abc456",str1.begin());

    str1.assign("123456");
    str1.replace(3,3,s);
    EXPECT_STREQ("123abc",str1.begin());

    str1.assign("123456");
    str1.replace(3,3,4,'a');
    EXPECT_STREQ("123aaaa",str1.begin());

    str1.assign("123456");
    str1.replace(str1.begin(),str1.end(),str2);
    EXPECT_STREQ("abc",str1.begin());

    str1.assign("123456");
    str1.replace(str1.begin(),str1.end(),s,2);
    EXPECT_STREQ("ab",str1.begin());

    str1.assign("123456");
    str1.replace(str1.begin(),str1.end(),s);
    EXPECT_STREQ("abc",str1.begin());
}

TEST(TestString,Find)
{
    string str("123456");
    string str1("3");
    string str2("23");
    int pos = str.find(str1.c_str(),0,1);
    EXPECT_EQ(2,pos);
    pos = str.find(str2.c_str(),0,2);
    EXPECT_EQ(1,pos);

    pos = str.find('4',1);
    EXPECT_EQ(3,pos);
    pos = str.find('7',0);
    EXPECT_EQ(string::npos,pos);

    pos = str.find(str2,0);
    EXPECT_EQ(1,pos);

    char * str3 = "45";
    pos = str.find(str3,0);
    EXPECT_EQ(3,pos);

    //rfind
    string str4("12121212");
    pos = str4.rfind("12",4,2);
    EXPECT_EQ(4,pos);

    pos = str4.rfind('2',4);
    EXPECT_EQ(3,pos);

    string str5("12");
    pos = str4.rfind(str5,3);
    EXPECT_EQ(2,pos);

    pos = str4.rfind("12",4);
    EXPECT_EQ(4,pos);

    //find_first_of
    str1.assign("123456");
    str2.assign("abc4");
    pos = str1.find_first_of(str2,0);
    EXPECT_EQ(3,pos);

    pos = str1.find_first_of("ac56");
    EXPECT_EQ(4,pos);

    pos = str1.find_first_of('2');
    EXPECT_EQ(1,pos);

    str1.assign("123456");
    str2.assign("12346");
    pos = str1.find_first_not_of(str2);
    EXPECT_EQ(4,pos);
    pos = str1.find_first_not_of("12346");
    EXPECT_EQ(4,pos);

}

TEST(TestString,Substr)
{
    string str1("123456");
    string str2 = str1.substr(2,3);
    EXPECT_STREQ("345",str2.begin());
}

TEST(TestString,Compare)
{
    string str1("12345");
    string str2("123");
    string str3("12345");
    string str4("1234567");
    int result = str1.compare(str2);
    EXPECT_EQ(1,result);
    result = str1.compare(str3);
    EXPECT_EQ(0,result);
    result = str1.compare(str4);
    EXPECT_EQ(-1,result);
}


