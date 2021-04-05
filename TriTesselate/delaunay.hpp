#ifndef DELAUNAY_HPP
#define DELAUNAY_HPP

//STEPS:
//1: Sort the points.  And initialize the convex hull. //do we need that first triangle?
//      - Points are sorted so that as added to hull, closest point is added next, so all remaining points are outside hull
//2: Connect points to 'visible' edges on the convex hull (cross product).  Generates 1 + triangles
//3: Update the hull.  Test edges for delaunay condition... circumcircle.  (convex hull when transposed on a paraboloid)
//4: Repeat till done

#include "../../SFML_Functions/SFML_Functions.hpp"
#include <vector>
#include <list>

namespace Delaunay
{

typedef unsigned int EdgeId;
typedef unsigned int MateId;
typedef unsigned int HullId;
typedef unsigned int PointId;

inline EdgeId next(EdgeId E)
{
    return E % 3 == 2 ? E - 2 : E + 1;
}

inline EdgeId last(EdgeId E)
{
    return E % 3 == 0 ? E + 2 : E - 1;
}

class Delaunay
{
public:
    Delaunay(std::vector<Point> &_points);
    const std::vector<PointId> &getTriangleIndices(){return m_edges;}
    const std::vector<EdgeId>  &getTriangleMates()  {return m_mates;}
    const std::vector<EdgeId>  &getHull()  {return m_hull;}
    void growHull();
    void growHull2();
private:
    std::vector<Point> &points;
    std::vector<PointId> m_indices;
    std::vector<PointId> m_edges;
    std::vector<EdgeId> m_mates;
    std::vector<EdgeId> m_hull;
    
    PointId m_nextPt;
    PointId m_nextIndice;
    std::list<EdgeId> m_uncheckedEdges;

    unsigned int edgeFlipCtr;
    unsigned int maxHullSz;
    unsigned int timesSame;
    unsigned int timesGrew;
    unsigned int timesShrunk;
    unsigned int lastHullSz;
    unsigned int totalTimes;

    bool isDelaunay(EdgeId A);
    inline void fixMate(MateId M, EdgeId E);
    void edgeFlip(EdgeId A);
    void checkEdges();
    void createTriangle(EdgeId a, EdgeId b, EdgeId c);
    void init();
    void init2();
    
    void addTriangle(PointId P, HullId H, bool isFirstTriangleAdded);
    void updateHull(HullId start, unsigned int numTrianglesAdded);
    void updateHull2(HullId start, HullId end);
    
};

}//namespace Delaunay

#endif