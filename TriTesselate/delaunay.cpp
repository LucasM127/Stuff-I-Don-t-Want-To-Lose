//TODOS:::
//FIX THAT HULL... takes too long to find, ... keep a binary sort order?  ie like with the last sweepline method, always started at top so never had to loop back...
//Other stuffs
//UNDERSTAND the isDelaunay formula better

#include "delaunay.hpp"
#include <algorithm>
#include <cassert>
//Logging...
#include <iostream>

//utilities
std::ostream &operator<<(std::ostream &o, Point &p)
{
    return o << p.x << " " << p.y;
}

float (*cross)(const Point &, const Point &) = SFUTIL::cross<float>;
float (*dot)(const Point , const Point ) = SFUTIL::dot<float>;
typedef Point Vec2;
bool (*circumCenter)(const sf::Vector2<double> &, const sf::Vector2<double> &, const sf::Vector2<double> &, SFUTIL::Circle<double> &) = SFUTIL::getCircumcenter<double>;
//returns false if is infinitely big, or... in otherwords the triangle is collinear, then... obviously not delaunay...
typedef SFUTIL::Circle<float> circlef;
typedef SFUTIL::Circle<double> circled;
const float epsilon = std::numeric_limits<float>::epsilon();

namespace Delaunay
{

inline HullId next(HullId E, unsigned int sz)
{
    return E == sz - 1 ? 0 : E + 1;
}

inline HullId last(HullId E, unsigned int sz)
{
    return E == 0 ? sz - 1 : E - 1;
}

bool sortxy(const Point &a, const Point &b)
{
    if (a.x == b.x)//almostEqual???
    {
        return a.y < b.y;
    }
    return a.x < b.x;
};

//sort by distance!
struct SortByDist
{
    SortByDist(const Point _p) : p(_p) {}
//    SortByDist(const Point _p, const std::vector<Point> &_points) : p(_p), points(_points) {}
    bool operator()(const Point &p1, const Point &p2)
//    bool operator()(const PointId p1, const PointId p2)
    {
        //Vec2 v1 = points[p1] - p;
        //Vec2 v2 = points[p2] - p;
        Vec2 v1 = p1 - p;
        Vec2 v2 = p2 - p;
        return SFUTIL::lengthSquared<float>(v1) < SFUTIL::lengthSquared<float>(v2);
    }
    const Point p;
    //const std::vector<Point> &points;
};

bool isCCW(const Point &p0, const Point &p1, const Point &p2)
{
    Vec2 V1 = p1 - p0;
    Vec2 V2 = p2 - p0;
    float result = cross(V1, V2);
    bool onLeft = false;
    if (result < epsilon && result > -epsilon) //collinear
    {
        std::cout << "COLINEAR!!!!!!!!!!!!!!!" << std::endl;
        onLeft = dot(V1, V2) < 0.0f;
    }
    else
        onLeft = result > epsilon;

    return onLeft;
}

const unsigned int INVALID_ID = -1;
const unsigned int MAX_HULL_SZ = 1000;
const unsigned int CEILING = 0 - MAX_HULL_SZ - 1;

//can I make this test better???
//calculate the DETERMINANT ... it is positive iff D lies inside circumcirlce.
//ie
//bool isDelaunay(EdgeId A){return determinant(MATRIX) > 0;}
//https://www.newcastle.edu.au/__data/assets/pdf_file/0019/22519/23_A-fast-algortithm-for-generating-constrained-Delaunay-triangulations.pdf
bool Delaunay::isDelaunay(EdgeId A)
{//return true;
    EdgeId B = m_mates[A];
    if(B >= CEILING) return true;//on hull
    sf::Vector2<float> p1(points[m_edges[A]]);
    sf::Vector2<float> p2(points[m_edges[next(A)]]);
    sf::Vector2<float> p3(points[m_edges[last(A)]]);
    sf::Vector2<float> p(points[m_edges[last(B)]]);
//is d in circumradius of triangle abc ???
    float x13 = p1.x - p3.x;
    float y13 = p1.y - p3.y;
    float x23 = p2.x - p3.x;
    float y23 = p2.y - p3.y;
    float x1p = p1.x - p.x;
    float y1p = p1.y - p.y;
    float x2p = p2.x - p.x;
    float y2p = p2.y - p.y;

    float lhs = (x13*x23 + y13*y23)*(x2p*y1p - x1p*y2p);
    float rhs = (x23*y13 - x13*y23)*(x2p*x1p + y1p*y2p);
    return rhs - lhs < epsilon;//0;//epsilon;//0;//-epsilon;
}//epsilon removes unnecessary if equidistant...
/*
bool Delaunay::isDelaunay(EdgeId A)
{
    EdgeId B = m_mates[A];
    if(B >= CEILING) return true;//on hull
    sf::Vector2<double> p1(points[m_edges[A]]);
    sf::Vector2<double> p2(points[m_edges[next(A)]]);
    sf::Vector2<double> p3(points[m_edges[last(A)]]);
    sf::Vector2<double> p(points[m_edges[last(B)]]);
//is d in circumradius of triangle abc ???
    double x13 = p1.x - p3.x;
    double y13 = p1.y - p3.y;
    double x23 = p2.x - p3.x;
    double y23 = p2.y - p3.y;
    double x1p = p1.x - p.x;
    double y1p = p1.y - p.y;
    double x2p = p2.x - p.x;
    double y2p = p2.y - p.y;

    double lhs = (x13*x23 + y13*y23)*(x2p*y1p - x1p*y2p);
    double rhs = (x23*y13 - x13*y23)*(x2p*x1p + y1p*y2p);
    return rhs - lhs < 0;//-epsilon;
}//????
*/
/*
bool Delaunay::isDelaunay(EdgeId A)
{
    EdgeId B = m_mates[A];
    if(B >= CEILING) return true;//on hull
    sf::Vector2<double> t1(points[m_edges[A]]);
    sf::Vector2<double> t2(points[m_edges[next(A)]]);
    sf::Vector2<double> t3(points[m_edges[last(A)]]);
    sf::Vector2<double> p(points[m_edges[last(B)]]);

    //std::cout << "Testing edge " << t1.x << " " << t1.y << " to " << t2.x << " " << t2.y << std::endl;
    circled c;
    if (!circumCenter(t1, t2, t3, c))
        return false;
    c.radius -= c.radius * 2.f * std::numeric_limits<double>::epsilon();//this is right as crashes otherwise without the 2
    return !SFUTIL::pointInCircle<double>(c.center, c.radius, p);
}*/

/*from website...

function circumcenter(a, b, c) {
    const ad = a[0] * a[0] + a[1] * a[1];
    const bd = b[0] * b[0] + b[1] * b[1];
    const cd = c[0] * c[0] + c[1] * c[1];
    const D = 2 * (a[0] * (b[1] - c[1]) + b[0] * (c[1] - a[1]) + c[0] * (a[1] - b[1]));
    return [
        1 / D * (ad * (b[1] - c[1]) + bd * (c[1] - a[1]) + cd * (a[1] - b[1])),
        1 / D * (ad * (c[0] - b[0]) + bd * (a[0] - c[0]) + cd * (b[0] - a[0])),
    ];
}*/

//inline?
inline void Delaunay::fixMate(MateId M, EdgeId E)
{
    if(M >= CEILING)
    {
        m_hull[M - CEILING] = E;
    }
    else
    {
        m_mates[M] = E;
    }
}

/*****************************
 *       b                     b
 *       /\                   /|\                   Edge Position in array
 *      /  \                 / | \             Before          =>         After
 *   m4/An  \m3           m4/  |  \m3    Edges  Point  Mates       Edges  Point  Mates 
 *    /    Al\             /B  | Al\    |A_last|  b  |  m3  |     |A_last|  b  |  m3  | <unchanged
 *   /   A    \           /    |    \   |  A   |  c  |  B   |     |  A   |  c  |  m2  |
 *  a----------c   ===>  a   Bn|An   c  |A_next|  a  |  m4  |     |A_next|  d  |  Bn  |
 *   \   B    /           \    |    /                                      
 *    \Bl    /             \Bl |  A/    |B_last|  d  |  m1  |     |B_last|  d  |  m1  | <unchanged
 *   m1\  Bn/m2           m1\  |  /m2   |  B   |  a  |  A   |     |  B   |  a  |  m4  |
 *      \  /                 \ | /      |B_next|  c  |  m2  |     |B_next|  b  |  An  |
 *       \/                   \|/       
 *       d                     d
 **************************/
void Delaunay::edgeFlip(EdgeId A)
{
    EdgeId B = m_mates[A];
    EdgeId A_next = next(A);
    EdgeId A_last = last(A);
    EdgeId B_next = next(B);
    EdgeId B_last = last(B);

    m_mates[A] = m_mates[B_next];
    m_mates[B] = m_mates[A_next];

    EdgeId A_mate = m_mates[A];//m2 on diagram
    EdgeId B_mate = m_mates[B];//m4

    fixMate(A_mate, A);
    fixMate(B_mate, B);
    
    m_edges[A_next] = m_edges[B_last];
    m_edges[B_next] = m_edges[A_last];

    m_mates[A_next] = B_next;
    m_mates[B_next] = A_next;
//one of these could be on the hull though

    //m_uncheckedEdges.push_back(A_last);
    m_uncheckedEdges.push_back(A);
    m_uncheckedEdges.push_back(B_last);
    //m_uncheckedEdges.push_back(B);
    edgeFlipCtr++;
}

//faster with a stack????
void Delaunay::checkEdges()
{
    while (m_uncheckedEdges.size())
    {
        EdgeId E = m_uncheckedEdges.front();
        if(!isDelaunay(E))
        {
            edgeFlip(E);
        }
        m_uncheckedEdges.pop_front();
    }
}

void Delaunay::createTriangle(EdgeId a, EdgeId b, EdgeId c)
{
    m_hull.resize(3);
    m_edges.resize(3);
    m_mates.resize(3);

    m_edges[0] = a;
    m_edges[1] = b;
    m_edges[2] = c;

    m_hull[0] = 0;
    m_hull[1] = 1;
    m_hull[2] = 2;

    m_mates[0] = CEILING + 0;
    m_mates[1] = CEILING + 1;
    m_mates[2] = CEILING + 2;
}

void Delaunay::init2()
{
    //try from center...
    float min_x = std::numeric_limits<float>::max();
    float min_y = min_x;
    float max_x = std::numeric_limits<float>::min();
    float max_y = max_x;
    for(auto &p : points)
    {
        if(min_x > p.x) min_x = p.x;
        if(max_x < p.x) max_x = p.x;
        if(min_y > p.y) min_y = p.y;
        if(max_y < p.y) max_y = p.y;
    }//aabb
    Point midPt((min_x + max_x)/2.f, (min_y + max_y)/2.f);
    SortByDist sorter(midPt);
    //SortByDist sorter(midPt, points);
//    std::sort(m_indices.begin(),m_indices.end(), sorter);
    std::sort(points.begin(), points.end(), sorter);
std::cout<<"Midpt "<<midPt<<std::endl;
    if (isCCW(points[0], points[1], points[2]))
//    if (isCCW(points[m_indices[0]], points[m_indices[1]], points[m_indices[2]]))
    {
        std::cout << "ccw" << std::endl;
        //is good the way it is
        createTriangle(0,1,2);
    }
    else
    {
        std::cout << "not ccw" << std::endl;
        createTriangle(0,2,1);
    }

    m_nextPt = 3;
    m_nextIndice = 3;
}

void Delaunay::init()
{
    //std::sort(points.begin(), points.end(), sortxy);

    if (isCCW(points[0], points[1], points[2]))
    {
        std::cout << "ccw" << std::endl;
        //is good the way it is
        createTriangle(0,1,2);
    }
    else
    {
        std::cout << "not ccw" << std::endl;
        createTriangle(0,2,1);
    }

    m_nextPt = 3;
}

/****************
 *   ___H1____P
 *  \    2   /
 *   \0    1/
 *    \H   /H2
 * Tri \  /
 *      \/
 * numbers -> corresponding edgeID vals of new triangle (ccw)
 * H -> old hull edge, gets destroyed after new point fully processed
 * H1, H2 -> possibly in the new hull edge, dependent on how many triangles the point creates
 *************/
/*******************
 *    \Hn
 *     \  ____e2____
 *      ||        _/
 *      ||      _/
 *     H||e0  _/
 *      ||  _/e1
 *      ||_/
 *     /
 *    /Hl
 *****************/
//pass it the edge it mates to
void Delaunay::addTriangle(PointId P, HullId H, bool isFirst)
{
    //setup
    PointId e0 = m_edges[m_hull[next(H,m_hull.size())]];//next(hull[H]);
    PointId e1 = m_edges[m_hull[H]];
    PointId e2 = P;
    EdgeId m0 = m_hull[H];//wrong
    EdgeId m1 = isFirst ? INVALID_ID : m_edges.size() - 1;//address of e2 of last triangle added
    EdgeId m2 = INVALID_ID;

    //add 
    m_mates[m_hull[H]] = m_edges.size();
    m_mates.push_back(m0);
    m_edges.push_back(e0);

    if(!isFirst)
        m_mates[m1] = m_edges.size();
    m_mates.push_back(m1);
    m_edges.push_back(e1);

    m_edges.push_back(e2);
    m_mates.push_back(m2);//top edge
}

void Delaunay::updateHull2(HullId start, HullId end)//inclusive
{
    auto fixHull = [this](EdgeId start)
    {
        for(EdgeId i = start; i < m_hull.size(); ++i)
        {
            m_mates[m_hull[i]] = CEILING + i;
        }
    };

    if(end >= start)
    {
        updateHull(start, end - start + 1);
        return;
    }
    //std::cout<<start<<" "<<end<<std::endl;
    //std::cout<<"Hull ids: "; for(uint i = 0; i<m_hull.size(); i++) std::cout<<m_hull[i]<<" ";
    //std::cout<<std::endl;
    unsigned int numTrianglesAdded = m_hull.size() - start + end + 1;
    EdgeId hullBottom = m_edges.size() - ((numTrianglesAdded-1) * 3) - 2;
    EdgeId hullTop = m_edges.size() - 1;
    if(numTrianglesAdded == 1)
    {
        m_hull[start] = hullBottom;
        m_hull[end] = hullTop;
        return;
    }
    m_hull.erase(m_hull.begin() + start, m_hull.end());

    m_hull.erase(m_hull.begin(), m_hull.begin() + end + 1);
    m_hull.push_back(hullBottom);
    m_hull.push_back(hullTop);
    fixHull(0);
    //std::cout<<"Hull ids: "; for(uint i = 0; i<m_hull.size(); i++) std::cout<<m_hull[i]<<" ";
    //std::cout<<std::endl;
}

void Delaunay::updateHull(HullId start, unsigned int numTrianglesAdded)
{
    assert(numTrianglesAdded != 0);

    auto fixHull = [this](EdgeId start)
    {
        for(EdgeId i = start; i < m_hull.size(); ++i)
        {
            m_mates[m_hull[i]] = CEILING + i;
        }
    };
    //std::cout<<"Hull ids: "; for(uint i = 0; i<m_hull.size(); i++) std::cout<<m_hull[i]<<" ";
    //std::cout<<std::endl;

    //create the 2 new hulledges for insertion
    EdgeId hullBottom = m_edges.size() - ((numTrianglesAdded-1) * 3) - 2;
    EdgeId hullTop = m_edges.size() - 1;

    //resize vector if needed...
    if(numTrianglesAdded == 1)
    {
        m_hull[start] = hullTop;
        m_hull.insert(m_hull.begin() + start, hullBottom);
        fixHull(start);
        //ctrHullGrows++;
    }
    else if(numTrianglesAdded == 2)
    {//must be here i never bothered to check :(
        //should I fix it first?
        m_hull[start] = hullBottom;
        m_hull[start+1] = hullTop;
        m_mates[m_hull[start]] = CEILING + start;
        m_mates[m_hull[start+1]] = CEILING + start + 1;
        //ctrHullSame++;
    }
    else //numTrianglesAdded>2
    {
        //erase...
        m_hull[start] = hullBottom;
        m_hull[start+1] = hullTop;
        unsigned int delta = numTrianglesAdded - 2;
        //erase doesn't include the last iterator position!!!
        m_hull.erase(m_hull.begin() + start + 2, m_hull.begin() + start + 2 + delta);
        fixHull(start);
        //ctrHullShrinks++;
    }
    //std::cout<<"Hull ids: "; for(uint i = 0; i<m_hull.size(); i++) std::cout<<m_hull[i]<<" ";
    //std::cout<<std::endl;
}

void Delaunay::growHull2()
{
    while(m_nextPt < points.size())
    //while(m_nextIndice < m_indices.size())
    {
        //const Point &curPoint = points[m_indices[m_nextIndice]];
        const Point &curPoint = points[m_nextPt];

        bool haveAddedTriangle = false;
        HullId startId;
        HullId endId;
        HullId curId = 0;
        unsigned int ctrLeft = 0;
        unsigned int ctrRight = 0;
        bool isDuplicatePoint = false;

        auto isLeftOf = [&](HullId hullId) -> bool
        {
            const Point &curHullEdgePoint = points[m_edges[m_hull[hullId]]];
            const Point &nextHullEdgePoint = points[m_edges[m_hull[next(hullId,m_hull.size())]]];
            if(curHullEdgePoint == curPoint)
            {
                isDuplicatePoint = true;
                return false;
            }
            Vec2 v_0p = curPoint -  curHullEdgePoint;
            Vec2 v_01 = nextHullEdgePoint - curHullEdgePoint;
            return cross(v_0p, v_01) > 0.0f;
        };

        if (isLeftOf(curId))//start == cur
        {
            //std::cout<<"In"<<std::endl;
            //ctrRight++;
            HullId lastId = last(curId,m_hull.size());
            while (isLeftOf(lastId))
            {
                lastId = last(lastId,m_hull.size());
                ctrLeft++;
            }
            HullId nextId = next(curId,m_hull.size());
            while (isLeftOf(nextId))
            {
                nextId = next(nextId,m_hull.size());
                ctrRight++;
            }
            startId = ctrLeft == 0 ? curId : m_hull.size() - ctrLeft;
            endId = ctrRight;
        }//counter to left ++
        else
        {
            //std::cout<<"After"<<std::endl;
            HullId nextId = next(curId,m_hull.size());
            while (!isLeftOf(nextId))
            {
                nextId = next(nextId,m_hull.size());
                ctrLeft++;
            }
            while (isLeftOf(nextId))
            {
                nextId = next(nextId,m_hull.size());
                ctrRight++;
            }
            startId = ctrLeft + 1;
            endId = ctrLeft + ctrRight;
        }
        //std::cout<<"Start Id: "<<startId<<" end id: "<<endId<<std::endl;
//if(isDuplicatePoint) {m_nextPt++; continue;}
        for(curId = startId; curId != next(endId,m_hull.size()); curId = next(curId,m_hull.size()))
        {
            addTriangle(m_nextPt, curId, !haveAddedTriangle);
            if(!haveAddedTriangle) haveAddedTriangle = true;
            m_uncheckedEdges.push_back(m_edges.size()-3);
        }
        
//        ++m_nextIndice;
        ++m_nextPt;

        updateHull2(startId, endId);

        checkEdges();

        if(lastHullSz == m_hull.size()) timesSame++;
        else if(lastHullSz > m_hull.size()) timesShrunk++;
        else timesGrew++;
        lastHullSz = m_hull.size();
        if(m_hull.size() > maxHullSz) maxHullSz = m_hull.size();
        totalTimes++;
    }
}

void Delaunay::growHull()
{
    while (m_nextPt < points.size())
    {
        const Point &curPoint = points[m_nextPt];

        bool haveAddedTriangle = false;

        EdgeId firstHullEdgeTriangleId;
        unsigned int numTrianglesAdded = 0;
        
        for(HullId curHullId = 0; curHullId < m_hull.size(); ++curHullId)
        {
            const Point &curHullEdgePoint = points[m_edges[m_hull[curHullId]]];
            const Point &nextHullEdgePoint = points[m_edges[m_hull[next(curHullId,m_hull.size())]]];
            if(curHullEdgePoint == curPoint) break;//almost equal???

            Vec2 v_0p = curPoint -  curHullEdgePoint;
            Vec2 v_01 = nextHullEdgePoint - curHullEdgePoint;
            float result = cross(v_0p, v_01);
            bool onLeft = false;
            if (result < 2.f * epsilon && result > -2.f * epsilon)//colinear//correct epsilon values ????
            {
                bool pointIsAheadCurEdgeStart = dot(v_01, v_0p) > -epsilon;
                if(!pointIsAheadCurEdgeStart && !haveAddedTriangle) onLeft = true;
            }
            else onLeft = result > 0.0f;
            if(onLeft)
            {   
                addTriangle(m_nextPt, curHullId, !haveAddedTriangle);
                ++numTrianglesAdded;
                if(!haveAddedTriangle) firstHullEdgeTriangleId = curHullId;
                haveAddedTriangle = true;
                //I CANNOT PUSH BACK THE hull[curHullId] but if i push back its mate, I only have to check 2 additional edges in edge flip algorithm
                //instead of all 4 edges for a massive speed boost.  IF ONLY I KNEW WHY !!!! :( grrrr
                m_uncheckedEdges.push_back(m_edges.size()-3);//the 'mate' of hull[curHullId]
                continue;
            }
            if(haveAddedTriangle) break;
        }
        ++m_nextPt;
        if(numTrianglesAdded==0) throw;
        
        updateHull(firstHullEdgeTriangleId, numTrianglesAdded);
        //would need a start index and an end index instead
        
        checkEdges();

        if(lastHullSz == m_hull.size()) timesSame++;
        else if(lastHullSz > m_hull.size()) timesShrunk++;
        else timesGrew++;
        lastHullSz = m_hull.size();
        if(m_hull.size() > maxHullSz) maxHullSz = m_hull.size();
        totalTimes++;
    }
}

Delaunay::Delaunay(std::vector<Point> &_points) : points(_points)
{
    m_edges.reserve(points.size()*3);
    m_mates.reserve(points.size()*3);
    //m_indices.resize(points.size());
    //for(PointId i = 0; i<m_indices.size(); ++i) m_indices[i] = i;

    edgeFlipCtr = 0;
    maxHullSz = 3;
    lastHullSz = 3;
    timesSame = 0;
    timesGrew = timesShrunk = 0;
    totalTimes = 0;
    if(points.size() < 3) throw(std::runtime_error("Delaunay needs at least 3 points to triangulate."));
    init2();
    growHull2();

    std::cout<<edgeFlipCtr<<" edgeflips performed"<<std::endl;
    std::cout<<maxHullSz<<" max Hull Size"<<std::endl;
    std::cout<<"Hull stayed the same/grew/shrunk "<<timesSame<<" "<<timesGrew<<" "<<timesShrunk<<" out of "<<totalTimes<<std::endl;
    std::cout<<m_edges.size()<<std::endl;
}

}//namespace Delaunay

///////////////////////////////LEGACIES//////////////////
//This version keeps the positions of edges constant...
/*****************************
 *       b                     b
 *       /\                   /|\                   Edge Position in array
 *      /  \                 / | \             Before          =>         After
 *   m4/An  \m3           m4/  |  \m3    Edges  Point  Mates       Edges  Point  Mates 
 *    /    Al\             /Bl | An\    |A_last|  b  |  m3  |     |A_last|  c  |  m2  |
 *   /   A    \           /    |    \   |  A   |  c  |  B   |     |  A   |  d  |  B   |
 *  a----------c   ===>  a    B|A    c  |A_next|  a  |  m4  |     |A_next|  b  |  m3  |
 *   \   B    /           \    |    /                                      
 *    \Bl    /             \Bn | Al/    |B_last|  d  |  m1  |     |B_last|  a  |  m4  |
 *   m1\  Bn/m2           m1\  |  /m2   |  B   |  a  |  A   |     |  B   |  b  |  A   |
 *      \  /                 \ | /      |B_next|  c  |  m2  |     |B_next|  d  |  m1  |
 *       \/                   \|/       
 *       d                     d
 **************************/
/*
void edgeFlip2(EdgeId A)
{
    EdgeId B = mates[A];
    EdgeId A_next = next(A);
    EdgeId A_last = last(A);
    EdgeId B_next = next(B);
    EdgeId B_last = last(B);

    //change the mates
    {
    MateId temp = mates[A_last];
    mates[A_last] = mates[B_next];
    mates[B_next] = mates[B_last];
    mates[B_last] = mates[A_next];
    mates[A_next] = temp;
    }
    //change their partners too...
    fixMate(mates[A_last],A_last);
    fixMate(mates[A_next],A_next);
    fixMate(mates[B_last],B_last);
    fixMate(mates[B_next],B_next);

    //change the points
    {
    PointId temp = edges[A_last];
    edges[A_last] = edges[A];
    edges[A] = edges[B_last];
    edges[B_last] = edges[B];
    edges[B] = temp;
    edges[A_next] = temp;
    edges[B_next] = edges[A];
    }

    uncheckedEdges.push_back(B_next);
    uncheckedEdges.push_back(A_last);
    uncheckedEdges.push_back(A_next);
    uncheckedEdges.push_back(B_last);
}*/