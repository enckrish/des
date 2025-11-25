#include <iostream>
#include <bitset>
#include <random>
#include "DES_lib/DES.h"

int main() {
    std::random_device rd;
    std::mt19937_64 mt(rd());
    std::uniform_int_distribution<uint_fast64_t> distrib(
        0, (static_cast<uint_fast64_t>(1) << 63) - 1
    );
    const uint64_t key = distrib(mt);
    std::cout << "Key:\t\t" << std::bitset<64>(key) << std::endl;

    const DES::Engine engine(key);

    const uint64_t data = distrib(mt);
    std::cout << "Data:\t\t" << std::bitset<64>(data) << std::endl;


    const auto enc = engine.encrypt(data);
    std::cout << "Encrypted:\t" << std::bitset<64>(enc) << std::endl;

    const auto dec = engine.decrypt(enc);
    std::cout << "Decrypted:\t" << std::bitset<64>(dec) << std::endl;
}
