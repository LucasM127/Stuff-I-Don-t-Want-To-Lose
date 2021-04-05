#include "LineGenerator.hpp"
#include <cassert>
#include <fstream>
#include <cfloat>

//AND IT SUCKS

//MORE TODO:::
//Seed the points linearly.  randomly seems to cause fuckups again.

//#include <cmath>//this shouldn't belong for line 'fixing'

//landing zone criterion:
//the line can't be going left (deltaX negative)... is on the roof then (assuming base line is going right)
//we need to have a big enough 'gap' for lander to land into.
//shouldn't be a problem: due to clearDist of points, if the slope of the line is flat enough, there will be enough gap
//will there be a flat enough spot??? depends on map  ''' welll see'''
//then fix it... insert a 'FLAT LINE' land zone at that location...
//then rest of map can 'fuzzy line' for interest

void LineGenerator::createLandingZone()
{
    //start at a random point on the line
    srand(time(NULL));
    int landInt = rand() % (m_line.size() - 1);
    int i = 0;
    for (auto it = m_line.begin();; ++i, ++it)
    {
        if (i == landInt)
        {
            landZone = it;
            break;
        }
    }
    //find the closest point on line that has the proper values of dx and m
    while (true)
    {
        //assume not hit end iterator else just jump to begininnn?
        if(std::next(landZone) == m_line.end()) landZone = m_line.begin();
        float dx = std::next(landZone)->x - landZone->x;
        if (dx > 0.f)//a floor
        {
            float dy = std::next(landZone)->y - landZone->y;
            float m = dy/dx;
            if(fabsf(m) < 0.5)//26 degrees or so  1 is 45 degrees
            {
                //fix it up
                //if m is positive, put flat section at beginning of line
                //else put flat section at the end of the line
                //size of flat section is??? let's say 40? sure... cos is 30 but yeah
                //just inserting a point 40 from left or 40 from rigth is all we are doing
                Point insertPoint;
                //or JUST put it in the middle.
                Point midPoint = ( *landZone + *std::next(landZone) )/ 2.f;
                insertPoint.y = midPoint.y;
                insertPoint.x = midPoint.x - 5.f;
                m_line.insert(std::next(landZone), insertPoint);
                ++landZone;
                insertPoint.x = midPoint.x + 5.f;
                m_line.insert(std::next(landZone), insertPoint);
                /*
                //need to check the dot products... several cases... check for overhangs adjacent.
                //if dot product of the two lines is positive is overhang...
                //else no overhang... // may seg fault
                sf::Vector2f leftLineVec = *landZone - *std::prev(landZone);
                sf::Vector2f curLineVec = *std::next(landZone) - *landZone;
                sf::Vector2f rightLineVec = *std::next(std::next(landZone)) - *std::next(landZone);

                bool isOverhangLeft = SFUTIL::dot<float>(leftLineVec, curLineVec) < 0.f;
                bool isOverhangRight = SFUTIL::dot<float>(rightLineVec, curLineVec) < 0.f;
//4 cases to test
//if no overhangs at all either side can just put wherever so the left is fine...
//if overhangs to the right then put to left
//and if overhangs both sides ??? put in middle => insert 2 points
                if(m > 0.f)// / shape so is right
                {
                    insertPoint.y = landZone->y;
                    insertPoint.x = landZone->x + 8.f;
                    m_line.insert(std::next(landZone), insertPoint);
                }
                else// \ shape so 
                {
                    insertPoint.y = std::next(landZone)->y;
                    insertPoint.x = std::next(landZone)->x - 8.f;
                    m_line.insert(std::next(landZone), insertPoint);
                    ++landZone;
                }*/
                
                break;
            }
        }
        ++landZone;
    }
}


float findClosestPoint(const Point &givenPoint,
                       std::list<Point>::iterator &foundPoint,
                       std::list<Point> &list)
{
    float closestDist = std::numeric_limits<float>::max();
    foundPoint = list.begin();
    for (auto it = list.begin(); it != list.end(); ++it)
    {
        float dist = SFUTIL::lengthSquared<float>((*it) - givenPoint);

        if (dist < closestDist)
        {
            foundPoint = it;
            closestDist = dist;
        }
    }

    return closestDist;
} //more a helper function

float findClosestPoint(const std::list<Point>::iterator &givenPoint,
                        std::list<Point>::iterator &foundPoint,
                        std::list<Point> &list)
{
    float closestDist = std::numeric_limits<float>::max();
    foundPoint = list.begin();
    for (auto it = list.begin(); it != list.end(); ++it)
    {
        float dist = SFUTIL::lengthSquared<float>((*it) - (*givenPoint));

        if (dist < closestDist)
        {
            foundPoint = it;
            closestDist = dist;
        }
    }

    return closestDist;
}//more a helper function

typedef sf::Vector2f Vec2;

float distPointToLine(const Point &point, const Point &L0, const Point &L1)
{
    const Vec2 L = L1 - L0;
    const Vec2 P = point - L0;
    float dot = SFUTIL::dot(L, L);
    float range = (L.x * P.x + L.y * P.y) / dot;

    if (range < 0.0f)
    {
        return SFUTIL::length<float>(point - L0);
    }
    if (range > 1.f)
    {
        return SFUTIL::length<float>(point - L1);
    }
    float cross = SFUTIL::cross(P, L);
    float length = sqrtf(dot);
    return fabsf(cross / length);
}

float findClosestLineToPoint(const std::list<Point>::iterator &givenPoint,
                            std::list<Point>::iterator &foundPoint,
                            std::list<Point> &list)
{
    float closestDist = std::numeric_limits<float>::max();
    foundPoint = list.begin();
    for(auto it = list.begin(); it != std::prev(list.end()); ++it)
    {
        float dist = distPointToLine(*givenPoint, *it, *std::next(it));
        //dist *= dist / SFUTIL::length<float>(*it - *std::next(it));
        //dist /= SFUTIL::length<float>(*it - *std::next(it));
//But have to have a UNIFORM point distribution...
        if(dist + 0.0001f < closestDist)//or somtheing?????
        {
            foundPoint = it;
            closestDist = dist;
        }
    }

    return closestDist;//?
}

//SEEMS TO WORK FINE :) till it doesnt
float findClosestMidPoint(const std::list<Point>::iterator &givenPoint,
                       std::list<Point>::iterator &foundPoint,
                       std::list<Point> &list)
{
    float closestDist = std::numeric_limits<float>::max();
    foundPoint = list.begin();
    for (auto it = list.begin(); it != std::prev(list.end()); ++it)
    {
        float dist = SFUTIL::lengthSquared<float>(( ( (*it) + (*std::next(it)) ) / 2.f) - (*givenPoint));
        //reduce splinter segments running away while a long line segment stays unbroken :(
        //dist /= SFUTIL::length<float>((*it) - (*std::next(it)));//THIS IS THE ONE... a linear distribution, as square dist is wonkys
        dist /= SFUTIL::lengthSquared<float>((*it) - (*std::next(it)));
        //likewise below creates CRAZY splintering
        //dist *= SFUTIL::lengthSquared<float>((*it) - (*std::next(it)));

        if (dist < closestDist)
        {
            foundPoint = it;
            closestDist = dist;
        }
    }

    return closestDist;
} //more a helper function

bool isLess(std::list<Point>::iterator i0, std::list<Point>::iterator i1, std::list<Point> &list)
{
    auto it = i0;
    while (true)
    {
        if (it == i1)
            return true;
        if (it == list.end())
            return false;
        ++it;
    }
    return true;
}

LineGenerator::LineGenerator() : m_maxBacktrackDistance(100.f)
{
    //unAdded = true;
    landZone = m_line.end();
}

LineGenerator::~LineGenerator()
{
}

void LineGenerator::fixSetup()
{
    removeJaggiesAndFlatties(m_line, 2500.f, 0.f);
    ItL1 = m_line.begin();
    ItL2 = std::next(ItL1);
    ItPoint = m_line.begin();
}

void LineGenerator::fixStep()
{
    if(ItPoint == std::prev(ItL1) || ItPoint == ItL1 || ItPoint == std::next(ItL1) || ItPoint == std::next(std::next(ItL1)))
        ItPoint = std::next(std::next(std::next(ItL1)));
    
    fixPinchPointsStep(m_line, 50.f, 50.f, ItL1, ItPoint);

    ++ItPoint;
    if(ItPoint == m_line.end())
    {
        ItPoint == m_line.begin();
        ++ItL1;
        ItL2 = std::next(ItL1);
    }
}

void LineGenerator::fix()
{
    //removeJaggiesAndFlatties(m_line, 2500.f, 0.f);//5000.f, 0.f);
    //removeJaggiesAndFlatties(m_line, 5000.f, 0.f);
    //fixPinchPoints(m_line, 50.f,50.f);//50.f,50.f);//80.f, 20.f);//75.f);//50.f);
    std::vector<PointFix> fixes;

    uint ctr = 0;
    delayedfixPinchPoints(m_line, 10.f, 10.f, 0.f, fixes);
    for (auto &F : fixes)
    {
        *F.it += F.fixVector;
        addPossibleCollision(F.it, std::next(F.it));
        addPossibleCollision(std::prev(F.it), F.it);
    }
    fixes.clear();

    while (m_possibleCollisionLines.size())
    {
        if (findClosestCollision())
        {
            handleCollision();
        }
        m_possibleCollisionLines.pop_front();
    }

    while (delayedfixPinchPoints(m_line, 10.f, 10.f, 1.f, fixes))
    {
        if(ctr > 100) break;
        //for (auto &F : fixes)
        //    *F.it += F.fixVector;
        for (auto &F : fixes)
        {
            *F.it += F.fixVector;
            addPossibleCollision(F.it, std::next(F.it));
            addPossibleCollision(std::prev(F.it), F.it);
        }
        fixes.clear();
        ctr++;
        while (m_possibleCollisionLines.size())
        {
            if (findClosestCollision())
            {
                handleCollision();
            }
            m_possibleCollisionLines.pop_front();
        }
    }
    LOG(ctr, " fixes were applied");
    

    //delayedfixPinchPoints(m_line, 50.f,50.f, fixes);
    //for(auto &F : fixes) *F.it += F.fixVector;


//    fixPinchPoints(m_line, 50.f,50.f);
    //fixPinchPoints(m_line, 80.f, 20.f);
    removeJaggiesAndFlatties(m_line, 100.f, 100.f);//5000.f, 0.f);

    subdivideLine(m_line, 10.f);
    //find landing zoen
    //randomly
    //create it too...
//    createLandingZone();
}

void LineGenerator::setup(const std::vector<Point> &points)
{
    m_pointList.clear();

    for (auto point : points)
        m_pointList.push_back(point);
    m_line.push_back(m_pointList.front());
    m_line.push_back(m_pointList.back());

    //m_line.push_back({300.f, 0.f});
    //m_line.push_back({500.f, 0.f});
    /*

    auto minPt = m_pointList.begin();
    auto maxPt = m_pointList.begin();
    float minX = 1000000;
    float maxX = -100;
    for(auto it = m_pointList.begin(); it != m_pointList.end(); ++it)
    {
        if(it->x < minX)
        {
            minX = it->x;
            minPt = it;
        }
        else if(it->x > maxX)
        {
            maxX = it->x;
            maxPt = it;
        }
    }

    m_line = 
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
        {193.927,145.571}//,
//        {178.57,197.428}
    };*/
/*
    m_line.splice(m_line.end(), m_pointList, minPt);
    m_line.splice(m_line.end(), m_pointList, maxPt);
*/
    //findClosestPoint({300.f, 0.f}, pt, m_pointList);
    //m_line.splice(m_line.end(), m_pointList, pt);
//    m_line.push_back({0.f, 0.f});
    //findClosestPoint({0.f, 0.f}, pt, m_pointList);
    //m_line.splice(m_line.end(), m_pointList, pt);
    //findClosestPoint({0.f,600.f}, pt, m_pointList);
    //m_line.splice(m_line.end(), m_pointList, pt);
    //findClosestPoint({800.f, 600.f}, pt, m_pointList);
    //m_line.splice(m_line.end(), m_pointList, pt);
//    m_line.push_back({800.f, 0.f});
    //findClosestPoint({800.f, 0.f}, pt, m_pointList);
    //m_line.splice(m_line.end(), m_pointList, pt);
    //findClosestPoint({500.f, 0.f}, pt, m_pointList);
    //m_line.splice(m_line.end(), m_pointList, pt);
    //m_line.push_back(m_line.front());

    isClosed = false;
}

void LineGenerator::generate(const std::vector<Point> &points)
{
    m_pointList.clear();
    m_possibleCollisionLines.clear();
    m_line.clear();

    for (auto point : points)
        m_pointList.push_back(point);

    
    //try minx maxx points and just adding internally what we get ?
    //instead of using 'leader' lines at edges
    auto startPt = m_pointList.begin();
    auto endPt = m_pointList.begin();
    for(auto it = m_pointList.begin(); it != m_pointList.end(); ++it)
    {
        if(it->x < startPt->x) startPt = it;
        else if(it->x > endPt->x) endPt = it;
    }
    m_line.push_back({300.f,0.f});
    m_line.push_back({500.f,0.f});
    //m_line.push_back({0.f,0.f});
    //m_line.push_back({0.f,600.f});
    //m_line.push_back({800.f,600.f});
    //m_line.push_back({800.f,0.f});
    //m_line.splice(m_line.end(), m_pointList, startPt);
    //m_line.splice(m_line.end(), m_pointList, endPt);

    //m_line.push_back({FLT_MIN, 0.f});//handler line start... 

    while(addNextPoint())
    {
        //test for intersections
        while (m_possibleCollisionLines.size())
        {
            if (findClosestCollision())
            {
                handleCollision();
            }
            m_possibleCollisionLines.pop_front();
        }
    }
}

bool LineGenerator::step()
{
    //if(!addNextPoint()) return;
    if (m_possibleCollisionLines.size())
    {
        if (findClosestCollision())
        {
            handleCollision();
        }
        m_possibleCollisionLines.pop_front();
        return true;
    }
    else return addNextPoint();
}

//either we addit to the end or to the middle of the line
bool LineGenerator::addNextPoint()
{
    if(m_pointList.size() == 0)
        return false;
    /*
    {
        if(unAdded) 
        {
            m_pointList.push_back({800.f,0.f});
            unAdded = false;
            return true;
        }
        return false;
    }*/
    
    //either we can 'grow' the line, 'advance' the line, or just discard the point from the list

    addInternalPoint();
    return true;

    if(addedABackTrackPoint()) return true;

    float dist = findClosestPoint(std::prev(m_line.end()), m_nextPoint, m_pointList);
    if(dist > 10000.f)
    {
        m_x += 10.f;
    }
    m_line.splice(m_line.end(), m_pointList, m_nextPoint);
    auto endLine2 = std::prev(m_line.end());
    auto endLine1 = std::prev(endLine2);
    
    addPossibleCollision(endLine1,endLine2);//can add the 'testlines visual code' to this function
    
    return true;
}

void LineGenerator::addInternalPoint()
{
    
    //no find closest here ? just add it
    m_nextPoint = m_pointList.begin();

    //find closest line point
    std::list<Point>::iterator pointOnLineClosest;
    closestDist = findClosestLineToPoint(m_nextPoint, pointOnLineClosest, m_line);
    //pointOnLineClosest = m_line.begin();
//    closestDist = findClosestMidPoint(m_nextPoint, pointOnLineClosest, m_line);
    /*
    closestDist = std::numeric_limits<float>::max();
    while (true)
    {
        closestDist = findClosestMidPoint(m_nextPoint, pointOnLineClosest, m_line);
        //closestDist = findClosestPoint(m_nextPoint, pointOnLineClosest, m_line);
        if(closestDist < 10000.f)
            break;
        ++m_nextPoint;
        if(m_nextPoint == m_pointList.end())
        {
            m_pointList.clear();
            return;
        }
    }*/
    /*
    //try finding closest distance of all lines... hmmm
    //for each point, find the closest line... use closest line and pt.
    closestDist = std::numeric_limits<float>::max();
    std::list<Point>::iterator pointOnLineClosest;
    for(auto pt = m_pointList.begin(); pt != m_pointList.end(); ++pt)
    {
        std::list<Point>::iterator closestLinePt;
        float closeDist = findClosestPoint(pt, closestLinePt, m_line);
        if(closeDist < closestDist)
        {
            m_nextPoint = pt;
            closestDist = closeDist;
            pointOnLineClosest = closestLinePt;
        }
    }*/
    
    //keep the 'shape'?
    //bool addedbegin = false;
    m_line.splice(std::next(pointOnLineClosest), m_pointList, m_nextPoint);
    addPossibleCollision(std::next(pointOnLineClosest), pointOnLineClosest);
    addPossibleCollision(std::next(pointOnLineClosest), std::next(std::next(pointOnLineClosest)));
    /*
    if(pointOnLineClosest == m_line.begin())
    {

        pointOnLineClosest = std::next(pointOnLineClosest);
        addedbegin = true;
    }*/
    /*
    m_line.splice(pointOnLineClosest, m_pointList, m_nextPoint);
    addPossibleCollision(std::prev(pointOnLineClosest), pointOnLineClosest);
    if(!addedbegin) addPossibleCollision(std::prev(pointOnLineClosest), std::prev(std::prev(pointOnLineClosest)));
    else 
    {
        assert(std::next(m_line.begin()) == std::prev(pointOnLineClosest));
        addPossibleCollision(m_line.begin(), std::prev(pointOnLineClosest));
    }*/
}

/*Makes the line more 'columney' no long overhang backtrack sections...*/
bool LineGenerator::addedABackTrackPoint()
{
    float curX = m_line.back().x < m_x ? m_x : m_line.back().x;
    float min_x = curX - m_maxBacktrackDistance;
    if(curX > 700.f) min_x = curX;

    bool foundPoint = false;
    for(auto it = m_pointList.begin(); it != m_pointList.end(); ++it)
    {
        if(it->x < min_x)
        {
            m_nextPoint = it;
            foundPoint = true;
            break;
        }
    }

    if (foundPoint)
    { //add it to the line at the closest point
        std::list<Point>::iterator pointOnLineClosest;
        findClosestPoint(m_nextPoint, pointOnLineClosest, m_line);
        m_line.splice(pointOnLineClosest, m_pointList, m_nextPoint);
        addPossibleCollision(std::prev(pointOnLineClosest), pointOnLineClosest);
        addPossibleCollision(std::prev(pointOnLineClosest), std::prev(std::prev(pointOnLineClosest)));
    }

    return foundPoint;
}

//ASSUME NO DUPLICATE POINTS (LINESEGS OF ZERO LENGTH) OR WE GET STUCK IN AN INFINITE LOOP :(
bool LineGenerator::findClosestCollision()
{
    auto p1 = m_possibleCollisionLines.front().first;
    if(m_possibleCollisionLines.front().first == std::next(m_possibleCollisionLines.front().second))
        p1 = m_possibleCollisionLines.front().second;
    auto p2 = std::next(p1);
    if(p2 == m_line.end()) return false;
    
    bool isCollision = false;
    float closestDist = std::numeric_limits<float>::max();
    closestIntPt = m_line.begin();
    for(auto it = m_line.begin(); ; ++it)
    {
        auto it2 = std::next(it);
        if(it2 == m_line.end())
            break;
        if(it2 == std::prev(m_line.end()) && p1 == m_line.begin() && isClosed) continue;
        if(p2 == std::prev(m_line.end()) && it == m_line.begin() && isClosed) continue;//no use continuing
        if (it == p1 || it == p2 || it2 == p1 || it2 == p2)
            continue;
        
        Point intPoint;
        if (SFUTIL::lineSegsIntersect(*it, *it2, *p1, *p2, &intPoint))
        {
            isCollision = true;
            float dist = SFUTIL::lengthSquared<float>((*p1) - intPoint);
            if (dist < closestDist)
            {
                closestDist = dist;
                closestIntPt = it;
            }
            break;//THINING
        }
    }

    return isCollision;
}

void LineGenerator::handleCollision()
{
    std::list<Point> spliceList;

    auto i10 = closestIntPt;
    auto i20 = m_possibleCollisionLines.front().first;
    if(m_possibleCollisionLines.front().first == std::next(m_possibleCollisionLines.front().second))
        i20 = m_possibleCollisionLines.front().second;

    if (!isLess(i10, i20, m_line))
    {
        std::swap(i10,i20);
    }
    auto i11 = std::next(i10);
    auto i21 = std::next(i20);

    //2 opt switch
    spliceList.splice(spliceList.begin(), m_line, i11, i21);
    spliceList.reverse();//std::reverse function messes up with the iterators so can't use
    m_line.splice(std::next(i10), spliceList);

    if(i21 != std::prev(m_line.end())) addPossibleCollision(i10, std::next(i10));//std::next(i21));
    addPossibleCollision(std::prev(i21), i21);
}