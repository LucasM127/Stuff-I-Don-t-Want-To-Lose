#include "LineGenerator.hpp"

bool isLess(std::list<Point>::iterator i0,
            std::list<Point>::iterator i1,
            std::list<Point>::iterator end)
{
    auto it = i0;
    while (true) //it != i1 && it != line.end())
    {
        if (it == i1)
            return true;
        if (it == end)
            return false;
        ++it;
    }
    return true;
}

LineGenerator::LineGenerator(const std::vector<Point> &_points)
{
    done = false;
    setup = true;
    for (auto point : _points)
        points.push_back(point);

    C.setFillColor(sf::Color(80, 80, 128, 128));
    C.setRadius(5.f);
    C.setOrigin(5.f, 5.f);

    for (auto &point : points)
    {
        C.setPosition(point);
        pointShapes.push_back(C);
    }

    nextLine.setPrimitiveType(sf::Lines);
    nextLine.resize(2);
    nextLine[0].color = nextLine[1].color = sf::Color::Red;

    lineShape.setPrimitiveType(sf::LineStrip);

    line.push_back({-1000.f, 0.f});
}

bool LineGenerator::advance()
{
    if (setup)
    {
        setup = false;
        if (points.size() == 0)
            return false;

        getNextClosestPoint();
        testLines.push_back({*std::prev(line.end()), sf::Color(255, 255, 0)});
        testLines.push_back({*closestPoint, sf::Color(255, 255, 0)});
        line.splice(line.end(), points, closestPoint);
        auto p = std::prev(line.end());
        auto n = std::prev(p);
        possibleCollisionLines.push_back(std::pair<std::list<Point>::iterator, std::list<Point>::iterator>(n,p));

        return true;
    }

    else if (possibleCollisionLines.size())
    {
        //while (possibleCollisionLines.size())
        {
            auto p1 = possibleCollisionLines.front().first;  //end point
            auto p2 = possibleCollisionLines.front().second; //closest point
            if (findCollisions(p1, p2))                      //why finding a collision?
            {
                handleCollisions(p1,p2);
            }
            testLines.erase(testLines.begin());
            testLines.erase(testLines.begin());
            possibleCollisionLines.pop_front();

            //line.splice(line.end(), points, closestPoint);
            //while(possibleCollisionLines.size())
            //   optFix(p1, p2);
            return true;
        }
    }
    else
    {
        line.splice(line.end(), points, closestPoint);
        setup = true;
        LOG("Points linked ", line.size());
        return true;
    }
}

void LineGenerator::optFix(std::list<Point>::iterator p1, std::list<Point>::iterator p2)
{ //we just add it first
    auto i10 = p1;
    auto i20 = p2;
    if (!isLess(i10, i20, line.end()))
    {
        std::swap(i10, i20);
    }
    auto i11 = std::next(i10);
    auto i21 = std::next(i20);

    testLines.push_back({*i10, sf::Color(255, 255, 128)});
    testLines.push_back({*i21, sf::Color(255, 255, 128)});
    testLines.push_back({*i11, sf::Color(255, 255, 128)});
    testLines.push_back({*i20, sf::Color(255, 255, 128)});
    possibleCollisionLines.push_back(std::pair<std::list<Point>::iterator, std::list<Point>::iterator>(i11, i20));
    possibleCollisionLines.push_back(std::pair<std::list<Point>::iterator, std::list<Point>::iterator>(i10, i21));

    std::list<Point> spliceList;

    spliceList.splice(spliceList.begin(), line, i11, i21);
    spliceList.reverse();
    line.splice(std::next(i10), spliceList);
}

void LineGenerator::handleCollisions(std::list<Point>::iterator p1, std::list<Point>::iterator p2)
{
    std::list<Point> spliceList;

    //just fix the one, and return...
    auto i10 = intersectors.front().it;//the closest intersection
    auto i20 = p1;

    if(!isLess(i10, i20, line.end()))
    {
        std::swap(i10,i20);
    }
    auto i11 = std::next(i10);
    auto i21 = std::next(i20);

    //now we just join i10 -> p (i21) (NO intersection known)
    //and i11 -> n (i20) //
    spliceList.splice(spliceList.begin(), line, i11, i21);
    spliceList.reverse();
    line.splice(std::next(i10), spliceList);

    //so line from i10 - n is the line to test next
    possibleCollisionLines.push_back(std::pair<std::list<Point>::iterator, std::list<Point>::iterator> (i10, std::next(i10)));
    opttestLines.push_back({*i10, sf::Color(255, 128, 0)});
    opttestLines.push_back({*std::next(i10), sf::Color(255, 128, 0)});

    possibleCollisionLines.push_back(std::pair<std::list<Point>::iterator, std::list<Point>::iterator>(std::prev(i21), i21));
    opttestLines.push_back({*i21, sf::Color(255, 0, 128 )});
    opttestLines.push_back({*std::prev(i21), sf::Color(255, 0, 128)});

    /*
    //first intersection, closest to p2
    {
        auto i10 = intersectors.front().it;
        auto i11 = std::next(i10);

        opttestLines.push_back({*i11, sf::Color(255, 0, 32)});
        opttestLines.push_back({*p2, sf::Color(255, 0, 32)});
        optCheckLines.push_back(std::pair<std::list<Point>::iterator, std::list<Point>::iterator>(i11, p2));
    }//splice
 
  // when itis an odd number
    for(uint i = 0; i < intersectors.size()-1; ++i)
    {
        auto i10 = intersectors[i].it;
        auto i11 = std::next(i10);
        auto i20 = intersectors[i + 1].it;
        auto i21 = std::next(i20);

        auto lower = line.begin();
        auto higher = line.begin();
        bool inOrder = isLess(i11, i20, line.end());

        if (inOrder)
        {
            lower = i11;
            higher = i21; //i11 -> i20
        }
        else
        {
            lower = i21;
            higher = i11; //i 21 -> i 10
        }

        opttestLines.push_back({*std::prev(lower), sf::Color(128, 255, 255)});
        opttestLines.push_back({*higher, sf::Color(128, 255, 255)});
        optCheckLines.push_back(std::pair<std::list<Point>::iterator, std::list<Point>::iterator>(std::prev(lower), higher));

        if(i%2 == 0)
        {
            //opttestLines.push_back({*std::prev(lower), sf::Color(128, 255, 128)});
            //opttestLines.push_back({*higher, sf::Color(128, 255, 128)});
            //optCheckLines.push_back(std::pair<std::list<Point>::iterator, std::list<Point>::iterator>(std::prev(lower), higher));

            std::list<Point> reverseLine;
            reverseLine.splice(reverseLine.end(), line, lower, higher);
            if (inOrder)
                reverseLine.reverse();
            reorderedPoints.splice(reorderedPoints.begin(), reverseLine); //line, i11, i21);
        }
        else
        {
            //opttestLines.push_back({*std::prev(lower), sf::Color(255, 255, 222)});
            //opttestLines.push_back({*higher, sf::Color(255, 255, 222)});
            //optCheckLines.push_back(std::pair<std::list<Point>::iterator, std::list<Point>::iterator>(std::prev(lower), higher));
            //opttestLines.push_back({*i10, sf::Color(255, 128, 0)});
            //opttestLines.push_back({*i21, sf::Color(255, 128, 0)});
            //optCheckLines.push_back(std::pair<std::list<Point>::iterator, std::list<Point>::iterator>(i10, i21));
        }
    }

    //is either a 'cut' line or a 'splice' line if odd/even
    {
        auto i10 = intersectors.back().it;
        auto i11 = std::next(i10);

        opttestLines.push_back({*i10, sf::Color(255, 255, 128)});
        opttestLines.push_back({*p1, sf::Color(255, 255, 128)});
        optCheckLines.push_back(std::pair<std::list<Point>::iterator, std::list<Point>::iterator>(i10, p1)); //(i10, p1));

        if (intersectors.size() % 2)
        {

            //is odd so we do the reverse
            std::list<Point> reverseLine;
            reverseLine.splice(reverseLine.end(), line, i11, p1);//lower, higher);
            reverseLine.reverse();                                    //this won't screw with the indices
            reorderedPoints.splice(reorderedPoints.begin(), reverseLine); //line, i11, p1);
        }
    }

    if(p2 == closestPoint) p2 = line.end();    
    line.splice(p2, reorderedPoints);
    */
    LOG(line.size(), " line size");
}


//sorted by distance to p2
//p2 could be point not on list
bool LineGenerator::findCollisions(std::list<Point>::iterator p1, std::list<Point>::iterator p2)
{
    bool collision = false;
    float closestDist2 = std::numeric_limits<float>::max();
    auto closestLine = line.begin();
    intersectors.clear();

    //test all but those 2...
    for (auto it = line.begin();; ++it)
    {
        auto it2 = std::next(it);
        if (it2 == line.end())
            break;
        if (it == p1 || it == p2 || it2 == p1 || it2 == p2)
            continue;
        Point intersection;
        if (SFUTIL::lineSegsIntersect(*it, *it2, *p1, *p2, &intersection))
        {
            LOG("Collision with points ", it->x, " ", it->y, " ", it2->x, " ", it2->y, " vs ", p1->x, " ", p1->y, " ", p2->x, " ", p2->y);
            collision = true;
            float dist = SFUTIL::lengthSquared<float>((*p2) - intersection);
            if (dist < closestDist2)
            {
                closestLine = it;
                closestDist2 = dist;
            }

            bool inserted = false;
            for (uint i = 0; i < intersectors.size(); ++i)
            { //inserts before...
                if (intersectors[i].dist > dist)
                {
                    intersectors.insert(intersectors.begin() + i, {dist, it}); //hmm
                    inserted = true;
                    break;
                }
            }
            if (!inserted)
                intersectors.push_back({dist, it});
        }
    }

    return collision;
}

void LineGenerator::getNextClosestPoint()
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

    nextLine[0].position = line.back();
    nextLine[1].position = *closestPoint;

    return;
}

void LineGenerator::render(sf::RenderTarget &target)
{
    
    lineShape.clear();
    for (auto &point : line)
        lineShape.append({point, sf::Color::Green});

    for (auto &c : pointShapes)
        target.draw(c);

    //target.draw(nextLine);
    target.draw(lineShape);
    //target.draw(&testLines[0], testLines.size(), sf::Lines);
    //target.draw(&opttestLines[0], opttestLines.size(), sf::Lines);
    
}