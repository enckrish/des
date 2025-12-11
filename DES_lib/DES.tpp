#include <bitset>
#include <immintrin.h>

namespace DES {
    /// Bit-wise permutes a value, based on a permutation array
    /// @tparam T type of input to be permuted
    /// @tparam V type of permutation array
    /// @param value input to apply permutation on (little-endian)
    /// @param perm permutation array; perm[i] stores index in value of bit to store at index i of out
    /// @param perm_size length of `perm`
    /// @return permutated value (little-endian)
    template<typename T, typename V>
    constexpr T apply_permutation(const T value, const V perm[], const int perm_size, const int input_width) {
        T out{0};
        for (int i = 0; i < perm_size; ++i) {
            int src_pos = input_width - perm[i];
            int dst_pos = (perm_size - 1) - i;

            out |= (value >> src_pos & 1) << dst_pos;
        }
        return out;
    }
}
