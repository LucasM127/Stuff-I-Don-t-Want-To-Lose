//Using SFML
//Basic Line Drawing Function
//Converts to vertexarray
//Line from point a to point b
//with a set width.

#include <SFML/Graphics.hpp>
#include "../SFML_Functions/SFML_Functions.hpp"

//or in other words a fancy rectangle...
//typedef sf::Vector2f Point;
typedef sf::Vector2f Vec2;

void continueLine(sf::VertexArray &lines, const Point &b, const float width_a)
{
    //create our rectangle from point A to point B
    sf::Vertex V;
    V.color = sf::Color(222,44,128,128);

    std::size_t n = lines.getVertexCount();
    Point a0 = lines[n-1].position;
    Point a1 = lines[n-2].position;
    Vec2 ab = b - SFUTIL::getMidpoint<float>(a0,a1);
    Vec2 ortho = SFUTIL::getUnitVec<float>({ab.y, -ab.x});
    Point b0 = b + ortho * width_a;
    Point b1 = b + ortho * -width_a;

    Vec2 za = lines[n-1].position - lines[n-3].position;
    if(SFUTIL::cross<float>(za,ab) > 0.f)
    {
        a0 = a1 - ortho * 2.f * width_a;
    }
    else
    {
        a1 = a0 + ortho * 2.f * width_a;
    }
    

    V.position = a0;
    lines.append(V);
    V.position = a1;
    lines.append(V);
    V.position = b0;
    lines.append(V);

    V.position = a0;
    lines.append(V);
    V.position = b0;
    lines.append(V);
    V.position = b1;
    lines.append(V);
}

void addLine(sf::VertexArray &lines, const Point &a, const Point &b, const float width_a)
{
    const float width_b = width_a;
    //create our rectangle from point A to point B
    sf::Vertex V;
    V.color = sf::Color(222,44,128,128);

    Vec2 ab = b - a;
    Vec2 ortho = SFUTIL::getUnitVec<float>({ab.y, -ab.x});
    Point a0 = a + ortho * width_a;
    Point a1 = a + ortho * -width_a;
    Point b0 = b + ortho * width_b;
    Point b1 = b + ortho * -width_b;

    V.position = a0;
    lines.append(V);
    V.position = a1;
    lines.append(V);
    V.position = b0;
    lines.append(V);

    V.position = a1;
    lines.append(V);
    V.position = b0;
    lines.append(V);
    V.position = b1;
    lines.append(V);
}

int main()
{
    sf::RenderWindow window(sf::VideoMode(800,600),"Lines");

    sf::VertexArray lines;
    lines.setPrimitiveType(sf::Triangles);

    enum click {first, second, cont} state = first;
    Point posA, posB;

    sf::Event event;
    while (window.isOpen())
    {
        window.clear(sf::Color(64,64,64));
        window.draw(lines);
        window.display();
        window.waitEvent(event);
        if(event.type == sf::Event::Closed || event.type == sf::Event::KeyPressed)
            window.close();
        if(event.type == sf::Event::MouseButtonPressed)
        {
            if(state == first)
            {
                posA = window.mapPixelToCoords(sf::Mouse::getPosition(window));
                state = second;
            }
            else if(state == second)
            {
                posB = window.mapPixelToCoords(sf::Mouse::getPosition(window));
                //state = first;
                addLine(lines, posA, posB, 10.f);
                state = cont;
                //continueLine(lines, posB,10.f);
            }
            else
            {
                posB = window.mapPixelToCoords(sf::Mouse::getPosition(window));
                continueLine(lines, posB, 10.f);
            }
            
        }
    }

    return 0;
}