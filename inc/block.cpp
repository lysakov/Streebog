#include "block.hpp"

template <size_t S>
Block<S>::Block (const std::string &bitset) : Block()
{
    if (bitset.size() > S / BYTE_SIZE) {
        throw std::runtime_error("Block constructor : string is too long.");
    }
    for (size_t i = 0; i < bitset.size(); ++i) {
        this->bitset[i] = bitset[i];
    }
    if (bitset.size() < S / BYTE_SIZE) {
        this->bitset[bitset.size()] = 0x01;
    }
}

template <size_t S>
Block<S>::Block (const std::string &bitset, Mode mode) : Block()
{
    enum {
        HEX_SIZE = 4
    };
    std::string buf = bitset;
    if (bitset.size() > S / HEX_SIZE) {
        throw std::runtime_error("Block constructor : string is too long.");
    }
    if (mode != HEX) {
        throw std::invalid_argument("Block constructor : unknown flag.");
    }
    size_t i = 0;
    while (!buf.empty()) {
        auto str = buf.substr(0, 2);
        buf.erase(0, 2);
        this->bitset[i] = std::stoi(str, 0, 16);
        ++i;
    }
    if (bitset.size() < S / HEX_SIZE) {
        this->bitset[bitset.size() / 2] = 0x01;
    }
}

template <size_t S>
template <size_t T>
Block<S>::Block (const std::vector< Block<T> > &seq)
{
    if (S % T) {
        throw std::invalid_argument("Block constructor : wrong template parametr.");
    }
    for (size_t i = 0; i < S / T; ++i) {
        for (size_t j = 0; j < T / BYTE_SIZE; ++j) {
            bitset[i * T / BYTE_SIZE + j] = seq[i].get_byte(j);
        }
    }
}       

template <size_t S>
Block<S>::Block ()
{
    for (size_t i = 0; i < S / BYTE_SIZE; ++i) {
        this->bitset[i] = 0;
    }
}

template <size_t S>
Block<S>::Block (const uint8_t *bitset, size_t size)
{
    size_t min = S / BYTE_SIZE <= size ? S / BYTE_SIZE : size;
    for (size_t i = 0; i < min; ++i) {
        this->bitset[i] = bitset[i];
    }
    if (size < S / BYTE_SIZE && size > 0) {
        if (!(this->bitset[size - 1] & 0xf0)) {
            this->bitset[size - 1] |= 0x10;
        } else {
            this->bitset[size] = 0x01;
        }
    }
    if (!size) {
        this->bitset[size] = 0x01;
    }
}

template <size_t S>
Block<S>::Block (const char *bitset, size_t size) : Block((uint8_t*)bitset, size)
{
}

template <size_t S>
Block<S>::Block (uint64_t num) : Block()
{
    enum {
        NUM_SIZE = 8
    };
    uint8_t *num_arr;
    num_arr = (uint8_t*)&num;
    size_t min = S / BYTE_SIZE <= NUM_SIZE ? S / BYTE_SIZE : NUM_SIZE;
    for (size_t i = 0; i < min; ++i) {
        this->bitset[i] = num_arr[i];
    }
}

template <size_t S>
Block<S>::operator uint64_t () const noexcept
{
    return *((uint64_t*)bitset);
}

template <size_t S>
template <size_t T>
Block<T> Block<S>::get_block (const uint8_t *bitset, int num) const
{
    uint8_t new_bitset[T / BYTE_SIZE] = {0};
    for (size_t i = 0; i < T / BYTE_SIZE; ++i) {
        new_bitset[i] = bitset[i + T / BYTE_SIZE * num];
    }
    return Block<T>(new_bitset, T / BYTE_SIZE);
}

template <size_t S>
template <size_t T>
std::vector< Block<T> > Block<S>::split () const
{
    if (T > S || S % T) {
        throw std::invalid_argument("Block::split() : wrong template parametr.");
    }
    std::vector< Block<T> > seq;
    for (size_t i = 0; i < S / T; ++i) {
        Block<T> block = get_block<T>(bitset, i);
        seq.push_back(block);
    }
    return seq;
}

template <size_t S>
std::ostream& operator << (std::ostream &stream, const Block<S> &block)
{
    enum {
        BYTE_SIZE = 8
    };
    size_t size = S / BYTE_SIZE;
    stream << std::hex;
    for (size_t i = 0; i < size; ++i) {
        size_t index = size - i - 1;
        //size_t index = i;
        if (block.bitset[index] < 0x10) {
            stream << 0 << (int)block.bitset[index];
        } else {
            stream << (int)block.bitset[index];
        }
    }
    stream << std::dec;
    return stream;
}

template <size_t S>
Block<S> Block<S>::operator ^ (const Block<S>& block) const noexcept
{
    Block<S> new_block;
    for (size_t i = 0; i < S / BYTE_SIZE; ++i) {
        new_block.bitset[i] = this->bitset[i] ^ block.bitset[i];
    }
    return new_block;
}

template <size_t S>
Block<S> Block<S>::operator * (bool symb) const noexcept
{
    if (!symb) {
        return Block<S>(0);
    }
    else {
        return *this;
    }
}

template <size_t S>
bool Block<S>::operator [] (size_t i) const noexcept
{
    size_t pos = 7 - i % BYTE_SIZE;
    return (bitset[i / BYTE_SIZE] & (1 << (sizeof(bitset[i / BYTE_SIZE]) * 8 - 1 - pos))) != 0; 
}

template <size_t S>
Block<S> Block<S>::operator + (const Block<S> &block) const noexcept
{
    enum {
        MOD = 256
    };
    uint8_t result[S / BYTE_SIZE];
    uint8_t flag = 0;
    for (size_t i = 0; i < S / BYTE_SIZE; ++i) {
        result[i] = (flag + block.get_byte(i) + this->bitset[i]) % MOD;
        flag = (flag + block.get_byte(i) + this->bitset[i]) / MOD;
    }
    return Block<S>(result, S / BYTE_SIZE);
}

template <size_t S>
bool Block<S>::operator == (const Block<S> &block) const noexcept
{
    for (size_t i = 0; i < S / BYTE_SIZE; ++i) {
        if (this->bitset[i] != block.bitset[i]) {
            return false;
        }
    }
    return true;
}

template <size_t S>
bool Block<S>::operator != (const Block<S> &block) const noexcept
{
    return !(*this == block);
}

template <size_t S>
uint8_t Block<S>::get_byte (size_t i) const noexcept
{
    return bitset[i];
}
