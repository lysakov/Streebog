#include "functions.hpp"

enum {
    BLOCK_SIZE = 64,
    BUF_SIZE = 4096
};

std::string hex_to_bin (const std::string &hex)
{
    std::stringstream str;
    for (char c : hex) {
        std::string buf;
        buf += c;
        int num = std::stoi(buf, nullptr, 16);
        str << std::bitset<4>(num);
    }
    return str.str();
}

std::ostream& hex_to_stream (std::ostream &output, const std::string &hex)
{
    std::string string;
    for (size_t i = 0; i < hex.size(); i += 2) {
        std::string buf;
        buf += hex[i];
        buf += hex[i + 1];
        char symb = (char)std::stoi(buf, nullptr, 16);
        string += symb;
    }
    output.write(string.c_str(), string.size());
    return output;
}

std::string str_to_bin (const char *str, size_t size)
{
    std::string bin;
    for (size_t i = 0; i < size; ++i) {
        bin += std::bitset<8 * sizeof(char)>(str[i]).to_string();
    }
    return bin;
}

std::string str_to_bin (const std::string &str)
{
    std::string bin;
    for (size_t i = 0; i < str.size(); ++i) {
        bin += std::bitset<8 * sizeof(char)>(str[i]).to_string();
    }
    return bin;
}

std::string bin_to_str (const std::bitset<512> &block)
{
    enum {
        BLOCK_NUM = 64
    };
    std::string result;
    for (int i = 0; i < BLOCK_NUM; ++i) {
        std::bitset<8> c(0);
        for (size_t j = 0; j < c.size(); ++j) {
            c[j] = block[j + BLOCK_NUM * i];
        }
        char symb = c.to_ulong();
        result.insert(result.begin(), symb);
    }
    return result;
}

std::string read (std::istream &input, size_t &offset)
{
    char buf[BUF_SIZE] = {0};
    if (offset >= BUF_SIZE) {
        offset -= BUF_SIZE;
        input.seekg(offset, std::ios_base::beg);
        input.read(buf, BUF_SIZE);
        std::string str = buf;
        str.resize(BUF_SIZE);
        return str;
    } else {
        input.seekg(0);
        input.read(buf, offset);
        offset = 0;
        std::string str = buf;
        return str;
    }
}

std::string& flip_str (std::string &arr)
{
    for (unsigned long i = 0; i < arr.size() / 2; i += 2) {
        char tmp1 = arr[i];
        char tmp2 = arr[i + 1];
        arr[i] = arr[arr.size() - i - 2];
        arr[arr.size() - i - 2] = tmp1;
        arr[i + 1] = arr[arr.size() - i - 1];
        arr[arr.size() - i - 1] = tmp2;
    }
    return arr;
}

/*std::string& flip_str (std::string &arr)
{
    for (unsigned long i = 0; i < arr.size() / 2; ++i) {
        char tmp1 = arr[i];
        arr[i] = arr[arr.size() - i - 1];
        arr[arr.size() - i - 1] = tmp1;
    }
    return arr;
}*/
