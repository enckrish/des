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
    template<int N>
    constexpr uint_fast64_t circ_lshift_u28(const uint_fast64_t key) {
        return (key << N | key >> (28 - N)) & 0x0FFFFFFF;
    }

    constexpr uint_fast64_t apply_round_fn(const uint_fast64_t rb, const uint_fast64_t r_key) {
        uint_fast64_t be{};
        for (int i = 3; i >= 0; i--) {
            be ^= LUT::E[i][rb >> (8 * i) & 0xff];
        }
        be ^= r_key;

        // applying S-box and P permutation at once
        uint_fast64_t bsp{};
        for (auto i = 0; i < 8; i++) {
            bsp ^= LUT::SP[i][be >> 6 * (7 - i) & 0b111111];
        }
        return bsp;
    }

    template<typename Iter>
    uint_fast64_t process_with_keys(const uint_fast64_t blk, Iter key_begin, const Iter &key_end) {
        uint_fast64_t block{};
        for (int i = 7; i >= 0; i--) {
            block ^= LUT::IP[i][blk >> (8 * i) & 0xff];
        }

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
        uint_fast64_t fblk{};
        for (int i = 7; i >= 0; i--) {
            fblk ^= LUT::FP[i][block >> (8 * i) & 0xff];
        }
        return fblk;
    }

    void generate_round_keys(const uint_fast64_t master, std::array<uint_fast64_t, 16> &keys) {
        uint_fast64_t mkey{};

        // PC-1 permutation
        int lv{};
        for (lv = 7; lv >= 0; lv--) {
            mkey ^= LUT::PC1[lv][master >> (8 * lv) & 0xff];
        }

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
            for (lv = 7; lv >= 0; lv--) {
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
        return process_with_keys(block, this->keys.cbegin(), this->keys.cend());
    }

    [[nodiscard]] uint_fast64_t Engine::decrypt(const uint_fast64_t block) const {
        return process_with_keys(block, this->keys.crbegin(), this->keys.crend());
    }
}
