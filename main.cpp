#include <iostream>
#include <bitset>
#include <random>
#include "DES_lib/des.cpp"

int main() {
    std::random_device rd;
    std::mt19937_64 mt(rd());
    std::uniform_int_distribution<uint_fast64_t> distrib(
        0, (static_cast<uint_fast64_t>(1) << 63) - 1
    );
    uint64_t data = distrib(mt);
    std::cout << "Data:\t\t" << std::bitset<64>(data) << std::endl;

    uint64_t key = distrib(mt);
    std::cout << "Key:\t\t" << std::bitset<64>(key) << std::endl;

    auto enc = DES::process(data, key, 16);
    std::cout << "Encrypted:\t" << std::bitset<64>(enc) << std::endl;

    auto dec = DES::process(enc, key, 16);
    std::cout << "Decrypted:\t" << std::bitset<64>(dec) << std::endl;
}
