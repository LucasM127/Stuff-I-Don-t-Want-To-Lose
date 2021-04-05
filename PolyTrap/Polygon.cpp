#include "Polygon.hpp"

const float PI = 3.14159265f;

bool isConvex(const Point &a, const Point &b, const Point &c)
{
    sf::Vector2f v1 = a - b;
    sf::Vector2f v2 = c - b;

    float ang1 = atan2f(v1.y, v1.x) * 180.f / PI;
    float ang2 = atan2f(v2.y, v2.x) * 180.f / PI;

    float ang = ang2 - ang1;
    if (ang < 0.f)
        ang += 360.f;
    //if (ccw)
    //return ang;
    return ang < 180.f;
    //return (360.f - ang) < 180.f;
}

float getYIntValAtX(float x, LSeg &ls)
{
    float m = (ls.right.y - ls.left.y) / (ls.right.x - ls.left.x);
    float b = ls.left.y - m * ls.left.x;
    float y = m * x + b;
    return y;
}