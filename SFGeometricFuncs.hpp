#ifndef SF_GEOM_FUNCS_HPP
#define SF_GEOM_FUNCS_HPP

#include "SFVectorFuncs.hpp"
#include "MathUtil.hpp"

namespace SFUTIL
{

template <class T>
struct AABB
{
    AABB() : x_min(0), x_max(0), y_min(0), y_max(0) {}
    AABB(const sf::Vector2<T> * const points, std::size_t numPts){resize(points,numPts);}
    AABB(const sf::Vector2<T> &a, const sf::Vector2<T> &b);
    bool intersects(const AABB<T> &) const;
    bool contains(const Point &point) const;
    void resize(const sf::Vector2<T> *const points, std::size_t numPts);
    T x_min;
    T x_max;
    T y_min;
    T y_max;
    T width() { return x_max - x_min; }
    T height() { return y_max - y_min; }
};

template <class T>
AABB<T>::AABB(const sf::Vector2<T> &a, const sf::Vector2<T> &b)
{
    greaterLesser(a.x, b.x, x_max, x_min);
    greaterLesser(a.y, b.y, y_max, y_min);
}

template <class T>
bool AABB<T>::contains(const Point &point) const
{
    return
    (
        point.x > x_min &&
        point.x < x_max &&
        point.y > y_min &&
        point.y < y_max
    );
}

template <class T>
bool AABB<T>::intersects(const AABB<T> &aabb) const
{
    return
    (
        x_min < aabb.x_max &&
        x_max > aabb.x_min &&
        y_min < aabb.y_max &&
        y_max > aabb.y_min
    );
}

template <class T>
void AABB<T>::resize(const sf::Vector2<T> * const points, std::size_t numPts)
{
    if (numPts == 0)
        return;
    x_min = x_max = points[0].x;
    y_min = y_max = points[0].y;
    for (std::size_t i = 1; i < numPts; ++i)
    {
        if (points[i].x < x_min)
            x_min = points[i].x;
        else if (points[i].x > x_max)
            x_max = points[i].x;
        if (points[i].y < y_min)
            y_min = points[i].y;
        else if (points[i].y > y_max)
            y_max = points[i].y;
    }
}

typedef AABB<float> AABBf;

template <class T>
struct LSeg
{
    sf::Vector2<T> a;
    sf::Vector2<T> b;
};

typedef LSeg<float> LSegf;

//signed area dependant on winding order...
template <class T>
inline T calcTriangleArea(const sf::Vector2<T> &p1, const sf::Vector2<T> &p2, const sf::Vector2<T> &p3)
{
    return (((p1.x - p3.x) * (p2.y - p3.y)) - ((p2.x - p3.x) * (p1.y - p3.y))) / (2.0);
}

template <class T>
bool isPointInTriangle(const sf::Vector2<T> &p, const sf::Vector2<T> &a, const sf::Vector2<T> &b, const sf::Vector2<T> &c)
{
    sf::Vector2<T> v0 = c - a;
    sf::Vector2<T> v1 = b - a;
    sf::Vector2<T> v2 = p - a;

    T dot00 = dot(v0, v0);
    T dot01 = dot(v0, v1);
    T dot02 = dot(v0, v2);
    T dot11 = dot(v1, v1);
    T dot12 = dot(v1, v2);

    T denominator = (dot00 * dot11 - dot01 * dot01);
    if (denominator == 0)
        return false;

    T u = (dot11 * dot02 - dot01 * dot12) / denominator;
    T v = (dot00 * dot12 - dot01 * dot02) / denominator;

    return (u >= 0) && (v >= 0) && (u + v < 1);
}

//https://stackoverflow.com/questions/563198/how-do-you-detect-where-two-line-segments-intersect
template <class T>
bool lineSegsIntersect(const sf::Vector2<T> &p1, const sf::Vector2<T> &p2, const sf::Vector2<T> &p3, const sf::Vector2<T> &p4, sf::Vector2<T> *intersectPoint = nullptr)
{
    T s1_x = p2.x - p1.x;
    T s1_y = p2.y - p1.y;
    T s2_x = p4.x - p3.x;
    T s2_y = p4.y - p3.y;

    T denominator = (-s2_x * s1_y + s1_x * s2_y);
    if (denominator == 0)
        return false; //parallel?

    T s = (-s1_y * (p1.x - p3.x) + s1_x * (p1.y - p3.y)) / denominator;
    T t = (s2_x * (p1.y - p3.y) - s2_y * (p1.x - p3.x)) / denominator;

    if (s >= 0 && s <= 1 && t >= 0 && t <= 1)//(s >= 0 && s <= 1 && t >= 0 && t <= 1)
    {
        if (intersectPoint)
        {
            intersectPoint->x = p1.x + (t * s1_x);
            intersectPoint->y = p1.y + (t * s1_y);
        }
        return true;
    }

    return false;
}

template <class T>
struct line//y = mx + b
{//ax + by = c
    T a;
    T b;
    T c;
};

template <class T>
bool linesIntersect(const line<T> &l1, const line<T> &l2, sf::Vector2<T> &intPt)
{
    T det = l1.a * l2.b - l2.a * l1.b;
    if(det == 0)//parallel lines
        return false;
    
    intPt.x = (l2.b * l1.c - l1.b * l2.c) / det;
    intPt.y = (l1.a * l2.c - l2.a * l1.c) / det;
    return true;
}

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
T yIntAtX(const line<T>& L, const T x)
{
    return (L.c - L.a * x) / L.b;
}

template <class T>
T xIntAtY(const line<T> &L, const T y)
{
    return (L.c - L.b * y) / L.a;
}

/***
void lineFromPoints(const Point &A, const Point &B, float &a,
                    float &b, float &c)
{
    a = A.y - B.y;
    b = B.x - A.x;
    c = a * (A.x) + b * (A.y);
}

float yIntAtX(const float &a, const float &b, const float &c, const float x)
{
    //ax + by = c
    //y = (c - ax)/b
    return (c - a * x) / b;
}

float xIntAtY(const float &a, const float &b, const float &c, const float y)
{
    //ax + by = c
    //x = (c - by)/a
    return (c - b * y) / a;
}***/

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

//circles and polygons
template <class T>
struct Circle
{
    sf::Vector2<T> center;
    T radius;
};

template <class T>
bool circlesIntersect(const sf::Vector2<T> &c1, const T r1, const sf::Vector2<T> &c2, const T r2)
{
    T distSqrd = (r1 + r2) * (r1 + r2);
    sf::Vector2<T> delta = c2 - c1;
    return (lengthSquared(delta) <= distSqrd);
} //can get intersection points?

template <class T>
bool pointInCircle(const sf::Vector2<T> &center, const T radius, const sf::Vector2<T> &point)
{
    sf::Vector2<T> delta = point - center;
    return (lengthSquared(delta) <= (radius * radius));
}

/**
void findCircumCenter(pdd P, pdd Q, pdd R) 
{ 
    // Line PQ is represented as ax + by = c 
    double a, b, c; 
    lineFromPoints(P, Q, a, b, c); 
  
    // Line QR is represented as ex + fy = g 
    double e, f, g; 
    lineFromPoints(Q, R, e, f, g); 
  
    // Converting lines PQ and QR to perpendicular 
    // vbisectors. After this, L = ax + by = c 
    // M = ex + fy = g 
    perpendicularBisectorFromLine(P, Q, a, b, c); 
    perpendicularBisectorFromLine(Q, R, e, f, g); 
  
    // The point of intersection of L and M gives 
    // the circumcenter 
    pdd circumcenter = 
           lineLineIntersection(a, b, c, e, f, g); 
  
    if (circumcenter.first == FLT_MAX && 
        circumcenter.second == FLT_MAX) 
    { 
        cout << "The two perpendicular bisectors "
                "found come parallel" << endl; 
        cout << "Thus, the given points do not form "
                "a triangle and are collinear" << endl; 
    } 
  
    else
    { 
        cout << "The circumcenter of the triangle PQR is: "; 
        cout << "(" << circumcenter.first << ", "
             << circumcenter.second  << ")" << endl; 
    } 
} **/

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

//https://en.wikipedia.org/wiki/Circumscribed_circle#Circumcenter_coordinates
template <class T>
sf::Vector2<T> circumcenter(const sf::Vector2<T> &a, const sf::Vector2<T> &b, const sf::Vector2<T> &c)
{//as long as D != 0 or in other words a,b,c are not collinear
    sf::Vector2<T> B = b - a;
    sf::Vector2<T> C = c - a;
    T D = 2 * (B.x * C.y - B.y * C.x);
    T BB = B.x * B.x + B.y * B.y;
    T CC = C.x * C.x + C.y * C.y;
    T Ux = C.y * BB - B.y * CC;
    T Uy = B.x * CC - C.x * BB;
    //radius = sqrt(ux2 + uy2)
    return sf::Vector2<T>(a.x + Ux/D, a.y + Uy/D);
}

//https://stackoverflow.com/questions/217578/how-can-i-determine-whether-a-2d-point-is-within-a-polygon
/*int pnpoly(int nvert, float *vertx, float *verty, float testx, float testy)
{
    int i, j, c = 0;
    for (i = 0, j = nvert - 1; i < nvert; j = i++)
    {
        if (((verty[i] > testy) != (verty[j] > testy)) &&
            (testx < (vertx[j] - vertx[i]) * (testy - verty[i]) / (verty[j] - verty[i]) + vertx[i]))
            c = !c;
    }
    return c;
}*/
//I run a semi-infinite ray horizontally (increasing x, fixed y) out from the test point, and count how many edges it crosses.
//At each crossing, the ray switches between inside and outside.
//This is called the Jordan curve theorem.
//if test point is inside, crosses even number of points,  HAS to cross a point if inside, if outside may not hit any or 2 or 4 etc...
template <class T>
bool pointInPoly(const sf::Vector2<T> *const points, std::size_t numPts, const sf::Vector2<T> &p)
{
    int i, j;
    bool c = false;
    for (i = 0, j = numPts - 1; i < numPts; j = i++)
    {
        if (((points[i].y > p.y) != (points[j].y > p.y)) &&
            (p.x < (points[j].x - points[i].x) * (p.y - points[i].y) / (points[j].y - points[i].y) + points[i].x))
            c = !c;
    }
    return c;
}
//TODO:
//rewrite to take into account winding order and multiple polygons,
//so can 'add' or 'subtract' polygon shapes
//winding ordre from that apple .ttf logic for fonts, goes up or rigth, add, else subtract, (instead of flipflopping)

template <class T>
bool isPolygonConvex(const sf::Vector2<T> *const points, std::size_t numPts, bool isClockwise)
{
    //return false when hits opposite sign...
    //test if is clockwise or not
    for (uint i = 0; i < numPts; i++)
    {
        uint next = NEXT(i, numPts);
        uint next2 = NEXT(next, numPts);
        //compute cross product
        sf::Vector2<T> a = points[next] - points[i];
        sf::Vector2<T> b = points[next2] - points[next];
        if (isClockwise)
        {
            if (cross(a, b) > 0)
                return false;
        }
        else
        {
            if (cross(a, b) < 0)
                return false;
        }
    }
    return true;
}
//polyLine... similar...
/* template typedef... same type of thing... good to know.
template <typename T>
using MyVector = std::vector<T, MyCustomAllocator<T>>;
*/

template <class T>
bool isPolygonClockwise(const sf::Vector2<T> *const points, std::size_t numPts)
{
    T area = 0;
    for (uint i = 0; i < numPts; i++)
    {
        uint next = NEXT(i, numPts);
        const Point &a = points[i];
        const Point &b = points[next];
        area += (b.x - a.x) * ((a.y + b.y) / 2);
    }
    return area > 0;
}

template <class T>
void changePolygonOrientation(const sf::Vector2<T> * points, std::size_t numPts)
{
    for(uint i = 0; i < numPts/2; i++)
        std::swap(points[i],points[numPts-1-i]);
}

//SAT test ???
//ASSUME Both Polygons are clockwise and convex
//for each line in each polygon
//check if all points are on the 'right' or the 'outside' side, if is is a separating axis line
//if any points are not... is not, just continue to next line... so break and continue???
//woopsies

/* THIS CODE WORKS FOR LINE SEGMENT ONLY
template <class T>
bool polygonsCollide(const sf::Vector2<T> *const points_a, std::size_t sz_a, const sf::Vector2<T> *const points_b, std::size_t sz_b)
{
    //just test the 2 lines... for now...
    Point a = points_a[0];
    Point b = points_a[1];
    Point c = points_b[0];
    Point d = points_b[1];
    Point ab = b - a;
    Point cd = d - c;
    Point ac = c - a;
    Point ad = d - a;
    Point ca = a - c;
    Point cb = b - c;

    if( (cross(ab,ac)>0) == (cross(ab,ad)>0) ) return false;
    if( (cross(cd,ca)>0) == (cross(cd,cb)>0) ) return false;
    return true;
}
*/

//This code is slower though...
template <class T>
bool doLineSegsIntersect(const sf::Vector2<T> &a1, const sf::Vector2<T> &a2, const sf::Vector2<T> &b1, const sf::Vector2<T> &b2)
{
    sf::Vector2<T> a = a2 - a1;
    sf::Vector2<T> b = b2 - b1;

    if( (cross(a, b1 - a1) > 0) == (cross(a, b2 - a1) > 0) ) return false;
    if( (cross(b, a1 - b1) > 0) == (cross(a, a2 - b1) > 0) ) return false;
    return true;
}

template <class T>
bool polygonIntersectLine(const std::vector< sf::Vector2<T> > &points, const sf::Vector2<T> &a, const sf::Vector2<T> &b)
{
    for(uint i = 0, next = points.size()-1; i < points.size(); ++i)
    {
        if(lineSegsIntersect(points[i],points[next],a,b)) return true;
        next = i;
    }
    return false;
}

//Works for CounterClockwise polygons
template <class T>
bool polygonsCollide(const sf::Vector2<T> *const points_a, std::size_t sz_a, const sf::Vector2<T> *const points_b, std::size_t sz_b)
{
    //for each line in a... see if is a SAT line, all points on the 'outside'
    //if any are not.  is not an sat line
    
    for (uint i = 0; i < sz_a; i++)
    {
        uint next = NEXT(i, sz_a);
        sf::Vector2<T> line = points_a[next] - points_a[i];
        bool isSATLine = true;

        for (uint j = 0; j < sz_b; j++)
        {
            if (cross(line, points_b[j] - points_a[i]) > 0) //on the inside so is not an SAT Line
            {
                isSATLine = false;
                break;
            }
        }

        if (isSATLine)
            return false;
    }

    for (uint i = 0; i < sz_b; i++)
    {
        uint next = NEXT(i, sz_b);
        sf::Vector2<T> line = points_b[next] - points_b[i];
        bool isSATLine = true;

        for (uint j = 0; j < sz_a; j++)
        {
            if (cross(line, points_a[j] - points_b[i]) > 0) //on the inside so is not an SAT Line
            {
                isSATLine = false;
                break;
            }
        }

        if (isSATLine)
            return false;
    }

    return true;//do collide
}

}//SFUTIL

#endif //SF_GEOM_FUNCS_HPP