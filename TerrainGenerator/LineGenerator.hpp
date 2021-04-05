#ifndef LINEGENERATOR_HPP
#define LINEGENERATOR_HPP

//This is fine...
//make more generic...
//as in it works on a line 'segment' from start point to end point
//add the points to the 'segment'
//thus can add future lines
//or connect together.

//not so much a line 'generator'
//more a line confuciusater.

#include <vector>
#include <unistd.h>
#include <list>
#include "../SFML_Functions/SFML_Functions.hpp"

#include "LineOptimizations.hpp"

typedef std::pair<std::list<Point>::iterator, std::list<Point>::iterator> ItPair;

class LineGenerator
{
public:
    LineGenerator();
    ~LineGenerator();
    void generate(const std::vector<Point> &points);
    bool step();
    const std::list<Point> &getLine(){return m_line;}
    void fix();
    void fixStep();
    void fixSetup();
    void setup(const std::vector<Point> &points);
    float m_maxBacktrackDistance;
    float m_x;
    bool isClosed;
    float minSegLength;
    const std::list<ItPair> getPossibleCollisions(){return m_possibleCollisionLines;}
    //DEBUGGING START
    std::list<Point>::iterator ItL1;
    std::list<Point>::iterator ItL2;
    std::list<Point>::iterator ItPoint;
    std::list<Point>::iterator landZone;
    void createLandingZone();
    //DEBUGGING END
private:
    std::list<Point> m_pointList;
    std::list<Point> m_line;

    std::list<Point>::iterator m_nextPoint;
    std::list<Point>::iterator closestIntPt;
    float closestDist;
    std::list<std::pair<std::list<Point>::iterator, std::list<Point>::iterator>> m_possibleCollisionLines;

    bool addNextPoint();
    bool findClosestCollision();
    void handleCollision();
    void addInternalPoint();

    bool addedABackTrackPoint();//shouldn't be here

    void removeTinySegments();

    inline void addPossibleCollision(std::list<Point>::iterator a, std::list<Point>::iterator b)
    {
        m_possibleCollisionLines.push_back(std::pair<std::list<Point>::iterator, std::list<Point>::iterator>(a,b));
    }
};

#endif //LINEGENERATOR_HPP