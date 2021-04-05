#include "delaunay.hpp"
#include <iostream>
#include <random>

using namespace Delaunay;

void drawEdge(sf::VertexArray &lineShape, std::vector<Point> &points, const std::vector<PointId> &edges, const std::vector<EdgeId> &hull)//convex Hull
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
void drawTriangles(sf::VertexArray &shape, std::vector<Point> &points, const std::vector<PointId> &edges)
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

void drawCircumcircles(std::vector<sf::CircleShape> &circles, std::vector<Point> &points, const std::vector<PointId> &edges)
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
/*
void printout(const Delaunay &D)
{
    const std::vector<PointId> &edges D.getTriangleIndices();
    const std::vector<EdgeId> &mates = D.getTriangleMates();
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
}*/
/*
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
}*/

int main()
{
    std::vector<Point> points;

    std::default_random_engine generator;
    std::uniform_real_distribution<float> distributionx(0.0,100.0f);
    //fails...
    std::uniform_real_distribution<float> distributiony(0.0,100.0f);

    points =
    {
        //{1,6},
        //{1,6},
        {2,5.33333},
        //{2,6},
        {2.5,5},
        {3,4.66667},
        //{3,6},
        //{4,1},
        //{4,1},
        //{4,2},
        //{4,3},
        //{4,4},
        //{4,4},
        //{4,6},
        //{4,6},
        {4.66667,2},
        {4.66667,7},
        {5,2.5},
        {5,7.5},
        {5.33333,3},
        {5.33333,8},
        //{6,4},
        //{6,4},
        //{6,6},
        //{6,6},
        //{6,7},
        //{6,8},
        //{6,9},
        //{6,9},
        //{7,4},
        {7,5.33333},
        {7.5,5},
        //{8,4},
        {8,4.66667}
        //{9,4},
        //{9,4}
    };
    
    for(uint i = 0; i< points.size(); i++)
    {
        for(uint j = i+1; j<points.size(); j++)
        {
            if(points[i] == points[j])
            {
                points.erase(points.begin()+j);
                j--;
            }
        }
    }

    for(int i = 1; i<10; i++)
        for(int j = 1; j<10; j++)//grid?
            points.push_back({i,j});
    
    //for(int i = 0; i<10000 ; i++)
    {
        //Point P(distributionx(generator),distributiony(generator));

        //points.push_back(P);
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
    //std::vector<sf::CircleShape> circles;
    //std::vector<sf::CircleShape> circumCircles;
    /*
    for (auto &p : points)
    {
        C.setPosition(p);
        circles.push_back(C);
    }*/
//pointId, hullId, isFirst?
    
    auto start = std::chrono::high_resolution_clock::now();
    Delaunay::Delaunay D(points);
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    std::cout << duration.count() << " microseconds" << std::endl; 

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

    drawEdge(hullShape,points,D.getTriangleIndices(),D.getHull());
    drawTriangles(triangleShape,points,D.getTriangleIndices());
    //drawCircumcircles(circumCircles);
//how to make the voronoi!!!
    //go throught them and find circumcenter points...
    //con

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
                D.growHull2();
                drawEdge(hullShape,points,D.getTriangleIndices(),D.getHull());
                drawTriangles(triangleShape,points,D.getTriangleIndices());
            }
            else window.close();
        }
    }
    std::cout<<"Hull size at end was: "<<D.getHull().size()<<std::endl;//98 for a million points
    return 0;
}