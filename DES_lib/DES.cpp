#include <cstdint>
#include "DES.h"

#include "DES_tables.h"

#include <ranges>

namespace DES {
    /// @brief Rotates a u28 left by n bits
    /// @param key value with bits 28-64 set to 0
    /// @param n number of bits to rotate key
    /// @return rotated key, bits 28-64 set to 0
    ///
    /// @note assumes arithmetic shift when using shift operators
    constexpr uint_fast64_t circ_lshift_u28(const uint_fast64_t key, const unsigned int n) {
        return (key << n | key >> (28 - n)) & 0x0FFFFFFF;
    }

    constexpr uint_fast64_t apply_round_fn(uint_fast64_t rb, const uint_fast64_t r_key) {
        rb = apply_permutation(rb, Table::E, 48, 32);
        rb ^= r_key;

        // applying S-box and P permutation at once
        decltype(rb) b{};
        for (auto i = 0; i < 8; i++) {
            b ^= LUT::SP[i][rb >> 6 * (7 - i) & 0b111111];
        }
        return b;
    }

    template<typename Iter>
    uint_fast64_t process_with_keys(const uint_fast64_t blk, Iter key_begin, const Iter &key_end) {
        auto block = apply_permutation(blk, Table::IP, 64, 64);
        for (; key_begin != key_end; ++key_begin) {
            // Calculate round key K_r
            // Apply round function on right half of block: F(B_r, K_r)
            auto rb = apply_round_fn(block & 0xffffffff, *key_begin);

            // XOR with left half: B_l ^ F(B_r, K_r)
            rb ^= block >> 32;
            // B_r || B_l ^ F(B_r, K_r)
            block = block << 32 | rb;
        }

        block = block >> 32 | block << 32;
        block = apply_permutation(block, Table::FP, 64, 64);
        return block;
    }

    void generate_round_keys(uint_fast64_t master, std::array<uint_fast64_t, 16> &keys) {
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
        return process_with_keys(block, this->keys.cbegin(), this->keys.cend());
    }

    [[nodiscard]] uint_fast64_t Engine::decrypt(const uint_fast64_t block) const {
        return process_with_keys(block, this->keys.crbegin(), this->keys.crend());
    }
}
