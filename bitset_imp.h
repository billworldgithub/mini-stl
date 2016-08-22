//bitset_imp.h
//实现bitset功能

#ifndef BITSET_IMP_H
#define BITSET_IMP_H

#include "configure.h"
#include "algobase.h"
#include <limits>
#include "string_imp.h"
#include <stdexcept>

using std::out_of_range;
using std::invalid_argument;

//bitset内部使用long存储bit数据
//每个双字包含的位数
#define BITS_PER_WORD (CHAR_BIT*sizeof(unsigned long))

//将位数转化为双字数目
#define BITSET_WORDS(n) \
    ((n) < 1 ? 1 : ((n)+ BITS_PER_WORD - 1)/BITS_PER_WORD)


STL_BEGIN_NAMESPACE



//存储每个单字节数中bit为1的个数，例如2只有bit1为1，5有bit0和bit2为1
template<bool dummy>
struct Bit_count
{
    static unsigned char bit_count[256];
};

//存储每个单字节第1个bit为1的位置
template<bool dummy>
struct First_one
{
    static unsigned char first_one[256];
};

//////////////////////////////////////////////
//bitset基类
template<size_t Nw>
struct Base_bitset
{
    //bitset内部使用long存储bit数据
    typedef unsigned long WordT;

    WordT w[Nw];

    Base_bitset(void)
    {
        do_reset();
    }

    Base_bitset(unsigned long val)
    {
        do_reset();
        w[0] = val;
    }

    //将bit位置转化为word位置
    static size_t whichword(size_t pos)
    {
        return pos/BITS_PER_WORD;
    }

    //除去整word后的pos转化到字节
    static size_t whichbyte(size_t pos)
    {
        return (pos % BITS_PER_WORD) / CHAR_BIT;
    }

    //除去整word后的pos转化为bit地址
    static size_t whichbit(size_t pos)
    {
        return (pos % BITS_PER_WORD);
    }

    //通过pos建立相应的掩码
    static WordT maskbit(size_t pos)
    {
        return (static_cast<WordT>(1)) << whichbit(pos);
    }

    //获取包括位的word
    WordT& getword(size_t pos)
    {
        return w[whichword(pos)];
    }

    WordT getword(size_t pos) const
    {
        return w[whichword(pos)];
    }

    //返回最高字节
    WordT& hiword()
    {
        return w[Nw-1];
    }

    WordT hiword() const
    {
        return w[Nw-1];
    }

    //逻辑操作
    void do_and(const Base_bitset<Nw>& x)
    {
        for(size_t i = 0;i < Nw;i++)
            w[i] &= x.w[i];
    }

    void do_or(const Base_bitset<Nw>& x)
    {
        for(size_t i = 0;i < Nw;i++)
            w[i] |= x.w[i];
    }

    void do_xor(const Base_bitset<Nw>& x)
    {
        for(size_t i = 0;i < Nw;i++)
            w[i] ^= x.w[i];
    }

    //向左偏移shift位
    void do_left_shift(size_t shift);

    //向右偏移shift位
    void do_right_shift(size_t shift);

    //全部取反
    void do_flip()
    {
        for(size_t i = 0;i < Nw;i++)
            w[i] = ~w[i];
    }

    //全部置1
    void do_set()
    {
        for(size_t i = 0;i < Nw;i++)
            w[i] = ~static_cast<WordT>(0);
    }

    //全部置0
    void do_reset()
    {
        memset(w,0,Nw*sizeof(WordT));
    }

    //相等返回true
    bool is_equal(const Base_bitset<Nw>& x) const
    {
        for(size_t i = 0;i < Nw;i++)
            if(w[i] != x.w[i])
                return false;
        return true;
    }

    //不为0返回true
    bool is_any() const
    {
        for(size_t i = 0;i < Nw;i++)
            if(w[i] != static_cast<WordT>(0))
                return true;
        return false;
    }

    //获取bit为1的个数
    size_t do_count() const
    {
        size_t result = 0;
        const unsigned char* byte_ptr = (const unsigned char*)w;
        const unsigned char* end_ptr = (const unsigned char*)(w+Nw);

        //通过查表计算一共有多少个1出现
        while(byte_ptr < end_ptr)
        {
            result += Bit_count<true>::bit_count[*byte_ptr];
            ++byte_ptr;
        }

        return result;
    }

    unsigned long do_to_ulong() const;

    //返回第个bit不为0的位置
    size_t do_find_first(size_t not_found) const;

    size_t do_find_next(size_t prev,size_t not_found) const;
};

template<size_t Nw>
void Base_bitset<Nw>::do_left_shift(size_t shift)
{
    if(shift != 0)
    {
        //偏移的word
        const size_t wshift = shift / BITS_PER_WORD;
        //偏移的bit
        const size_t offset = shift % BITS_PER_WORD;

        if(offset == 0)
        {
            for(size_t n = Nw-1;n >= shift;--n)
                w[n] = w[n-shift];
        }
        else
        {
            const size_t sub_offset = BITS_PER_WORD - offset;
            for(size_t n = Nw-1;n > wshift;--n)
                w[n] = (w[n-wshift] << offset | w[n-wshift-1] >> sub_offset);
            w[wshift] = w[0] << offset;
        }

        fill(w+0,w+wshift,static_cast<WordT>(0));

    }
}

template<size_t Nw>
void Base_bitset<Nw>::do_right_shift(size_t shift)
{
    if(shift != 0)
    {
        const size_t wshift = shift / BITS_PER_WORD;
        const size_t offset = shift % BITS_PER_WORD;
        const size_t limit = Nw - wshift - 1;

        if(offset == 0)
        {
            for(size_t n = 0;n <= limit;++n)
                w[n] = w[n+wshift];
        }
        else
        {
            const size_t sub_offset = BITS_PER_WORD - offset;

            for(size_t n = 0;n < limit;++n)
            {
                w[n] = (w[n+wshift]>>offset | w[n+wshift+1] << sub_offset);
            }
            w[limit] = w[Nw-1] >> offset;
        }

        fill(w+limit+1,w+Nw,static_cast<WordT>(0));
    }
}

//返回word
template<size_t Nw>
unsigned long Base_bitset<Nw>::do_to_ulong() const
{
    return w[0];
}

template<size_t Nw>
size_t Base_bitset<Nw>::do_find_first(size_t not_found) const
{
    for(size_t i = 0;i < Nw;i++)
    {
        WordT thisword = w[i];
        if(thisword != static_cast<WordT>(0))
        {
            for(size_t j = 0;j < sizeof(WordT);j++)
            {
                unsigned char this_byte = static_cast<unsigned char>(thisword & (~(unsigned char)0));
                if(this_byte)
                    return i * BITS_PER_WORD + j * CHAR_BIT + First_one<true>::first_one[this_byte];
                thisword >>= CHAR_BIT;
            }
        }
    }

    return not_found;
}

template<size_t Nw>
size_t Base_bitset<Nw>::do_find_next(size_t prev, size_t not_found) const
{
    //在非整word中搜索
    ++prev;

    if(prev >= Nw*BITS_PER_WORD)
        return not_found;

    size_t i = whichword(prev);
    WordT thisword = w[i];

    //prev之前的数据清零
    thisword &= (~static_cast<WordT>(0)) << whichbit(prev);

    if(thisword != static_cast<WordT>(0))
    {
        thisword >>= whichbyte(prev) * CHAR_BIT;

        for(size_t j = whichbyte(prev);j < sizeof(WordT);j++)
        {
            unsigned char this_byte = static_cast<unsigned char>(thisword & (~(unsigned char)0));
            if(this_byte)
                return i*BITS_PER_WORD+j*CHAR_BIT+First_one<true>::first_one[this_byte];
            thisword >>= CHAR_BIT;
        }
    }

    //在整word中搜索
    i++;
    for(;i < Nw;i++)
    {
        WordT thisword = w[i];
        if(thisword != static_cast<WordT>(0))
        {
            for(size_t j = 0;j < sizeof(WordT);j++)
            {
                unsigned char this_byte = static_cast<unsigned char>(thisword & (~(unsigned char)0));
                if(this_byte)
                    return i*BITS_PER_WORD+j*CHAR_BIT+First_one<true>::first_one[this_byte];
                thisword >>= CHAR_BIT;
            }
        }
    }

    return not_found;
}

//这里使用类模板是为了能够控制Extrabits
template <size_t Extrabits>
struct Sanitize
{
    static void do_sanitize(unsigned long& val)
    {
        val &= ~((~static_cast<unsigned long>(0)) << Extrabits);
    }
};

template<>
struct Sanitize<0>
{
    static void do_sanitize(unsigned long) {}
};


//////////////////////////////////////////////
//bitset
template<size_t Nb>
class bitset : private Base_bitset<BITSET_WORDS(Nb)>
{
private:
    typedef Base_bitset<BITSET_WORDS(Nb)> Base;
    typedef unsigned long WordT;

private:

    //将最高word的多余bit清空，这里需要this，否则需要using
    void do_sanitize()
    {
        Sanitize<Nb%BITS_PER_WORD>::do_sanitize(this->hiword());
    }

public: //实现p[i]操作
    class reference;
    friend class reference;

    class reference
    {
        friend class bitset;

        WordT* wp;
        size_t bpos;
        reference();

    public:
        reference(bitset& b,size_t pos)
        {
            wp = &b.getword(pos);
            bpos = Base::whichbit(pos);
        }

        ~reference() {}

    public:
        //b[i] = x;
        reference& operator =(bool x)
        {
            if(x)
                *wp |= Base::maskbit(bpos);
            else
                *wp &= ~(Base::maskbit(bpos));
            return *this;
        }

        //b[i] = b[j]
        reference& operator =(const reference& j)
        {
            if((*j.wp) & Base::maskbit(j.bpos))
                *wp |= Base::maskbit(bpos);
            else
                *wp &= ~(Base::maskbit(bpos));
            return *this;
        }

        bool operator ~() const
        {
            return (*wp & Base::maskbit(bpos)) == 0;
        }

        operator bool() const
        {
            return (*wp & Base::maskbit(bpos)) != 0;
        }

        reference& flip()
        {
            *wp ^= Base::maskbit(bpos);
            return *this;
        }
    };

public:
    bitset() {}
    bitset(unsigned long val)
        : Base(val)
    {
        do_sanitize();
    }

    explicit bitset(const string& s,size_t pos = 0,size_t n = string::npos)
        : Base()
    {
        if(pos > s.size())
            STL_THROW(out_of_range("bitset"));

        copy_from_string(s,pos,n);
    }

public: //操作符重载
    bitset<Nb>& operator &=(const bitset<Nb>& rhs)
    {
        this->do_and(rhs);
        return *this;
    }

    bitset<Nb>& operator |=(const bitset<Nb>& rhs)
    {
        this->do_or(rhs);
        return *this;
    }

    bitset<Nb>& operator ^=(const bitset<Nb>& rhs)
    {
        this->do_xor(rhs);
        return *this;
    }

    bitset<Nb>& operator <<=(size_t pos)
    {
        this->do_left_shift(pos);
        this->do_sanitize();
        return *this;
    }

    bitset<Nb>& operator >>=(size_t pos)
    {
        this->do_right_shift(pos);
        this->do_sanitize();
        return *this;
    }


public: //扩展功能
    bitset<Nb>& unchecked_set(size_t pos)
    {
        this->getword(pos) |= Base::maskbit(pos);
    }

    bitset<Nb>& unchecked_set(size_t pos,int val)
    {
        if(val)
            this->getword(pos) |= Base::maskbit(pos);
        else
            this->getword(pos) &= ~Base::maskbit(pos);

        return *this;
    }

    bitset<Nb>& unchecked_reset(size_t pos)
    {
        this->getword(pos) &= ~Base::maskbit(pos);
        return *this;
    }

    bitset<Nb>& unchecked_flip(size_t pos)
    {
        this->getword(pos) ^= Base::maskbit(pos);
        return *this;
    }

    bool unchecked_test(size_t pos) const
    {
        return ((this->getword(pos) & Base::maskbit(pos)) != static_cast<WordT>(0));
    }

    //set,reset,flip
    bitset<Nb>& set()
    {
        this->do_set();
        this->do_sanitize();
        return *this;
    }

    bitset<Nb>& set(size_t pos)
    {
        if(pos > Nb)
            STL_THROW(out_of_range("bitset"));

        return unchecked_set(pos);
    }

    bitset<Nb>& set(size_t pos,int val)
    {
        if(pos > Nb)
            STL_THROW(out_of_range("bitset"));
        return unchecked_set(pos,val);
    }

    bitset<Nb>& reset()
    {
        this->do_reset();
        return *this;
    }

    bitset<Nb>& reset(size_t pos)
    {
        if(pos > Nb)
            STL_THROW(out_of_range("bitset"));
        return unchecked_reset(pos);
    }

    bitset<Nb>& flip()
    {
        this->do_flip();
        this->do_sanitize();
        return *this;
    }

    bitset<Nb>& flip(size_t pos)
    {
        if(pos > Nb)
            STL_THROW(out_of_range("bitset"));
        return unchecked_flip(pos);
    }

    bitset<Nb> operator~() const
    {
        return bitset<Nb>(*this).flip();
    }

public: //operator[]
    reference operator[](size_t pos)
    {
        return reference(*this,pos);
    }

    bool operator[](size_t pos) const
    {
        return unchecked_test(pos);
    }

public: //helper
    unsigned long to_ulong() const
    {
        return this->do_to_ulong();
    }

    //这里的string表示的是实际存储形式，string[0]表示的是内部最高bit
    void copy_from_string(const string&,size_t,size_t);
    void copy_to_string(string&) const;

    size_t count() const
    {
        return this->do_count();
    }

    size_t size() const
    {
        return Nb;
    }

    bool operator==(const bitset<Nb>& rhs) const
    {
        return this->is_equal(rhs);
    }

    bool operator!=(const bitset<Nb>& rhs) const
    {
        return !this->is_equal(rhs);
    }

    bool test(size_t pos) const
    {
        if(pos > Nb)
            STL_THROW(out_of_range("bitset"));
        return unchecked_test(pos);
    }

    bool any() const
    {
        return this->is_any();
    }

    bool none() const
    {
        return !this->is_any();
    }

    bitset<Nb> operator <<(size_t pos) const
    {
        return bitset<Nb>(*this) <<= pos;
    }

    bitset<Nb> operator >>(size_t pos) const
    {
        return bitset<Nb>(*this) >>= pos;
    }

    size_t find_first() const
    {
        return this->do_find_first(Nb);
    }

    size_t find_next(size_t prev) const
    {
        return this->do_find_next(prev,Nb);
    }



};

//将string转化成bitset
template<size_t Nb>
void bitset<Nb>::copy_from_string(const string& s,size_t pos,size_t n)
{
    //清空数据
    reset();

    size_t tmp = Nb;
    const size_t nbits = min(tmp,min(n,s.size()-pos));
    for(size_t i = 0;i < nbits;i++)
    {
        switch (s[pos+nbits-i-1])
        {
        case '0':
            break;
        case '1':
            set(i);
            break;
        default:
            STL_THROW(invalid_argument("bitset"));
        }
    }
}

template<size_t Nb>
void bitset<Nb>::copy_to_string(string& s) const
{
    s.assign(Nb,'0');
    for(size_t i = 0;i < Nb;i++)
    {
        if(unchecked_test(i))
            s[Nb-1-i] = '1';
    }
}

template<size_t Nb>
inline bitset<Nb> operator&(const bitset<Nb>& x,const bitset<Nb>& y)
{
    bitset<Nb> result(x);
    result &= y;
    return result;
}

template<size_t Nb>
inline bitset<Nb> operator|(const bitset<Nb>& x,const bitset<Nb>& y)
{
    bitset<Nb> result(x);
    result |= y;
    return result;
}

template<size_t Nb>
inline bitset<Nb> operator^(const bitset<Nb>& x,const bitset<Nb>& y)
{
    bitset<Nb> result(x);
    result ^= y;
    return result;
}

template<bool dummy>
unsigned char Bit_count<dummy>::bit_count[] = {
  0, /*   0 */ 1, /*   1 */ 1, /*   2 */ 2, /*   3 */ 1, /*   4 */
  2, /*   5 */ 2, /*   6 */ 3, /*   7 */ 1, /*   8 */ 2, /*   9 */
  2, /*  10 */ 3, /*  11 */ 2, /*  12 */ 3, /*  13 */ 3, /*  14 */
  4, /*  15 */ 1, /*  16 */ 2, /*  17 */ 2, /*  18 */ 3, /*  19 */
  2, /*  20 */ 3, /*  21 */ 3, /*  22 */ 4, /*  23 */ 2, /*  24 */
  3, /*  25 */ 3, /*  26 */ 4, /*  27 */ 3, /*  28 */ 4, /*  29 */
  4, /*  30 */ 5, /*  31 */ 1, /*  32 */ 2, /*  33 */ 2, /*  34 */
  3, /*  35 */ 2, /*  36 */ 3, /*  37 */ 3, /*  38 */ 4, /*  39 */
  2, /*  40 */ 3, /*  41 */ 3, /*  42 */ 4, /*  43 */ 3, /*  44 */
  4, /*  45 */ 4, /*  46 */ 5, /*  47 */ 2, /*  48 */ 3, /*  49 */
  3, /*  50 */ 4, /*  51 */ 3, /*  52 */ 4, /*  53 */ 4, /*  54 */
  5, /*  55 */ 3, /*  56 */ 4, /*  57 */ 4, /*  58 */ 5, /*  59 */
  4, /*  60 */ 5, /*  61 */ 5, /*  62 */ 6, /*  63 */ 1, /*  64 */
  2, /*  65 */ 2, /*  66 */ 3, /*  67 */ 2, /*  68 */ 3, /*  69 */
  3, /*  70 */ 4, /*  71 */ 2, /*  72 */ 3, /*  73 */ 3, /*  74 */
  4, /*  75 */ 3, /*  76 */ 4, /*  77 */ 4, /*  78 */ 5, /*  79 */
  2, /*  80 */ 3, /*  81 */ 3, /*  82 */ 4, /*  83 */ 3, /*  84 */
  4, /*  85 */ 4, /*  86 */ 5, /*  87 */ 3, /*  88 */ 4, /*  89 */
  4, /*  90 */ 5, /*  91 */ 4, /*  92 */ 5, /*  93 */ 5, /*  94 */
  6, /*  95 */ 2, /*  96 */ 3, /*  97 */ 3, /*  98 */ 4, /*  99 */
  3, /* 100 */ 4, /* 101 */ 4, /* 102 */ 5, /* 103 */ 3, /* 104 */
  4, /* 105 */ 4, /* 106 */ 5, /* 107 */ 4, /* 108 */ 5, /* 109 */
  5, /* 110 */ 6, /* 111 */ 3, /* 112 */ 4, /* 113 */ 4, /* 114 */
  5, /* 115 */ 4, /* 116 */ 5, /* 117 */ 5, /* 118 */ 6, /* 119 */
  4, /* 120 */ 5, /* 121 */ 5, /* 122 */ 6, /* 123 */ 5, /* 124 */
  6, /* 125 */ 6, /* 126 */ 7, /* 127 */ 1, /* 128 */ 2, /* 129 */
  2, /* 130 */ 3, /* 131 */ 2, /* 132 */ 3, /* 133 */ 3, /* 134 */
  4, /* 135 */ 2, /* 136 */ 3, /* 137 */ 3, /* 138 */ 4, /* 139 */
  3, /* 140 */ 4, /* 141 */ 4, /* 142 */ 5, /* 143 */ 2, /* 144 */
  3, /* 145 */ 3, /* 146 */ 4, /* 147 */ 3, /* 148 */ 4, /* 149 */
  4, /* 150 */ 5, /* 151 */ 3, /* 152 */ 4, /* 153 */ 4, /* 154 */
  5, /* 155 */ 4, /* 156 */ 5, /* 157 */ 5, /* 158 */ 6, /* 159 */
  2, /* 160 */ 3, /* 161 */ 3, /* 162 */ 4, /* 163 */ 3, /* 164 */
  4, /* 165 */ 4, /* 166 */ 5, /* 167 */ 3, /* 168 */ 4, /* 169 */
  4, /* 170 */ 5, /* 171 */ 4, /* 172 */ 5, /* 173 */ 5, /* 174 */
  6, /* 175 */ 3, /* 176 */ 4, /* 177 */ 4, /* 178 */ 5, /* 179 */
  4, /* 180 */ 5, /* 181 */ 5, /* 182 */ 6, /* 183 */ 4, /* 184 */
  5, /* 185 */ 5, /* 186 */ 6, /* 187 */ 5, /* 188 */ 6, /* 189 */
  6, /* 190 */ 7, /* 191 */ 2, /* 192 */ 3, /* 193 */ 3, /* 194 */
  4, /* 195 */ 3, /* 196 */ 4, /* 197 */ 4, /* 198 */ 5, /* 199 */
  3, /* 200 */ 4, /* 201 */ 4, /* 202 */ 5, /* 203 */ 4, /* 204 */
  5, /* 205 */ 5, /* 206 */ 6, /* 207 */ 3, /* 208 */ 4, /* 209 */
  4, /* 210 */ 5, /* 211 */ 4, /* 212 */ 5, /* 213 */ 5, /* 214 */
  6, /* 215 */ 4, /* 216 */ 5, /* 217 */ 5, /* 218 */ 6, /* 219 */
  5, /* 220 */ 6, /* 221 */ 6, /* 222 */ 7, /* 223 */ 3, /* 224 */
  4, /* 225 */ 4, /* 226 */ 5, /* 227 */ 4, /* 228 */ 5, /* 229 */
  5, /* 230 */ 6, /* 231 */ 4, /* 232 */ 5, /* 233 */ 5, /* 234 */
  6, /* 235 */ 5, /* 236 */ 6, /* 237 */ 6, /* 238 */ 7, /* 239 */
  4, /* 240 */ 5, /* 241 */ 5, /* 242 */ 6, /* 243 */ 5, /* 244 */
  6, /* 245 */ 6, /* 246 */ 7, /* 247 */ 5, /* 248 */ 6, /* 249 */
  6, /* 250 */ 7, /* 251 */ 6, /* 252 */ 7, /* 253 */ 7, /* 254 */
  8  /* 255 */
}; // end _Bit_count

template<bool dummy>
unsigned char First_one<dummy>::first_one[] = {
  0, /*   0 */ 0, /*   1 */ 1, /*   2 */ 0, /*   3 */ 2, /*   4 */
  0, /*   5 */ 1, /*   6 */ 0, /*   7 */ 3, /*   8 */ 0, /*   9 */
  1, /*  10 */ 0, /*  11 */ 2, /*  12 */ 0, /*  13 */ 1, /*  14 */
  0, /*  15 */ 4, /*  16 */ 0, /*  17 */ 1, /*  18 */ 0, /*  19 */
  2, /*  20 */ 0, /*  21 */ 1, /*  22 */ 0, /*  23 */ 3, /*  24 */
  0, /*  25 */ 1, /*  26 */ 0, /*  27 */ 2, /*  28 */ 0, /*  29 */
  1, /*  30 */ 0, /*  31 */ 5, /*  32 */ 0, /*  33 */ 1, /*  34 */
  0, /*  35 */ 2, /*  36 */ 0, /*  37 */ 1, /*  38 */ 0, /*  39 */
  3, /*  40 */ 0, /*  41 */ 1, /*  42 */ 0, /*  43 */ 2, /*  44 */
  0, /*  45 */ 1, /*  46 */ 0, /*  47 */ 4, /*  48 */ 0, /*  49 */
  1, /*  50 */ 0, /*  51 */ 2, /*  52 */ 0, /*  53 */ 1, /*  54 */
  0, /*  55 */ 3, /*  56 */ 0, /*  57 */ 1, /*  58 */ 0, /*  59 */
  2, /*  60 */ 0, /*  61 */ 1, /*  62 */ 0, /*  63 */ 6, /*  64 */
  0, /*  65 */ 1, /*  66 */ 0, /*  67 */ 2, /*  68 */ 0, /*  69 */
  1, /*  70 */ 0, /*  71 */ 3, /*  72 */ 0, /*  73 */ 1, /*  74 */
  0, /*  75 */ 2, /*  76 */ 0, /*  77 */ 1, /*  78 */ 0, /*  79 */
  4, /*  80 */ 0, /*  81 */ 1, /*  82 */ 0, /*  83 */ 2, /*  84 */
  0, /*  85 */ 1, /*  86 */ 0, /*  87 */ 3, /*  88 */ 0, /*  89 */
  1, /*  90 */ 0, /*  91 */ 2, /*  92 */ 0, /*  93 */ 1, /*  94 */
  0, /*  95 */ 5, /*  96 */ 0, /*  97 */ 1, /*  98 */ 0, /*  99 */
  2, /* 100 */ 0, /* 101 */ 1, /* 102 */ 0, /* 103 */ 3, /* 104 */
  0, /* 105 */ 1, /* 106 */ 0, /* 107 */ 2, /* 108 */ 0, /* 109 */
  1, /* 110 */ 0, /* 111 */ 4, /* 112 */ 0, /* 113 */ 1, /* 114 */
  0, /* 115 */ 2, /* 116 */ 0, /* 117 */ 1, /* 118 */ 0, /* 119 */
  3, /* 120 */ 0, /* 121 */ 1, /* 122 */ 0, /* 123 */ 2, /* 124 */
  0, /* 125 */ 1, /* 126 */ 0, /* 127 */ 7, /* 128 */ 0, /* 129 */
  1, /* 130 */ 0, /* 131 */ 2, /* 132 */ 0, /* 133 */ 1, /* 134 */
  0, /* 135 */ 3, /* 136 */ 0, /* 137 */ 1, /* 138 */ 0, /* 139 */
  2, /* 140 */ 0, /* 141 */ 1, /* 142 */ 0, /* 143 */ 4, /* 144 */
  0, /* 145 */ 1, /* 146 */ 0, /* 147 */ 2, /* 148 */ 0, /* 149 */
  1, /* 150 */ 0, /* 151 */ 3, /* 152 */ 0, /* 153 */ 1, /* 154 */
  0, /* 155 */ 2, /* 156 */ 0, /* 157 */ 1, /* 158 */ 0, /* 159 */
  5, /* 160 */ 0, /* 161 */ 1, /* 162 */ 0, /* 163 */ 2, /* 164 */
  0, /* 165 */ 1, /* 166 */ 0, /* 167 */ 3, /* 168 */ 0, /* 169 */
  1, /* 170 */ 0, /* 171 */ 2, /* 172 */ 0, /* 173 */ 1, /* 174 */
  0, /* 175 */ 4, /* 176 */ 0, /* 177 */ 1, /* 178 */ 0, /* 179 */
  2, /* 180 */ 0, /* 181 */ 1, /* 182 */ 0, /* 183 */ 3, /* 184 */
  0, /* 185 */ 1, /* 186 */ 0, /* 187 */ 2, /* 188 */ 0, /* 189 */
  1, /* 190 */ 0, /* 191 */ 6, /* 192 */ 0, /* 193 */ 1, /* 194 */
  0, /* 195 */ 2, /* 196 */ 0, /* 197 */ 1, /* 198 */ 0, /* 199 */
  3, /* 200 */ 0, /* 201 */ 1, /* 202 */ 0, /* 203 */ 2, /* 204 */
  0, /* 205 */ 1, /* 206 */ 0, /* 207 */ 4, /* 208 */ 0, /* 209 */
  1, /* 210 */ 0, /* 211 */ 2, /* 212 */ 0, /* 213 */ 1, /* 214 */
  0, /* 215 */ 3, /* 216 */ 0, /* 217 */ 1, /* 218 */ 0, /* 219 */
  2, /* 220 */ 0, /* 221 */ 1, /* 222 */ 0, /* 223 */ 5, /* 224 */
  0, /* 225 */ 1, /* 226 */ 0, /* 227 */ 2, /* 228 */ 0, /* 229 */
  1, /* 230 */ 0, /* 231 */ 3, /* 232 */ 0, /* 233 */ 1, /* 234 */
  0, /* 235 */ 2, /* 236 */ 0, /* 237 */ 1, /* 238 */ 0, /* 239 */
  4, /* 240 */ 0, /* 241 */ 1, /* 242 */ 0, /* 243 */ 2, /* 244 */
  0, /* 245 */ 1, /* 246 */ 0, /* 247 */ 3, /* 248 */ 0, /* 249 */
  1, /* 250 */ 0, /* 251 */ 2, /* 252 */ 0, /* 253 */ 1, /* 254 */
  0, /* 255 */
}; // end _First_one


STL_END_NAMESPACE

#endif // BITSET_IMP_H
