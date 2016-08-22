#include <iostream>
#include <cstring>
#include <gtest/gtest.h>


using std::cout;
using std::endl;

//class ConstTest
//{
//public:
//    void print() const
//    {
//        cout << "const func" << endl;
//    }

//    void print()
//    {
//        cout << "common func" << endl;
//    }
//};

//class A
//{};

//class B
//{
//public:
//    virtual void printf(){}
//    virtual void printf1() {}
//    int value;
//};

//class C
//{
//    char a;
//    short b;
//    int c;
//};

template<class T>
void Hello(T value)
{
    cout << "common template" << endl;
}

template<>
void Hello(int value)
{
    cout << "special template" << endl;
}

void Hello(short value)
{
    cout << "common func" << endl;
}

//void Hello(int value)
//{
//    cout << "common func" << endl;
//}


int main(int argc, char *argv[])
{
    int a = 1;
    short b = 1;
    char c = 1;
    Hello(a);
    Hello(b);
    Hello(c);
    Hello("hello");

//    ConstTest test;
//    test.print();
//    const ConstTest test1;
//    test1.print();

//    cout << sizeof(A) << endl;
//    cout << sizeof(B) << endl;
//    cout << sizeof(C) << endl;

//    std::string str4("12121212");
//    std::string str5("12");
//    int pos = str4.rfind(str5,3);
//    cout << pos << endl;

//    std::vector<int> vect;
//    vect.push_back(1);
//    vect.push_back(2);
//    vect.push_back(3);

//    std::vector<int>::reverse_iterator iter = vect.rbegin();
//    cout << *(iter.base()) << endl;
//    cout << *(iter.base()-1) << endl;
//    cout << *iter << endl;

    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();



    return 0;
}


