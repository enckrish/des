#include <cstdint>
#include "DES.h"

#include "DES_tables.h"

#include <iostream>
#include <chrono> // For std::chrono::seconds
#include <thread> // For std::this_thread::sleep_for

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

    /// Performs DES S-Box substitution
    /// @param block 48-bit intermediate data to apply S-Box substitution to
    /// @return 32-bit block
    uint_fast32_t S_box_process(const uint_fast64_t block) {
        uint_fast32_t out{0};

        // split into groups of 6 bits
        // and apply them on the corresponding S-Box
        for (auto i = 0; i < 8; i++) {
            const auto v = block >> (6 * (7 - i)) & 0b111111;
            const auto row = v >> 4 & 0b10 | v & 0b1;
            const auto col = (v & 0b011110) >> 1;

            out |= Table::S[i][row][col] << (4 * (7 - i));
        }
        return out;
    }

    uint_fast64_t apply_round_fn(uint_fast64_t rb, const uint_fast64_t r_key) {
        rb = apply_permutation(rb, Table::E, 48, 32);
        rb ^= r_key;
        rb = S_box_process(rb);
        rb = apply_permutation(rb, Table::P, 32, 32);
        return rb;
    }

    template<bool Encrypt>
    uint_fast64_t process_with_keys(const uint_fast64_t blk, const uint_fast64_t keys[16]) {
        auto block = apply_permutation(blk, Table::IP, 64, 64);
        for (auto i = 0; i < 16; i++) {
            // Calculate round key K_r
            const unsigned key_index = Encrypt ? i : 16 - i - 1;
            // Apply round function on right half of block: F(B_r, K_r)
            auto rb = apply_round_fn(block & 0xffffffff, keys[key_index]);

            // XOR with left half: B_l ^ F(B_r, K_r)
            rb ^= block >> 32;
            // B_r || B_l ^ F(B_r, K_r)
            block = block << 32 | rb;
        }

        block = block >> 32 | block << 32;
        block = apply_permutation(block, Table::FP, 64, 64);
        return block;
    }

    void generate_round_keys(uint_fast64_t master, uint_fast64_t keys[16]) {
        master = apply_permutation(master, Table::PC1, 56, 64);
        for (auto i = 0; i < 16; i++) {
            constexpr auto msk = 0xfffffff;
            const auto sft = Table::L_SHIFTS[i];
            master = circ_lshift_u28(master >> 28, sft) << 28 | circ_lshift_u28(master & msk, sft);
            keys[i] = apply_permutation(master, Table::PC2, 48, 56);
        }
    }
}

namespace DES {
    Engine::Engine(const uint_fast64_t master) {
        generate_round_keys(master, this->keys);
    }

    [[nodiscard]] uint_fast64_t Engine::encrypt(const uint_fast64_t block) const {
        return process_with_keys<true>(block, this->keys);
    }

    [[nodiscard]] uint_fast64_t Engine::decrypt(const uint_fast64_t block) const {
        return process_with_keys<false>(block, this->keys);
    }
}
