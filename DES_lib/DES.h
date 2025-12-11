#ifndef S_DES_DES_H
#define S_DES_DES_H
#include <array>
#include <cstdint>

#endif //S_DES_DES_H

namespace DES {
    /// Encrypts/decrypts a block of data given the 64-bit key and the number of rounds
    /// @param block input block to DES
    /// @param key 64-bit key to DES
    /// @param key
    /// @param encrypt whether encryption or decryption is being done
    /// @return DES encrypted/decrypted block
    uint_fast64_t process(uint_fast64_t block, uint_fast64_t key, bool encrypt);

    template<typename T, typename V>
    constexpr T apply_permutation(T value, const V perm[], int perm_size, int input_width);

    constexpr uint_fast64_t apply_round_fn(uint_fast64_t rb, uint_fast64_t r_key);

    /// Engine is the user facing interface for using the DES implementation.
    /// It removes need for the user to cache round keys and perform BigEndian conversions
    /// by doing those by itself.
    class Engine {
        std::array<uint_fast64_t, 16> keys{};

    public:
        explicit Engine(uint_fast64_t master);

        [[nodiscard]] uint_fast64_t encrypt(uint_fast64_t block) const;

        [[nodiscard]] uint_fast64_t decrypt(uint_fast64_t block) const;
    };
}

#include "DES.tpp"