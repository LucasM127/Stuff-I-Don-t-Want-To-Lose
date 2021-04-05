//TODO; chop off the skinny segments somehow...
//slider for skinny ness allowed before /after chopping.
//also craters -, other shapes. etc...

//create a crater ... how
//deform it at x and y...
//keep the relationships.
float sliverValue = 500.f;
float smoothValue = 100.f;

//too much smoothing (5000) can cause self-intersection though...
bool isSliver(const Point &p1, const Point &p2, const Point &p3)
{
    //cross product dot product, arrbitratryy value
    Point A = p1 - p2;
    Point B = p3 - p2;

    float dot = SFUTIL::dot<float>(A, B);     //AB cos0 when angle is 0 cos is 1 when angle is 180 cos is -1
    float cross = SFUTIL::cross<float>(A, B); //AB sin0 when angle is 0 sin = 0, when angle is 180 sin is 0
    cross = fabsf(cross);
    return (cross < sliverValue && dot > 0.f); //dot > instead of <
} //for slivers

//too much smoothing (5000) can cause self-intersection though...
bool isSimilar(const Point &p1, const Point &p2, const Point &p3)
{
    //cross product dot product, arrbitratryy value
    Point A = p1 - p2;
    Point B = p3 - p2;

    float dot = SFUTIL::dot<float>(A, B);     //AB cos0 when angle is 0 cos is 1 when angle is 180 cos is -1
    float cross = SFUTIL::cross<float>(A, B); //AB sin0 when angle is 0 sin = 0, when angle is 180 sin is 0
    cross = fabsf(cross);
    return (cross < smoothValue && dot < 0.f); //dot > instead of <
} //this got rid of 'flat pieces'

typedef Point Vec2;

//New FUnction -> distance line to point and also if point is in range of line (0-1)
float isPointTooCloseToLine(const Point &l0, const Point &l1, const Point &p)
{
    //cross product |A||B|sin0 if 0 is perpendicular = |A||B|... so dist = crossproduct / |B| or of the line segment
    //if is in 'range' 0 - 1?  use the function i calculated.
    const Vec2 L = l1 - l0;
    const Vec2 P = p - l0;
    float dot = SFUTIL::dot(L, L);
    float range = L.x * P.x + L.y * P.y;
    range /= dot;
    if (range > 1.1f || range < -0.1f)
        return 0;
    //    if(range > 1.f || range < 0.f) return 0;
    float cross = SFUTIL::cross(P, L);
    float length = sqrtf(dot);
    float distance = cross / length;
    distance = fabs(distance);
    //if(distance < 10.f) return true;
    return distance;
}

LineGenerator::LineGenerator() //const std::vector<Point> &_points) : setup(true), done(false)
{
    lastWasMerge = false;
    //    generate(_points);
}

LineGenerator::~LineGenerator()
{
}

void LineGenerator::setSmoothSliver(float smooth, float sliver)
{
    smoothValue = smooth;
    sliverValue = sliver;
}

void LineGenerator::resetSmoothness()
{
    newShape.clear();
    closeLines.clear();
    closePoints.clear();
    line.clear();
    for (uint i = 0; i < lineShape.getVertexCount(); ++i)
        line.push_back(lineShape[i].position);
    done = false;
}

void LineGenerator::generate(const std::vector<Point> &_points)
{
    setup = true;
    done = false;
    points.clear();
    line.clear();
    testLines.clear();
    alltestLines.clear();
    opttestLines.clear();
    closeLines.clear();
    lineShape.clear();
    newShape.clear();
    pointShapes.clear();
    closePoints.clear();

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

    //testLines.setPrimitiveType(sf::Lines);
    lineShape.setPrimitiveType(sf::LineStrip);
    newShape.setPrimitiveType(sf::LinesStrip);
    //line.push_back({0.f,0.f});
    //line.push_back({0.f, -5000.f});
    line.push_back({-5000.f, 0.f});
    //line.splice(std::next(line.begin()), points, points.begin());
    //line.splice(line.begin(),points,points.begin());
    //line.splice(line.begin(), points, points.begin());
}

bool LineGenerator::isLess(std::list<Point>::iterator i0, std::list<Point>::iterator i1)
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

void LineGenerator::advance()
{
    do
    {

        if (setup)
        {
            setup = false;
            assert(line.size() != 0);
            if (points.size() == 0)
            {
                /*if(!done)
            {
                line.front().x = 0.f;
                line.back().x = 5000.f;
                line.push_back(*line.begin());
                possibleCollisionLines.push_back(std::pair<std::list<Point>::iterator, std::list<Point>::iterator>(std::prev(line.end()), std::prev(std::prev(line.end()))));
            }*/
                if (!done)
                {
                    for (auto it = line.begin(); it != std::prev(line.end()); ++it)
                    {
                        //remove tiny segments....
                        float length = SFUTIL::lengthSquared(*std::next(it) - *it);
                        length = sqrtf(length); //range 0 - 50
                        if (length < minLength) //100)
                        {
                            line.erase(std::next(it));
                        }
                    }

                    for (auto it = std::next(line.begin()); it != std::prev(line.end()); ++it)
                    {
                        if (isSliver(*std::prev(it), *it, *std::next(it)))
                        {
                            LOG("Removing sliver");
                            line.erase(it);
                            it--;
                            it--;
                        }

                        if (isSimilar(*std::prev(it), *it, *std::next(it)))
                        {
                            LOG("Removing flat");
                            line.erase(it);
                            it--;
                            //it--;
                        }
                    }
                    //should be a better way
                    for (auto it = std::next(line.begin()); it != std::prev(std::prev(line.end())); ++it) //(auto it = line.begin(); it != std::prev(std::prev(std::prev(line.end()))); ++it)
                    {
                        float cleardist;
                        //check it with evry other line from 2 and later... // the points...
                        for (auto it2 = line.begin(); it2 != line.end(); ++it2) //for(auto it2 = std::next(std::next(std::next(it))); it2 != line.end(); ++it2)
                        {
                            if (it2 == it || it2 == std::next(it) || it2 == std::prev(it) || it2 == std::next(std::next(it)))
                                continue;
                            if (cleardist = isPointTooCloseToLine(*it, *std::next(it), *it2))
                            { //if point is to the right is internal (closest assumed due to seeding shit) ... cross product
                                bool internal = false;
                                float cross = SFUTIL::cross(*std::next(it) - *it, *it2 - *it);
                                if (cross < 0.f)
                                {
                                    internal = true;
                                    //internal... yes remove as 'sliver' unnatural
                                    if (cleardist > 15.f)
                                        continue;
                                }
                                else if (cleardist > 20.f)
                                    continue; //external... don't remove, use for 'cleartest' so know where not to put landing zone
                                //color that line
                                if (internal)
                                {
                                    closeLines.push_back({*it, sf::Color::Red});
                                    closeLines.push_back({*std::next(it), sf::Color::Red});
                                }
                                else
                                {
                                    closeLines.push_back({*it, sf::Color::Green});
                                    closeLines.push_back({*std::next(it), sf::Color::Green});
                                }

                                //remove it from the line???
                                C.setPosition(*it2);
                                C.setFillColor(sf::Color(255, 128, 80, 128));
                                closePoints.push_back(C);
                            }
                        }
                    }

                    //add a crater??? hmmm

                    newShape.clear();
                    for (auto &point : line)
                        newShape.append({point, sf::Color::White});
                }

                done = true;

                std::ofstream ofs("outfile");
                //if (!ofs.is_open())
                //  return -1;
                //for (auto &point : line)
                for (uint i = 1; i < newShape.getVertexCount(); ++i)
                {
                    Point point = newShape[i].position;
                    ofs << point.x << " " << point.y << std::endl;
                }
                ofs.close();

                return;
            }

            //testSweepLine()
            /***   This makes it 'columney' a bit too much
         * better to find a closer point to that point*/
            //is cutting back at the very last bit, not liking that :(
            //use he width ???

            float x = line.back().x;
            float min_x = x - 100.f; //800 width, 600 height/16 2*unitheight
            //if( (800 - x) < min_x) min_x = 800-x;
            bool closer = false;
            for (auto it = points.begin(); it != points.end(); it++)
            //Tag a point to 'merge' or 'grow'
            { //the closer point should be within the x-range of it else go swweeping over too much
                if (it->x < min_x)
                {
                    closestPoint = it;
                    closer = true;
                    break;
                }
            }
            //points that may not necessarily be 'closer' but are more 'vertical' prioritize how?

            if (closer)
            {
                closestDist2 = std::numeric_limits<float>::max();
                auto closeLinePt = line.begin();
                for (auto it = line.begin(); it != line.end(); ++it)
                {
                    float dist = SFUTIL::lengthSquared<float>((*it) - *closestPoint);

                    if (dist < closestDist2)
                    {
                        closeLinePt = it;
                        closestDist2 = dist;
                    }
                }
                //got the closest point in the line...
                //now add it to the line there, and test before, and after for collision
                line.splice(closeLinePt, points, closestPoint);
                possibleCollisionLines.push_back(std::pair<std::list<Point>::iterator, std::list<Point>::iterator>(std::prev(closeLinePt), closeLinePt));
                possibleCollisionLines.push_back(std::pair<std::list<Point>::iterator, std::list<Point>::iterator>(std::prev(closeLinePt), std::prev(std::prev(closeLinePt))));
                //return;
                lastWasMerge = true;
                //continue;
                //lastWasMerge = true;
            }
            getNextClosestPoint();

            line.splice(line.end(), points, closestPoint);
            auto endLine2 = std::prev(line.end());
            auto endLine1 = std::prev(endLine2);
            testLines.clear();
            testLines.push_back({*endLine1, sf::Color(255, 255, 0)});
            testLines.push_back({*endLine2, sf::Color(255, 255, 0)});
            possibleCollisionLines.push_back(std::pair<std::list<Point>::iterator, std::list<Point>::iterator>(endLine1, endLine2));
            //possibleCollisionLines.push_back(endLine1);
            //return;
            lastWasMerge = false;
        }

        else if (possibleCollisionLines.size())
        //while (possibleCollisionLines.size())
        {
            opttestLines.clear();

            //auto p = possibleCollisionLines.front().first;
            if (findClosestCollision()) //;//p));
            {
                handleCollision(); //p);
            }
            //if(testLines.size()){
            //testLines.erase(testLines.begin());
            //testLines.erase(testLines.begin());}
            possibleCollisionLines.pop_front();

            lineShape.clear();
            for (auto p : line)
                lineShape.append({p, sf::Color(0, 255, 0, 40)}); //sf::Color::Green});
            //return;
        }
        else
        {
            LOG("Points linked ", line.size(), " with ", points.size(), " points remaining");
            setup = true;
        }
    } while (true); //(points.size());
}

void LineGenerator::getNextClosestPoint()
{
    closestDist2 = std::numeric_limits<float>::max();
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

//DEBUGGING 101 -> can you reproduce the bug in smallest code possible?
//hasn't even found the first collision yet... so haven't even 'swapped' any iterators...
//we haven't had a merge last did we???
bool LineGenerator::findClosestCollision()
{
    LOG("Testing for collision");

    auto p1 = possibleCollisionLines.front().first;
    if (possibleCollisionLines.front().first == std::next(possibleCollisionLines.front().second))
        p1 = possibleCollisionLines.front().second;
    /*else
    {
        //if (possibleCollisionLines.front().second != std::next(possibleCollisionLines.front().first))
          //  throw(std::runtime_error("NOT ADJACENT!"));
        p1 = possibleCollisionLines.front().first;
    }*/

    //well that is useless
    /** THIS FAILS
    auto p1 = possibleCollisionLines.front().first;
    auto p2 = possibleCollisionLines.front().second;
    
    **/

    opttestLines.push_back({*p1, sf::Color::White});
    opttestLines.push_back({*std::next(p1), sf::Color::White});

    auto p2 = std::next(p1);
    if (p2 == line.end())
        return false;

    bool collision = false;
    closestDist2 = std::numeric_limits<float>::max();
    auto closestLine = line.begin();
    //it2 == p1 ????
    //test all but those 2...
    for (auto it = line.begin();; ++it)
    {
        auto it2 = std::next(it);
        if (it2 == line.end())
            break;
        if (it == p1 || it == p2 || it2 == p1 || it2 == p2)
            continue;
        if (*it2 == *p1)
        {
            //is p1 in line?
            for (auto itt = line.begin(); itt != line.end(); ++itt)
            {
                if (itt == p1)
                { //it is the same as the previous
                    LOG(p1->x, p1->y, " ", std::next(p1)->x, std::next(p1)->y, " ", std::prev(p1)->x, std::prev(p1)->y);
                    throw;
                }
            }
            //std::list
            throw;
        } //throw;//continue;//Why is this a bug???
        Point intersection;
        if (SFUTIL::lineSegsIntersect(*it, *it2, *p1, *p2, &intersection))
        { //adjacent points hitting wtf
            LOG("Collision with points ", it->x, " ", it->y, " ", it2->x, " ", it2->y, " vs ", p1->x, " ", p1->y, " ", p2->x, " ", p2->y);
            collision = true;
            float dist = SFUTIL::lengthSquared<float>((*p1) - intersection); //CHANGED
            if (dist < closestDist2)
            {
                closestIntPt = it;
                closestDist2 = dist;
            }
        }
    }

    return collision;
}

void LineGenerator::handleCollision()
{
    std::list<Point> spliceList;

    //just fix the one, and return...
    auto i10 = closestIntPt;                         //the closest intersection to p2 or i21
    auto i20 = possibleCollisionLines.front().first; //p;
    if (possibleCollisionLines.front().first == std::next(possibleCollisionLines.front().second))
        i20 = possibleCollisionLines.front().second;
    else
        i20 = possibleCollisionLines.front().first;

    if (!isLess(i10, i20))
    {
        std::swap(i10, i20);
    }
    auto i11 = std::next(i10);
    auto i21 = std::next(i20);

    //now we just join i10 -> p (i21) (NO intersection known)
    //and i11 -> n (i20) //
    spliceList.splice(spliceList.begin(), line, i11, i21);
    spliceList.reverse();
    line.splice(std::next(i10), spliceList);

    //when we reverse it,  the iterator positions are 'fine' but the positions are perhaps 'reversed'
    //what if save a 'pair' ???
    alltestLines.push_back({*i10, sf::Color(255, 128, 0)});
    alltestLines.push_back({*std::next(i10), sf::Color(255, 128, 0)});
    possibleCollisionLines.push_back(std::pair<std::list<Point>::iterator, std::list<Point>::iterator>(i10, std::next(i21)));

    possibleCollisionLines.push_back(std::pair<std::list<Point>::iterator, std::list<Point>::iterator>(std::prev(i21), i21));
    alltestLines.push_back({*i21, sf::Color(255, 0, 128)});
    alltestLines.push_back({*std::prev(i21), sf::Color(255, 0, 128)});
    //alltestLines.push_back({*i10, sf::Color(255, 128, 0)});
    //alltestLines.push_back({*std::next(i10), sf::Color(255, 128, 0)});
}

void LineGenerator::render(sf::RenderTarget &target)
{
    //if(!done){
    //lineShape.clear();
    //for (auto &point : line)
    //    lineShape.append({point, sf::Color::Green});}
    if (!done)
    {
        for (auto &c : pointShapes)
            target.draw(c);
    }
    if (!done)
        target.draw(&testLines[0], testLines.size(), sf::Lines);

    if (done)
        target.draw(lineShape);
    if (done)
        target.draw(newShape);
    if (done)
        target.draw(&closeLines[0], closeLines.size(), sf::Lines);
    for (auto &c : closePoints)
        target.draw(c);
    if (!done)
        target.draw(&alltestLines[0], alltestLines.size(), sf::Lines);
    if (!done)
        target.draw(&opttestLines[0], opttestLines.size(), sf::Lines);
}