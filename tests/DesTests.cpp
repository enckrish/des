#include <random>
#include <bitset>
#include <catch2/catch_test_macros.hpp>
#include "DES.h"

unsigned long rand64();

TEST_CASE("DES data retrieval", "[DesTests]") {
    const uint_fast64_t data = rand64();
    const uint_fast64_t key = rand64();

    const auto enc = DES::process(data, key, 16, true);
    const auto dec = DES::process(enc, key, 16, false);
    INFO("Data:\t" << std::bitset<64>(data));
    INFO("Dec:\t" << std::bitset<64>(dec));
    CHECK(data == dec);
}

TEST_CASE("DES Complementation", "[DesTests]") {
    const uint_fast64_t data = rand64();
    const uint_fast64_t key = rand64();

    const auto enc = DES::process(data, key, 16, true);

    const auto data_bar = ~data;
    const auto key_bar = ~key;
    const auto enc_bar = DES::process(data_bar, key_bar, 16, true);

    INFO("Enc(bar):\t" << std::bitset<64>(~enc));
    INFO("Enc_bar:\t" << std::bitset<64>(enc_bar));
    CHECK(enc == ~enc_bar);
}

// Feistel structures don't alter the right half of the block after one round
TEST_CASE("Single Round R_p Equality", "[DESTests]") {
    const uint_fast64_t block = rand64();
    const uint_fast64_t key = rand64();

    auto post_ip = block;
    post_ip = DES::apply_permutation(post_ip, DES::Table::IP, 64);

    auto enc = DES::process(block, key, 1, true);
    enc = DES::apply_permutation(enc, DES::Table::IP, 64);

    const auto msk32 = 0xffffffff;
    INFO("Enc:\t" << std::bitset<64>(enc));
    INFO("Data:\t" << std::bitset<64>(post_ip));
    CHECK((enc & msk32) == (post_ip & msk32));
}

TEST_CASE("Single Round Encryption<->Decryption", "[DesTests]") {
    const uint_fast64_t block = rand64();
    const uint_fast64_t key = rand64();

    const auto enc = DES::process(block, key, 1, true);
    const auto dec = DES::process(enc, key, 1, false);

    INFO("Enc:\t" << std::bitset<64>(enc));
    INFO("Dec:\t" << std::bitset<64>(dec));
    CHECK(block == dec);
}

TEST_CASE("Double Round Encryption<->Decryption", "[DesTests]") {
    const uint_fast64_t block = rand64();
    const uint_fast64_t key = rand64();

    const auto enc = DES::process(block, key, 2, true);
    const auto dec = DES::process(enc, key, 2, false);

    INFO("Enc:\t" << std::bitset<64>(enc));
    INFO("Dec:\t" << std::bitset<64>(dec));
    CHECK(block == dec);
}