#include "PlotterManipulator.hpp"
#include "ValleyPlotter.hpp"
//#include "PointPlotter.hpp"
#include "../../SFML_Functions/SFML_Functions.hpp"
#include <iostream>
#include <fstream>

void drawPoints(const std::vector<Point> &points, sf::VertexArray &V, const sf::Color &c)
{
    V.clear();
    
    for(auto &p : points) V.append({p, c});
}

int main()
{
    unsigned int width = 640;//1400;//900;
    unsigned int height = 640;//640;//700;//500;//600;

    sf::Font font;
    if(!font.loadFromFile("../../Moonlander/Soft Elegance.ttf")) throw std::runtime_error("No font");

    PerlinPlotter plotter({(float)width, (float)height});
    plotter.setDistribution(Plotter::Distribution::NORMAL);
    //plotter.plot();

    /*std::random_device random_device;
    std::mt19937 random_engine(random_device());
    std::uniform_real_distribution<float> uniform_dist(-1.0, 1.0);
    plotter2.offset[0] = uniform_dist(random_engine);*/
    
    sf::RenderWindow window(sf::VideoMode(width, height), "Plotter");

    sf::View view = window.getDefaultView();
    sf::View refView = view;
    CameraController camera(window, view);

    sf::VertexArray drawnBaseLine;
    //drawnBaseLine.setPrimitiveType(sf::LineStrip);
    sf::VertexArray drawnVarianceLine;
    //drawnVarianceLine.setPrimitiveType(sf::LineStrip);

    std::list<Point> points;
    float clearDist = 5.f;
    int numPts = 20;//500;

    sf::VertexArray drawnPoints;
    //drawnPoints.setPrimitiveType(sf::Points);

//    plotter.randomize();
    srand(time(NULL));
    //rectangular plot width by height/2
    for(int i = 0; i<numPts; i++)
    {
        points.push_back({(float)(rand()%(width*10))/10.f,(float)(rand()%(height*10))/10.f});
    }
    //plotter.plot(points, numPts, clearDist);
    points.insert(points.begin(),{-1,height/2});
    points.insert(points.end(),{width+1,height/2});
    /*
points.clear();
srand(time(NULL));
    for(uint i = 0; i < 64; ++i)
    {
        float x = rand()%64;
        float y = rand()%64;
        x *= 10.f;
        y *= 10.f;
for(auto &p : points) if(p.x == x && p.y == y) continue;
        points.push_back({x,y});
    }
    std::cout<<points.size()<<std::endl;
*/
    drawnPoints.clear();
    for(auto &point : points) drawnPoints.append({point, sf::Color::Cyan});

    //PlotterManipulator manip(plotter, font, points);

    while (window.isOpen())
    {
        //drawPoints(plotter.getBaseLine(), drawnBaseLine, sf::Color::White);
        //drawPoints(plotter.getVarianceLine(), drawnVarianceLine, sf::Color::Red);
        drawnPoints.clear();
        for (auto &point : points)
            drawnPoints.append({point, sf::Color::Cyan});

        window.clear(sf::Color::Black);
        //window.draw(drawnBaseLine);
        //window.draw(drawnVarianceLine);
        window.draw(drawnPoints);
        window.setView(refView);
        //manip.render(window);
        window.setView(view);
        window.display();

        sf::Event event;
        window.waitEvent(event);
        switch (event.type)
        {
        case sf::Event::Closed:
            window.close();
            break;
        case sf::Event::Resized:
        {
            refView.setSize(event.size.width, event.size.height);
            refView.setCenter(refView.getSize() / 2.f);
            break;
        }
        case sf::Event::KeyPressed:
        {
            if(event.key.code == sf::Keyboard::Escape) window.close();
        }
            break;
        default:
            break;
        }
//        if(manip.handleEvent(event))
        {
            //plotter.plot(points, numPts, clearDist);
            ;
        }
  //      else
            camera.handleEvent(event);
    }

    std::ofstream outfile;
    outfile.open("pointData");
    for(auto &p : points)
    {
        outfile << p.x << " " << p.y << " ";
    }
    outfile.close();

    return 0;
}