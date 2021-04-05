#include <vector>
#include <unistd.h>
#include <list>
#include "../SFML_Functions/SFML_Functions.hpp"
#include <stack>
#include <cmath>

unsigned int width = 800;
unsigned int height = 600;
bool closed = false;

std::vector<std::list<Point>::iterator> possibleInts;
std::list<Point> points;
std::list<Point> line;
auto closestPoint = points.begin();

bool isLess(std::list<Point>::iterator i0,
            std::list<Point>::iterator i1)
{
    auto it = i0;
    while (true) //it != i1 && it != line.end())
    {
        if (it == i1)
            return true;
        if (it == line.end())
            return false;
        ++it;
    }
    return true;
}

void getNextClosestPoint()
{
    float closestDist2 = std::numeric_limits<float>::max();
    closestPoint = points.begin();
    for (auto it = points.begin(); it != points.end(); ++it)
    {
        float dist = SFUTIL::lengthSquared<float>((*it) - line.back());

        if (dist < closestDist2)
        {
            closestPoint = it;
            closestDist2 = dist;
        }
    }

    return;
}

void optFix(std::list<Point>::iterator p1, std::list<Point>::iterator p2)
{ //we just add it first
    auto i10 = p1;
    auto i20 = p2;
    if (!isLess(i10, i20))
    {
        std::swap(i10, i20);
    }
    auto i11 = std::next(i10);
    auto i21 = std::next(i20);

    possibleInts.push_back(i11);
    possibleInts.push_back(i20);
    possibleInts.push_back(i10);
    possibleInts.push_back(i20);
    
    std::list<Point> spliceList;
    spliceList.splice(spliceList.begin(), line, i11, i21);
    spliceList.reverse();
    line.splice(std::next(i10), spliceList);
}

void addPoint()
{
    
    if(points.size() == 0)
    {
        if(!closed)
        {
            line.push_back(line.front());
            closed = true;
        }
        bool hit = false;
        while (true)
        {
            hit = false;
            //ensure is free of intersections.
            for (auto it = line.begin(); it != std::prev(std::prev(line.end())); ++it)
            {
                for(auto it2 = std::next(it); it2 != std::prev(line.end()); ++it2)
                {
                    if (SFUTIL::lineSegsIntersect(*it, *std::next(it), *it2, *std::next(it2)))
                    {
                        optFix(it, it2);
                        hit = true;
                        break;
                    }
                }
            }
            if (!hit)
                break;
        }
        LOG("DONE");
        return;
    }
    //add a point and 2opt it out
    getNextClosestPoint();
    line.splice(line.end(),points,closestPoint);
    possibleInts.push_back(std::prev(line.end()));
    possibleInts.push_back(std::prev(std::prev(line.end())));

    while (possibleInts.size())
    {
        auto p1 = possibleInts[0];
        auto p2 = possibleInts[1];
        bool hit = false;
        for(auto it = line.begin(); it != std::prev(line.end()); ++it)
        {
            auto it2 = std::next(it);
            if(it == p1 || it == p2 || it2 == p1 || it2 == p2) continue;
            if(SFUTIL::lineSegsIntersect(*it, *it2, *p1, *p2))
            {
                optFix(it, p1);
                hit = true;
                break;
            }
        }
        if(hit == false) possibleInts.clear();
    }
    
}

void seed()
{
    
    for (uint i = 0; i < 128; i++) //128
    {
        float x = rand() % width;                                                                 //
        float y = rand() % height;
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
    for (uint i = 0; i < 128; i++) //128
    {
        float x = rand() % width;                                                                 //
        float y = /*rand()%height;* / (fabsf(x - (width / 2)) / width) * (float)(rand() % height); //f(x)
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
    }*/
    //points.push_back({2000.f, 0.f});

    LOG(points.size(), " points created");
}

int main(int args, char **argv)
{
    int rand = time(NULL); //3;
    if (args == 2)
        rand = std::stoi(argv[1]);

    srand(rand); //time(NULL));

    sf::RenderWindow window(sf::VideoMode(width, height), "Line");
    sf::View view(window.getDefaultView());
    view.setSize(view.getSize().x, -view.getSize().y);
    window.setView(view);
    CameraController camera(window, view);

    seed();

    std::vector<sf::Vertex> lineShape;

    sf::CircleShape C;
    C.setFillColor(sf::Color(80, 80, 128, 128));
    C.setRadius(5.f);
    C.setOrigin(5.f, 5.f);

    std::vector<sf::CircleShape> shapes;
    for (auto &point : points)
    {
        C.setPosition(point);
        shapes.push_back(C);
    }

    //line.push_back({-1000,0});
    line.splice(line.end(), points, points.begin());
//    line.splice(line.end(), points, points.begin());

    sf::Event event;

    while (window.isOpen())
    {
        window.clear(sf::Color::Black);
        for (auto &s : shapes)
            window.draw(s);
        window.draw(&lineShape[0], lineShape.size(), sf::LineStrip);
        window.display();

        window.waitEvent(event);
        switch (event.type)
        {
        case sf::Event::KeyPressed:
            if (event.key.code == sf::Keyboard::Space)
            {
                addPoint();
                lineShape.clear();
                for(auto &p : line) lineShape.push_back({p,sf::Color::Green});
                break;
            }
        case sf::Event::Closed:
            window.close();
            break;
        default:
            break;
        }
        camera.handleEvent(event);
    }

    std::ofstream ofs("outfile");
    if (!ofs.is_open())
        return -1;
    for (auto &point : line)
    {
        ofs << point.x << " " << point.y << std::endl;
    }
    ofs.close();

    return 0;
}