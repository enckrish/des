#include <iostream>
#include "DES_lib/DES.h"

// A simple example of using the DES library

int main() {
    constexpr uint64_t key = 0x133457799BBCDFF1;
    constexpr uint64_t data = 0x0123456789ABCDEF;

    std::cout << "Key:\t\t" << std::hex << key << std::endl;
    std::cout << "Data:\t\t" << std::hex << data << std::endl;

    const DES::Engine engine(key);

    const auto enc = engine.encrypt(data);
    std::cout << "Encrypted:\t" << std::hex << enc << std::endl;

    const auto dec = engine.decrypt(enc);
    std::cout << "Decrypted:\t" << std::hex << dec << std::endl;
}
