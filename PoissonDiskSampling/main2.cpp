#include <vector>
#include <cmath>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <fstream>
#include "SpatialGrid.hpp"

typedef Vec2 Point;


std::ostream &operator <<(std::ostream &o, const Point &p)
{
    return o <<p.x<<" "<<p.y<<" ";
}

int main()
{
    srand(time(NULL));
    //Setups
    const float width = 100.f;//640.f;//200.f;
    const float height = 100.f;//640.f;//200.f;
    const float minDist = 10.f;//5.f; //clear distance between points
    const float r = minDist;
    const uint k = 30; //limit of samples to choose before rejection in the algorithm
    //const int n = 2;   //dimension
    const float cellSz = r / sqrtf(float(2));
    const float pi = M_PI;
    
    GridMap gridMap({width, height}, cellSz);
    std::vector<uint> active_list;
    std::vector<Point> points;

    //Step 1
    //randomly chosen from the domain ...
    /*
    {
        std::ifstream ifile;
        ifile.open("terrainPoints");
        Point p;
        while(ifile)
        {
            ifile >> p.x >> p.y;
            int index = points.size();
            points.push_back(p);
            active_list.push_back(index);
            gridMap.set(p,index);
        }
        ifile.close();
        std::cout<<points.size()<<std::endl;
    }
    */
    
    {
        Point p;
        p.x = width * (float)(rand() % RAND_MAX) / (float)RAND_MAX;
        p.y = height * (float)(rand() % RAND_MAX) / (float)RAND_MAX;
        int index = points.size();
        points.push_back(p);
        active_list.push_back(index);
        gridMap.set(p,index);
    }

    

    sf::RenderWindow window(sf::VideoMode(800.f, 800.f), ":)");
    sf::View view;
    view.setSize(width, height);
    view.setCenter(width / 2.f, height / 2.f);
    window.setView(view);
    sf::Event event;
    sf::CircleShape C;
    C.setFillColor(sf::Color::Blue);
    C.setRadius(r / 2);
    C.setOrigin(r / 2, r / 2);
    sf::CircleShape CR(C);
    CR.setFillColor(sf::Color::Green);
    uint selectedID = INVALID_ID;
    bool removed = false;
    std::vector<Point> newPoints;
    sf::VertexArray gridLines;
    gridLines.setPrimitiveType(sf::Lines);
    for(uint i = 0; i < gridMap.getGridDimensions().x; ++i)
    {
        sf::Vertex V;
        V.color = sf::Color(255,255,255,100);
        //from 0 to hight
        V.position = {cellSz * i, 0.f};
        gridLines.append(V);
        V.position = {cellSz * i, height};
        gridLines.append(V);
    }
    for (uint i = 0; i < gridMap.getGridDimensions().y; ++i)
    {
        sf::Vertex V;
        V.color = sf::Color(255, 255, 255, 100);
        //from 0 to hight
        V.position = {0.f, cellSz * i};
        gridLines.append(V);
        V.position = {width, cellSz * i};
        gridLines.append(V);
    }

    while (window.isOpen())
    {
        window.clear(sf::Color(128, 128, 128));
        for (auto p : points)
        {
            C.setPosition(p.x, p.y);
            window.draw(C);
        }
        if (selectedID != INVALID_ID)
        {
            if(removed) CR.setFillColor(sf::Color::Red);
            else CR.setFillColor(sf::Color::Green);
            CR.setPosition(points[selectedID].x, points[selectedID].y);
            window.draw(CR);
        }
        for (auto p : newPoints)
        {
            CR.setFillColor(sf::Color::Cyan);
            CR.setPosition(p.x, p.y);
            window.draw(CR);
        }
        window.draw(gridLines);
        window.display();

        window.waitEvent(event);
        if (event.type == sf::Event::Closed)
            window.close();
        if (event.type == sf::Event::KeyPressed)
        {
            //step it through...
            if (active_list.size())
//            while (active_list.size())
            {
                newPoints.clear();
                uint randomIndex = rand() % active_list.size();
                selectedID = active_list[randomIndex];
                const Point seedPoint = points[active_list[randomIndex]];//resizing it fucks up pointers
                bool foundPtToAdd = false;

                float dtheta = 2 * pi / (float)k;
                for (uint i = 0; i < k; ++i)
                {//positive or negative
                    float randomDist = r + r * (float)(rand() % RAND_MAX) / (float)RAND_MAX;
                    float randDX = randomDist * (float)(rand() % RAND_MAX) / (float)RAND_MAX;
                    float DY = sqrtf(randomDist * randomDist - randDX * randDX);
                    if(rand()%2) DY *= -1.f;
                    if(rand()%2) randDX *= -1.f;
                    float theta = dtheta * i;
                    Point possibleNewPoint;
                    possibleNewPoint.x = seedPoint.x + randDX;
                    possibleNewPoint.y = seedPoint.y + DY;
                    //possibleNewPoint.x = seedPoint.x + randomDist * cosf(theta);
                    //possibleNewPoint.y = seedPoint.y + randomDist * sinf(theta);
//i almost should time this...  the sin cos method or a pseudo angle method seems to take less time
                    //check if is even a valid spot to put the point first?
                    if(!gridMap.isValid(possibleNewPoint)) continue;
                    uint indice = gridMap.get(possibleNewPoint);
                    if (indice != INVALID_ID)
                        continue;

                    //check if could collide with any neighbouring cells? the 8 neighbours
                    std::vector<uint> neighbourIndices;
                    gridMap.addNeighbours(possibleNewPoint, neighbourIndices);

                    bool collide = false;
                    for (auto i : neighbourIndices)
//                    for(uint i =0 ; i < points.size(); ++i)
                    {
                        if(i == selectedID) continue;
                        Point &p = points[i];
                        float dx = p.x - possibleNewPoint.x;
                        float dy = p.y - possibleNewPoint.y;
                        float d = sqrtf(dx * dx + dy * dy);
                        if (d < 10.f)//0.5f*r)
                        {
                            collide = true;
                            break; //from the inner loop
                        }
                    }
                    if (!collide)
                    {
                        int index = points.size();
                        points.push_back(possibleNewPoint);
                        gridMap.set(possibleNewPoint, index);//seed point is changed???
                        active_list.push_back(index);
                        foundPtToAdd = true;
                        newPoints.push_back(possibleNewPoint);
                        //add to our collision check here too!
                    }
                } //for (uint i = 0; i < k; ++i)

                //if (!foundPtToAdd)
                    active_list.erase(active_list.begin() + randomIndex);
                removed = !foundPtToAdd;

            } //while (active_list.size())
        }
    }

    const std::vector<uint> &grid = gridMap.getGrid();

    {
        int i = 0;
        for(auto &O : grid)
            if(O != -1) i++;
        std::cout<<"size used: "<<i<<" vs full "<<grid.size()<<std::endl;
    }
    std::cout<<points.size()<<std::endl;

    std::ofstream ofs("points");
    for(auto &p : points) ofs << p;
    ofs.close();

    return 0;
}

/*
//bleh it sucks what do I know?
//return -1 for it being a null object...
template <class T>
class spatialGridMap
{
public:
    spatialGridMap(float width, float height, float cellSize);
    T *get(float x, float y);
    bool set(float x, float y, T t);
    void clear(T clearVal){for (auto &O : m_grid) O = clearVal;}
    void getNeighbours(float x, float y, std::vector<T*> &neighbours);
    void print(){int i = 0; for(auto &O : m_grid) if(O != -1) i++; std::cout<<"size used: "<<i<<" vs full "<<m_grid.size()<<std::endl;}
    float getGridWidth(){return m_gridWidth;}
    float getGridHeight(){return m_gridHeight;}
private:
    const float m_width, m_height;
    const float m_cellSize;
    const uint m_gridWidth, m_gridHeight;
    std::vector<T> m_grid;

    T *locate(float x, float y);
    T null;
};

template <class T>
spatialGridMap<T>::spatialGridMap(float width, float height, float cellSize)
    : m_width(width), m_height(height), m_cellSize(cellSize),
      m_gridWidth(uint(m_width / m_cellSize) + 1),
      m_gridHeight(uint(m_height / m_cellSize) + 1)
{
    m_grid.resize(m_gridWidth * m_gridHeight);
}

template <class T>
T *spatialGridMap<T>::locate(float x, float y)
{
    if(x < 0.f || y < 0.f || x > m_width || y > m_height) return nullptr;
    uint i = uint(x / m_cellSize);
    uint j = uint(y / m_cellSize);
    return &m_grid[i + m_gridWidth * j];
}

template <class T>
T *spatialGridMap<T>::get(float x, float y)
{
    return locate(x,y);
}

template <class T>
bool spatialGridMap<T>::set(float x, float y, T t)
{
    T *p = locate(x,y);
    if(!p) return false;
    *p = t;
    return true;
}

template <class T>
void spatialGridMap<T>::getNeighbours(float x, float y, std::vector<T*> &neighbours)
{
    uint i = uint(x / m_cellSize);
    uint j = uint(y / m_cellSize);
    //for (uint x = i > 1 ? i - 2 : i > 0 ? i - 1 : 0; x < (i == m_gridWidth - 1 ? m_gridWidth : i == m_gridWidth - 2 ? m_gridWidth : i + 3); ++x)
    for (uint x = i > 0 ? i - 1 : 0; x < (i == m_gridWidth - 1 ? m_gridWidth : i + 2); ++x)
    {
        //for (uint y = j > 1 ? j - 1 : j > 0 ? j - 1 : 0; y < (j == m_gridHeight - 1 ? m_gridHeight : j == m_gridHeight - 2 ? m_gridHeight : j + 3); ++y)
        for (uint y = j > 0 ? j - 1 : 0; y < (j == m_gridHeight - 1 ? m_gridHeight : j + 2); ++y)
        {
            if (x == i && y == j)
              continue;
            neighbours.push_back(&m_grid[x + m_gridWidth * y]);
        }
    }
}

struct point
{
    float x, y;
};
*/