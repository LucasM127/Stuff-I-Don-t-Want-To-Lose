#include <SFML/Graphics.hpp>
#include "../SFML_Functions/SFML_Functions.hpp"
//https://forum.libcinder.org/topic/smooth-thick-lines-using-geometry-shader#23286000001269127
//for the 'channel' effect.
//each point is like a 'node' or 'circle'
//perpendicular
//cross

typedef Point Vec2;

struct Node;

sf::VertexArray line;
sf::VertexArray outerOutline;
sf::VertexArray innerOutline;
sf::VertexArray lineShape;
std::vector<Node> nodes;
std::vector<sf::CircleShape> nodeCircles;

struct Node
{
    Node(float x, float y, float w) : pos(x,y), r(w) {}
    Point pos;
    float r;
};//ie a circle

void genLines()
{//ASSUMES END POINT IS AT LEAST DISTANCE R AWAY FROM PREVIOUS LINE(S)
    line.clear();
    outerOutline.clear();
    innerOutline.clear();
    lineShape.clear();

    for(uint i = 0; i<nodes.size(); ++i)
    {
        bool toLeft; 
        Node &n = nodes[i];
        line.append({n.pos, sf::Color::White});
        //get perpendicular normal
        if(i == 0)
        {
            Vec2 v1 = nodes[i+1].pos - nodes[i].pos;
            Vec2 normal(-v1.y, v1.x);
            normal = SFUTIL::getUnitVec(normal);
            innerOutline.append({n.pos - normal * n.r, sf::Color(255,255,128)});
            outerOutline.append({n.pos + normal * n.r, sf::Color(255,255,128)});
        }
        else if(i == nodes.size() - 1)
        {
            Vec2 v2 = nodes[i].pos - nodes[i-1].pos;
            Vec2 normal(-v2.y, v2.x);
            normal = SFUTIL::getUnitVec(normal);
            innerOutline.append({n.pos - normal * n.r, sf::Color(255,255,128)});
            outerOutline.append({n.pos + normal * n.r, sf::Color(255,255,128)});
        }
        else
        {
            Vec2 v1 = nodes[i+1].pos - nodes[i].pos;
            Vec2 v2 = nodes[i].pos - nodes[i-1].pos;
            v1 = SFUTIL::getUnitVec(v1);
            v2 = SFUTIL::getUnitVec(v2);
            Vec2 v1n(-v1.y,v1.x);
            Vec2 v2n(-v2.y,v2.x);//use these instead of miter....
            toLeft = SFUTIL::cross<float>(v1,v2) < 0.f;
            Vec2 tangent = SFUTIL::getUnitVec(v1 + v2);
            Vec2 miter(-tangent.y, tangent.x);
            float length = n.r / SFUTIL::dot<float>(miter,{-v1.y,v1.x});
            if(!toLeft)
            {
                innerOutline.append({n.pos - miter * length, sf::Color(255,255,128)});
                outerOutline.append({n.pos + v2n * n.r, sf::Color(255,255,128)});
                outerOutline.append({n.pos + v1n * n.r, sf::Color(255,255,128)});
            }
            else
            {
                outerOutline.append({n.pos + miter * length, sf::Color(255,255,128)});
                innerOutline.append({n.pos - v2n * n.r, sf::Color(255,255,128)});
                innerOutline.append({n.pos - v1n * n.r, sf::Color(255,255,128)});
            }
            
            //innerOutline.append({n.pos - miter * length, sf::Color(255,255,128)});
            //outerOutline.append({n.pos + miter * length, sf::Color(255,255,128)});
        }
    }

    for(uint i = 0; i<nodes.size()-1; ++i)
    {
        //2 triangle for each segment...
        sf::Color color = sf::Color(rand()%255,rand()%255,rand()%255,128);
        //t1 outer inner -> inner next node
        //t2 outer -> inner outer
        lineShape.append({outerOutline[i].position,color});
        lineShape.append({innerOutline[i].position,color});
        lineShape.append({innerOutline[i+1].position,color});
        //color = sf::Color(rand()%255,rand()%255,rand()%255);
        lineShape.append({outerOutline[i].position,color});
        lineShape.append({innerOutline[i+1].position,color});
        lineShape.append({outerOutline[i+1].position,color});
    }

    sf::CircleShape C;
    C.setFillColor(sf::Color(255,0,0,64));
    for(auto &n : nodes)
    {
        C.setRadius(n.r);
        C.setOrigin(n.r,n.r);
        C.setPosition(n.pos);
        nodeCircles.push_back(C);
    }
}

int main()
{    
    line.setPrimitiveType(sf::LineStrip);
    outerOutline.setPrimitiveType(sf::LineStrip);
    innerOutline.setPrimitiveType(sf::LineStrip);
    lineShape.setPrimitiveType(sf::Triangles);

    //take a subset of points
    //draw the lines
    //draw the min/max radius values as half transparent lines

    //sample data
    /*nodes =
    {
        {100,100,50},
        {150,400,40},
        //{200,400,40},
        {300,100,30},
        {500,500,20},
        {700,300,10},
        {700,100,30},
        {400,100,30}
    };*/

    //lines...
    //randothick
    
    //draw circle for each node ?

    sf::RenderWindow window(sf::VideoMode(800,600),"Lines");
    sf::Event event;
    float thickness = 80;
    while (window.isOpen())
    {
        window.clear(sf::Color(64,64,64));
        window.draw(line);
        window.draw(innerOutline);
        window.draw(outerOutline);
        for(auto &c : nodeCircles) window.draw(c);
        //window.draw(lineShape);
        window.display();
        window.waitEvent(event);
        if(event.type == sf::Event::Closed || event.type == sf::Event::KeyPressed)
            window.close();
        if(event.type == sf::Event::MouseButtonPressed)
        {
            //float thickness = rand()%21 + 20;
            sf::Vector2f pos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
            nodes.emplace_back(pos.x, pos.y, thickness);
            genLines();
            thickness -= 10;
        }
    }
}