#include <random>
#include <bitset>
#include <catch2/catch_test_macros.hpp>
#include "DES.h"

unsigned long rand64();

TEST_CASE("DES data retrieval", "[DESTests]") {
    const uint_fast64_t data = rand64();
    const uint_fast64_t key = rand64();

    const auto enc = DES::process(data, key, true);
    const auto dec = DES::process(enc, key, false);
    INFO("Data:\t" << std::bitset<64>(data));
    INFO("Dec:\t" << std::bitset<64>(dec));
    CHECK(data == dec);
}

TEST_CASE("DES Engine data retrieval", "[DESTests]") {
    const uint_fast64_t data = rand64();
    const uint_fast64_t key = rand64();

    const DES::Engine engine(key);
    const auto enc = engine.encrypt(data);
    const auto dec = engine.decrypt(enc);
    INFO("Data:\t" << std::bitset<64>(data));
    INFO("Dec:\t" << std::bitset<64>(dec));
    CHECK(data == dec);
}

TEST_CASE("DES process()<->Engine Matching", "[DESTests]") {
     uint_fast64_t key = rand64();
    const DES::Engine engine(key);

    // Converting the key to BE-format to make it comparable to DES::Engine's,
    // which converts it internally
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    key = __builtin_bswap64(key);
#endif

     uint_fast64_t data = rand64();

    // To reduce data's endianness changes, we are assuming that data is
    // already in BE, but since DES::Engine will change the endianness of data
    // when it's on a LE machine, we will do one conversion before Engine::encrypt
    const auto enc_pr = DES::process(data, key, true);


    auto dec_pr = DES::process(enc_pr, key, false);
    // Changing dec_pr's endianness to match with Engine's
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    dec_pr = __builtin_bswap64(dec_pr);
#endif

    SECTION("Encryption") {
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
        data = __builtin_bswap64(data);
#endif
        const auto enc_en = engine.encrypt(data);
        INFO("Encrypt Process:\t" << std::bitset<64>(enc_pr));
        INFO("Encrypt Engine:\t" << std::bitset<64>(enc_en));
        CHECK(enc_en == enc_pr);
    }

    SECTION("Decryption") {
        const auto dec_en = engine.decrypt(enc_pr);
        INFO("Decrypt Process:\t" << std::bitset<64>(dec_pr));
        INFO("Decrypt Dec:\t" << std::bitset<64>(dec_en));
        CHECK(dec_en == dec_pr);
    }
}

TEST_CASE("DES Complementation", "[DESTests]") {
    const uint_fast64_t data = rand64();
    const uint_fast64_t key = rand64();

    const auto enc = DES::process(data, key, true);

    const auto data_bar = ~data;
    const auto key_bar = ~key;
    const auto enc_bar = DES::process(data_bar, key_bar, true);

    INFO("Enc(bar):\t" << std::bitset<64>(~enc));
    INFO("Enc_bar:\t" << std::bitset<64>(enc_bar));
    CHECK(enc == ~enc_bar);
}
