#include <bitset>
#include <immintrin.h>

// including read_bit and set_bit in the shared header allows them to be inlined,
// since the functions are very minimal. Large functions wouldn't have benefited as
// the compiler won't have inlined them.

inline bool read_bit(const uint_fast64_t num, const unsigned int k) {
    // Create a mask with only the k-th bit set
    uint_fast64_t mask = 1;
    mask <<= k;
    // Perform bitwise AND; if the result is non-zero, the bit is set
    return (num & mask) != 0;
}

inline void set_bit(uint_fast64_t &num, const unsigned int k) {
    uint_fast64_t mask = 1;
    mask <<= k;
    num |= mask;
}

namespace DES {
    /// Bit-wise permutes a value, based on a permutation array
    /// @tparam T type of input to be permuted
    /// @tparam V type of permutation array
    /// @param value input to apply permutation on (little-endian)
    /// @param perm permutation array; perm[i] stores index in value of bit to store at index i of out
    /// @param perm_size length of `perm`
    /// @return permutated value (little-endian)
    template<typename T, typename V>
T apply_permutation(const T value, const V perm[], const int perm_size, const int input_width) {
        T out{0};

        for (auto tgt = 0; tgt < perm_size; tgt++) {
            int output_pos = (perm_size - 1) - tgt;
            if (read_bit(value, input_width - perm[tgt])) {
                set_bit(out, output_pos);
            }
        }
        return out;
    }
}