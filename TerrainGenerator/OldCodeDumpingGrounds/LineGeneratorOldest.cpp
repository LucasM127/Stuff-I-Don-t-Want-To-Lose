#include "LineGenerator.hpp"

std::list<Point>::iterator watcher;

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
    watcher = line.begin();
}

bool LineGenerator::advance()
{
    if(setup){
setup = false;
    if(points.size()==0) return false;

    getNextClosestPoint();
    //float dist = SFUTIL::lengthSquared<float>((*closestPoint) - line.back());
testLines.push_back({*std::prev(line.end()),sf::Color(255,255,128)});
testLines.push_back({*closestPoint,sf::Color(255,255,128)});
line.splice(line.end(), points, closestPoint);
    possibleCollisionLines.push_back(std::pair<std::list<Point>::iterator, std::list<Point>::iterator>(std::prev(line.end()), std::prev(std::prev(line.end()))));
    
    return true;
    }

    //test for a possible collision?
    //find collisions fills the intersectors sorted by distance from second point
    
    //test for intersection with the proposed line and the new line.
    //creat a list of lines we made too to test... against hmmm
    //while (possibleCollisionLines.size())
    //maybe is over complicating things?  should 2 opt move after makeing it with closest move algorithm, perhaps.
    else if(possibleCollisionLines.size())
    {
    while(possibleCollisionLines.size()){
        auto p1 = possibleCollisionLines.front().first;//end point
        auto p2 = possibleCollisionLines.front().second;//closest point
        if (findCollisions(p1, p2))//why finding a collision?
        {
            optFix(p1,p2);
//            handleCollisions(p1,p2); //invalidates the list though...
            //possibleCollisionLines.clear();
        }
        testLines.erase(testLines.begin());
        testLines.erase(testLines.begin());
        possibleCollisionLines.pop_front();
        return true;
    }
    }
    
    /*if(findCollisions(std::prev(line.end()),closestPoint))
    {
        handleCollisions();
        LOG("Handled collision ", possibleCollisionLines.size(), " lines to test?");
        
        while(possibleCollisionLines.size())
        {
            auto p1 = possibleCollisionLines.front().first;
            auto p2 = possibleCollisionLines.front().second;
            if(findCollisions(p1,p2))
            {
                handleCollisions();//invalidates the list though...
                //possibleCollisionLines.clear();
            }
            possibleCollisionLines.pop_front();
        }
    }*/

//take it?
    //line.splice(closestPoint,)
    
    else{
    //line.splice(line.end(),points,closestPoint);
    
setup = true;
    //points.erase(closestPoint);
LOG("Points linked ",line.size());
    return true;}
}

//collisions are sorted by distance from p2 (may or may not 'exist' in the line though)
//p1 = end point
//p2 = 'closestPoint'
//starting closest to the end point
//get line from n -> i10
//and a line from i11 -> i20
//i21 -> i30 etc...
//we know there is 1 or more 'intersection' between line from p1 to p2
//swap it out for each line thing and add the lines we create to test to test for future tests???

//SIGH
/***
void LineGenerator::handleCollisions(std::list<Point>::iterator p1, std::list<Point>::iterator p2)
{
    std::list<Point> reorderedPoints;

    //first intersection, closest to p2
    {
        auto i10 = intersectors.front().it;
        auto i11 = std::next(i10);
        //connect the 2 not splice
        testLines.push_back({*i11, sf::Color(255, 255, 128)});
        testLines.push_back({*p2, sf::Color(255, 255, 128)});
        possibleCollisionLines.push_back(std::pair<std::list<Point>::iterator, std::list<Point>::iterator>(i11, p2));
    }

    //inbetween checks...
    for (uint i = 0; i < intersectors.size() - 1; ++i)
    {
        auto i10 = intersectors[i].it;
        auto i11 = std::next(i10);
        auto i20 = intersectors[i + 1].it;
        auto i21 = std::next(i20);

        testLines.push_back({*i10, sf::Color(255, 255, 222)});
        testLines.push_back({*i21, sf::Color(255, 255, 222)});
        possibleCollisionLines.push_back(std::pair<std::list<Point>::iterator, std::list<Point>::iterator>(i10, i21));

        if (i % 2 == 0) //is even
        {               //why are we LOSING points
            bool inOrder = isLess(i10, i21, line.end());
            std::list<Point> reverseLine;
            reverseLine.splice(reverseLine.end(), line, i11, i21); //lower, higher);//lower inclusive up to higher
                                                                   //if(inOrder)
            if(inOrder) reverseLine.reverse();                                 //this won't screw with the indices
            reorderedPoints.splice(reorderedPoints.begin(), reverseLine);
        }
    }

    //last possible check, from furthest intersection to the current end of the line (p1)
    {
        auto i10 = intersectors.back().it;
        auto i11 = std::next(i10);

        if (intersectors.size() % 2)
        {
            //is odd so we do the reverse
            std::list<Point> reverseLine;
            reverseLine.splice(reverseLine.end(), line, i10, p1);         //lower, higher);
            reverseLine.reverse();                                        //this won't screw with the indices
            reorderedPoints.splice(reorderedPoints.begin(), reverseLine); //line, i11, p1);
        }

        testLines.push_back({*i10, sf::Color(255, 255, 128)});
        testLines.push_back({*p1, sf::Color(255, 255, 128)});
        possibleCollisionLines.push_back(std::pair<std::list<Point>::iterator, std::list<Point>::iterator>(i10, p1)); //(i10, p1));
    }

    if (p2 == closestPoint)
        p2 = line.end();
    line.splice(p2, reorderedPoints); //inserts before....
    LOG("Line size: ", line.size());
}//is my iterators are getting 'F'd up
***/

//Should work, if we keep the linesegs to test... 
//handleIntersection, then optfix it?
//after add the point.
void LineGenerator::optFix(std::list<Point>::iterator p1, std::list<Point>::iterator p2)
{//we just add it first
    auto i10 = p1;
    auto i20 = p2;
    if (!isLess(i10, i20, line.end()))
    {
        std::swap(i10,i20);
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
    line.splice(std::next(i10) ,spliceList);
}

void LineGenerator::handleCollisions(std::list<Point>::iterator p1, std::list<Point>::iterator p2)
{
    std::list<Point> reorderedPoints;

    for(uint i = 0; i < intersectors.size(); ++i)
    {
        std::list<Point>::iterator i10, i11, i21, i22, lower, higher;
        if(i == 0)
        {
            i10 = intersectors.front().it;

        }

    }

    //first intersection, closest to p2
    {
        auto i10 = intersectors.front().it;
        auto i11 = std::next(i10);
        //is it inOrder
        auto lower = line.begin();
        auto higher = line.begin();
        bool inOrder = true;
        if(p2 != closestPoint)//is on the line
            inOrder = isLess(i10, p2, line.end());
        if(!inOrder)
        {
            lower = i10;
        }
        else
        {
            lower = i11;
        }
        
        testLines.push_back({*lower, sf::Color(255, 0, 128)});
        testLines.push_back({*p2, sf::Color(255, 0, 128)});
        possibleCollisionLines.push_back(std::pair<std::list<Point>::iterator, std::list<Point>::iterator>(lower, p2));
    }
    if (p2 == closestPoint)
        p2 = line.end();

    //inbetween checks...
    for(uint i = 0; i<intersectors.size()-1; ++i)
    {
        auto i10 = intersectors[i].it;
        auto i11 = std::next(i10);
        auto i20 = intersectors[i+1].it;
        auto i21 = std::next(i20);

        auto lower = line.begin();
        auto higher = line.begin();
        bool inOrder = isLess(i10, i21, line.end());
        if (inOrder)
        {
            lower = i11;
            higher = i21; //i11 -> i20
            //i10 joins i21
        }
        else
        {
            lower = i21;
            higher = i11; //i 21 -> i 10
            //i20 joins i10
        }

        testLines.push_back({*std::prev(lower), sf::Color(100, 100, 255)});
        testLines.push_back({*higher, sf::Color(100, 100, 255)});
        possibleCollisionLines.push_back(std::pair<std::list<Point>::iterator, std::list<Point>::iterator>(std::prev(lower), higher));

        if(i%2 == 0)//is even
        {//why are we LOSING points
            
            //splice
            std::list<Point> reverseLine;
            reverseLine.splice(reverseLine.end(), line, lower, higher);//lower inclusive up to higher
            if(inOrder)
                reverseLine.reverse(); //this won't screw with the indices
            reorderedPoints.splice(reorderedPoints.begin(), reverseLine);
        }
        else
        {
            //testLines.push_back({*i10, sf::Color(255, 255, 222)});
            //testLines.push_back({*i21, sf::Color(255, 255, 222)});
            //possibleCollisionLines.push_back(std::pair<std::list<Point>::iterator, std::list<Point>::iterator>(i10, i21));
        }
        
        
    }

    //last possible check, from furthest intersection to the current end of the line (p1)
    {
        auto i10 = intersectors.back().it;
        auto i11 = std::next(i10);

        bool inOrder = isLess(p1, i11, line.end());
        auto lower = line.begin();
        auto higher = line.begin();
        if (inOrder)
        {
            lower = p1;
            higher = i11;
        }
        else
        {
            lower = i11;
            higher = p1;
        }

        testLines.push_back({*lower, sf::Color(255, 255, 128)});
        testLines.push_back({*higher, sf::Color(255, 255, 128)});
        possibleCollisionLines.push_back(std::pair<std::list<Point>::iterator, std::list<Point>::iterator>(lower, higher)); //(i10, p1));

        if (intersectors.size() % 2)
        {
            
            
            //is odd so we do the reverse
            std::list<Point> reverseLine;
            reverseLine.splice(reverseLine.end(), line, lower, higher);// i10, p1);//lower, higher);
            if(!inOrder) reverseLine.reverse(); //this won't screw with the indices
            reorderedPoints.splice(reorderedPoints.begin(), reverseLine); //line, i11, p1);
        }

        
        LOG("Possible last test with ", i10->x, " ", i10->y, " to ", p1->x, " ", p1->y);
    }

    line.splice(p2, reorderedPoints);//inserts before....
}

/**
void LineGenerator::handleCollisions(std::list<Point>::iterator p1, std::list<Point>::iterator p2)
{
    if(intersectors.size()>2)
    {
        int x = 1;
    }
    //get intersectors.size()+1 lines to test... (possible)
    std::list<Point> reorderedPoints;

    //first intersection, closest to p2
    {//i11 is changing its value??? how in the world is that?
        auto i10 = intersectors.front().it;
        auto i11 = std::next(i10);
        //connect the 2 not splice
        testLines.push_back({*i11, sf::Color(255, 255, 128)});
        testLines.push_back({*p2, sf::Color(255, 255, 128)});
        possibleCollisionLines.push_back(std::pair<std::list<Point>::iterator, std::list<Point>::iterator>(i11,p2));
        LOG("Possible first test with ", i11->x, " ", i11->y, " to ", p2->x, " ", p2->y);
        watcher = i11;
        LOG("Watcher ", watcher->x, " ", watcher->y);
    }

    //add a line to test for every intersection pair, splice if even
    for(uint i = 0; i < intersectors.size()-1; ++i)
    {
        auto i10 = intersectors[i].it;
        auto i11 = std::next(i10);
        auto i20 = intersectors[i + 1].it;
        auto i21 = std::next(i20);

        testLines.push_back({*i10, sf::Color(255, 255, 128)});
        testLines.push_back({*i21, sf::Color(255, 255, 128)});
        possibleCollisionLines.push_back(std::pair<std::list<Point>::iterator, std::list<Point>::iterator>(i10, i21));
    }
    for(uint i = 0; i<intersectors.size()/2; i++)
    {
        auto i10 = intersectors[2*i].it;
        auto i11 = std::next(i10);
        auto i20 = intersectors[2*i + 1].it;
        auto i21 = std::next(i20);

        std::list<Point> reverseLine;
       // bool inOrder = isLess(i21, i11, line.end());
        //if (inOrder)
          //  std::swap(i21, i11);
        reverseLine.splice(reverseLine.end(), line, i11, p1);
        //if (!inOrder)
        {
            reverseLine.reverse();
        }
        //std::reverse(i11, i21);
        reorderedPoints.splice(reorderedPoints.begin(), reverseLine); //line, i11, i21);
    }
    /*
    for(uint i = 0; i < intersectors.size()-1; ++i)
    {
        auto i10 = intersectors[i].it;
        auto i11 = std::next(i10);
        auto i20 = intersectors[i + 1].it;
        auto i21 = std::next(i20);

        if(i%2 == 0)
        {
        std::list<Point> reverseLine;
        bool inOrder = isLess(i21, i11, line.end());
        if (inOrder)
            std::swap(i21, i11);
        reverseLine.splice(reverseLine.end(), line, i11, p1);
        if (!inOrder)
        {
            reverseLine.reverse();
        }
            std::reverse(i11, i21);
        reorderedPoints.splice(reorderedPoints.begin(), reverseLine);//line, i11, i21);
        }
        testLines.push_back({*i10, sf::Color(255, 255, 128)});
        testLines.push_back({*i21, sf::Color(255, 255, 128)});
        possibleCollisionLines.push_back(std::pair<std::list<Point>::iterator, std::list<Point>::iterator>(i10, i21));
    }* /

    //add a line for last intersection -> p1
    //is either a 'cut' line or a 'splice' line if odd/even
    {
        auto i10 = intersectors.back().it;
        auto i11 = std::next(i10);

        if(intersectors.size() %2)
        {/*
            bool inOrder = isLess(p1, i11, line.end());
            if (inOrder)
                std::swap(p1, i11);
            if (!inOrder)
                std::reverse(i11, p1);* /
            LOG("Watcher ", watcher->x, " ", watcher->y);
            //reverses order in range first to last... swaps them... doesn't take them and ...
            std::list<Point> reverseLine;
            reverseLine.splice(reverseLine.end(),line,i11,p1);
            reverseLine.reverse();//this won't screw with the indices
//            std::reverse(i11, p1);//f-ing hell screwing with my indices.
            LOG("Watcher ", watcher->x, " ", watcher->y);
            reorderedPoints.splice(reorderedPoints.begin(), reverseLine);//line, i11, p1);
        }

        testLines.push_back({*i10, sf::Color(255, 255, 128)});
        testLines.push_back({*p1, sf::Color(255, 255, 128)});
        possibleCollisionLines.push_back(std::pair<std::list<Point>::iterator, std::list<Point>::iterator>(i10,p1));//(i10, p1));
        LOG("Possible last test with ", i10->x, " ", i10->y, " to ", p1->x, " ", p1->y);
    }

    LOG("Watcher ", watcher->x, " ", watcher->y);

    //line.insert(p2, reorderedPoints)
    LOG(line.size(), " line size");
    if(p2 == closestPoint) p2 = line.end();    
    line.splice(p2, reorderedPoints);
    LOG(line.size(), " line size");

    LOG("Watcher ", watcher->x, " ", watcher->y);
}
*/
//sorted by distance to p2
//p2 could be point not on list
bool LineGenerator::findCollisions(std::list<Point>::iterator p1, std::list<Point>::iterator p2)
{
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
            return true;
        }
    }

    return false;
}
/**

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
}**/

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

    for(auto &c : pointShapes) target.draw(c);

    target.draw(nextLine);
    target.draw(lineShape);
    target.draw(&testLines[0],testLines.size(),sf::Lines);
}