#include "kernel/math.h"

uint32_t is_power_of_2 (uint32_t n) {
    return !((n-1) & n) && n;
}


uint32_t next_power_of_2 (uint32_t n) {
    if(is_power_of_2(n))
        return n;
    n |= n >> 1;
    n |= n >> 2;
    n |= n >> 4;
    n |= n >> 8;
    n |= n >> 16;

    return n + 1;
}

uint32_t power_of_2 (uint32_t n) {
    uint32_t i = 0;
    while(! (n & (uint32_t)1)) {
        n = n >> 1;
        ++i;
    }
    return i;
}