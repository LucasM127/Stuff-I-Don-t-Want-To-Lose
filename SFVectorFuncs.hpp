#ifndef SF_VECTOR_FUNCS_HPP
#define SF_VECTOR_FUNCS_HPP

#include <cmath>
#include <SFML/System/Vector2.hpp>

namespace SFUTIL
{

template <class T>
T lengthSquared(const sf::Vector2<T>& v)
{
    return v.x * v.x + v.y * v.y;
}

template <class T>
T length(const sf::Vector2<T>& v)
{
    return sqrt(lengthSquared(v));
}

template <class T>
sf::Vector2<T> getUnitVec(const sf::Vector2<T> &v)
{
    T l = length(v);
    return sf::Vector2<T>(v.x/l, v.y/l);
}

template <class T>
T cross(const sf::Vector2<T> &lhs, const sf::Vector2<T> &rhs)
{
    return (lhs.x * rhs.y) - (lhs.y * rhs.x);
}

template <class T>
T dot(sf::Vector2<T> lhs, sf::Vector2<T> rhs)
{
    return lhs.x * rhs.x + lhs.y * rhs.y;
}

}//SFUTIL

typedef sf::Vector2f Point;
typedef sf::Vector2<double> Pointd;

#endif //SF_VECTOR_FUNCS_HPP