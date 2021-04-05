//version 2.0 same program
#include "PolyTriangulator.hpp"
#include "PolyLoader.hpp"
#include "SFPoly.hpp"

#include <iostream>
#include <string>

#include <fstream>

float epsilon = 0.00002f;//0.0002f;//test purposes added extra 0

void makeGridLines(std::vector<sf::Vertex> &gridlines)
{
    sf::Vertex V;
    V.color = sf::Color(200,200,200,50);
    //0-12 so at 1 - 11 both ways...
    for(int i = 0; i<11; i++)
    {
        //x at y = ...
        V.position.x = 0;
        V.position.y = i + 1;
        gridlines.push_back(V);
        V.position.x = 12;
        gridlines.push_back(V);
        V.position.y = 0;
        V.position.x = i + 1;
        gridlines.push_back(V);
        V.position.y = 12;
        gridlines.push_back(V);
    }
}

int main(int argc, char** argv)
{
    /*
    Polygon P = 
    {
        {1, 3},
        {1, 4},
        {2, 4},
        {2, 5},
        {3, 5},
        {3, 6},
        {5, 6},
        {5, 5},
        {4, 5},
        {4, 4},
        {5, 4},
        {5, 3},
        {4, 3},
        {4, 2},
        {5, 2},
        {5, 1},
        {3, 1},
        {3, 3}
    };*/

    /*Polygon P = 
    {
        {1.f,1.f},
        {6.f,4.f},
        {5.f,2.f},
        {8.f,3.f},
        {8.f,6.f},
        {4.f,6.f},
        {9.f,8.f},
        {9.f,1.f}
    };*/
/*
    Polygon P = 
    {
        {0,2},
        {4,1},
        {6,4},
        {4,7},
        {1,4},
        {2,4.5},
        {2,2}
    };
*/
    srand(time(NULL));

    Polygon P_main;
    PolyLoader PL;
    std::string file;
    if(argc > 1) file = argv[1];
    else file = "data";
    PL.loadPoly(file,P_main);

    float min_x, max_x, min_y, max_y;

    sf::Transform T;
    //T.scale(0.04f,0.04f);
    T.translate(6.f,6.f);
    //T.rotate(11.f);
    T.rotate(15.f);//15.f);//90.f);
    //T.rotate(123.f);//about 6,6
    T.translate(-6.f,-6.f);

    Polygon P = P_main;
    //for(int i = 0; i<6; i++)
    //for(auto &point : P) point = T.transformPoint(point);

 //   for(auto &point : P)
    {
//        point = T.transformPoint(point);
  //      snapToInt(point.x);
    //    snapToInt(point.y);
    }
    //for(auto &point : P) point = T.transformPoint(point);
    
    min_x = max_x = P[0].x;
    min_y = max_y = P[0].y;
    for(const auto &p : P)
    {
        if(p.x < min_x) min_x = p.x;
        else if(p.x > max_x) max_x = p.x;
        if(p.y < min_y) min_y = p.y;
        else if(p.y > max_y) max_y = p.y;
    }
    epsilon *= (max_x - min_x);

    sfPoly sfpoly;
    sfpoly.set(P);

    PolyTriangulator PT(P);
    //PT.sweep2(emitLines);

    std::vector<Trap> trapezoids;
    std::vector<sf::Vertex> trapShapes;
    std::vector<int> numTriPPoly;
    try {PT.sweep4(numTriPPoly);}
    catch(...){;}
    //PT.sweep4(numTriPPoly);
    convertTraps(PT.getTraps(),trapShapes);
    //const std::vector<sf::Vertex>& emitLines = PT.getLines();

    std::vector<Polygon> monoPolygons;
    getMonotonePolygons(monoPolygons,PT.getVertices(),PT.getMonotonePolygons(),P);
    std::vector<sfPoly> drawableMonoPolys;
    for(auto &monoP : monoPolygons)
    {
        sfPoly sfp;
        sfp.set(monoP);
        drawableMonoPolys.push_back(sfp);
    }

    std::vector<sf::Vertex> triangles;
    getTriangles(triangles, PT.getTriangles(), P, numTriPPoly);

    unsigned int shapeNum = 0;

    std::vector<sf::Vertex> gridlines;
    makeGridLines(gridlines);

    sf::RenderWindow window(sf::VideoMode(850, 850), "Poly");

    sf::View view;
    float maxWidth = fmax((max_x - min_x), (max_y - min_y));
    view.setSize(maxWidth * 1.1f, maxWidth * -1.1f);
    view.setCenter((max_x + min_x)*0.5f,(max_y + min_y)*0.5f);
    //view.setCenter(150.f, 200.f);
//    view.setSize(400.f, 400.f);
//    view.setCenter(6.f, 6.f);
//    view.setSize(14.f, -14.f);
    window.setView(view);

    sf::Event event;

    std::cout<<"trap nums "<<trapezoids.size()<<" "<<trapShapes.size()<<std::endl;
    for(auto V : trapShapes) std::cout<<V.position.x<<" "<<V.position.y<<std::endl;
    
    bool drawTraps = false;

    while (window.isOpen())
    {
        window.clear(sf::Color::White);
        window.draw(sfpoly);
        //window.draw(drawableMonoPolys[shapeNum]);
        window.draw(&gridlines[0], gridlines.size(), sf::Lines);
        //window.draw(&emitLines[0], emitLines.size(), sf::Lines);
        //window.draw(&PT.getLines()[0],PT.getLines().size(), sf::Lines);
        if(drawTraps)
        {window.draw(&trapShapes[0], trapShapes.size(), sf::Triangles);
        window.draw(&trapShapes[shapeNum * 6], 6, sf::Triangles);
        }
        //window.draw(&triangles[0],triangles.size(), sf::Triangles);
        if(!drawTraps){
        window.draw(&triangles[0],triangles.size(),sf::Triangles);
        window.draw(&triangles[shapeNum * 3], 3, sf::Triangles);}
        window.display();

        window.waitEvent(event);
        if (event.type == sf::Event::Closed)
            window.close();
        if (event.type == sf::Event::KeyPressed)
        {
            if(event.key.code == sf::Keyboard::Space)
            {
            //P = P_main;
            min_x = max_x = P[0].x;
            min_y = max_y = P[0].y;
            for (auto &p : P)
            {
                p = T.transformPoint(p);
                //snapToInt(point.x);
                //snapToInt(point.y);
                if(p.x < min_x) min_x = p.x;
                else if(p.x > max_x) max_x = p.x;
                if(p.y < min_y) min_y = p.y;
                else if(p.y > max_y) max_y = p.y;
            }
            maxWidth = fmax((max_x - min_x), (max_y - min_y));
            view.setSize(maxWidth * 1.1f, maxWidth * -1.1f);
            view.setCenter((max_x + min_x) * 0.5f, (max_y + min_y) * 0.5f);
            window.setView(view);
            trapShapes.clear();
            PT.init();
            trapezoids.clear();
            numTriPPoly.clear();
            triangles.clear();
            PT.sweep4(numTriPPoly);
            getTriangles(triangles, PT.getTriangles(), P, numTriPPoly);
            trapShapes.clear();
            convertTraps(PT.getTraps(), trapShapes);
            sfpoly.set(P);
            }
            else if(event.key.code == sf::Keyboard::T)
            {
                shapeNum++;
                if(!drawTraps) {if(shapeNum == (triangles.size()/3)) shapeNum = 0;}
                else {if(shapeNum == (trapShapes.size()/6)) shapeNum = 0;}
            }
            else if(event.key.code == sf::Keyboard::R)
            {
                drawTraps = !drawTraps;
                shapeNum = 0;
            }
            else
            window.close();
        }
    }

    std::ofstream outFile("trapData");
    
    for(auto &trap : PT.getTraps())
    {
        outFile << trap.topLeft.x << " " << trap.topLeft.y << " ";
        outFile << trap.bottomLeft.x << " " << trap.bottomLeft.y << " ";
        outFile << trap.topRight.x << " " << trap.topRight.y << " ";
        outFile << trap.topRight.x << " " << trap.topRight.y << " ";
        outFile << trap.bottomRight.x << " " << trap.bottomRight.y << " ";
        outFile << trap.bottomLeft.x << " " << trap.bottomLeft.y << " ";
    }
    outFile.close();

    outFile.open("triangleData");

    for (auto &tri : PT.getTriangles())
    {
        outFile << tri.
        outFile << trap.topLeft.x << " " << trap.topLeft.y << " ";
        outFile << trap.bottomLeft.x << " " << trap.bottomLeft.y << " ";
        outFile << trap.topRight.x << " " << trap.topRight.y << " ";
        outFile << trap.topRight.x << " " << trap.topRight.y << " ";
        outFile << trap.bottomRight.x << " " << trap.bottomRight.y << " ";
        outFile << trap.bottomLeft.x << " " << trap.bottomLeft.y << " ";
    }
    outFile.close();

    return 0;
}
