//
// Created by krishnendu on 11/25/25.
//

#ifndef S_DES_DES_H
#define S_DES_DES_H
#include <cstdint>

#endif //S_DES_DES_H

namespace DES {
    /// Encrypts/decrypts a block of data given the 64-bit key and the number of rounds
    /// @param block input block to DES
    /// @param key 64-bit key to DES
    /// @param key
    /// @param rounds number of DES rounds
    /// @param encrypt whether encryption or decryption is being done
    /// @return DES encrypted/decrypted block
    uint64_t process(uint_fast64_t block, uint_fast64_t key, unsigned int rounds, bool encrypt);

    template<typename T, typename V>
    T apply_permutation(T value, const V perm[], int psize);
    uint_fast64_t circ_lshift_u28(uint_fast64_t key, unsigned int n);
    uint_fast64_t circ_shift_key(uint_fast64_t key, unsigned int n, bool left);
    uint_fast32_t S_box_process(uint_fast32_t block);
    uint_fast64_t round_transform(uint_fast64_t block, uint_fast64_t rkey);
}
