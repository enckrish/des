#include <catch2/catch_test_macros.hpp>
#include <catch2/benchmark/catch_benchmark_all.hpp>
#include "DES.h"

unsigned long rand64();

TEST_CASE("DES Benchmarks", "[DESTests]") {
    BENCHMARK_ADVANCED("Key Generation/Engine Initialization")(Catch::Benchmark::Chronometer meter) {
        const auto master = rand64();
        meter.measure([&] {
            auto _ = DES::Engine(master);
        });
    };

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
};
