// including read_bit and set_bit in the shared header allows them to be inlined,
// since the functions are very minimal. Large functions wouldn't have benefitted as
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
    /// @param value input to apply permutation on
    /// @param perm permutation array; perm[i] stores index in value of bit to store at index i of out
    /// @param perm_size length of `perm`
    /// @return permutated value
    template<typename T, typename V>
    T apply_permutation(const T value, const V perm[], const int perm_size) {
        T out{0};
        for (auto i = 0; i < perm_size; i++) {
            if (read_bit(value, perm[i] - 1)) {
                set_bit(out, i);
            }
        }
        return out;
    }
}