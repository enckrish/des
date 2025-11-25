#include <cstdint>
#include "DES.h"
#include "des_tables.h"

bool read_bit(uint_fast64_t num, unsigned int k);
void set_bit(uint_fast64_t& num, unsigned int k);

namespace DES {
    /// Bit-wise permutes a value, based on a permutation array
    /// @tparam T type of input to be permuted
    /// @tparam V type of permutation array
    /// @param value input to apply permutation on
    /// @param perm permutation array; perm[i] stores index in value of bit to store at index i of out
    /// @param psize length of `perm`
    /// @return permutated value
    template<typename T, typename V>
    T apply_permutation(const T value, const V perm[], const int psize) {
        T out{0};
        for (auto i = 0; i < psize; i++) {
            if (read_bit(value, perm[i] - 1)) {
                set_bit(out, i);
            }
        }
        return out;
    }

    /// @brief Rotates a u28 left by n bits
    /// @param key value with bits 28-64 set to 0
    /// @param n number of bits to rotate key
    /// @return rotated key, bits 28-64 set to 0
    ///
    /// @note assumes arithmetic shift when using shift operators
    uint_fast64_t circ_lshift_u28(uint_fast64_t key, const unsigned int n) {
        // mask over invalid bits 56-64
        auto msk = static_cast<uint_fast64_t>(-1);
        msk <<= 28;

        key <<= n;
        const uint_fast64_t msk_v = key & msk;
        key |= msk_v >> 28;
        key &= ~msk;
        return key;
    }

    /// @brief Rotates a u28 right by n bits
    /// @param key value with bits 28-64 set to 0
    /// @param n number of bits to rotate key
    /// @return rotated key, bits 28-64 set to 0
    ///
    /// @note assumes arithmetic shift when using shift operators
    uint_fast64_t circ_rshift_u28(uint_fast64_t key, const unsigned int n) {
        // mask over invalid bits 56-64
        auto msk = static_cast<uint_fast64_t>(-1);
        msk <<= 28;

        const auto msk_v = key & (1 << n) - 1;
        key >>= n;
        key |= msk_v << (28 - n);
        key &= ~msk;
        return key;
    }

    /// Rotates each half of key by n bits
    /// @param key 56-bit DES key, bits 56-64 set to 0
    /// @param n number of bits to rotate key
    /// @param left if key is to be rotated left, else rotated right
    /// @return rotated key, bits 56-64 set to 0
    uint_fast64_t circ_shift_key(uint_fast64_t key, const unsigned int n, const bool left) {
        constexpr auto msk = 0xfffffff;
        if (left) {
            key = circ_lshift_u28(key >> 28, n) << 28 | circ_lshift_u28(key & msk, n);
        } else {
            key = circ_rshift_u28(key >> 28, n) << 28 | circ_rshift_u28(key & msk, n);
        }
        return key;
    }

    /// Performs DES S-Box substitution
    /// @param block 48-bit intermediate data to apply S-Box substitution to
    /// @return 32-bit block
    uint_fast32_t S_box_process(const uint_fast64_t block) {
        uint_fast32_t out{0};

        // split into groups of 6 bits
        // and apply them on the corresponding S-Box
        for (auto i = 0; i < 8; i++) {
            constexpr uint_fast32_t mmsk = 0x3f;
            const auto v = block >> (6 * (7 - i)) & mmsk;
            const auto row = v >> 5 | v & 0b1;
            const auto col = (v & 0b011110) >> 1;

            out |= Table::S[i][row][col] << (i * 4);
        }
        return out;
    }

    uint_fast64_t apply_round_fn(uint_fast64_t rb, const uint_fast64_t rkey) {
        rb = apply_permutation(rb, Table::E, 48);
        rb ^= rkey;
        rb = S_box_process(rb);
        rb = apply_permutation(rb, Table::P, 32);
        return rb;
    }

    uint_fast64_t process(uint_fast64_t block, uint_fast64_t key, const unsigned int rounds, const bool encrypt) {
        block = apply_permutation(block, Table::IP, 64);
        key = apply_permutation(key, Table::PC1, 56);
        for (auto i = 0; i < rounds; i++) {
            // Calculate round key K_r
            const unsigned shift_n = encrypt ? Table::L_SHIFTS[i] : Table::R_SHIFTS[i];
            key = circ_shift_key(key, shift_n, encrypt);
            const auto r_key = apply_permutation(key, Table::PC2, 48);

            // Apply round function on right half of block: F(B_r, K_r)
            auto rb = apply_round_fn(block & 0xffffffff, r_key);

            // XOR with left half: B_l ^ F(B_r, K_r)
            rb ^= block >> 32;
            // B_r || B_l ^ F(B_r, K_r)
            block = block << 32 | rb;
        }

        block = block >> 32 | block << 32;
        block = apply_permutation(block, Table::FP, 64);
        return block;
    }
}
