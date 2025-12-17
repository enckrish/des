#include "DES.h"

#include "DES_tables.h"

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
        for (int i = 0; i < 4; i++) {
            be ^= LUT::E[i][rb >> (8 * i) & 0xff];
        }

        // XOR with key
        be ^= r_key;

        // applying S-box and P permutation at once
        uint_fast64_t bsp{};
        for (auto i = 0; i < 8; i++) {
            bsp ^= LUT::SP[i][be >> 6 * (7 - i) & 0b111111];
        }
        return bsp;
    }

    template<bool Encrypt>
    uint_fast64_t process_with_keys(const uint_fast64_t blk, uint_fast64_t const *key_pointer) {
        // IP
        uint_fast64_t block{};
        for (int i = 0; i < 8; i++) {
            block ^= LUT::IP[i][blk >> (8 * i) & 0xff];
        }

        // Unrolling factor 16 reduced time from 88 ns to 84-85 ns
#pragma GCC unroll 16
        for (auto i = 0; i < 16; i++) {
            const auto lb = block >> 32;
            const auto rb = block & 0xFFFFFFFF;

            // Apply round function on right half of block: F(B_r, K_r)
            const auto rrb = apply_round_fn(rb, *key_pointer);

            // Prior to this commit, we were using indexing to access the keys
            // with index i for encryption and index 16-i for decryption.
            // This resulted in decryption taking slightly more time.
            // Doing the same using pointer arithmetic results in equal times for both.
            // Change pointer to next key
            Encrypt ? key_pointer++ : key_pointer--;

            // XOR with left half: B_l ^ F(B_r, K_r)
            // B_r || B_l ^ F(B_r, K_r)
            block = (rb << 32) | (rrb ^ lb);
        }

        // L-R Swap + FP
        uint_fast64_t fblk{};
        for (int i = 0; i < 8; i++) {
            fblk ^= LUT::Swap_FP[i][block >> (8 * i) & 0xff];
        }
        return fblk;
    }

    void generate_round_keys(const uint_fast64_t master, uint_fast64_t keys[16]) {
        uint_fast64_t mkey{};

        // PC-1 permutation
        int lv{};
        for (lv = 0; lv < 8; lv++) {
            mkey ^= LUT::PC1[lv][master >> (8 * lv) & 0xff];
        }

        // Unrolling factor 16 reduced time from 40 ns to 28 ns
#pragma GCC unroll 16
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
        return process_with_keys<true>(block, keys);
    }

    [[nodiscard]] uint_fast64_t Engine::decrypt(const uint_fast64_t block) const {
        return process_with_keys<false>(block, keys + 15);
    }
}
