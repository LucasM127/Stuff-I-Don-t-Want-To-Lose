#ifndef FIX_FLOAT_HPP
#define FIX_FLOAT_HPP

#include <cmath>

//float epsilon = 0.0001f;//seems to be good per unit.  for numbers of ~1000 scale by 1000
extern float epsilon;

inline float lowerOf(float a, float b)
{
    if (a < b)
        return a;
    return b;
}

inline bool areEqual(const float a, const float b)
{
    return (fabs(a - b) < epsilon);
}

inline bool isLess(const float a, const float b)
{
    return ((a + epsilon) < b);
}

inline bool isGreater(const float a, const float b)
{
    return (a > (b + epsilon));
}

inline void snapToInt(float &a)
{
    if (int(a + epsilon) != int(a - epsilon))
        a = int(a + epsilon);
}

#endif