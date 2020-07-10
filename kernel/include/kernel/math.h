#ifndef KERNEL_MATH_
#define KERNEL_MATH_
#include <stdint.h>


// check if n is power of 2
uint32_t is_power_of_2 (uint32_t n);

// get the least number is power of 2 which is bigger than or equal to n
uint32_t next_power_of_2 (uint32_t n);

// get the exponent of n if n is power of 2
uint32_t power_of_2 (uint32_t n);


#endif // KERNEL_MATH_