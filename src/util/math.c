#include <assert.h>

#include "math.h"

i32 max(i32 a, i32 b)
{
    return (a > b) ? a : b;
}

i32 min(i32 a, i32 b)
{
    return (a < b) ? a : b;
}

i32 mod(i32 a, i32 m)
{
    assert(m != 0);
    int r = a % m;
    return r < 0 ? r + m : r;
}