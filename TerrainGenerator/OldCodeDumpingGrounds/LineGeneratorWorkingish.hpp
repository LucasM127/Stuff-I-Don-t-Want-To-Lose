#ifndef LINEGENERATOR_HPP
#define LINEGENERATOR_HPP

#include <vector>
#include <unistd.h>
#include <list>
#include "../SFML_Functions/SFML_Functions.hpp"
#include <stack>
#include <cmath>

class LineGenerator
{
public:
    LineGenerator(const std::vector<Point> &_points);
    bool advance();
    void render(sf::RenderTarget &target);
    bool setup;
    bool done;
private:
    std::list<Point>::iterator closestPoint;
    //float closestDist2;
    void getNextClosestPoint();//list iterator
    void optFix(std::list<Point>::iterator p1, std::list<Point>::iterator p2);

    struct itDist
    {
        float dist;
        std::list<Point>::iterator it;
    };
    std::vector<itDist> intersectors;

    bool findCollisions(std::list<Point>::iterator p1, std::list<Point>::iterator p2);
    void handleCollisions(std::list<Point>::iterator p1, std::list<Point>::iterator p2);
    std::list<Point> points;
    std::list<Point> line;
    std::list < std::pair<std::list<Point>::iterator, std::list<Point>::iterator > > possibleCollisionLines;
    std::list<std::pair<std::list<Point>::iterator, std::list<Point>::iterator>> optCheckLines;

    std::vector<sf::Vertex> testLines;
    std::vector<sf::Vertex> opttestLines;
    sf::VertexArray nextLine;
    sf::VertexArray lineShape;
    sf::CircleShape C;
    std::vector<sf::CircleShape> pointShapes;
};

#endif //LINEGENERATOR_HPP