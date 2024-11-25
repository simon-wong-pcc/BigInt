#pragma once
#include <iostream>
#include <string>

typedef unsigned char byte;
typedef unsigned int fourByteChunk;
typedef unsigned long long eightByteChunk;

void printByte(eightByteChunk b);
void printBytes(eightByteChunk* b, size_t c);

byte getBit(eightByteChunk b, eightByteChunk pos);
void setBit(eightByteChunk& b, byte pos);
void unsetBit(eightByteChunk& b, byte pos);
void setBit(eightByteChunk& b, byte pos, byte value);
void flipBit(eightByteChunk& b, byte pos);

// BigInt Class Stored as little endian
template<size_t T>
class BigInt {
private:
    const static size_t byteSize = T;
    eightByteChunk bytes[T];

private:
    void initalizeBits(bool bit);
    BigInt<T> reverseBits(BigInt<T> a);

    eightByteChunk at(int i) const;

    // Index Operator
    inline eightByteChunk& operator[](int i) {
        return bytes[i];
    }

    inline eightByteChunk operator[](int i) const {
        return bytes[i];
    }

public:

    /*********************
    *    Constructors    *
    *********************/

    BigInt(int n = 0) {
        if (T < 1) {
            throw;
        }
        initalizeBits(n < 0);

        bytes[0] = n;
    }

    BigInt(long long n) {
        if (T < 1) {
            throw;
        }
        initalizeBits(n < 0);

        bytes[0] = n;
    }

    BigInt(const char* s) {

        const char* ptr = s;
        bool negetive = false;
        if (ptr[0] == '-') {
            negetive = true;
            ptr = &ptr[1];
        }

        initalizeBits(negetive);

        for (int i = 0; i < strlen(ptr); i++)
        {
            //            cout << ptr[i] << endl;
            if (!isdigit(ptr[i])) {
                throw("Invalid Number: Letter In String");
            }

            *this += ptr[i] - '0';

            if (i < strlen(ptr) - 1) {
                *this *= 10;
            }
        }

        //        *this /= 10;
    }

    BigInt(const BigInt<T>& ref) {
        for (int i = 0; i < ref.byteSize; i++) {
            bytes[i] = ref[i];
        }
    }

    void print();
    void printR();

    std::string toString() const;

    /******************
    *    Operators    *
    ******************/

    // Casting //
    explicit operator unsigned char() {
        return *((char*)this->bytes);
    }
    explicit operator unsigned short() {
        return *((short*)this->bytes);
    }
    explicit operator unsigned int() {
        return *((int*)this->bytes);
    }
    explicit operator unsigned long long() {
        return *((long long*)this->bytes);
    }

    // Assignment Operator //
    BigInt<T>& operator=(const BigInt<T>& ref) {
        for (int i = 0; i < T; i++) {
            bytes[i] = ref[i];
        }
        return *this;
    }

    ////  Post/Pre - Incrementation  //
    BigInt<T>& operator++() {
        *this += 1;
        return *this;
    }

    BigInt<T> operator++(int temp) {
        BigInt a;
        a = *this;
        ++(*this);
        return a;
    }

    BigInt<T>& operator--() {
        *this -= 1;
        return *this;
    }

    BigInt<T> operator--(const int temp) {
        BigInt<T> a;
        a = *this;
        --(*this);
        return a;
    }

    //  Addition and Subtraction  //
    // https://stackoverflow.com/questions/199333/how-do-i-detect-unsigned-integer-overflow
    friend BigInt<T>& operator+=(BigInt<T>& a, const BigInt<T>& b) {
        eightByteChunk carry[2] = { 0, 0 };

        for (int i = 0; i < T; i++) {
            carry[1] = 0;

            eightByteChunk oldVal = a[i];

            a[i] = a[i] + b[i] + carry[0];

            // Test for overflow
            if (b[i] + carry[0] > 0 && a[i] < oldVal + 1) {
                carry[1] = 1;
            }

            carry[0] = carry[1];

        }

        return a;
    }

    friend BigInt<T> operator+(const BigInt<T>& a, const BigInt<T>& b) {
        BigInt<T> result = a;
        result += b;
        return result;
    }

    friend BigInt<T> operator-(const BigInt<T>& a) {
        BigInt<T> out;
        for (int i = 0; i < T; i++) {
            out[i] = ~a[i];
        }
        return ++out;
    }

    friend BigInt<T> operator-(const BigInt<T>& a, const BigInt<T>& b) {
        BigInt<T> result = a;
        result -= b;
        return result;
    }

    friend BigInt<T>& operator-=(BigInt<T>& a, const BigInt<T>& b) {
        eightByteChunk carry[2] = { 0, 0 };

        for (int i = 0; i < T; i++) {
            carry[1] = 0;

            eightByteChunk oldVal = a[i];

            a[i] = a[i] - b[i] - carry[0];

            // Test for overflow
            if (b[i] + carry[0] > 0 && a[i] > oldVal) {
                carry[1] = 1;
            }

            carry[0] = carry[1];

        }

        return a;

    }

    //  Multiplication and Division  //
    friend BigInt<T>& operator*=(BigInt<T>& a, const BigInt<T>& b) {
        BigInt<T> product;
        BigInt<T> bCopy = b;

        bool negetive = false;

        if (a < 0) {
            negetive = !negetive;
            a = -a;
        }

        if (bCopy < 0) {
            negetive = !negetive;
            bCopy = -bCopy;
        }

        for (int i = 0; i < T; i++) {
            for (int pos = 0; pos < sizeof(T) * 8; pos++) {
                if (getBit(bCopy[i], pos) > 0) {
                    product += a << (i * sizeof(T)) + pos;
                }
            }
        }

        a = negetive ? -product : product;

        return a;
    }

    friend BigInt<T> operator*(const BigInt<T>& a, const BigInt<T>& b) {
        BigInt<T> result = a;
        result *= b;
        return result;
    }

    // https://gist.github.com/vvaltchev/d9f680b70a372559f88105daae846d8e //
    friend BigInt<T>& operator/=(BigInt<T>& a, const BigInt<T>& b) {
        if (b == 0) {
            throw("Division by 0");
        }

        bool negetive = false;

        BigInt<T> r = 0;
        BigInt<T> d = b;

        if (a < 0) {
            negetive = !negetive;
            a = -a;
        }
        if (d < 0) {
            negetive = !negetive;
            d = -d;
        }

        for (int i = T - 1; i >= 0; i--) {
            for (int pos = sizeof(T) * 8 - 1; pos >= 0; pos--) {
                r = r << 1;
                setBit(r[0], 0, getBit(a[i], pos) & 1);

                if (r >= d) {

                    // 'r', made by the first 'i' digits of 'n',
                    // is finally bigger than 'd'. Now subtract the divisor
                    // from it and set the i-th bit in the quotient to 1.
                    r -= d;
                    setBit(a[i], pos);
                }
                else {
                    unsetBit(a[i], pos);
                }
            }
        }
        a = negetive ? -a : a;
        return a;
    }

    friend BigInt<T> operator/(const BigInt<T>& n, const BigInt<T> d) {
        BigInt<T> temp = n;
        temp /= d;
        return temp;
    }

    //  Modulo  //
    friend BigInt<T> operator%(const BigInt<T>& a, const BigInt<T>& b) {
        BigInt<T> result = a;
        result %= b;
        return result;
    }

    friend BigInt<T>& operator%=(BigInt<T>& a, const BigInt<T>& b) {
        if (b == 0) {
            throw("Division by 0");
        }

        bool negetive = false;

        BigInt<T> r = 0;
        BigInt<T> d = b;

        if (a < 0) {
            negetive = !negetive;
            a = -a;
        }
        if (d < 0) {
            negetive = !negetive;
            d = -d;
        }

        for (int i = T - 1; i >= 0; i--) {
            for (int pos = sizeof(T) * 8 - 1; pos >= 0; pos--) {
                r = r << 1;
                setBit(r[0], 0, getBit(a[i], pos) & 1);

                if (r >= d) {

                    // 'r', made by the first 'i' digits of 'n',
                    // is finally bigger than 'd'. Now subtract the divisor
                    // from it and set the i-th bit in the quotient to 1.
                    r -= d;
                }
            }
        }
        a = negetive ? -r : r;
        return a;
    }

    //  Comparison operators  //
    friend bool operator==(const BigInt<T>& a, const BigInt<T>& b) {
        for (int i = 0; i < T; i++) {
            if (a[i] != b[i]) {
                return false;
            }
        }
        return true;
    }

    friend bool operator!=(const BigInt<T>& a, const BigInt<T>& b) {
        return !(a == b);
    }

    friend bool operator>(const BigInt<T>& a, const BigInt<T>& b) {
        BigInt<T> difference = a - b - 1;
        byte signBit = getBit(difference[T - 1], (sizeof(eightByteChunk) * 8) - 1);
        return (signBit == 0);
    }

    friend bool operator>=(const BigInt<T>& a, const BigInt<T>& b) {
        BigInt<T> difference = a - b;
        byte signBit = getBit(difference[T - 1], (sizeof(eightByteChunk) * 8) - 1);
        return (signBit == 0);
    }

    friend bool operator<(const BigInt<T>& a, const BigInt<T>& b) {
        BigInt<T> difference = a - b;
        byte signBit = getBit(difference[T - 1], (sizeof(eightByteChunk) * 8) - 1);
        return (signBit == 1);
    }

    friend bool operator<=(const BigInt<T>& a, const BigInt<T>& b) {
        BigInt<T> difference = a - b - 1;
        byte signBit = getBit(difference[T - 1], (sizeof(eightByteChunk) * 8) - 1);
        return (signBit == 1);
    }

    friend BigInt<T> operator<<(const BigInt<T>& a, const int& amount) {
        if (amount == 0) {
            return a;
        }

        BigInt<T> out = a;
        eightByteChunk carryA = 0;
        eightByteChunk carryB = 0;
        for (int i = 0; i < T; i++) {
            carryB = out[i] >> (sizeof(T) * 8) - amount;
            out[i] <<= amount;
            out[i] |= carryA;
            carryA = carryB;
        }

        return out;
    }

    friend BigInt<T> operator>>(const BigInt<T>& a, const int& amount) {
        if (amount == 0) {
            return a;
        }

        BigInt<T> out = a;
        eightByteChunk carryA = 0;
        eightByteChunk carryB = 0;
        for (int i = 0; i < T; i++) {
            carryB = out[i] << (sizeof(T) * 8) - amount;
            out[i] >>= amount;
            out[i] |= carryA;
            carryA = carryB;
        }

        return out;
    }

    //  Read and Write  //
    friend std::ostream& operator<<(std::ostream& out, const BigInt<T>& ref) {
        out << ref.toString();
        return out;
    }
};

// GeekForGeeks
template <size_t T>
BigInt<T> sqrt(BigInt<T>& a) {
    BigInt<T> left(1), right(a), v(1), mid, prod;
    right /= 2;
    while (left <= right) {
        mid += left;
        mid += right;
        mid /= 2;
        prod = (mid * mid);
        if (prod <= a) {
            v = mid;
            ++mid;
            left = mid;
        }
        else {
            --mid;
            right = mid;
        }
        mid = BigInt<T>();
    }
    return v;
}
