#ifndef LINE_OPTIMIZATIONS_HPP
#define LINE_OPTIMIZATIONS_HPP

#include <list>
#include "../SFML_Functions/SFML_Functions.hpp"

typedef std::pair<std::list<Point>::iterator, std::list<Point>::iterator> ItPair;

struct PointFix
{
    std::list<Point>::iterator it;
    Point fixVector;
};


void removeTinySegments(std::list<Point> &line, float minLength);
void removeJaggiesAndFlatties(std::list<Point> &line, float sliverVal, float smoothVal);
void markPinchPoints(std::list<Point> &line, float minThickness, float minClearance, std::list<ItPair> &squeezePts, std::list<ItPair> &thinPts);
void fixPinchPoints(std::list<Point> &line, float minThickness, float minClearance);

bool delayedfixPinchPoints(std::list<Point> &line, float minThickness, float minClearance, float variance, std::vector<PointFix> &fixes);
void fixPinchPointsStep(std::list<Point> &line, float minThickness, float minClearance, std::list<Point>::iterator l0, std::list<Point>::iterator p);

void subdivideLine(std::list<Point> &line, float maxLength);//or minCLearance

#endif //LINE_OPTIMIZER_HPP