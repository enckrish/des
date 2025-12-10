#include <random>
#include <bitset>
#include <catch2/catch_test_macros.hpp>
#include <catch2/benchmark/catch_benchmark_all.hpp>
#include "DES.h"

unsigned long rand64();

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

TEST_CASE("DES Complementation", "[DESTests]") {
    const uint_fast64_t data = rand64();
    const uint_fast64_t key = rand64();
    const auto engine = DES::Engine(key);

    const auto enc = engine.encrypt(data);

    const auto data_bar = ~data;
    const auto key_bar = ~key;
    const auto engine_bar = DES::Engine(key_bar);
    const auto enc_bar = engine_bar.encrypt(data_bar);

    INFO("Enc(bar):\t" << std::bitset<64>(~enc));
    INFO("Enc_bar:\t" << std::bitset<64>(enc_bar));
    CHECK(enc == ~enc_bar);
}

TEST_CASE("DES Benchmarks", "[DESTests]") {
    BENCHMARK_ADVANCED("Encryption")(Catch::Benchmark::Chronometer meter) {
        auto engine = DES::Engine(rand64());
        meter.measure([engine] {
            auto _ = engine.encrypt(rand64());
        });
    };

    BENCHMARK_ADVANCED("Decryption")(Catch::Benchmark::Chronometer meter) {
        auto engine = DES::Engine(rand64());
        meter.measure([engine] {
            auto _ = engine.decrypt(rand64());
        });
    };
}

TEST_CASE("DES Test Cases", "[DESTests]") {
    constexpr uint_fast64_t keys[] = {0x133457799BBCDFF1, 0x0E329232EA6D0D73};
    constexpr uint_fast64_t data[] = {0x0123456789ABCDEF, 0x8787878787878787};
    constexpr uint_fast64_t expected[] = {0x85e813540f0Ab405, 0x0000000000000000};

    constexpr auto len = std::size(keys);
    for (int i = 0; i < len; i++) {
        const auto engine = DES::Engine(keys[i]);

        const auto enc = engine.encrypt(data[i]);
        INFO("Failed for key: " << i);
        CHECK(enc == expected[i]);

        const auto dec = engine.decrypt(enc);
        INFO("Failed for key: " << i);
        CHECK(dec == data[i]);
    }
}
