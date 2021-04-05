//fix the hull is acting up :(
#include "../../SFML_Functions/SFML_Functions.hpp"
#include <algorithm>
#include <cassert>
#include <iostream>
#include <list>
#include <random>

int ctrHullSame = 0;
int ctrHullGrows = 0;
int ctrHullShrinks = 0;

std::ostream &operator<<(std::ostream &o, Point &p)
{
    return o << p.x << " " << p.y;
}

typedef unsigned int EdgeId;
typedef unsigned int MateId;
typedef unsigned int HullId;
typedef unsigned int PointId;

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

inline EdgeId next(EdgeId E, unsigned int sz)
{
    return E == sz - 1 ? 0 : E + 1;
}

inline EdgeId last(EdgeId E, unsigned int sz)
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

//Is this reallly ccw ???
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

//Common Data -> class eventually
const unsigned int INVALID_ID = -1;
const unsigned int MAX_HULL_SZ = 1000;
const unsigned int CEILING = 0 - MAX_HULL_SZ - 1;
std::vector<Point> points;
std::vector<EdgeId> edges;
std::vector<EdgeId> mates;
std::vector<EdgeId> hull;

unsigned int point_id = 0;
std::list<EdgeId> uncheckedEdges;

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

void fixMate(MateId M, EdgeId E)
{
    if(M >= CEILING)
    {
        hull[M - CEILING] = E;
    }
    else
    {
        mates[M] = E;
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
void edgeFlip(EdgeId A)
{//which one is the new point ? d m3 m4   B and Al
//    std::cout<<"Before edge flip:"<<std::endl;
//    printout();

    EdgeId B = mates[A];
    EdgeId A_next = next(A);
    EdgeId A_last = last(A);
    EdgeId B_next = next(B);
    EdgeId B_last = last(B);

    mates[A] = mates[B_next];
    mates[B] = mates[A_next];

    EdgeId A_mate = mates[A];//m2 on diagram
    EdgeId B_mate = mates[B];//m4

    fixMate(A_mate, A);
    fixMate(B_mate, B);
    

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
    //uncheckedEdges.push_back(A_last);
    uncheckedEdges.push_back(A);
    uncheckedEdges.push_back(B_last);
    //uncheckedEdges.push_back(B);
}//visualize these edges to make sure that they are right somehow???
//BUT WHY!!!!!!
//I HAD TO MAKE SURE THE EDGE LOADED WAS IN THE NEW TRIANGLE ADDED, not the curHull Edge
//

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
}

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

//called once at beginning
void createTriangle(EdgeId a, EdgeId b, EdgeId c)
{
    hull.resize(3);
    edges.resize(3);
    mates.resize(3);

    edges[0] = a;
    edges[1] = b;
    edges[2] = c;

    hull[0] = 0;
    hull[1] = 1;
    hull[2] = 2;

    mates[0] = CEILING + 0;
    mates[1] = CEILING + 1;
    mates[2] = CEILING + 2;
}

void init()
{
    std::sort(points.begin(), points.end(), sortxy);

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
void addTriangle(PointId P, HullId H, bool isFirst)
{
    //setup
    EdgeId e0 = edges[hull[next(H,hull.size())]];//next(hull[H]);
    EdgeId e1 = edges[hull[H]];
    EdgeId e2 = P;
    MateId m0 = hull[H];//wrong
    MateId m1 = isFirst ? INVALID_ID : edges.size() - 1;//address of e2 of last triangle added
    MateId m2 = INVALID_ID;

    //add 
    mates[hull[H]] = edges.size();
    mates.push_back(m0);
    edges.push_back(e0);

    if(!isFirst)
        mates[m1] = edges.size();
    mates.push_back(m1);
    edges.push_back(e1);

    edges.push_back(e2);
    mates.push_back(m2);//top edge
}

void fixHull(EdgeId start)
{
    for(EdgeId i = start; i < hull.size(); ++i)
    {
        mates[hull[i]] = CEILING + i;
    }
}

//start -> inclusive

//FIX ME

void updateHull(HullId start, unsigned int numTrianglesAdded)
{
    assert(numTrianglesAdded != 0);

    //create the 2 new hulledges for insertion
    HullId hullBottom = edges.size() - ((numTrianglesAdded-1) * 3) - 2;
    HullId hullTop = edges.size() - 1;

    //resize vector if needed...
    if(numTrianglesAdded == 1)
    {
        hull[start] = hullTop;
        hull.insert(hull.begin() + start, hullBottom);
        fixHull(start);
        ctrHullGrows++;
    }
    else if(numTrianglesAdded == 2)
    {//must be here i never bothered to check :(
        //should I fix it first?
        hull[start] = hullBottom;
        hull[start+1] = hullTop;
        mates[hull[start]] = CEILING + start;
        mates[hull[start+1]] = CEILING + start + 1;
        ctrHullSame++;
    }
    else //numTrianglesAdded>2
    {
        //erase...
        hull[start] = hullBottom;
        hull[start+1] = hullTop;
        unsigned int delta = numTrianglesAdded - 2;
        //erase doesn't include the last iterator position!!!
        hull.erase(hull.begin() + start + 2, hull.begin() + start + 2 + delta);
        fixHull(start);
        ctrHullShrinks++;
    }
}

bool step()
{

    if(point_id >= points.size())
    {
        return false;
    }

    Point &curPoint = points[point_id];

    bool haveAddedTriangle = false;
    bool isFirstEdge = true;

    HullId curHullId = 0;//start at zero
    HullId firstHullEdgeTriangleId;
    unsigned int numTrianglesAdded = 0;
    
    for(curHullId = 0; curHullId < hull.size(); curHullId++)
    {
        Point &curHullEdgePoint = points[edges[hull[curHullId]]];
        Point &nextHullEdgePoint = points[edges[hull[next(curHullId,hull.size())]]];
        if(curHullEdgePoint == curPoint) break;//similar points //almost equal???
        Vec2 v_0p = curPoint -  curHullEdgePoint;
        Vec2 v_01 = nextHullEdgePoint - curHullEdgePoint;
        float result = cross(v_0p, v_01);
        bool onLeft = false;
        if (result < 2.f * epsilon && result > -2.f * epsilon)//colinear
        {
            bool pointIsAheadCurEdgeStart = dot(v_01, v_0p) > -epsilon;
            if(!pointIsAheadCurEdgeStart && !haveAddedTriangle) onLeft = true;

        }
        else onLeft = result > 0.0f;
        if(onLeft)
        {//add the triangle... simpler
            //m_uncheckedEdges.push_back(curHullId);
            //before adding it is -1
            
            addTriangle(point_id, curHullId, !haveAddedTriangle);
            numTrianglesAdded++;
            if(!haveAddedTriangle) firstHullEdgeTriangleId = curHullId;
            //if(haveAddedTriangle) uncheckedEdges.push_back(edges.size()-2);//m_uncheckedEdges.push_back(curHullEdge->last);... the edge we added
            //assert(hull[curHullId] == (edges.size() - 3));
            haveAddedTriangle = true;
            //I CANNOT PUSH BACK THE hull[curHullId] but if i push back its mate, I only have to check 2 additional edges in edge flip algorithm
            //instead of all 4 edges for a massive speed boost.  IF ONLY I KNEW WHY !!!! :( grrrr
            uncheckedEdges.push_back(edges.size()-3);//the 'mate' of hull[curHullId]
//            uncheckedEdges.push_back(hull[curHullId]);//holds the 'address'
//            uncheckedEdges.push_back(hull[curHullId].mate);//if i could change it over would be nice
            continue;
        }
        if(haveAddedTriangle) break;
    }
    ++point_id;
    if(numTrianglesAdded==0) return true;
    //change in hull = 2 - numTrianglesAdded or change in hullIndex
    //delta T = 2
    //and first hullId added need too
    updateHull(firstHullEdgeTriangleId, numTrianglesAdded);
    
    //edge flip fix here
    //make separate step???
    checkEdges();

    return true;
}

void drawEdge(sf::VertexArray &lineShape);
void drawTriangles(sf::VertexArray &shape);
void drawCircumcircles(std::vector<sf::CircleShape> &circles);
void checkOutput();
void printout();

//up to 100 size hull
int main()
{
    std::default_random_engine generator;
    std::uniform_real_distribution<float> distribution(0.0,100.0f);
    //float number = distribution(generator);
    //srand(time(NULL));
    for(int i = 0; i<10000 ; i++)
    {
        Point P(distribution(generator),distribution(generator));
//        Point P((float)(rand()%100000)/1000.f, (float)(rand()%100000)/1000.f);
        //bool collided = false;
        //for(auto &p : points) if(P == p) collided = true;
        //if(collided) {continue;}
        points.push_back(P);
    }
    /*
    points = {
        {40, 50},
        {50, 20},
        {50, 90},
        {50, 10},
        {60, 50}
    };*/

    sf::CircleShape C;
    C.setFillColor(sf::Color(255, 255, 255, 128));
    C.setOrigin(.5f, .5f);
    C.setRadius(.5f);
    std::vector<sf::CircleShape> circles;
    std::vector<sf::CircleShape> circumCircles;
    
    for (auto &p : points)
    {
        C.setPosition(p);
        circles.push_back(C);
    }
//pointId, hullId, isFirst?
    
    auto start = std::chrono::high_resolution_clock::now();
    init();
    while(step());
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    std::cout << duration.count() << " microseconds" << std::endl; 
    /*
    addTriangle(3,2,true);    
    updateHull(2,1);//not expected behaviour
    addTriangle(4,3,true);
    updateHull(3,1);
    //so far so good... now try and add 3 triangles and update hull at end
    addTriangle(5,1,true);
    addTriangle(5,2,false);
    addTriangle(5,3,false);
    updateHull(1,3);//start index 1 of hull*/
//    updateHull(start,p,numtriadded)
    //updateHull(3,4,1);//hull
/*
    
*/
    //printout();
    sf::RenderWindow window(sf::VideoMode(640,640),":)");
    sf::View view;
    view.setCenter(50,50);
    view.setSize(100,-100);
    window.setView(view);
    CameraController camera(window,view);

    sf::VertexArray hullShape;
    hullShape.setPrimitiveType(sf::LineStrip);
    sf::VertexArray triangleShape;
    triangleShape.setPrimitiveType(sf::Triangles);

    drawEdge(hullShape);
    drawTriangles(triangleShape);
    //drawCircumcircles(circumCircles);

    sf::Event event;
    while (window.isOpen())
    {
        window.clear(sf::Color::Black);
        window.draw(triangleShape);
        window.draw(hullShape);
        //for(auto &c : circles) window.draw(c);
        //for(auto &c : circumCircles) window.draw(c);
        window.display();

        window.waitEvent(event);
        camera.handleEvent(event);
        if(event.type == sf::Event::Closed) window.close();
        if(event.type == sf::Event::KeyPressed)
        {
            if(event.key.code == sf::Keyboard::Space)
            {
                step();
                //edgeFlip(1);

                drawEdge(hullShape);
                drawTriangles(triangleShape);
                //drawCircumcircles(circumCircles);
                //printout();*/
            }
            else window.close();
        }
    }
std::cout<<hull.size()<<std::endl;
    //checkOutput();
    //need to check if is delauney...
    //for each circle, test all other points and see if intersect...
    std::cout<<"The hull grew/shrunk/stayed same "<<ctrHullGrows<<"/"<<ctrHullShrinks<<"/"<<ctrHullSame<<std::endl;
    return 0;
}

void drawEdge(sf::VertexArray &lineShape)//convex Hull
{
    lineShape.clear();
    for(uint i = 0; i < hull.size(); i++)
    {
        Point P = points[edges[hull[i]]];
        lineShape.append({P, sf::Color(255, 255, 255, 128)});
    }
    {
        Point P = points[edges[hull[0]]];
        lineShape.append({P, sf::Color(255, 255, 255, 128)});
    }
    
    lineShape[0].color = sf::Color::Green;
}
//try this with the delaunator triangles.. see if is radial
void drawTriangles(sf::VertexArray &shape)
{
    shape.clear();
    int cl = 0;
    sf::Color color(cl,cl,cl);
    for (uint i = 0; i < edges.size()/3; ++i)
    {
        
        //every tenth?
        
        //cl = cl > 255 ? 0 : cl + 1;
        sf::Color randColor;
        randColor.r = rand() % 128 + 128;
        randColor.g = rand() % 128 + 128;
        randColor.b = rand() % 128 + 128;
        //randColor.a = 32;
        if(i % 500 == 0) color = randColor;
        
        shape.append({points[edges[ 3*i ]], sf::Color(0, 255, 0, 128)}); //randColor});
        shape.append({points[edges[3*i+1]], sf::Color(0, 0, 255, 128)});
        shape.append({points[edges[3*i+2]], sf::Color(255, 0, 0, 128)});
    }
}

void drawCircumcircles(std::vector<sf::CircleShape> &circles)
{
    sf::CircleShape C;
    C.setFillColor(sf::Color::Transparent);
    C.setOutlineColor(sf::Color(128, 128, 128, 128));
    C.setOutlineThickness(0.2f);//0.5f);
    circles.clear();
    for (uint i = 0; i < edges.size()/3; ++i)
    {//the three points of the triangle
        const Point &a = points[edges[3*i+0]];
        const Point &b = points[edges[3*i+1]];
        const Point &c = points[edges[3*i+2]];
        
        SFUTIL::Circle<float> cc;
        SFUTIL::getCircumcenter<float>(a,b,c, cc);
        C.setRadius(cc.radius);
        C.setOrigin(cc.radius, cc.radius);
        C.setPosition(cc.center);
        circles.push_back(C);
    }
}

void printout()
{
    //printout triangle and mate and edges...
    for(uint i = 0; i < edges.size(); ++i)
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

void checkOutput()
{
    //for each triangle -> create circumcircle
    //for each circumcircle, test if contains ANY other point.

    //THAT's all
    for (uint i = 0; i < edges.size()/3; ++i)
    {//the three points of the triangle
        const sf::Vector2<double> a(points[edges[3*i+0]].x,points[edges[3*i+0]].y);
        const sf::Vector2<double> b(points[edges[3*i+1]].x,points[edges[3*i+1]].y);
        const sf::Vector2<double> c(points[edges[3*i+2]].x,points[edges[3*i+2]].y);
        
        SFUTIL::Circle<double> cc;
        SFUTIL::getCircumcenter<double>(a,b,c, cc);

        for(uint j = 0; j < points.size(); j++)
        {
            //if equal continue
            if(j == edges[3*i+0] || j == edges[3*i+1] || j == edges[3*i+2]) continue;
            sf::Vector2<double> p(points[j]);
            if(SFUTIL::pointInCircle<double>(cc.center, cc.radius, p))
            {
                std::cout<<"Not delaunay."<<std::endl;
                return;
            }
        }
    }
    std::cout<<"Is delaunay."<<std::endl;
}
/****
 * bool isDelaunay(EdgeId A)
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
}*/