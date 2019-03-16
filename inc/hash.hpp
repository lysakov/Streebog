#ifndef HASH
#define HASH

#include <fstream>
#include "block.hpp"

class Context
{

  public :
    enum Mode {
        HASH_256,
        HASH_512
    };
    enum Flag {
        TRACING_ON,
        TRACING_OFF
    };
    Context (Mode, const std::string&);
    bool eof () const noexcept;
    void update (Flag);
    Block::Block<512>& finish (Flag);
    Block::Block<512> hash512 ();
    Block::Block<256> hash256 ();

  private :
    enum {
        MATRIX_SIZE = 64,
        ITER_CONST_TABLE_SIZE = 12,
        BUF_SIZE = 4096,
        BLOCK_SIZE = 64,
        BYTE_SIZE = 8
    };
    std::string matrix[MATRIX_SIZE] = {
        "8e20faa72ba0b470", "47107ddd9b505a38", "ad08b0e0c3282d1c", "d8045870ef14980e",
        "6c022c38f90a4c07", "3601161cf205268d", "1b8e0b0e798c13c8", "83478b07b2468764",
        "a011d380818e8f40", "5086e740ce47c920", "2843fd2067adea10", "14aff010bdd87508",
        "0ad97808d06cb404", "05e23c0468365a02", "8c711e02341b2d01", "46b60f011a83988e",
        "90dab52a387ae76f", "486dd4151c3dfdb9", "24b86a840e90f0d2", "125c354207487869",
        "092e94218d243cba", "8a174a9ec8121e5d", "4585254f64090fa0", "accc9ca9328a8950",
        "9d4df05d5f661451", "c0a878a0a1330aa6", "60543c50de970553", "302a1e286fc58ca7",
        "18150f14b9ec46dd", "0c84890ad27623e0", "0642ca05693b9f70", "0321658cba93c138",
        "86275df09ce8aaa8", "439da0784e745554", "afc0503c273aa42a", "d960281e9d1d5215",
        "e230140fc0802984", "71180a8960409a42", "b60c05ca30204d21", "5b068c651810a89e",
        "456c34887a3805b9", "ac361a443d1c8cd2", "561b0d22900e4669", "2b838811480723ba",
        "9bcf4486248d9f5d", "c3e9224312c8c1a0", "effa11af0964ee50", "f97d86d98a327728",
        "e4fa2054a80b329c", "727d102a548b194e", "39b008152acb8227", "9258048415eb419d",
        "492c024284fbaec0", "aa16012142f35760", "550b8e9e21f7a530", "a48b474f9ef5dc18",
        "70a6a56e2440598e", "3853dc371220a247", "1ca76e95091051ad", "0edd37c48a08a6d8",
        "07e095624504536c", "8d70c431ac02a736", "c83862965601dd1b", "641c314b2b8ee083"
    };
    std::string iter_const_table[ITER_CONST_TABLE_SIZE] = {
        "b1085bda1ecadae9ebcb2f81c0657c1f2f6a76432e45d016714eb88d7585c4fc4b7ce09192676901a2422a08a460d31505767436cc744d23dd806559f2a64507",
        "6fa3b58aa99d2f1a4fe39d460f70b5d7f3feea720a232b9861d55e0f16b501319ab5176b12d699585cb561c2db0aa7ca55dda21bd7cbcd56e679047021b19bb7",
        "f574dcac2bce2fc70a39fc286a3d843506f15e5f529c1f8bf2ea7514b1297b7bd3e20fe490359eb1c1c93a376062db09c2b6f443867adb31991e96f50aba0ab2",
        "ef1fdfb3e81566d2f948e1a05d71e4dd488e857e335c3c7d9d721cad685e353fa9d72c82ed03d675d8b71333935203be3453eaa193e837f1220cbebc84e3d12e",
        "4bea6bacad4747999a3f410c6ca923637f151c1f1686104a359e35d7800fffbdbfcd1747253af5a3dfff00b723271a167a56a27ea9ea63f5601758fd7c6cfe57",
        "ae4faeae1d3ad3d96fa4c33b7a3039c02d66c4f95142a46c187f9ab49af08ec6cffaa6b71c9ab7b40af21f66c2bec6b6bf71c57236904f35fa68407a46647d6e",
        "f4c70e16eeaac5ec51ac86febf240954399ec6c7e6bf87c9d3473e33197a93c90992abc52d822c3706476983284a05043517454ca23c4af38886564d3a14d493",
        "9b1f5b424d93c9a703e7aa020c6e41414eb7f8719c36de1e89b4443b4ddbc49af4892bcb929b069069d18d2bd1a5c42f36acc2355951a8d9a47f0dd4bf02e71e",
        "378f5a541631229b944c9ad8ec165fde3a7d3a1b258942243cd955b7e00d0984800a440bdbb2ceb17b2b8a9aa6079c540e38dc92cb1f2a607261445183235adb",
        "abbedea680056f52382ae548b2e4f3f38941e71cff8a78db1fffe18a1b3361039fe76702af69334b7a1e6c303b7652f43698fad1153bb6c374b4c7fb98459ced",
        "7bcd9ed0efc889fb3002c6cd635afe94d8fa6bbbebab076120018021148466798a1d71efea48b9caefbacd1d7d476e98dea2594ac06fd85d6bcaa4cd81f32d1b",
        "378ee767f11631bad21380b00449b17acda43c32bcdf1d77f82012d430219f9b5d80ef9d1891cc86e71da4aa88e12852faf417d5d9b21b9948bc924af11bd720"
    };
    std::ifstream input;
    uint8_t buf[BUF_SIZE];
    size_t cur_buf_size;
    size_t cur_buf_pos;
    Block::Block<512> cur_block;
    size_t gcount;
    bool _eof_;
    bool last_iteration;
    Block::Block<512> h;
    Block::Block<512> n;
    Block::Block<512> sigma;
    Block::Block<512> result;
    uint64_t cnt = 0;
    void matrix_init (std::string*, size_t);
    Block::Block<8> substitution (const Block::Block<8>&) const noexcept;
    std::vector< Block::Block<8> >& permutation (std::vector< Block::Block<8> >&) const noexcept;
    Block::Block<64> linear_transformation (const Block::Block<64>&) const noexcept;
    Block::Block<512> X (const Block::Block<512>&, const Block::Block<512>&) const noexcept;
    Block::Block<512> S (const Block::Block<512>&) const noexcept;
    Block::Block<512> P (const Block::Block<512>&) const noexcept;
    Block::Block<512> L (const Block::Block<512>&) const noexcept;
    Block::Block<512> iteration_const (const Block::Block<512>&, size_t) const;
    Block::Block<512> E (const Block::Block<512>&, const Block::Block<512>&) const noexcept;
    Block::Block<512> round_function (const Block::Block<512>&, const Block::Block<512>&, const Block::Block<512>&) const noexcept;
    void read_block ();
    void fill_buffer ();
    void modify_buffer ();
    void trace () const;
    void hash256_init () noexcept;
    void hash512_init () noexcept;

};

#endif
