#include <cstdint>
#include <iostream>
#include <bitset>

// wrong both rob
bool read_bit(const uint_fast64_t num, const uint k) {
    // Create a mask with only the k-th bit set
    uint_fast64_t mask = 1;
    mask <<= k;
    // Perform bitwise AND; if the result is non-zero, the bit is set
    return (num & mask) != 0;
}

void set_bit(uint_fast64_t& num, const uint k) {
    uint_fast64_t mask = 1;
    mask <<= k;
    num |= mask;
}