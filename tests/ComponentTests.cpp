#include <catch2/catch_test_macros.hpp>
#include "DES.h"
#include "des_tables.h"
#include <bitset>

unsigned long rand64();


template<size_t N>
std::bitset<N> rotate_left(const std::bitset<N> &b, size_t n) {
    n %= N;
    return b << n | b >> (N - n);
}

template<size_t N>
std::bitset<N> rotate_right(const std::bitset<N> &b, size_t n) {
    n %= N;
    return b >> n | b << (N - n);
}

TEST_CASE("28-bit rotation", "[ComponentTests]") {
    // create a 56 bit data
    const uint_fast64_t data = rand64() % (1UL << 28);

    SECTION("1-Rotate") {
        // rotate it by 1 or 2 bits
        auto rot1bs = std::bitset<28>(data);
        rot1bs = rotate_left(rot1bs, 1);

        const auto rot = rot1bs.to_ulong();
        const auto rot_impl = DES::circ_lshift_u28(data, 1);
        CHECK(rot == rot_impl);
    }

    SECTION("2-Rotate") {
        // rotate it by 1 or 2 bits
        auto rot2bs = std::bitset<28>(data);
        rot2bs = rotate_left(rot2bs, 2);

        const auto rot = rot2bs.to_ulong();
        const auto rot_impl = DES::circ_lshift_u28(data, 2);
        CHECK(rot == rot_impl);
    }
}

TEST_CASE("Key Rotate", "[ComponentTests]") {
    const uint_fast64_t key = rand64() % (1UL << 56);
    auto rotbs_r = std::bitset<28>(key);
    rotbs_r = rotate_left(rotbs_r, 2);
    auto rotbs_l = std::bitset<28>(key >> 28);
    rotbs_l = rotate_left(rotbs_l, 2);

    const auto rotbs = rotbs_l.to_ulong() << 28 | rotbs_r.to_ulong();
    const auto rot_impl = DES::circ_shift_key(key, 2, true);
    CHECK(rotbs == rot_impl);
}

TEST_CASE("apply_permutation/IP-InverseIP<->FP", "[ComponentTests]") {
    // create a 56 bit data
    const uint_fast64_t data = rand64();

    const auto ipv = DES::apply_permutation(data, DES::Table::IP, 64);
    const auto inv_ipv = DES::apply_permutation(ipv, DES::Table::FP, 64);
    INFO("Data:\n" << std::bitset<64>(data));
    INFO("FP:\n" << std::bitset<64>(inv_ipv));
    CHECK(data == inv_ipv);
}

