#ifndef S_DES_DES_H
#define S_DES_DES_H
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
    T apply_permutation(T value, const V perm[], int perm_size, int input_width);

    uint_fast64_t apply_round_fn(uint_fast64_t rb, const uint_fast64_t r_key);

    uint_fast32_t S_box_process(uint_fast32_t block);

    /// Engine is the user facing interface for using the DES implementation.
    /// It removes need for the user to cache round keys and perform BigEndian conversions
    /// by doing those by itself.
    class Engine {
        uint_fast64_t keys[16]{};

    public:
        explicit Engine(uint_fast64_t master);

        [[nodiscard]] uint_fast64_t encrypt(uint_fast64_t block) const;

        [[nodiscard]] uint_fast64_t decrypt(uint_fast64_t block) const;
    };
}

#include "DES.tpp"