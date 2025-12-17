# DES C++ Library

DES is a now-defunct secret key-based encryption algorithm. This implementation provides easy-to-use (*and faster
than a naive implementation*) C++ classes for using DES in your projects (*for god knows why*). Alternatively, you can
build up a **3DES** implementation using this and call it another project ;-)

I wrote this to learn more about the low-level optimizations applicable to codebases that work exclusively with bit sequences and are loop-heavy. DES, being a Feistel cipher, satisfied both criteria while remaining simple at
its core.

You can read the commit history to get a bird's-eye view of the optimizations I applied and the difference they made to
the runtimes.

> **Note:** This is a **WIP**. I will be trying to make it *complete* and faster as I learn more things along the way.

## Usage

`DES::Engine` is the core class through which all operations are performed. Its interface is as follows (can also be
found in [DES.h](DES_lib/DES.h)):

```cpp
/// Initiates the Engine class for further encryption or decryption
/// @param master 64-bit DES key
Engine(uint_fast64_t master);

/// Encrypts a data block
/// @param block 64-bit block of data to encrypt
/// @return 64-bit encrypted block
uint_fast64_t encrypt(uint_fast64_t block) const;

/// Decrypts a data block
/// @param block 64-bit block of data to decrypt
/// @return 64-bit decrypted block
uint_fast64_t decrypt(uint_fast64_t block) const;

```

`uint_fast64_t` can be found in `<cstdint>` (or `stdint.h`) and resolves to `unsigned long` on 64-bit systems.

> **Environment Note:** I only test this on an **x86 Fedora 43** machine using the **GCC** compiler.

## Current State

The library currently works on a **single block at a time** and does not support any mode of encryption other than **ECB**.

### Performance

`DES::Engine` generates all round keys during instantiation and **caches them** for later use. The code includes benchmarks ([benchmarks.cpp](tests/benchmarks.cpp)) for measuring the time taken for key generation and
encryption/decryption.

**System Specs:**
My machine runs **Fedora 43** on a **Ryzen AI 7 350**, which reaches **5 GHz** clock speeds on a single thread during
the benchmarks. The figures inside the parantheses are with `-march=native` flag on in CMake. The measurements below are
specific to my machine:

| Procedure                 | Time Taken (ns) | Estimated Clock Cycles |
|---------------------------|-----------------|------------------------|
| **Key Generation**        | 28 - 29 (24-25) | 145 (125)              |
| **Encryption/Decryption** | 84 - 85 (76-77) | 425 (385)              |

**Optimizations Applied:**

* **Almost branchless code:** Using templates to generate code for specific branches wherever needed.
* **Function inlining:** To reduce call overhead.
* **[Combined S-box and P permutation](https://github.com/enckrish/des/commit/c51d9ec5dd38b2f289521d600470e59859f7045a)**.
* **[Precomputed lookup tables](https://github.com/enckrish/des/commit/3428981fd59ce728211e666e545065ba22de57ba):** Used
  to implement all permutations and S-box operations.
* **[Explicit loop unrolling](https://github.com/enckrish/des/commit/533be8543e21c954d0cc0776dec27254ef9cddbe)** (GCC
  only).
* **Using `uint_fast32_t` and `uint_fast64_t`**: Explicitly using sized ints resulted in slowdowns of ~5% (tested by
  replacing `uint_fast32_t` with `uint32_t`). The *"fast"* ints resolve to the fastest type for the arch.

### What I plan to add next

1. **Using x86 Intrinsics**
2. **Multi-block Computation:** Processing bigger blocks (e.g., four or eight 64-bit blocks) at once to amortize the
   costs of encryption/decryption.
3. **Multithreaded processing:** To do multiple of the multi-block processings concurrently.  
