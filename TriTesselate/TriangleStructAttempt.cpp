#include <iostream>
#include <vector>
#include <cassert>
#include <list>

#include "../../SFML_Functions/SFML_Functions.hpp"// <- to remove this would be nice one day
#include <algorithm>

auto sortxy = [](const Point &a, const Point &b) -> bool 
{
    if (a.x == b.x)
    {
        return a.y < b.y;
    }
    return a.x < b.x;
};

//is all relationship... so not dependant on type of points
//and if initial triangle is cw or ccw, all triangles made to be the same cw or ccw

const unsigned int INVALID_INDEX = 9;//-1;
/*
struct Vec2
{
    float x, y;
};

typedef Vec2 Point;
*/
inline unsigned int next(unsigned int i)
{
    return (i + 1) % 3;
}

inline unsigned int last(unsigned int i)
{
    return (i + 2) % 3;
}

struct Edge
{
    unsigned int t;
    unsigned int i;
};

struct Triangle
{
    Triangle(){}
    Triangle(unsigned int p1, unsigned int p2, unsigned int p3, unsigned int t1, unsigned int t2, unsigned int t3)
        : p{p1,p2,p3}, t{t1,t2,t3} {}
    unsigned int p[3];
    unsigned int t[3];
};

//or a linked list ?
struct HullEdge
{
    Edge E;
    HullEdge *next;
};

std::vector<Point> points;
std::vector<Triangle> triangles;

void addTriangle(Edge E, unsigned int p)
{
    unsigned int p1 = triangles[E.t].p[E.i];
    unsigned int p2 = triangles[E.t].p[next(E.i)];
    triangles[E.t].t[E.i] = triangles.size();
    triangles.emplace_back(p2,p1,p,E.t,INVALID_INDEX,INVALID_INDEX);
}

void addTriangle(Edge E1, Edge E2)
{
    triangles[E1.t].t[E1.i] = triangles[E2.t].t[E2.i] = triangles.size();
    unsigned int p1 = triangles[E1.t].p[E1.i];
    unsigned int p2 = triangles[E2.t].p[E2.i];

    if(triangles[E2.t].p[next(E2.i)] == triangles[E1.t].p[E1.i])
    {
        unsigned int p3 = triangles[E1.t].p[next(E1.i)];
        triangles.emplace_back(p1,p2,p3,E2.t,INVALID_INDEX,E1.t);
    }
    else
    {
        unsigned int p3 = triangles[E2.t].p[next(E2.i)];
        triangles.emplace_back(p2, p1, p3, E1.t, INVALID_INDEX, E2.t);
    }
}

inline Edge getOpp(Edge &&E)
{
    Triangle &T = triangles[E.t];

    Edge oppEdge;
    oppEdge.t = T.t[E.i];
    assert(oppEdge.t != INVALID_INDEX);

    const Triangle &T_adj = triangles[T.t[E.i]];
    unsigned int P = T.p[E.i];
    //it is either 0 1 or 2
    
    if(T_adj.p[0] == P) oppEdge.i = 2;//is the last edge as make them clockwise
    else if(T_adj.p[1] == P) oppEdge.i = 0;
    else oppEdge.i = 1;

    return oppEdge;
}

//YAY IT WORKS!!!
void edgeFlip(Edge &&E)
{
    Triangle &T1 = triangles[E.t];
    Edge E2 = getOpp({E.t,E.i});
    Triangle &T2 = triangles[E2.t];

    Triangle T1_, T2_;
    T1_.p[E.i] = T2.p[last(E2.i)];//T2 previous
    T1_.p[next(E.i)] = T1.p[last(E.i)];
    T1_.p[last(E.i)] = T2.p[next(E2.i)];
    //and the triangles too
    T1_.t[E.i] = T1.t[E.i];
    T1_.t[next(E.i)] = T1.t[last(E.i)];
    T1_.t[last(E.i)] = T2.t[next(E2.i)];
    //exact same for the other triangle too just reversed
    T2_.p[E2.i] = T1.p[last(E.i)];
    T2_.p[next(E2.i)] = T2.p[last(E2.i)];
    T2_.p[last(E2.i)] = T1.p[next(E.i)];
    //and the triangles too
    T2_.t[E2.i] = T2.t[E2.i];
    T2_.t[next(E2.i)] = T2.t[last(E2.i)];
    T2_.t[last(E2.i)] = T1.t[next(E.i)];

    //mate them too
    //only the ones that have changed...
    //int asf = T1.t[next(E.i)];//this guy is partnered to t2
    //int adf = T2.t[next(E2.i)];//and this guy is partnered to t1
    if(T1.t[next(E.i)] != INVALID_INDEX)
    {
    Edge E_t1n = getOpp({E.t,next(E.i)});
    triangles[E_t1n.t].t[E_t1n.i] = E2.t;
    }
    if(T2.t[next(E2.i)] != INVALID_INDEX)
    {
    Edge E_t2n = getOpp({E2.t,next(E2.i)});
    triangles[E_t2n.t].t[E_t2n.i] = E.t;
    }

    //and assign it over
    T1 = T1_;
    T2 = T2_;//This seems to work with arrays too.  is it compiler dependent????
/*    for(unsigned int i = 0; i < 3; ++i)
    {
        T1.p[i] = T1_.p[i];
        T1.t[i] = T1_.t[i];
        T2.p[i] = T2_.p[i];
        T2.t[i] = T2_.t[i];
    }*/
}

//print out triangle...
void printout()
{
    for(unsigned int i = 0; i<triangles.size(); i++)
    {
        Triangle &t = triangles[i];
        std::cout<<"Triangle "<<i<<": p["<<t.p[0]<<","<<t.p[1]<<","<<t.p[2]<<"] t["<<t.t[0]<<","<<t.t[1]<<","<<t.t[2]<<"]"<<std::endl;
    }
}

/////////////////FINISH FRAMEWORKS////////////////////
/////////////////START TRIANGLE DEL GENERATION////////
//the triangulator shit goes here.... now

float (*cross)(const Point &, const Point &) = SFUTIL::cross<float>;
float (*dot)(const Point , const Point ) = SFUTIL::dot<float>;
typedef Point Vec2;
bool (*circumCenter)(const sf::Vector2<double> &, const sf::Vector2<double> &, const sf::Vector2<double> &, SFUTIL::Circle<double> &) = SFUTIL::getCircumcenter<double>;
//bool (*circumCenter)(const Point &, const Point &, const Point &, SFUTIL::Circle<float> &) = SFUTIL::getCircumcenter<float>;
//returns false if is infinitely big, or... in otherwords the triangle is collinear, then... obviously not delaunay...
typedef SFUTIL::Circle<float> circlef;
typedef SFUTIL::Circle<double> circled;
const float epsilon = std::numeric_limits<float>::epsilon();

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

bool isDelaunay(Edge &&E)
{
    if(triangles[E.t].t[E.i] == INVALID_INDEX) return true;

    Triangle &T = triangles[E.t];
    Edge E_opp = getOpp({E.t,E.i});
    sf::Vector2<double> t1(points[T.p[E.i]]);
    sf::Vector2<double> t2(points[T.p[next(E.i)]]);
    sf::Vector2<double> t3(points[T.p[last(E.i)]]);
    sf::Vector2<double> p(points[triangles[E_opp.t].p[last(E_opp.i)]]);
/*
    sf::Vector2<double> t1(e->p->x,e->p->y);
    sf::Vector2<double> t2(e->next->p->x, e->next->p->y);
    sf::Vector2<double> t3(e->last->p->x, e->last->p->y);
    sf::Vector2<double> p(e->betterHalf->last->p->x, e->betterHalf->last->p->y);
*/
    circled c;
    if (!circumCenter(t1, t2, t3, c))
        return false;
//assert(c.center.x > e->last->p->x);//else is wrong way???
//test this one next....
//std::cout<<"Radius "<<c.radius<<std::endl;
//if(c.radius > 1000.f) if(c.center.x < p.x) std::cout<<c.center.x<<" "<<p.x<<std::endl;
//maybe another method for co-circular points???

    c.radius -= c.radius * 2.f * std::numeric_limits<double>::epsilon();//epsilon;//this is right as crashes otherwise without the 2
//got to move closer to the point we are testing too?
    //c.radius *= 0.999999f; //ddddd1.0f - 8.f * std::numeric_limits<float>::epsilon();//THE PROBLEM LIES HERE>>>
    return !SFUTIL::pointInCircle<double>(c.center, c.radius, p);
}//THINKING..........

unsigned int m_edgeCtr;
unsigned int m_nextPt;
HullEdge *m_firstHullEdge;
std::list<HullEdge> m_hullEdge;
unsigned int m_hullSz;

void init()
{
    m_edgeCtr = 0;
    std::sort(points.begin(), points.end(), sortxy);

    HullEdge *H1 = new HullEdge;
    HullEdge *H2 = new HullEdge;
    HullEdge *H3 = new HullEdge;
    H1->E.t = H2->E.t = H3->E.t = 0;
    H1->next = H2;
    H2->next = H3;
    H3->next = H1;

    if (isCCW(points[0], points[1], points[2]))
    {
        std::cout << "ccw" << std::endl;
        //is good the way it is
        triangles.emplace_back(0,1,2,INVALID_INDEX,INVALID_INDEX,INVALID_INDEX);
        H1->E.i = 0;
        H2->E.i = 1;
        H3->E.i = 2;
    }
    else
    {
        std::cout << "not ccw" << std::endl;
        triangles.emplace_back(0,2,1,INVALID_INDEX,INVALID_INDEX,INVALID_INDEX);
        H1->E.i = 0;//0;
        H2->E.i = 1;//2;
        H3->E.i = 2;//1;//these are indices in teh triangles they correspond to, not to the points.
    }
    m_firstHullEdge = H1;
    m_nextPt = 3;
    m_hullSz = 3;
    m_hullEdge.push_back(*H1);
    m_hullEdge.push_back(*H2);
    m_hullEdge.push_back(*H3);
}

inline Point& getPoint(Edge E)
{
    return points[triangles[E.t].p[E.i]];
}

void step()
{
    if (m_nextPt >= points.size())
    {
        std::cout<<"Triangles: "<<triangles.size()<<std::endl;
        return;
    }

    Point &curPoint = points[m_nextPt];

//    HullEdge *curHullEdge = m_firstHullEdge;

    bool addedTriangle = false;
    bool isFirstEdge = true;
//we want it to be on the right!!!!
    unsigned int hullctr = 0;
    //while (hullctr < m_hullSz)
    for(auto curHullEdge = m_hullEdge.begin(); curHullEdge != m_hullEdge.end();)
    {
        std::cout<<"CurHullEdge E "<<curHullEdge->E.t<<" "<<curHullEdge->E.i<<std::endl;
        //hullctr++;
        if(curPoint == getPoint(curHullEdge->E)) break;
        Vec2 v_0p = curPoint - getPoint(curHullEdge->E);
        Vec2 v_01;
        if(std::next(curHullEdge)!=m_hullEdge.end())
        v_01 = getPoint(std::next(curHullEdge)->E) - getPoint(curHullEdge->E);
        else
        v_01 = getPoint(m_hullEdge.begin()->E) - getPoint(curHullEdge->E);

        float result = cross(v_0p, v_01);
        bool onRight = false;
        if (result < 2.f * epsilon && result > -2.f * epsilon)//collinear
        {
            bool pointIsAheadCurEdgeStart = dot(v_01, v_0p) > -epsilon;
            if(!pointIsAheadCurEdgeStart && !addedTriangle) onRight = true;
        }
        else onRight = result < 0.0f;
        if(onRight)
        {
            std::cout<<"Point on right of hull edge "<<hullctr<<std::endl;
            //add triangle /s
            //delauney fix the triangles as added ?
            HullEdge lastHullEdge;
            if(curHullEdge == m_hullEdge.begin()) lastHullEdge = *std::prev(m_hullEdge.end());
            else lastHullEdge = *std::prev(curHullEdge);
//            if(getPoint(std::prev(curHullEdge)->E) == curPoint)
            if(getPoint(lastHullEdge.E) == curPoint)
            {
                std::cout<<"Add after"<<std::endl;
                addTriangle(std::prev(curHullEdge)->E,curHullEdge->E);
                //find the new edge... is index 2 and triangle t = triangles.size()
                HullEdge newEdge;
                newEdge.E.t = triangles.size() - 1;
                newEdge.E.i = 1;//always
                //remove the 2 and insert the new , and don't advance iterator
                m_hullEdge.erase(std::prev(curHullEdge));//returns iterator to next edge
                curHullEdge = m_hullEdge.erase(curHullEdge);
                m_hullEdge.insert(curHullEdge, newEdge);//inserts before
                continue;
            }
            else
            {
                std::cout<<"Add first triangle single"<<std::endl;
                addTriangle(curHullEdge->E,m_nextPt);
//                addTriangle(std::next(curHullEdge)->E,m_nextPt);
                HullEdge newEdge;
                newEdge.E.t = triangles.size() - 1;
                newEdge.E.i = 1;
                curHullEdge = m_hullEdge.erase(curHullEdge);
                m_hullEdge.insert(curHullEdge, newEdge);
                newEdge.E.i = 2;
                m_hullEdge.insert(curHullEdge, newEdge);
                //continue;
            }
            isFirstEdge = false;
            addedTriangle = true;
            continue;
        }
        if(addedTriangle) break;
        
        curHullEdge++;
    }

        ++m_nextPt;

        //delauney fix

        return;
    
}

void drawEdge(sf::VertexArray &lineShape)
{
    lineShape.clear();
    for(auto &H : m_hullEdge)
    {
        Point P = getPoint(H.E);
        lineShape.append({P, sf::Color(255, 255, 255, 128)});
    }
    lineShape.append(lineShape[0]);
    
    lineShape[0].color = sf::Color::Green;
}

void drawTriangles(sf::VertexArray &shape)
{
    shape.clear();
    for (uint i = 0; i < triangles.size(); ++i)
    {
        sf::Color randColor;
        randColor.r = rand() % 128 + 128;
        randColor.g = rand() % 128 + 128;
        randColor.b = rand() % 128 + 128;
        randColor.a = 32;

        shape.append({points[triangles[i].p[0]], sf::Color(0, 255, 0, 128)}); //randColor});
        shape.append({points[triangles[i].p[1]], sf::Color(0, 0, 255, 128)});
        shape.append({points[triangles[i].p[2]], sf::Color(255, 0, 0, 128)});
    }
}

int main()
{
    srand(time(NULL));
    //srand(33);
    for(int i = 0; i<8; i++) points.push_back({(float)(rand()%100),(float)(rand()%100)});

    sf::CircleShape C;
    C.setFillColor(sf::Color(255, 255, 255, 128));
    C.setOrigin(2.f, 2.f);
    C.setRadius(2.f);

    std::vector<sf::CircleShape> circles;
    
    for (auto &p : points)
    {
        C.setPosition(p);
        circles.push_back(C);
    }

    init();

    sf::VertexArray lineShape;
    lineShape.setPrimitiveType(sf::LineStrip);
    drawEdge(lineShape);
    sf::VertexArray triShape;
    triShape.setPrimitiveType(sf::Triangles);
    drawTriangles(triShape);

    sf::RenderWindow window(sf::VideoMode(640,640),"And aagain...");
    sf::View view;
    view.setCenter(50,50);
    view.setSize(100,-100);
    window.setView(view);

    sf::Event event;
    while (window.isOpen())
    {
        window.clear(sf::Color::Black);
        window.draw(triShape);
        for(auto &C : circles) window.draw(C);
        window.draw(lineShape);
        window.display();
        window.waitEvent(event);
        if(event.type == sf::Event::Closed) window.close();
        if(event.type == sf::Event::KeyPressed)
        {
            step();
            drawTriangles(triShape);
            drawEdge(lineShape);
        }
    }
    
    printout();
/***
    triangles.emplace_back(0,1,2,INVALID_INDEX,INVALID_INDEX,INVALID_INDEX);
    addTriangle({0,1},3);
    addTriangle({0,0},4);
    addTriangle({1,1},{2,2});
    printout();
    Edge e = getOpp({2,2});
    std::cout<<"The opposite edge of T[2] e[2] is "<<e.t<<" "<<e.i<<std::endl;
    e = getOpp({3,2});
    std::cout<<"The opposite edge of T[3] e[2] is "<<e.t<<" "<<e.i<<std::endl;

    edgeFlip({0,1});
    edgeFlip({0,1});
    edgeFlip({0,1});
    edgeFlip({0,1});
    printout();***/
    return 0;
}