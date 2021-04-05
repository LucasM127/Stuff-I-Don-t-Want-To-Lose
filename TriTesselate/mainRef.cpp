#include "../../SFML_Functions/SFML_Functions.hpp"
#include <algorithm>
#include <cassert>
#include <iostream>
#include <list>

std::ostream &operator<<(std::ostream &o, Point &p)
{
    return o << p.x << " " << p.y;
}

typedef unsigned int EdgeId;
typedef unsigned int HullId;
typedef sf::Vector2f Vec2;

float (*cross)(const Point &, const Point &) = SFUTIL::cross<float>;
float (*dot)(const Point , const Point ) = SFUTIL::dot<float>;
typedef Point Vec2;
bool (*circumCenter)(const sf::Vector2<double> &, const sf::Vector2<double> &, const sf::Vector2<double> &, SFUTIL::Circle<double> &) = SFUTIL::getCircumcenter<double>;
//bool (*circumCenter)(const Point &, const Point &, const Point &, SFUTIL::Circle<float> &) = SFUTIL::getCircumcenter<float>;
//returns false if is infinitely big, or... in otherwords the triangle is collinear, then... obviously not delaunay...
typedef SFUTIL::Circle<float> circlef;
typedef SFUTIL::Circle<double> circled;
const float epsilon = std::numeric_limits<float>::epsilon();

inline EdgeId next(EdgeId E)
{
    return E % 3 == 2 ? E - 2 : E + 1;
}

inline EdgeId last(EdgeId E)
{
    return E % 3 == 0 ? E + 2 : E - 1;
}

inline HullId next(HullId H, unsigned int sz)
{
    ++H;
    return H >= sz ? 0 : H;
}

inline HullId last(HullId H, unsigned int sz)
{
    return H == 0 ? sz - 1 : H-1;
}

bool sortxy(const Point &a, const Point &b)
{
    if (a.x == b.x)//almostEqual???
    {
        return a.y < b.y;
    }
    return a.x < b.x;
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
        onLeft = result < -epsilon;

    return onLeft;
}

//pre allocate a size... let's worry about that later... or from 'new'
//just use the existing hulledge ????
/*struct HullEdge
{
    EdgeId mate;
    unsigned int pid;//point id
};*/

//Common Data Structures of CLASS
const unsigned int CEILING = -100;//so have space for 100
std::vector<Point> points;//input
std::vector<EdgeId> edges(100);
std::vector<EdgeId> mates(100);
unsigned int numEdges = 0;
//unsigned int hullSz = 0;
std::vector<EdgeId> hull;
//std::vector<HullEdge> hull(100);
unsigned int point_id = 0;
std::list<EdgeId> uncheckedEdges;//edges that may or may not be delaunay...
//HullEdge * hullStart = nullptr;
//

bool isDelaunay(EdgeId A)
{
    EdgeId B = mates[A];
    if(B >= CEILING) return true;//on hull
    sf::Vector2<double> t1(points[edges[A]]);
    sf::Vector2<double> t2(points[edges[next(A)]]);
    sf::Vector2<double> t3(points[edges[last(A)]]);
    sf::Vector2<double> p(points[edges[last(B)]]);

    //std::cout << "Testing edge " << t1.x << " " << t1.y << " to " << t2.x << " " << t2.y << std::endl;
    circled c;
    if (!circumCenter(t1, t2, t3, c))
        return false;
    c.radius -= c.radius * 2.f * std::numeric_limits<double>::epsilon();//this is right as crashes otherwise without the 2
    return !SFUTIL::pointInCircle<double>(c.center, c.radius, p);
}

//INIT
//when we add or remove a triangle, it affects the convex Hull... so that is this guy here...
HullId createTriangle(EdgeId a, EdgeId b, EdgeId c)
{//only called once...
    assert(numEdges % 3 == 0);
    unsigned int hullSz = hull.size();

    mates[numEdges] = CEILING + hullSz;
    edges[numEdges] = a;
    hull.push_back(numEdges);
    hullSz = hull.size();
    numEdges++;

    mates[numEdges] = CEILING + hullSz;
    edges[numEdges] = b;
    hull.push_back(numEdges);
    hullSz = hull.size();
    numEdges++;

    mates[numEdges] = CEILING + hullSz;
    edges[numEdges] = c;
    hull.push_back(numEdges);
    numEdges++;
/*
    HullEdge * H0 = &hull[0];//RAII fix...
    HullEdge * H1 = &hull[1];
    HullEdge * H2 = &hull[2];
    //EdgeId hullId = CEILING;

    H2->mate = numEdges;
    mates[numEdges] = CEILING + hullSz + 2;
    edges[numEdges++] = a;

    H1->mate = numEdges;
    mates[numEdges] = CEILING + hullSz + 1;
    edges[numEdges++] = b;

    H0->mate = numEdges;
    mates[numEdges] = CEILING + hullSz;
    edges[numEdges++] = c;
    hullSz += 3;

    H0->pid = a;
    H1->pid = c;
    H2->pid = b;
*/
    return 0;
}
typedef unsigned int PointId;
typedef unsigned int MateId;
//void printout();

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
void edgeFlip(EdgeId A)
{
//    std::cout<<"Before edge flip:"<<std::endl;
//    printout();

    EdgeId B = mates[A];
    EdgeId A_next = next(A);
    EdgeId A_last = last(A);
    EdgeId B_next = next(B);
    EdgeId B_last = last(B);

//this one i need to fix too!
    mates[A] = mates[B_next];
    mates[B] = mates[A_next];

    EdgeId A_mate = mates[A];//m2 on diagram
    EdgeId B_mate = mates[B];//m4
    if(A_mate >= CEILING)
    {
        hull[A_mate - CEILING].mate = A;
    }
    else
    {
        mates[A_mate] = A;
    }
    if(B_mate >= CEILING)
    {
        hull[B_mate - CEILING].mate = B;
    }
    else
    {
        mates[B_mate] = B;
    }
    

    edges[A_next] = edges[B_last];
    edges[B_next] = edges[A_last];
//these I need to fix! this one's ok, is the interior
    mates[A_next] = B_next;
    mates[B_next] = A_next;
//one of these could be on the hull though


//i hope this is right??? it is what I wrote.
//    std::cout<<"After edge flip:"<<std::endl;
//    printout();
    //m_uncheckedEdges.push_back(e2->next); B_next ->
    //m_uncheckedEdges.push_back(e1->last); A_last
    // or A_last and A ??? the right side? or the left side ??? figure it out
    uncheckedEdges.push_back(A_last);
    uncheckedEdges.push_back(A);
}//visualize these edges to make sure that they are right somehow???

void checkEdges()
{
    while (uncheckedEdges.size())
    {
        EdgeId E = uncheckedEdges.front();
        if(!isDelaunay(E))
        {
            edgeFlip(E);
        }
        uncheckedEdges.pop_front();
    }
}

void init()
{
    //load the first three points and ccw test and triangle make...
    std::sort(points.begin(), points.end(), sortxy);
    assert(points[0] != points[1]);

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

    point_id = 3;
}

//ASCII ART TIME!
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
//Add the triangles that form from hull edge to point storing new hull edge in a temp array
//when done, we replace the old hull segment with the new hull segment, updating mates[] as needed
void addTriangle(EdgeId P, HullId H, std::vector<HullEdge> &temp)
{
    //HullEdge &H = hull[H];
    HullEdge H1;
    HullEdge H2;//temps

    H1.pid = hull[H].pid;
    H2.pid = P;

    //The first edge of new triangle [0] is now interior, no longer linked to convex hull
    mates[numEdges] = hull[H].mate;//the triangle added to
    mates[hull[H].mate] = numEdges;
    edges[numEdges++] = H1.pid;

    //second edge [1] linked to H2 same point as the next point in the hull
    //mates[numEdges] = 0;//not sure where H2 will end up in hullvector after resizing it
    H2.mate = numEdges;
    edges[numEdges++] = hull[H+1].pid;

    //second edge [2] linked to H1
    H1.mate = numEdges;
    edges[numEdges++] = P;

//is always 2, the first and the last... we may have to mate them as they connect internally though... 
//HOW TO FIX

    if(temp.size() > 0)
    {
        //second or third triangle added, fix the link between H2 of temp and our H1
        //edge 2 mates to edge 1 of last triangle  0 1 2 0 1 2_ -> 0 1_ 2 0 1 2 //go back 4
        mates[numEdges-1] = numEdges - 5;
        mates[numEdges-5] = numEdges - 1;
        temp.back() = H2;
    }
    else
    {
        temp.push_back(H1);
        temp.push_back(H2);
    }
    
    //then when done with the point we can clean up....
    //move the remaining things up 1 so can insert the new hull into the vector!
    //fix all references as we do so too
}

//updates all mate values for all hull edges that have been shifted up or down in the vector
void fixMates(HullId start)
{
    for(HullId i = start; i < hullSz; ++i)
    {
        mates[hull[i].mate] = CEILING + i;
    }
}

// i think I have a bug here tooo
void updateHull(HullId startId, std::vector<HullEdge> &temp, int deltaHullSz)
{
    //if deltaHullSz > 0 == 1 only... move all to the right 1, 
    //and put temp in at startid , startid +1
    //if deltaHullSz == 0 only, swap temp[0] temp[1] for startId, startId+1
    //if < 0 ... move to the left by delta and override temp at startid, start id+1
    //startid + 2 ... etc... become = to what was on the right by delta
    //update hullsz
    //or use a vector
    assert(deltaHullSz < 2);
    if(deltaHullSz == 1)//move to the right 1
    {
        for(HullId i = hullSz; i > startId + 1; --i)
        {
            hull[i] = hull[i-1];
        }
        //put temp in its spot
        hullSz += 1;
    }
    else if(deltaHullSz < 0)//move to left n
    {
        std::cout<<"Hull was: ";
        for(HullId i = 0; i < hullSz; i++)
        {
            std::cout<<hull[i].pid<<" "<<hull[i].mate<<std::endl;
        }
        for(HullId i = startId + 2; i < hullSz - abs(deltaHullSz); ++i)
        {
            hull[i] = hull[i + abs(deltaHullSz)];
        }
        //put temp in its spot
        hullSz -= abs(deltaHullSz);
        
    }
    
    //put temp in its spot
    hull[startId] = temp[0];
    hull[startId+1] = temp[1];

    //fix mates
    if(deltaHullSz != 0)
        fixMates(startId);
    else
    {
        mates[hull[startId].mate] = CEILING + startId;
        mates[hull[startId+1].mate] = CEILING + startId + 1;
    }
    if(deltaHullSz < 0)
    {
        std::cout<<"Hull became: ";
        for(HullId i = 0; i < hullSz; i++)
        {
            std::cout<<hull[i].pid<<" "<<hull[i].mate<<std::endl;
        }
    }
}

bool step(){return true;}
/*
{
    //for each p point
    //for each hullEdge.
    //if on right on left ? check point on left
    //if on left add triangle
    //while on left add triangle
    //calculate delta
    //change in hull = 2 - numTrianglesAdded or change in hullIndex
    //change the hull and put in the 2 temp values...
    //each hulledge you had to shift over, update their values.
    //for each edge in added triangles, test for delauney and edge flip... recursively
    //the edge where hull used to be, and numTrisAdded-1 for in between... ??? are those ones even need testing????
    if(point_id >= points.size())
    {
        return false;
    }

    Point &curPoint = points[point_id];

    bool haveAddedTriangle = false;
    bool isFirstEdge = true;

    HullId curHullId = 0;//start at zero
    HullId firstHullEdgeTriangleId;
    //HullEdge temp[2];
    std::vector<HullEdge> temp;
    int numTrianglesAdded = 0;
    
    for(curHullId = 0; curHullId < hullSz; curHullId++)
    {
        Point &curHullEdgePoint = points[hull[curHullId].pid];
        Point &nextHullEdgePoint = points[hull[next(curHullId,hullSz)].pid];//next ????
        if(curHullEdgePoint == curPoint) break;//similar points //almost equal???
        Vec2 v_0p = curPoint -  curHullEdgePoint;
        Vec2 v_01 = nextHullEdgePoint - curHullEdgePoint;
        float result = cross(v_0p, v_01);
        bool onLeft = false;
        if (result < 2.f * epsilon && result > -2.f * epsilon)//collinear
        {
            bool pointIsAheadCurEdgeStart = dot(v_01, v_0p) > -epsilon;
            if(!pointIsAheadCurEdgeStart && !haveAddedTriangle) onLeft = true;

        }
        else onLeft = result > 0.0f;
        if(onLeft)
        {//add the triangle... simpler
            Point &lastHullEdgePoint = points[hull[last(curHullId,hullSz)].pid];
            //m_uncheckedEdges.push_back(curHullId);
            addTriangle(point_id, curHullId, temp);
            numTrianglesAdded++;
            if(!haveAddedTriangle) firstHullEdgeTriangleId = curHullId;
            haveAddedTriangle = true;
//            uncheckedEdges.push_back(hull[curHullId].mate);//if i could change it over would be nice
            continue;
        }
        if(haveAddedTriangle) break;
    }
    ++point_id;
    //change in hull = 2 - numTrianglesAdded or change in hullIndex
    //delta T = 2
    //and first hullId added need too
    int deltaHullSz = 2 - numTrianglesAdded;
    if(deltaHullSz == 2) return true;//no triangles added (duplicate point) so no need to do anything
    updateHull(firstHullEdgeTriangleId, temp, deltaHullSz);

    //edge flip fix here
    //checkEdges();

    return true;
}*/

void printout()
{
    //printout triangle and mate and edges...
    for(uint i = 0; i < numEdges; ++i)
    {
        std::cout<<"i "<<i<<" e: "<<edges[i]
        <<" m: ";//<< mates[i]<<std::endl;
        uint m = mates[i];
        m = m >= CEILING ? m - CEILING : m;
        std::cout<<((mates[i] >= CEILING) ? 'h' : ' ')<<m<<std::endl;
//        std::cout<<"T "<<i<<" e: "<<edges[3*i]<<" "<<edges[3*i+1]<<" "<<edges[3*i+2]
//            <<" m: "<<mates[3*i]<<" "<<mates[3*i+1]<<" "<<mates[3*i+2] << std::endl;
    }
    for(uint i = 0; i < hull.size(); ++i)
    {
        std::cout<<"h"<<i<<" e: "<<edges[hull[i]]<<std::endl;
    }
}
//draw function
void drawEdge(sf::VertexArray &lineShape);
void drawTriangles(sf::VertexArray &shape);

int main()
{
    
    //srand(13);//11);
    /*
    srand(time(NULL));
    for(int i = 0; i<25 ; i++)
    {
        points.push_back({(float)(rand()%100), (float)(rand()%100)});
    }
    std::cout<<"Points:"<<std::endl;
    for(auto &p : points) std::cout<<p<<std::endl;*/
    points = {
        {29, 92},
        {20, 91},
        {96, 98},
        {69, 37},
        {50, 66},
        {70, 82},
        {77, 15},
        {76, 75},
        {95, 81},
        {11, 97},
        {95, 68},
        {56, 51},
        {44, 43},
        {31, 84},
        {17, 10},
        {46, 46},
        {2 , 18},
        {89, 51},
        {68, 58},
        {40, 19},
        {77, 10},
        {53,  6},
        {25, 29},
        {81, 72},
        {62, 92}
    };
    std::sort(points.begin(),points.end(),sortxy);
    points.erase(points.begin(), points.begin() + 17);//see if this works...
    points.erase(points.begin()+2);
    for(auto &p : points)
    {
        p.x -= 75.f;
        p.x *= 4.f;
    }
    std::cout<<"Points:"<<std::endl;
    for(auto &p : points) std::cout<<p<<std::endl;
    /* Debug points
    points = {
        {59, 39},
        {52, 67},
        {15, 3 },
        {70, 40},
        {78, 31},
        {88, 42},
        {35, 75},
        {50, 10},
        {68, 51},
        {16, 91} };
        */
    /*points.push_back({10,10});
    
    points.push_back({80,20});
    points.push_back({60,60});
    points.push_back({60,90});
    points.push_back({90,90});*/
    /*points.push_back({20,50});
    points.push_back({50,20});
    points.push_back({50,80});
    points.push_back({80,50});*/

    sf::CircleShape C;
    C.setFillColor(sf::Color(255, 255, 255, 128));
    C.setOrigin(.5f, .5f);
    C.setRadius(.5f);
    std::vector<sf::CircleShape> circles;
    
    for (auto &p : points)
    {
        C.setPosition(p);
        circles.push_back(C);
    }

    init();

    //printout();

    sf::RenderWindow window(sf::VideoMode(640,640),":)");
    sf::View view;
    view.setCenter(50,50);
    view.setSize(100,-100);
    window.setView(view);

    sf::VertexArray hullShape;
    hullShape.setPrimitiveType(sf::LineStrip);
    sf::VertexArray triangleShape;
    triangleShape.setPrimitiveType(sf::Triangles);


    //while(step());

    drawEdge(hullShape);
    drawTriangles(triangleShape);

    sf::Event event;
    while (window.isOpen())
    {
        window.clear(sf::Color::Black);
        window.draw(triangleShape);
        window.draw(hullShape);
        for(auto &c : circles) window.draw(c);
        window.display();

        window.waitEvent(event);
        if(event.type == sf::Event::Closed) window.close();
        if(event.type == sf::Event::KeyPressed)
        {
            if(event.key.code == sf::Keyboard::Space)
            {
                step();
                //edgeFlip(1);

                drawEdge(hullShape);
                drawTriangles(triangleShape);
                printout();
            }
            else window.close();
        }
    }
    //printout();
    //for(uint i = 0; i<3; i++) std::cout<<points[edges[i]]<<" "<<edges[i]<<" "<<mates[i]-CEILING<<std::endl;
    return 0;
}

//cyclical!
void drawEdge(sf::VertexArray &lineShape)//convex Hull
{
    lineShape.clear();
    for(uint i = 0; i < hullSz; i++)
    {
        Point P = points[hull[i].pid];
        lineShape.append({P, sf::Color(255, 255, 255, 128)});
    }
    {
        Point P = points[hull[0].pid];
        lineShape.append({P, sf::Color(255, 255, 255, 128)});
    }
    
    lineShape[0].color = sf::Color::Green;
}

void drawTriangles(sf::VertexArray &shape)
{
    shape.clear();
    for (uint i = 0; i < edges.size()/3; ++i)
    {
        sf::Color randColor;
        randColor.r = rand() % 128 + 128;
        randColor.g = rand() % 128 + 128;
        randColor.b = rand() % 128 + 128;
        randColor.a = 32;

        shape.append({points[edges[ 3*i ]], sf::Color(0, 255, 0, 128)}); //randColor});
        shape.append({points[edges[3*i+1]], sf::Color(0, 0, 255, 128)});
        shape.append({points[edges[3*i+2]], sf::Color(255, 0, 0, 128)});
    }
}

/*
    //createTriangle(0,1,2);
    //add a point 3 let's try this
    std::vector<HullEdge> temp;
    addTriangle(3, 0, temp);
    addTriangle(3, 1, temp);
    //change in hull = 2 - numTrianglesAdded or change in hullIndex
    //delta T = 2
    
    //update the hull
    //remove 1 and insert temp[0-1]
    hull[0] = temp[0];
    //insert before the specified position
    hull[1] = temp[1];
    //fix mates (negative or null change?)
    for(HullId i = 0; i <= 1; ++i)
    {
        mates[hull[i].mate] = CEILING + i;
    }
//    hull.insert(hull.begin() + 2, temp[1]);
    //hullSz += 1;
    
    //fix!
    //fixMates(1);
*/