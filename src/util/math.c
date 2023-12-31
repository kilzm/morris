#include <assert.h>

#include "math.h"

int32_t max(int32_t a, int32_t b)
{
    return (a > b) ? a : b;
}

int32_t min(int32_t a, int32_t b)
{
    return (a < b) ? a : b;
}

int32_t mod(int32_t a, int32_t m)
{
    assert(m != 0);
    int r = a % m;
    return r < 0 ? r + m : r;
}