#include "BigInt.h"

byte getBit(eightByteChunk b, eightByteChunk pos) {
    return (b >> pos) & 1;
};

void setBit(eightByteChunk& b, byte pos) {
    b |= ((eightByteChunk)1 << pos);
};

void unsetBit(eightByteChunk& b, byte pos) {
    b &= ~((eightByteChunk)1 << pos);
}

void setBit(eightByteChunk& b, byte pos, byte value) {
    value ? setBit(b, pos) : unsetBit(b, pos);
}

void flipBit(eightByteChunk& b, byte pos) {
    b ^= ((eightByteChunk)1 << pos);
}

void printByte(eightByteChunk b) {
    for (int i = 0; i < sizeof(eightByteChunk) * 8; i++) {
        std::cout << static_cast<unsigned short>(getBit(b, i));
    }
    std::cout << " ";
}

void printBytes(eightByteChunk* bytes, size_t c) {
    for (int i = 0; i < c; i++) {
        printByte(bytes[i]);
    }
    std::cout << std::endl;
}

template <size_t T>
void BigInt<T>::initalizeBits(bool bit) {

    eightByteChunk fillByte = bit ? 0xff'ff'ff'ff'ff'ff'ff'ff : 0x00;

    for (int i = 0; i < T; i++) {
        bytes[i] = fillByte;
    }
}

template <size_t T>
BigInt<T> BigInt<T>::reverseBits(BigInt<T> a) {
    BigInt<T> out;
    for (int i = 0; i < T; i++) {
        for (int pos = 0; pos < sizeof(eightByteChunk) * 8; pos++) {
            setBit(out[T - 1 - i], (sizeof(eightByteChunk) * 8) - 1 - pos, getBit(a[i], pos));
        }
    }
    return out;
}

template <size_t T>
eightByteChunk BigInt<T>::at(int i) const {
    if (i >= T || i < 0) {
        return 0;
    }
    return bytes[i];
}

template <size_t T>
void BigInt<T>::print() {
    printBytes((eightByteChunk*)this->bytes, T);
}

template <size_t T>
void BigInt<T>::printR() {
    printBytes((eightByteChunk*)&reverseBits(*this).bytes, T);
}

constexpr int countZeros(unsigned long long n) {
    int counter = 0;

    while (n >= 10) {
        counter++;
        n /= 10;
    }
    return counter;
}

template <size_t T>
std::string BigInt<T>::toString() const {
    std::string outStr = "";
    BigInt<T> temp = *this;

    bool negetive = false;

    if (temp < 0) {
        negetive = true;
        temp = -temp;
    }

    do {
        const long long numChunk = 1'000'000'000'000'000'000; // Largest Number that is multiple of 10
        constexpr int numZeros = countZeros(numChunk);
        long long c = static_cast<eightByteChunk>(temp % numChunk);
        temp /= numChunk;

        std::string subStr = std::to_string(c);
        if (temp > 0) {
            while (subStr.length() < numZeros) {
                subStr.insert(0, "0");
            }
        }

        outStr.insert(0, subStr);

    } while (temp > 0);

    if (negetive) {
        outStr.insert(0, "-");
    }

    return outStr;
}

