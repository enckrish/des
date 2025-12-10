#include <catch2/catch_test_macros.hpp>
#include "DES.h"
#include "DES_tables.h"
#include <bitset>

unsigned long rand64();

TEST_CASE("apply_permutation/IP-InverseIP<->FP", "[ComponentTests]") {
    // create a 56 bit data
    const uint_fast64_t data = rand64();

    const auto ipv = DES::apply_permutation(data, DES::Table::IP, 64, 64);
    const auto inv_ipv = DES::apply_permutation(ipv, DES::Table::FP, 64, 64);
    INFO("Data:\n" << std::bitset<64>(data));
    INFO("FP:\n" << std::bitset<64>(inv_ipv));
    CHECK(data == inv_ipv);
}
