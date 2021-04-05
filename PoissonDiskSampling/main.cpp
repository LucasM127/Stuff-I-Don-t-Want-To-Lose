#ifndef SPATIAL_GRID_HPP
#define SPATIAL_GRID_HPP

#include <SFML/System/Vector2.hpp>
#include <vector>

typedef unsigned int uint;

typedef float REAL;

typedef sf::Vector2<REAL> Vec2;

const uint INVALID_ID = -1;

struct Coord
{
    Coord(unsigned int a = 0, unsigned int b = 0) : i(a), j(b) {}
    Coord operator + (const Coord &C) {return Coord(i + C.i, j + C.j);}
    bool operator != (const Coord &C) {return i != C.i || j != C.j;}
    unsigned int i, j;
};

class GridMap
{
public:
    GridMap(){}
    void init(uint width, uint height, REAL cellSz);
    inline bool isValid(const Coord C) const {return (C.i < m_width && C.j < m_height);}
    inline const uint width()  const { return m_width; }
    inline const uint height() const { return m_height; }
    inline Coord coord(const Vec2 p){ return Coord(p.x/m_cellSz, p.y/m_cellSz);}
    inline uint at(const Coord C) const {return C.i + C.j * m_width;}
    inline bool set(const uint x, const Coord C);
    inline bool get(uint &data, const Coord C);
    const std::vector<uint> &data(){return m_data;}
    const REAL cellSz() const {return m_cellSz;}
private:
    uint m_width, m_height;
    REAL m_cellSz;
    std::vector<uint> m_data;
};

#endif //SPATIAL_GRID_HPP

void GridMap::init(uint width, uint height, REAL cellSz)
{
    m_width = width;
    m_height = height;
    m_cellSz = cellSz;
    m_data.resize(m_width * m_height, INVALID_ID);
}

bool GridMap::set(const uint x, const Coord C)
{
    if(!isValid(C)) return false;
    m_data[at(C)] = x;
    return true;
}

bool GridMap::get(uint &data, const Coord C)
{
    if(!isValid(C)) return false;
    data = m_data[at(C)];
    return true;
}

////////////////////////////////////////////////////////
#include <list>
#include <cmath>
#include <iostream>

typedef Vec2 Point;

bool collides(const Point &a, const Point &b, REAL radius)
{
    REAL dx = a.x - b.x;
    REAL dy = a.y - b.y;
    return (dx * dx + dy * dy) < radius * radius;
}

//poisson distribution function
//INPUT = domain (x,y), radius, startingPoint List
//OutPut, GridMap, pointing to vector of allpoints

class PoissonPointPlotter
{
public:
    PoissonPointPlotter(Vec2 domainSz, REAL minDist, const std::vector<Point> &startPoints);
    const std::vector<Point> &points(){return m_points;}
    const REAL minDist(){return m_minDist;}
    const GridMap &gridMap(){return m_gridMap;}
    Point rpoint;

    void step();
    void iterate();//tries to add points to active id, first point on activelist
private:
    GridMap m_gridMap;
    REAL m_minDist;//radius
    std::list<uint> m_activeList;
    std::vector<Point> m_points;
    Vec2 m_sz;
    
    void addPoint(const Point &refPoint);//tries to add a point
};

PoissonPointPlotter::PoissonPointPlotter(Vec2 sz, REAL minDist, const std::vector<Point> &startPoints)
    : m_minDist(minDist), m_points(startPoints), m_sz(sz)
{
    srand(time(NULL));
std::cout<<"sz "<<m_points.size()<<std::endl;
std::cout<<m_points[0].x<<" "<<m_points[0].y<<std::endl;
    REAL cellSz = m_minDist / M_SQRT2;
    m_gridMap.init(uint(sz.x / cellSz) + 1, uint(sz.y / cellSz) + 1, cellSz);
    for(uint i = 0; i < m_points.size(); ++i)
    {
        m_activeList.push_back(i);
        m_gridMap.set(i,m_gridMap.coord(m_points[i]));
    }
    
    //while (m_activeList.size())
    //{
    //    iterate();
    //}
}

//or a queue
void PoissonPointPlotter::iterate()
{//That is just too weird
//reference to front fails... so copy it instead
    const Point refPoint = m_points[m_activeList.front()];

    for(uint i = 0; i < 30; ++i)
    {
        addPoint(refPoint);
    }
    m_activeList.pop_front();
}

void PoissonPointPlotter::step()
{
    if(!m_activeList.size()) return;
    const Point refPoint = m_points[m_activeList.front()];
    rpoint = refPoint;
std::cout<<"rpoint "<<rpoint.x<<" "<<rpoint.y<<std::endl;
    for(uint i = 0; i < 30; ++i)
    {
        addPoint(refPoint);
    }
    m_activeList.pop_front();
}

void PoissonPointPlotter::addPoint(const Point &refPoint)
{
    float r = m_minDist;

    float randomDist = r + r * (float)(rand() % RAND_MAX) / (float)RAND_MAX;
    float DX = randomDist * (float)(rand() % RAND_MAX) / (float)RAND_MAX;
    float DY = sqrtf(randomDist * randomDist - DX * DX);
    if(rand()%2) DY *= -1.f;
    if(rand()%2) DX *= -1.f;
    //use grid map to find all existing points within 2r
    
    Point p(refPoint.x + DX, refPoint.y + DY);
    //is it in my domain?
    if(p.x > m_sz.x || p.y > m_sz.y || p.x < 0.f || p.y < 0.f) return;
    //if it is, it must be in the gridmap
    Coord refCoord = m_gridMap.coord(p);
    //find any points that it may hit now
    //the 9 grid points
    for(uint x = 0; x < 5; ++x)
        for(uint y = 0; y<5; ++y)
        {
            Coord C(refCoord.i + x - 2, refCoord.j + y - 2);
            uint existingPointId = 0;
            if(!m_gridMap.get(existingPointId, C)) continue;
            if(existingPointId == INVALID_ID) continue;
            //found a point that may hit, test if collides.
            if(collides(p, m_points[existingPointId], m_minDist)) return;
        }
    
    uint index = m_points.size();
    m_points.push_back(p);
    m_gridMap.set(index, refCoord);
    m_activeList.push_back(index);
    //foundpttoadd=true
    std::cout<<refPoint.x<<" "<<refPoint.y<<" - "<<p.x<<" "<<p.y<<" "<<randomDist<<" "<<DX<<" "<<DY<<std::endl;
}

#include <SFML/Graphics.hpp>

int main()
{
    const float width = 100.f;
    const float height = 100.f;
    const float minDist = 10.f;
    float r = minDist;

    sf::RenderWindow window(sf::VideoMode(800.f, 800.f), ":)");
    sf::View view;
    view.setSize(width, height);
    view.setCenter(width / 2.f, height / 2.f);
    window.setView(view);

    std::vector<Point> startPoints;
    startPoints.push_back(Point(50.f,50.f));

    PoissonPointPlotter plotter({width,height}, minDist, startPoints);

    sf::CircleShape C;
    C.setFillColor(sf::Color::Blue);
    C.setRadius(r / 2.f);
    C.setOrigin(r / 2.f, r / 2.f);

    std::vector<sf::CircleShape> circles;
    for(auto &p : plotter.points())
    {
        C.setPosition(p);
        circles.push_back(C);
    }
    circles.front().setFillColor(sf::Color::Red);

    sf::VertexArray gridLines;
    gridLines.setPrimitiveType(sf::Lines);
    //dimension?
    float gridsz = plotter.gridMap().cellSz();
    for(uint i = 0; i < plotter.gridMap().width(); ++i)
    {
        sf::Vertex V;
        V.color = sf::Color(255,255,255,100);
        //from 0 to hight
        V.position = {gridsz * i, 0.f};
        gridLines.append(V);
        V.position = {gridsz * i, height};
        gridLines.append(V);
    }
    for(uint i = 0; i < plotter.gridMap().height(); ++i)
    {
        sf::Vertex V;
        V.color = sf::Color(255,255,255,100);
        //from 0 to hight
        V.position = {0.f, gridsz * i};
        gridLines.append(V);
        V.position = {width, gridsz * i};
        gridLines.append(V);
    }

    sf::Event event;

    while (window.isOpen())
    {
        window.clear(sf::Color(128, 128, 128));
        for(auto &c : circles) window.draw(c);
        window.draw(gridLines);
        window.display();

        window.waitEvent(event);
        if(event.type == sf::Event::Closed)
            window.close();
        if(event.type == sf::Event::KeyPressed)
        {
            plotter.step();
            circles.clear();
            for(auto &p : plotter.points())
            {
                C.setPosition(p);
                circles.push_back(C);
            }
            C.setPosition(plotter.rpoint);
            circles.push_back(C);
            circles.back().setFillColor(sf::Color::Red);
        }
    }
}