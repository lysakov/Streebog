#ifndef BLOCK
#define BLOCK

#include <iostream>
#include <fstream>
#include <bitset>
#include <stdint.h>
#include <vector>
#include "functions.hpp"

namespace Block
{

    template <size_t S>
    class Block
    {

      public :
        enum Mode {
            HEX
        };
        Block ();
        Block (const std::string&);
        Block (const std::string&, Mode);
        Block (const uint8_t*, size_t);
        Block (const char*, size_t);
        Block (uint64_t);
        template <size_t T> Block (const std::vector< Block<T> >&);
        template <size_t T> std::vector< Block<T> > split () const;
        template <size_t T> friend std::ostream& operator << (std::ostream&, const Block<T>&);
        operator uint64_t () const noexcept;
        Block operator + (const Block&) const noexcept;
        Block operator ^ (const Block&) const noexcept;
        Block operator * (bool) const noexcept;
        bool operator == (const Block&) const noexcept;
        bool operator != (const Block&) const noexcept;
        bool operator [] (size_t) const noexcept;
        uint8_t get_byte (size_t) const noexcept;

      private :
        enum {
            BYTE_SIZE = 8
        };
        uint8_t bitset[S / BYTE_SIZE] = {0};
        template <size_t T> Block<T> get_block (const uint8_t*, int) const;

    };

    #include "block.cpp"

}

#endif
