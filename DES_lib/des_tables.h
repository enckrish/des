#ifndef S_DES_TABLES_H
#define S_DES_TABLES_H
#include <cstdint>

#endif //S_DES_TABLES_H

namespace DES::Table {
    extern uint8_t IP[64];
    extern uint8_t FP[64];
    extern uint8_t E[48];
    extern uint8_t S[8][4][16];
    extern uint8_t P[32];
    extern uint8_t PC1[56];
    extern int PC2[48];
    extern uint8_t L_SHIFTS[16];
    extern uint8_t R_SHIFTS[16];
}
