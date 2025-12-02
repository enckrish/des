#include <cstdint>
#include "DES.h"

#include "DES_tables.h"

namespace DES {
    /// @brief Rotates a u28 left by n bits
    /// @param key value with bits 28-64 set to 0
    /// @param n number of bits to rotate key
    /// @return rotated key, bits 28-64 set to 0
    ///
    /// @note assumes arithmetic shift when using shift operators
    uint_fast64_t circ_lshift_u28(const uint_fast64_t key, const unsigned int n) {
        return (key << n | key >> (28 - n)) & 0x0FFFFFFF;
    }

    /// @brief Rotates a u28 right by n bits
    /// @param key value with bits 28-64 set to 0
    /// @param n number of bits to rotate key
    /// @return rotated key, bits 28-64 set to 0
    ///
    /// @note assumes arithmetic shift when using shift operators
    uint_fast64_t circ_rshift_u28(uint_fast64_t key, const unsigned int n) {
        return ((key & ~(1 << n)) << (28 - n) | key >> n) & 0x0FFFFFFF;
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
            constexpr uint_fast32_t msk = 0x3f;
            const auto v = block >> (6 * (7 - i)) & msk;
            const auto row = v >> 5 | v & 0b1;
            const auto col = (v & 0b011110) >> 1;

            out |= Table::S[i][row][col] << (i * 4);
        }
        return out;
    }

    uint_fast64_t apply_round_fn(uint_fast64_t rb, const uint_fast64_t r_key) {
        rb = apply_permutation(rb, Table::E, 48);
        rb ^= r_key;
        rb = S_box_process(rb);
        rb = apply_permutation(rb, Table::P, 32);
        return rb;
    }

    uint_fast64_t process(uint_fast64_t block, uint_fast64_t key, const bool encrypt) {
        block = apply_permutation(block, Table::IP, 64);
        key = apply_permutation(key, Table::PC1, 56);
        for (auto i = 0; i < 16; i++) {
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

    uint_fast64_t process_with_keys(uint_fast64_t block, const uint_fast64_t keys[16], const bool encrypt) {
        block = apply_permutation(block, Table::IP, 64);
        for (auto i = 0; i < 16; i++) {
            // Calculate round key K_r
            const unsigned key_index = encrypt ? i : 16 - i - 1;
            // Apply round function on right half of block: F(B_r, K_r)
            auto rb = apply_round_fn(block & 0xffffffff, keys[key_index]);

            // XOR with left half: B_l ^ F(B_r, K_r)
            rb ^= block >> 32;
            // B_r || B_l ^ F(B_r, K_r)
            block = block << 32 | rb;
        }

        block = block >> 32 | block << 32;
        block = apply_permutation(block, Table::FP, 64);
        return block;
    }

    void generate_round_keys(uint_fast64_t master, uint_fast64_t keys[16]) {
        master = apply_permutation(master, Table::PC1, 56);
        for (auto i = 0; i < 16; i++) {
            master = circ_shift_key(master, Table::L_SHIFTS[i], true);
            keys[i] = apply_permutation(master, Table::PC2, 48);
        }
    }
}

namespace DES {
    Engine::Engine(uint_fast64_t master_key) {
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
        master_key = __builtin_bswap64(master_key);
#endif
        generate_round_keys(master_key, this->keys);
    }

    [[nodiscard]] uint_fast64_t Engine::encrypt(uint_fast64_t block) const {
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
        block = __builtin_bswap64(block);
#endif
        return process_with_keys(block, this->keys, true);
    }

    [[nodiscard]] uint_fast64_t Engine::decrypt(const uint_fast64_t block) const {
        auto data = process_with_keys(block, this->keys, false);
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
        data = __builtin_bswap64(data);
#endif
        return data;
    }
}
