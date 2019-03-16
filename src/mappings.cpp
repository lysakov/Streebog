#include "block.hpp"
#include "hash.hpp"

Block::Block<8> Context::substitution (const Block::Block<8> &block) const noexcept
{
    uint64_t subs_table[] = {
        252, 238, 221,  17, 207, 110,  49,  22, 251, 196, 250,
        218,  35, 197,   4,  77, 233, 119, 240, 219, 147,  46,
        153, 186,  23,  54, 241, 187,  20, 205,  95, 193, 249,
        24, 101,  90, 226,  92, 239,  33, 129,  28,  60,  66,
        139,   1, 142,  79,   5, 132,   2, 174, 227, 106, 143,
        160,   6,  11, 237, 152, 127, 212, 211,  31, 235,  52,
        44,  81, 234, 200,  72, 171, 242,  42, 104, 162, 253,
        58, 206, 204, 181, 112,  14,  86,   8,  12, 118,  18,
        191, 114,  19,  71, 156, 183,  93, 135,  21, 161, 150,
        41,  16, 123, 154, 199, 243, 145, 120, 111, 157, 158,
        178, 177,  50, 117,  25,  61, 255,  53, 138, 126, 109,
        84, 198, 128, 195, 189,  13,  87, 223, 245,  36, 169,
        62, 168,  67, 201, 215, 121, 214, 246, 124,  34, 185,
        3, 224,  15, 236, 222, 122, 148, 176, 188, 220, 232,
        40,  80,  78,  51,  10,  74, 167, 151,  96, 115,  30,
        0,  98,  68,  26, 184,  56, 130, 100, 159,  38,  65,
        173,  69,  70, 146,  39,  94,  85,  47, 140, 163, 165,
        125, 105, 213, 149,  59,   7,  88, 179,  64, 134, 172,
        29, 247,  48,  55, 107, 228, 136, 217, 231, 137, 225,
        27, 131,  73,  76,  63, 248, 254, 141,  83, 170, 144,
        202, 216, 133,  97,  32, 113, 103, 164,  45,  43,   9,
        91, 203, 155,  37, 208, 190, 229, 108,  82,  89, 166,
        116, 210, 230, 244, 180, 192, 209, 102, 175, 194,  57,
        75,  99, 182
    };
    return Block::Block<8>(subs_table[(uint8_t)block]);
}

std::vector< Block::Block<8> >& Context::permutation (std::vector< Block::Block<8> > &seq) const noexcept
{
    uint64_t perm_table[] = {
        0,  8, 16, 24, 32, 40, 48, 56,
        1,  9, 17, 25, 33, 41, 49, 57,
        2, 10, 18, 26, 34, 42, 50, 58,
        3, 11, 19, 27, 35, 43, 51, 59,
        4, 12, 20, 28, 36, 44, 52, 60,
        5, 13, 21, 29, 37, 45, 53, 61,
        6, 14, 22, 30, 38, 46, 54, 62,
        7, 15, 23, 31, 39, 47, 55, 63};
    seq.resize(64);
    std::vector< Block::Block<8> > copy = seq;
    for (size_t i = 0; i < seq.size(); ++i) {
        seq[i] = copy[perm_table[i]];
    }
    return seq;
}

Block::Block<64> Context::linear_transformation (const Block::Block<64> &block) const noexcept
{
    Block::Block<64> result = 0;
    for (size_t i = 0; i < 64; ++i) {
        result = result ^ (Block::Block<64>((const uint8_t*)matrix[i].c_str(), matrix[i].size()) * block[63 - i]);
    }
    return result;
}

Block::Block<512> Context::X (const Block::Block<512> &k, const Block::Block<512> &a) const noexcept
{
    return k ^ a;
}

Block::Block<512> Context::S (const Block::Block<512> &a) const noexcept
{
    auto seq = a.split<8>();
    for (auto &block8 : seq) {
        block8 = substitution(block8);
    } 
    return Block::Block<512>(seq);
}

Block::Block<512> Context::P (const Block::Block<512> &a) const noexcept
{
    auto seq = a.split<8>();
    return Block::Block<512>(permutation(seq));
}

Block::Block<512> Context::L (const Block::Block<512> &block) const noexcept
{
    auto seq = block.split<64>();
    for (auto &block64 : seq) {
        block64 = linear_transformation(block64); 
    }
    return Block::Block<512>(seq);
}

Block::Block<512> Context::iteration_const (const Block::Block<512> &k, size_t i) const
{
    enum {
        ZERO_ROUND = 0,
        ROUNDS_NUM = 12
    };
    if (i == ZERO_ROUND) {
        return k;
    }
    if (i <= ROUNDS_NUM) {
        return L(P(S(k ^ Block::Block<512>((const uint8_t*)iter_const_table[i - 1].c_str(), BLOCK_SIZE))));
    }
    throw std::runtime_error("iteration_const : error");
}

Block::Block<512> Context::E (const Block::Block<512> &k, const Block::Block<512> &m) const noexcept
{
    enum {
        ROUNDS_NUM = 12
    };
    Block::Block<512> result = m;
    Block::Block<512> new_k = k;
    for (int i = 0; i < ROUNDS_NUM; ++i) {
        new_k = iteration_const(new_k, i);
        result = L(P(S(X(new_k, result))));
    }
    return X(iteration_const(new_k, ROUNDS_NUM), result);
}   

Block::Block<512> Context::round_function (const Block::Block<512> &n, const Block::Block<512> &h, const Block::Block<512> &m) const noexcept
{
    return E(L(P(S(h ^ n))), m) ^ h ^ m;
}
