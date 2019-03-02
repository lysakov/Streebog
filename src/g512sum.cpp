#include <fstream>
#include <iostream>
#include "hash.hpp"

using namespace Block;

int main (int argc, char **argv)
{
    try {
        Context ctx(Context::HASH_512, argv[argc - 1]);
        std::cout << argv[argc - 1] << " : " << ctx.hash512() << std::endl;
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
    }
    return 0;
}
