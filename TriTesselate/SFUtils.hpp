//for the sfutillib... test other variations...
#ifndef ASDLJKLFJA
#define ASDLJKLFJA

#include <SFML/System/Vector2.hpp>

namespace SFUTIL
{

template <class T>
inline T lengthSquared(const sf::Vector2<T>& v)
{
    return v.x * v.x + v.y * v.y;
}

template <class T>
inline T length(const sf::Vector2<T>& v)
{
    return sqrt(lengthSquared(v));
}

template <class T>
inline sf::Vector2<T> getUnitVec(const sf::Vector2<T> &v)
{
    T l = length(v);
    return sf::Vector2<T>(v.x/l, v.y/l);
}

template <class T>
inline T cross(const sf::Vector2<T> &lhs, const sf::Vector2<T> &rhs)
{
    return (lhs.x * rhs.y) - (lhs.y * rhs.x);
}

template <class T>
inline T dot(sf::Vector2<T> lhs, sf::Vector2<T> rhs)
{
    return lhs.x * rhs.x + lhs.y * rhs.y;
}

template <class T>
struct line//y = mx + b
{//ax + by = c
    T a;
    T b;
    T c;
};

template <class T>
struct Circle
{
    sf::Vector2<T> center;
    T radius;
};

//geeksforgeeks
//ax + bx = c format for line
template <class T>
void lineFromPoints(const sf::Vector2<T> &p1, const sf::Vector2<T> &p2, line<T> &l)
{
    //l.a = p2.y - p1.y;
    l.a = p1.y - p2.y;
    l.b = p2.x - p1.x;
    //l.b = p1.x - p2.x;
    l.c = l.a * p1.x + l.b * p2.y;
}

template <class T>
sf::Vector2<T> getMidpoint(const sf::Vector2<T> &p1, const sf::Vector2<T> &p2)
{
    return (p2 + p1) / (T)2;
}

template <class T>
void perpendicularBisectorFromLine(const sf::Vector2<T> &p1, const sf::Vector2<T> &p2, line<T> &perp)
{
    sf::Vector2<T> midPoint = getMidpoint(p1, p2);
    //c = -bx + ay
    perp.c = -perp.b * midPoint.x + perp.a * midPoint.y;
    T temp = perp.a;
    perp.a = -perp.b;
    perp.b = temp;
}

//circumcenter of triangle...
template <class T>
bool getCircumcenter(const sf::Vector2<T> &p1, const sf::Vector2<T> &p2, const sf::Vector2<T> &p3, Circle<T> &circle)
{
    line<T> l1;
    lineFromPoints(p1,p2,l1);//gets eqn for line goes through the points...
    line<T> l2;
    lineFromPoints(p2,p3,l2);
    perpendicularBisectorFromLine(p1,p2,l1);
    perpendicularBisectorFromLine(p2,p3,l2);

    if(!linesIntersect(l1,l2,circle.center)) return false;
    //get radius, to any point
    circle.radius = length(p1 - circle.center);
    return true;
}//

template <class T>
bool pointInCircle(const sf::Vector2<T> &center, const T radius, const sf::Vector2<T> &point)
{
    sf::Vector2<T> delta = point - center;
    return (lengthSquared(delta) <= (radius * radius));
}

}//SFUTIL

typedef sf::Vector2f Point;
typedef sf::Vector2<double> Pointd;

#endif