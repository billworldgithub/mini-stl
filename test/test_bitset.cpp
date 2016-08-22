//test_bitset.cpp

#include <gtest/gtest.h>
#include "../bitset_imp.h"

using namespace mini_stl;

TEST(TestBitset,Constructor)
{
    string str;
    bitset<10> bits;
    bits.copy_to_string(str);
    EXPECT_STREQ("0000000000",str.c_str());

    bitset<40> bs2(0x1a2b3c4d);
    bs2.copy_to_string(str);
    EXPECT_STREQ("0000000000011010001010110011110001001101",str.c_str());

    string str1 = "1001010011101";
    bitset<8> bs3(str1,0,6);
    bs3.copy_to_string(str);
    EXPECT_STREQ("00100101",str.c_str());


}

TEST(TestBitset,Operator)
{
    string str;
    string str1;
    //operator &=
    str1.assign("10110110");
    string str2("10010010");
    bitset<8> bs4(str1);
    bitset<8> bs5(str2);
    bs4 &= bs5;
    bs4.copy_to_string(str);
    EXPECT_STREQ("10010010",str.c_str());

    //operator |=
    str1.assign("1011");
    str2.assign("1001");
    bs4.copy_from_string(str1,0,str1.size());
    bs5.copy_from_string(str2,0,str2.size());
    bs4 |= bs5;
    bs4.copy_to_string(str);
    EXPECT_STREQ("00001011",str.c_str());

    //operator ^=
    str1.assign("1011");
    str2.assign("1001");
    bs4.copy_from_string(str1,0,str1.size());
    bs5.copy_from_string(str2,0,str2.size());
    bs4 ^= bs5;
    bs4.copy_to_string(str);
    EXPECT_STREQ("00000010",str.c_str());

    //operator <<=
    str1.assign("1011");
    bs4.copy_from_string(str1,0,str1.size());
    bs4 <<= 2;
    bs4.copy_to_string(str);
    EXPECT_STREQ("00101100",str.c_str());

    //operator >>=
    str1.assign("1011");
    bs4.copy_from_string(str1,0,str1.size());
    bs4 >>= 2;
    bs4.copy_to_string(str);
    EXPECT_STREQ("00000010",str.c_str());
}

TEST(TestBitset,Extension)
{
    bitset<8> bs1;
    string str1 = "11010011";
    string str0;
    bs1.copy_from_string(str1,0,str1.size());

    bs1.set();
    bs1.copy_to_string(str0);
    EXPECT_STREQ("11111111",str0.c_str());

    bs1.reset();
    bs1.copy_to_string(str0);
    EXPECT_STREQ("00000000",str0.c_str());

    bs1.set(0);
    bs1.copy_to_string(str0);
    EXPECT_STREQ("00000001",str0.c_str());

    bs1.set(1,1);
    bs1.copy_to_string(str0);
    EXPECT_STREQ("00000011",str0.c_str());

    bs1.reset(1);
    bs1.copy_to_string(str0);
    EXPECT_STREQ("00000001",str0.c_str());

    bs1.flip();
    bs1.copy_to_string(str0);
    EXPECT_STREQ("11111110",str0.c_str());

    bs1.flip(7);
    bs1.copy_to_string(str0);
    EXPECT_STREQ("01111110",str0.c_str());

    bitset<8> bs2 = ~bs1;
    bs2.copy_to_string(str0);
    EXPECT_STREQ("10000001",str0.c_str());
    EXPECT_EQ(true,bs2[0]);
    EXPECT_EQ(0x81,bs2.to_ulong());
    EXPECT_EQ(2,bs2.count());
    EXPECT_EQ(8,bs2.size());
    EXPECT_EQ(true,bs2.any());
    EXPECT_EQ(false,bs2.none());
    EXPECT_EQ(0,bs2.find_first());
    EXPECT_EQ(7,bs2.find_next(0));

    str1.assign("10000001");
    bs1.copy_from_string(str1,0,str1.size());
    EXPECT_EQ(true,bs1 == bs2);

    bs1.set(1);
    EXPECT_EQ(true,bs1 != bs2);

    bs2 = bs1 << 1;
    bs2.copy_to_string(str0);
    EXPECT_STREQ("00000110",str0.c_str());

    bs2 = bs1 >> 2;
    bs2.copy_to_string(str0);
    EXPECT_STREQ("00100000",str0.c_str());

    str1.assign("10011001");
    bs1.copy_from_string(str1,0,str1.size());

    str1.assign("11100110");
    bs2.copy_from_string(str1,0,str1.size());

    bitset<8> bs3;
    bs3 = bs1 & bs2;
    bs3.copy_to_string(str0);
    EXPECT_STREQ("10000000",str0.c_str());

    bs3 = bs1 | bs2;
    bs3.copy_to_string(str0);
    EXPECT_STREQ("11111111",str0.c_str());

    bs3 = bs1 ^ bs2;
    bs3.copy_to_string(str0);
    EXPECT_STREQ("01111111",str0.c_str());

}
