#ifndef LINEGENERATOR_HPP
#define LINEGENERATOR_HPP

#include <vector>
#include <unistd.h>
#include <list>
#include "../SFML_Functions/SFML_Functions.hpp"

class LineGenerator
{
public:
    LineGenerator();//const std::vector<Point> &_points);
    ~LineGenerator();
    void generate(const std::vector<Point> &_points);
    void advance();
    void render(sf::RenderTarget &target);
    bool setup;
    bool done;
    void setSmoothSliver(float a, float b);
    void resetSmoothness();
    void setTinySegs(float sz){minLength = sz;}
private:
float minLength;

bool lastWasMerge;
    std::list<Point>::iterator closestPoint;
    float closestDist2;
    void getNextClosestPoint(); //list iterator
    bool isLess(std::list<Point>::iterator p1, std::list<Point>::iterator p2);
    bool findClosestCollision();
    void handleCollision();
    std::list<Point> points;
    std::list<Point> line;
    std::list<std::pair<std::list<Point>::iterator, std::list<Point>::iterator> > possibleCollisionLines;
//    std::list<std::list<Point>::iterator> possibleCollisionLines;
    std::list<Point>::iterator closestIntPt;

    std::vector<sf::Vertex> testLines;
    std::vector<sf::Vertex> alltestLines;
    std::vector<sf::Vertex> opttestLines;
    std::vector<sf::Vertex> closeLines;
    sf::VertexArray lineShape;
    sf::VertexArray newShape;
    sf::CircleShape C;
    std::vector<sf::CircleShape> pointShapes;
    std::vector<sf::CircleShape> closePoints;
};

#endif //LINEGENERATOR_HPP