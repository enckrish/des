#include <cstdint>
#include "DES.h"

#include "DES_tables.h"

#include <ranges>

namespace DES {
    /// @brief Rotates a u28 left by n bits
    /// @param key value with bits 28-64 set to 0
    /// @return rotated key, bits 28-64 set to 0
    ///
    /// @note assumes arithmetic shift when using shift operators
    template<int N>
    constexpr uint_fast64_t circ_lshift_u28(const uint_fast64_t key) {
        return (key << N | key >> (28 - N)) & 0x0FFFFFFF;
    }

    constexpr uint_fast64_t apply_round_fn(const uint_fast64_t rb, const uint_fast64_t r_key) {
        // E perm
        uint_fast64_t be{};
#pragma GCC unroll 4
        for (int i = 0; i < 4; i++) {
            be ^= LUT::E[i][rb >> (8 * i) & 0xff];
        }

        // XOR with key
        be ^= r_key;

        // applying S-box and P permutation at once
        uint_fast64_t bsp{};
#pragma GCC unroll 8
        for (auto i = 0; i < 8; i++) {
            bsp ^= LUT::SP[i][be >> 6 * (7 - i) & 0b111111];
        }
        return bsp;
    }

    template<bool Encrypt>
    uint_fast64_t process_with_keys(const uint_fast64_t blk, const std::array<uint_fast64_t, 16> &keys) {
        // IP
        uint_fast64_t block{};
#pragma GCC unroll 8
        for (int i = 0; i < 8; i++) {
            block ^= LUT::IP[i][blk >> (8 * i) & 0xff];
        }

#pragma GCC unroll 4
        for (auto i = 0; i < 16; i++) {
            const auto lb = block >> 32;
            const auto rb = block & 0xFFFFFFFF;

            // Calculate round key K_r
            const auto key_index = Encrypt ? i : 15 - i;
            // Apply round function on right half of block: F(B_r, K_r)
            const auto rrb = apply_round_fn(rb, keys[key_index]);

            // XOR with left half: B_l ^ F(B_r, K_r)
            // B_r || B_l ^ F(B_r, K_r)
            block = rb << 32 | rrb ^ lb;
        }

        // L-R Swap + FP
        uint_fast64_t fblk{};
#pragma GCC unroll 8
        for (int i = 0; i < 8; i++) {
            fblk ^= LUT::Swap_FP[i][block >> (8 * i) & 0xff];
        }
        return fblk;
    }

    void generate_round_keys(const uint_fast64_t master, std::array<uint_fast64_t, 16> &keys) {
        uint_fast64_t mkey{};

        // PC-1 permutation
        int lv{};
#pragma GCC unroll 8
        for (lv = 0; lv < 8; lv++) {
            mkey ^= LUT::PC1[lv][master >> (8 * lv) & 0xff];
        }

#pragma GCC unroll 4
        for (auto i = 0; i < 16; i++) {
            // Key rotate
            constexpr auto msk = 0xfffffff;
            const auto m_update = [&]<int N> {
                mkey = circ_lshift_u28<N>(mkey >> 28) << 28 | circ_lshift_u28<N>(mkey & msk);
            };
            if (Table::L_SHIFTS[i] == 1) {
                m_update.operator()<1>();
            } else {
                m_update.operator()<2>();
            }

            // PC-2 permutation
            uint_fast64_t ki{};
#pragma GCC unroll 7
            for (lv = 0; lv < 7; lv++) {
                ki ^= LUT::PC2[lv][mkey >> (8 * lv) & 0xff];
            }
            keys[i] = ki;
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
