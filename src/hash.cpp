#include "hash.hpp"

Context::Context (Mode mode, const std::string &file_name)
{
    matrix_init(matrix, MATRIX_SIZE);
    matrix_init(iter_const_table, ITER_CONST_TABLE_SIZE); 
    input.open(file_name);    
    if (!input) {
        throw std::runtime_error("File doesn't exist");
    }
    gcount = 0;
    _eof_ = false;
    last_iteration = false;
    fill_buffer();
    if (mode == HASH_256) {
        hash256_init();
    } else {
        hash512_init();
    }
}

void Context::matrix_init (std::string *matrix, size_t size)
{
    for (size_t i = 0; i < size; ++i) {
        std::string new_string;
        while (!matrix[i].empty()) {
            std::string buf = matrix[i].substr(matrix[i].size() - 2, matrix[i].size());
            matrix[i].resize(matrix[i].size() - 2);
            new_string += (char)std::stoi(buf, nullptr, 16);
        }
        matrix[i] = new_string;
    }
}

void Context::hash256_init () noexcept
{
    h = 0;
    n = 0;
    sigma = 0;
    auto seq = h.split<8>();
    for (auto &block : seq) {
        block = 1;
    }
    h = Block::Block<512>(seq);
}

void Context::hash512_init () noexcept
{
    h = 0;
    n = 0;
    sigma = 0;
}

void Context::trace () const
{
    std::cout << "m = " << cur_block << std::endl;
    std::cout << "h = " << h << std::endl;
    std::cout << "N = " << n << std::endl;
    std::cout << "sigma = " << sigma << std::endl;
    std::string str;
    std::cin >> str;
    if (str == "q") {
        throw std::runtime_error("'q' was pressed");
    }
}

void Context::fill_buffer ()
{
    input.read((char*)buf, BUF_SIZE);
    cur_buf_size = input.gcount();
    cnt += cur_buf_size;
    if (cnt > 10000000) {
        std::cout << "10MB" << '\n';
        cnt = 0;
    }
    if (last_iteration || (!cur_buf_size && gcount % BLOCK_SIZE)) {
        _eof_ = true;
    }
    const size_t block_size = BLOCK_SIZE;
    gcount = cur_buf_size > block_size ? block_size : cur_buf_size;
    if (!gcount) {
        last_iteration = true;
    }
    cur_block = Block::Block<512>(buf, gcount);
    cur_buf_pos = gcount;
}

void Context::modify_buffer ()
{
    const size_t block_size = BLOCK_SIZE;
    gcount = (cur_buf_size - cur_buf_pos) > block_size ? block_size : (cur_buf_size - cur_buf_pos);
    cur_block = Block::Block<512>(buf + cur_buf_pos, gcount);
    cur_buf_pos += gcount;
}

void Context::update (Flag tracing = TRACING_OFF)
{
    if (tracing == TRACING_ON) {
        trace();
    }
    h = round_function(n, h, cur_block);
    n = n + Block::Block<512>(BYTE_SIZE * gcount);
    sigma = sigma + cur_block;
    if (cur_buf_pos == cur_buf_size) {
        fill_buffer();
    } else {
        modify_buffer();
    }
}

Block::Block<512>& Context::finish (Flag tracing = TRACING_OFF)
{
    if (tracing == TRACING_ON) {
        trace();
    }
    h = round_function(0, h, n);
    if (tracing == TRACING_ON) {
        trace();
    }
    result = round_function(0, h, sigma);
    return result;
}

bool Context::eof () const noexcept
{
    return _eof_;
}

Block::Block<512> Context::hash512 ()
{
    hash512_init();
    while (!eof()) {
        update();
    }
    return finish();
}

Block::Block<256> Context::hash256 ()
{
    hash256_init();
    while (!eof()) {
        update();
    }
    auto block = finish().split<256>();
    return block[1];
}
