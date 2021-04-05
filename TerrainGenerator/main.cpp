//so far is pretty good...
//fix my fixer to fix angles instead of cross produtc/ dot product

//Plotter
//f(x) upper, and g(x) bottom may or may not be related or not or cross or whatever, bounds
//variance, distribution. normal or linear distribution of random numbers

//Line
//Line just worries about the line
//what can you do with a line?
//Add in a point
//Remove a point
//move a point
//worries about interpreting it in ... which line to subtend, how to deal with self intersections etc...

//Order of adding points to line matters too...
//add them randomly
//or sort them by x or by y (so grows upwards)
//how does it effect final product
//ie caves to bottom of map.  add them after you create line, works as intended, otherwise not.

#include "LineGenerator.hpp"
#include "LineOptimizations.hpp"
#include "SFML/Graphics.hpp"
#include <fstream>

#include <iostream>

unsigned int width = 800;
unsigned int height = 600;
std::vector<Point> points;
//i seed my points at 50 clearance and min thickness set to 50 seems to work fine.
//seed a hole?
//x2 + y2 = r2 (x-400) (y-300) r = 200
void plot()
{
    srand(time(NULL));
    points.clear();
//Valley distribution |\/| shape
    points.push_back({0.f,height/2});
    points.push_back({width, height/2});
    for (uint i = 0; i < 128; i++) //128
    {
        float x = rand() % width; //
        float y = -1.f * (fabsf(x - (width / 2)) / width) * (float)(rand() % height) + height; //f(x)
        Point pos(x, y);                                                                          //rand()%width,rand()%(height/8)+(height/2));
        bool collide = false;
        for (auto &point : points)
        {
            Point delta = pos - point;
            if (delta.x * delta.x + delta.y * delta.y < 100.f)
            {
                collide = true;
                break;
            }
        }
        if (collide)
            continue;
        points.push_back(pos);
    }
/*
            for (uint i = 0; i < 1000; ++i) //64; ++i)
        {
            float x = rand() % 1000;
            float y = rand() % 1000;
            x /= 1000.f;
            y /= 1000.f;
            x *= width;
            y *= height;
            Point pos(x, y);

            //if( x > 200 && x < 400 && y < 200 ||//)
            if (((x - 400) * (x - 400) + (y - 300) * (y - 300) < 200 * 200) || ((x - 400) * (x - 400) + (y - 300) * (y - 300) > 400 * 400))
            {
                --i;
                continue;
            }

            bool collide = false;
            for (auto &point : points)
            {
                Point delta = pos - point;
                if (delta.x * delta.x + delta.y * delta.y < 2500.f) //25.f)//100.f)
                {
                    collide = true;
                    break;
                }
            }
            if (collide)
            {
                //--i;
                continue;
            }
            //y = -y;
            points.push_back({x, y});
        }*/
}


//normalize it to 0 - 1?
//scale the transform by x * length, y * 1
sf::Transform  getLineTransform(Point &a, Point &b)
{
    //translation = a
    Point d = b - a;
    float length = sqrtf(d.x * d.x + d.y * d.y);
    //cos -sin
    //sin cos
    float cos = d.x/length;//a/h
    float sin = d.y/length;

    return sf::Transform(   d.x, -sin, a.x,
                            d.y, cos,  a.y,
                            0,   0,     1);
}//YAY!
//THE MATRIX OF CHAMPIONS

Point  getRandPoint(float h)
{
    float x = 1.f * (float)(rand()%100)/100.f;
    float y = h * (0.5f - fabsf(0.5f - x)) * (float)(rand()%100)/100.f;
    return Point(x,y);
}

//and it doesn't work of course :(
int main()
{
    
    //128 rands per thingy...
    //for(uint i = 0; i<13*128; ++i) rand();
    //plot();
    //plot();
    /*
    std::ifstream infile;
    infile.open("Plotter/pointData");
    if(!infile.is_open()) return 0;
    while(infile)
    {
        Point p;
        infile >> p.x >> p.y;
        if(infile.eof()) break;
        points.push_back(p);
    }*/

    //add some points... x = 500 y = 800?
    //elipse
    //almost needs a function library for this shit too
    srand(time(NULL));
    //just random?

    std::vector<Point> poly = 
    {
        {178.57,197.428},
        {141.43,182.572},
        {161.371,143.614},
        {119.395,109.508},
        {75.1851,143.016},
        {43.359,141.094},
        {31.6f,55.f},//{31.6795,105.547},
        {48.3f,44.f},//{48.3205,94.453},
        {54.2256,99.5517},
        {89.6297,33.9683},
        {120,20},
        {190,50},
        {190,70},
        {169.936,78.5987},
        {192,131},
        {193.927,145.571}
    };
    sf::VertexArray polyShape;
    polyShape.setPrimitiveType(sf::LineStrip);
    for(auto &p : poly) polyShape.append({p,sf::Color(200,200,200,125)});
    //polyShape.append(polyShape[0]);

    std::vector<sf::Transform> lineTransforms;
    std::vector<float> linelengths;
    std::vector<int> iterations;
    for(uint i = 0; i<poly.size()-1; i++)
    {
        sf::Transform T = getLineTransform(poly[i+1],poly[i]);
        lineTransforms.push_back(T);
        float relLength = fabs(poly[i+1].y - poly[i].y) + fabs(poly[i+1].x - poly[i].x);
        linelengths.push_back(relLength);
    }
    iterations.resize(lineTransforms.size(),0);
    //iterations[7] = iterations[8] = 0;
//reverse it...
    uint curEdgeId = 0;
    bool done = false;
    while(!done)
    {
        int ctr = 0;
        while(iterations[curEdgeId] >= (int)linelengths[curEdgeId]/6)
        {
            if(ctr == linelengths.size())
            {
                done = true;
                break;
            }
            ctr++;
            curEdgeId++;
            if(curEdgeId >= linelengths.size()) curEdgeId = 0;
        }

        Point P = getRandPoint(40.f);//linelengths[curEdgeId]/3.f);
        P = lineTransforms[curEdgeId].transformPoint(P);

        bool collided = false;
        for(auto &p : points)
        {
            if(p == P)
            {
                collided = true;
                std::cout<<"OOPS"<<std::endl;
                break;
            }
        }
        if(!SFUTIL::pointInPoly<float>(&poly[0],poly.size(),P) && !collided)
        points.push_back(P);

        iterations[curEdgeId]++;

        curEdgeId++;
        if(curEdgeId >= linelengths.size()) curEdgeId = 0;
    }


    for(uint i = 0; i<poly.size()-1; i++)//for each edge... gen random points...
    {
        sf::Transform T = getLineTransform(poly[i+1],poly[i]);
        float relLength = fabs(poly[i+1].y - poly[i].y) + fabs(poly[i+1].x - poly[i].x);

        
    }
    /*
    for(int i = 0; i< 150; i++)
    {
        float x = rand()%200;
        float y = rand()%200;
        Point P(x,y);
        bool collided = false;
        for(auto &p : points)
        {
            if(p == P)
            {
                collided = true;
                std::cout<<"OOPS"<<std::endl;
                break;
            }
        }
        if(!SFUTIL::pointInPoly<float>(&poly[0],poly.size(),P) && !collided)
            points.push_back(P);
        //(x - 500)^2 + (y -600)^2/4 = r^2
    }*/
    /*
    points.push_back({500, 700});
    points.push_back({400, 700});
    points.push_back({400, 600});
    points.push_back({500, 600});
    */

   points.clear();
   std::ifstream infile;
    infile.open("Plotter2/points");
    if(!infile.is_open()) return 0;
    while(infile)
    {
        Point p;
        infile >> p.x >> p.y;
        if(infile.eof()) break;
        p = p*100.f;
        points.push_back(p);
    }
    std::random_shuffle(points.begin()+1, points.end() - 1);

    LineGenerator LG;
    LG.m_x = 0.f;
    LG.m_maxBacktrackDistance = 400.f;
    //points.push_back({100000.f, -10000.f});
    //LG.generate(points);
    //LG.fixSetup();
    LG.setup(points);
    //LG.fix();
    //LG.fix();

    

    float r = 1.f;
    sf::CircleShape pointShape;
    pointShape.setFillColor(sf::Color(255,255,0,32));//128));
    pointShape.setOrigin(r,r);
    pointShape.setRadius(r);
    std::vector<sf::CircleShape> pointShapes;

    for (auto &point : points)
    {
        pointShape.setPosition(point);
        pointShapes.push_back(pointShape);
    }

    sf::Vertex testLine[2];
    testLine[0].color = testLine[1].color = sf::Color::Green;

    sf::VertexArray CollisionLines(sf::Lines);
    sf::VertexArray LineShape;
    LineShape.setPrimitiveType(sf::LineStrip);
    sf::VertexArray LineShape2;
    LineShape2.setPrimitiveType(sf::LineStrip);
    sf::Vertex V;
    V.color = sf::Color::Cyan;
    sf::Vertex V2;
    V2.color = sf::Color(255,0,0,128);
    for(auto &p : LG.getLine())
    {
        V.position = p;
        LineShape.append(V);
    }
    //LG.fix();
    LineShape2.clear();
    for (auto &p : LG.getLine())
    {
        V2.position = p;
        LineShape2.append(V2);
    }

    //testLine[0].position = *LG.ItL1;
    //testLine[1].position = *LG.ItL2;
    //pointShape.setPosition(*LG.ItPoint);

    sf::RenderWindow window(sf::VideoMode(640,640),"TEST");//width,height),"LineTest");
    sf::Event event;

    sf::View view;
    view.setCenter(150,150);(width/2,height/2);
    view.setSize(300,-300);//(width*1.1,height*1.1);
    window.setView(view);
    bool bshow = false;
    bool showLandZone = false;
    sf::VertexArray landZoneLine;
    landZoneLine.setPrimitiveType(sf::Lines);

    CameraController camera(window, view);

    while (window.isOpen())
    {
        window.clear(sf::Color::Black);
        for(auto &c : pointShapes) window.draw(c);
        if(bshow) window.draw(LineShape);
        window.draw(LineShape2);
        window.draw(pointShape);
        window.draw(CollisionLines);
        //window.draw(polyShape);
        if(showLandZone) window.draw(landZoneLine);
        //window.draw(testLine, 2, sf::Lines);
        window.display();

        window.waitEvent(event);
        if(event.type == sf::Event::Closed)
            window.close();
        if(event.type == sf::Event::KeyPressed)
        {
            if(event.key.code == sf::Keyboard::Space)
            {
                bshow = !bshow;
                continue;
            }
            if(event.key.code == sf::Keyboard::Escape)
            {
                window.close();
                break;
            }
            /*
            plot();
            LG.generate(points);
            LineShape.clear();
            for (auto &p : LG.getLine())
            {
                V.position = p;
                LineShape.append(V);
            }
            LG.fix();
            */
                LineShape2.clear();
                for (auto &p : LG.getLine())
                {
                    V2.position = p;
                    LineShape2.append(V2);
                }
                //plot();
                //while(LG.step());
                if(!LG.step()&&!showLandZone)
                {
                    /*
                    LG.fix();
                    //plot();
                    //LG.setup(points);
                    landZoneLine.append({*LG.landZone, sf::Color::Green});
                    landZoneLine.append({*std::next(LG.landZone), sf::Color::Green});
                    showLandZone = true;*/
                }
                
                //LG.step();
                CollisionLines.clear();
                for(auto pair : LG.getPossibleCollisions())
                {
                    CollisionLines.append({*pair.first, sf::Color::Magenta});
                    CollisionLines.append({*pair.second, sf::Color::Magenta});
                }
                if(CollisionLines.getVertexCount() > 1)
                {
                    CollisionLines[0].color = sf::Color::Green;
                    CollisionLines[1].color = sf::Color::Green;
                }
                /***
                LG.fixStep();
                LineShape.clear();
                for (auto &p : LG.getLine())
                {
                    V.position = p;
                    LineShape.append(V);
                }
                testLine[0].position = *LG.ItL1;
                testLine[1].position = *LG.ItL2;
                pointShape.setPosition(*LG.ItPoint);
                ***/
        }

        camera.handleEvent(event);
    }

    float x_min = 1000000;
    float x_max = -100;
    float y_min = 1000000;
    std::ofstream outFile;
    outFile.open("terrainPoints");
    for(auto &p : LG.getLine())
    {
        if(p.x < x_min) x_min = p.x;
        if(p.x > x_max) x_max = p.x;
        if(p.y < y_min) y_min = p.y;
        outFile << p.x << " " <<p.y << " ";
    }
    y_min -= 10.f;
    outFile << x_max << " " << y_min << " ";
    outFile << x_min << " " << y_min << " ";

    return 0;
}