#ifndef POLYGON_HPP
#define POLYGON_HPP

#include <SFML/System/Vector2.hpp>
#include <vector>
#include "FixFloat.hpp"

typedef sf::Vector2f Point;
typedef std::vector<Point> Polygon;

struct Vertice;

struct LSeg
{
    unsigned int id;
    Point left;
    Point right;
    unsigned int lVertice;
    unsigned int rVertice;
    //Vertice *leftV;
    //Vertice *rightV;
};

struct Vertice
{
    unsigned int id;
    unsigned int top;
    //unsigned int bottom;
    //bool isConvex;
    unsigned int segA;
    unsigned int segB;
    float x;
    enum Type
    {
        MERGE,
        SPLIT,
        CONTINUE
    } type;
};

struct Edge
{
    unsigned int topLSeg;
    unsigned int bottomLSeg;
    unsigned int vertice;
    //not sure but maybe???
    unsigned int m_monotonePoly;
    int m_sharedPoly;//=-1
};

struct Trap
{
    Point topLeft, topRight;
    Point bottomLeft, bottomRight;
};

struct Triangle
{
    unsigned int a;
    unsigned int b;
    unsigned int c;
};//int = vertex in P

//for CCW ordering
bool isConvex(const Point &a, const Point &b, const Point &c);

float getYIntValAtX(float x, LSeg &ls);

#endif