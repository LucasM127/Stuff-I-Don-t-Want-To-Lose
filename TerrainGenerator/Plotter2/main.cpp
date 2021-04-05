#include "imgui.h"
#include "imgui-SFML.h"
#include <SFML/Graphics.hpp>
#include <fstream>

#include "PerlinNoise.h"
//COOL
#include <cmath>

float x_offset = 0.f;
float period = 2.f;
float variance = 0.2f;
sf::VertexArray line;
sf::VertexArray offsetLine;
sf::VertexArray groundLine;
PerlinNoise PN;

//bunch of 'nodes' ie 'circles'
struct Node
{
    Node(sf::Vector2f _p, float _r) : p(_p), r(_r) {}
    sf::Vector2f p;
    float r;
};
std::vector<Node> nodes;
std::vector<sf::CircleShape> nodeShapes;

float normRand()
{
    return float(rand()%100) / 100.f;
}

void foo(float x)
{
    sf::CircleShape C;
    C.setFillColor(sf::Color(255,0,0,128));
    sf::Vector2f pn;
    pn.x = x;
    float r = 0.01f + normRand() * 0.04;//1 high... 0.01?
    //get the x y from variance...
    pn.y = PN.noise(pn.x + x_offset, pn.x, 0);
    pn.x /= period;
    //delta
    sf::Vector2f dpn;
    dpn.x = pn.x + 0.005f;
    dpn.y = PN.noise(dpn.x + x_offset, dpn.x, 0);
    dpn.x /= period;
    sf::Vector2f tangent = pn - dpn;
    sf::Vector2f normal(-tangent.y, tangent.x);
    float nl = sqrtf(normal.x * normal.x + normal.y * normal.y);
    normal /= nl;//normalize
    sf::Vector2f p = pn + normal * variance * normRand();
    
    //fit?
    bool collide = false;
    for(auto &n : nodes)
    {
        sf::Vector2f d = n.p - p;
        float l2 = d.x * d.x + d.y * d.y;
        float min2 = n.r + r + 0.005f;
        min2 *= min2;
        if(l2 < min2)
        {
            collide = true;
            break;
        }
    }
    if(!collide)
    {
        //add
        C.setOrigin(r,r);
        C.setRadius(r);
        C.setPosition(p);
        nodes.emplace_back(p,r);
        nodeShapes.push_back(C);
    }
}

void gen2()
{
    nodes.clear();
    nodeShapes.clear();
    //use the parameters... fill it with nodes...
    //draw with sf::circleShapes
    
//try and make 25...
    foo(0.f);
    for(int i = 0; i < 55; i++)
    {
        foo(normRand() * 2.f * period);
    }
    foo(2.f * period);
}

void gen()
{
    offsetLine.clear();
    line.clear();
    groundLine.clear();
    float min_x = 0.f;
    float max_x = 2.f * period;
    float dx = (max_x - min_x) / 200.f;

    for(float x = min_x; x < max_x; x += dx)
    {//0 - 1 y
        float y = PN.noise(x + x_offset,x,0);
        sf::Vector2f P(x / period,y);
        line.append({P, sf::Color::Green});
    }

    //create offset line... go for each point... find d.. with next and place,,, duplicate offset for last point
    for(size_t i = 0; i < line.getVertexCount() - 1; ++i)
    {
        sf::Vector2f tangent = line[i+1].position - line[i].position;
        sf::Vector2f normal(-tangent.y, tangent.x);
        float nl = sqrtf(normal.x * normal.x + normal.y * normal.y);
        normal /= nl;//normalize
        float v = PN.noise(0,0,0.01f*(float)i);
        v -= 0.5f;
        float offsetDist = variance;// * v;
        if(i == 0) offsetLine.append({line[0].position + normal * offsetDist, sf::Color::Cyan});
        offsetLine.append({line[i+1].position + normal * offsetDist, sf::Color::Cyan});

        
    }

    //ground Line... (nodes ?)  less... accurate.
    //size_t i = 0;
    for(size_t i = 0; i < line.getVertexCount(); )
    {
        //lerp
        float ratio = float(rand()%100) / 100.f;
        sf::Vector2f P = offsetLine[i].position - line[i].position;
        P = P * ratio + line[i].position;
        groundLine.append({P, sf::Color::Black});
        
        if(i == (line.getVertexCount() - 1)) break;
        i += rand()%15 + 10;//i += rand()%10 + 1;
        if(i >= line.getVertexCount()) i = line.getVertexCount() - 1;
    }

    gen2();
}

int main()
{
    
    sf::RenderWindow window(sf::VideoMode(800,600),"Noise...");
    sf::Event event;
    sf::View view;
    view.setCenter(1.f,0.65f);
    view.setSize(2.2f,-2.2f*6.f/8.f);
    window.setView(view);
    sf::RectangleShape outlineShape;
    outlineShape.setFillColor(sf::Color::White);
    outlineShape.setPosition(0,0);
    outlineShape.setOutlineThickness(0.01f);
    outlineShape.setOutlineColor(sf::Color::Black);
    outlineShape.setSize({2.f,1.f});

    
    line.setPrimitiveType(sf::LineStrip);
    offsetLine.setPrimitiveType(sf::LineStrip);
    groundLine.setPrimitiveType(sf::LineStrip);
    gen();

    ImGui::SFML::Init(window);
    bool showLines = false;
    
    sf::Clock clock;
    while(window.isOpen())
    {
        ImGui::SFML::Update(window,clock.restart());

        ImGui::Begin("Hello, world!");
        if(
            ImGui::SliderFloat("x_offset", &x_offset, 0.f, 64.f, "%.1f") ||
            ImGui::SliderFloat("period", &period, 1.f, 10.f, "%.2f") ||
            ImGui::SliderFloat("variance", &variance, 0.f, 1.f, "%.05f")
        )
        {
            gen();
        }
        if( ImGui::Button("Show") )
        {
            showLines = !showLines;
        }
        ImGui::End();

        window.clear(sf::Color::White);
        window.draw(outlineShape);
        for(auto &c : nodeShapes) window.draw(c);
        if(showLines){
        window.draw(line);
        window.draw(offsetLine);}
        window.draw(groundLine);
        ImGui::SFML::Render(window);
        window.display();

        window.waitEvent(event);
        ImGui::SFML::ProcessEvent(event);
        if(event.type == sf::Event::Closed) window.close();// || event.type == sf::Event::KeyPressed) window.close();

    }

    ImGui::SFML::Shutdown();

    int mini = 0;
    float minf = 10000.f;
    float maxf = -111.f;
    float maxi = 0;
    for(int i = 0; i < nodes.size(); i++)
    {
        if(nodes[i].p.x < minf)
        {   
            mini = i;
            minf = nodes[i].p.x;
        }
        if(nodes[i].p.x > maxf)
        {   
            maxi = i;
            maxf = nodes[i].p.x;
        }
    }

    std::ofstream o("points");
    for(uint i = 0; i < nodes.size(); i++)
    {
        o << nodes[i].p.x << " " << nodes[i].p.y << " ";
    }

    return 0;
}