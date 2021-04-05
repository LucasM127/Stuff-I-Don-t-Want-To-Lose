#define LOG_DEBUG_STDOUT //FILE
#include <SFML/Graphics.hpp>
#include "../SFML_Functions/SFML_Functions.hpp"
#include "SpatialHashMap.hpp"
#include "TriangleShape.hpp"

using namespace SFUTIL;
//Yeah... it should work
//loop through our AABBs to find collidables, 
//loop through collidable lines to find if any intersect
//shouldn't be too bad.

//Does a shape... or is it object...
//Need to supply it to our SH map. somehow...

TriangleShape::TriangleShape()
{
    m_vertices.resize(3);
    m_vertices[0].position = sf::Vector2f(0.f, 1.f); //-0.866025403781f);
    m_vertices[1].position = sf::Vector2f(-0.5f, 0.f);
    m_vertices[2].position = sf::Vector2f(0.5f, 0.f);
    m_vertices[0].color = sf::Color(250, 80, 10);
    m_vertices[1].color = sf::Color(100, 0, 250);
    m_vertices[2].color = m_vertices[1].color;
    //scale(50.f, 50.f); //50m big
    setOrigin(sf::Vector2f(0.f, 0.333f));
}

void TriangleShape::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
    states.transform *= getTransform();
    target.draw(&m_vertices[0],m_vertices.size(),sf::Triangles,states);
}

void TriangleShape::update()
{
    m_transformedPoints.clear();
    for(auto &v : m_vertices)
    {
        Point p = v.position;
        p = getTransform().transformPoint(p);
        m_transformedPoints.push_back(p);
    }
    m_aabb.resize(&m_transformedPoints[0], 3);
}

bool TriangleShape::collidesLine(const Point &a, const Point &b)
{
    for(int i = 0; i < 3; i++)
    {
        int next = (i == 2 ? 0 : i+1);
        Point c = m_transformedPoints[i];
        Point d = m_transformedPoints[next];
        if(lineSegsIntersect(a,b,c,d)) return true;
    }
    return false;
}

//just do a line intersection test...
int main()
{
    srand(time(NULL));

    sf::RenderWindow window(sf::VideoMode(960, 540), "Map");
    sf::View view;
    view.setSize(48, -27);
    view.setCenter(24, 13.5);
    window.setView(view);
    CameraController camera(window,view);

    std::vector<sf::Vector2f> P;
    
    if (!loadPoly("mapData", P))
        return -1;

    sf::VertexArray outlineShape;
    convertPolygonOutline(P, outlineShape);

    AABBf bounds;
    bounds.x_min = bounds.y_min = 0.f;
    bounds.x_max = 48.f;
    bounds.y_max = 27.f;
    SpatialHash SH(bounds);

    //create AABBS
    std::vector<AABBf> aabbs;
    for (unsigned int i = 0; i < P.size(); i++)
    {
        unsigned int next = (i == P.size() - 1 ? 0 : i + 1);
        LSegf lseg;
        lseg.a = P[i];
        lseg.b = P[next];
        SH.insert(lseg);
        //AABBf aabb(P[i], P[next]);
        //aabbs.push_back(aabb);
    }
    //sf::VertexArray aabbOutlines;
    //convertAABBtoLines(aabbs, aabbOutlines);

    TriangleShape T;
    sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
    T.setPosition(mousePos);

    sf::Event event;
    while (window.isOpen())
    {
        window.clear(sf::Color::White);
        window.draw(outlineShape);
        SH.render(window);
        //window.draw(aabbOutlines);
        window.draw(T);
        window.display();

        window.waitEvent(event);
        if (event.type == sf::Event::Closed)
            window.close();
        if (event.type == sf::Event::MouseMoved)
        {
            T.rotate(1.f);
            mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
            T.setPosition(mousePos);
            T.update();

            SH.testHit(T);
            /***
            //for each.... test if collides...
            for(unsigned int i = 0; i<aabbs.size(); i++) 
            {
                if(T.collides(aabbs[i]))
                {
                    //set a range to
                    aabbOutlines[8 * i + 0].color = sf::Color::Red;
                    aabbOutlines[8 * i + 1].color = sf::Color::Red;
                    aabbOutlines[8 * i + 2].color = sf::Color::Red;
                    aabbOutlines[8 * i + 3].color = sf::Color::Red;
                    aabbOutlines[8 * i + 4].color = sf::Color::Red;
                    aabbOutlines[8 * i + 5].color = sf::Color::Red;
                    aabbOutlines[8 * i + 6].color = sf::Color::Red;
                    aabbOutlines[8 * i + 7].color = sf::Color::Red;
                    aabbOutlines[8 * i + 8].color = sf::Color::Red;

                    //test if intersect line i-i+1
                    int next = ( i == P.size() - 1 ? 0 : i+1);
                    if(T.collidesLine(P[i],P[next]))
                    {
                        outlineShape[i].color = sf::Color::Red;
                    }
                }
            }***/
        }
        camera.handleEvent(event);
    }

    return 0;
}