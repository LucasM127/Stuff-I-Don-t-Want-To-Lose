#include "LineOptimizations.hpp"

void removeTinySegments(std::list<Point> &line, float minLength)
{
    for (auto it = line.begin(); it != std::prev(line.end()); ++it)
    {
        float length = SFUTIL::length<float>(*std::next(it) - *it);
        if (length < minLength)
        {
            line.erase(std::next(it));
        }
    }
}

typedef Point Vec2;
void removeJaggiesAndFlatties(std::list<Point> &line, float jagVal, float flatVal)
{
    for (auto it = std::next(line.begin()); it != std::prev(line.end()); ++it)
    {
        if (it == line.begin())
            continue;
        auto lastIt = std::prev(it);
        auto nextIt = std::next(it);

        Vec2 A = *nextIt - *it;
        Vec2 B = *lastIt - *it;//normalize???

        float dot = SFUTIL::dot<float>(A, B);     //AB cos0 when angle is 0 cos is 1 when angle is 180 cos is -1
        float cross = SFUTIL::cross<float>(A, B); //AB sin0 when angle is 0 sin = 0, when angle is 180 sin is 0
//assert isClockwise
        cross = fabsf(cross);

        if( (cross < jagVal && dot > 0.f) || //sliver angle dependant on lengths of A and B
            (cross < flatVal && dot < 0.f) ) //obtuse angle dependant on lengths of A and B
        {
            line.erase(it);
            --it;
            --it;
        }
    }
}

//Tests if point is directly perpendicular to line segment, (range 0-1), then uses cross product for perpendicular dist to line segment
float howCloseIsPointToLine(const Point &l0, const Point &l1, const Point &p)
{
    const Vec2 L = l1 - l0;
    const Vec2 P = p - l0;
    float dot = SFUTIL::dot(L, L);
    float range = L.x * P.x + L.y * P.y;
    range /= dot;
    if (range > 1.1f || range < -0.1f)//make slightly over for triangular pinch points '/\'
        return 0;
    //    if(range > 1.f || range < 0.f) return 0;
    float cross = SFUTIL::cross(P, L);
    float length = sqrtf(dot);
    float distance = cross / length;
    distance = fabs(distance);
    return distance;
}

void fixPinchPointsStep(std::list<Point> &line, float minThickness, float minClearance, std::list<Point>::iterator it, std::list<Point>::iterator it2)
{
    //just copypasta it in here
    Point l0 = *it;
    Point l1 = *std::next(it);
    Point p = *it2;
    //copypasta
    const Vec2 L = l1 - l0;
    const Vec2 P = p - l0;
    float dot = SFUTIL::dot(L, L);
    float range = L.x * P.x + L.y * P.y;
    range /= dot;
    if (range > 1.01f || range < -0.01f) //make slightly over for triangular pinch points '/\'
        return;                        //point is not in range to be a pinch point
    float cross = SFUTIL::cross(P, L);
    float length = sqrtf(dot);
    float distance = cross / length;
    //distance = fabs(distance);

    //if(distance < minThickness)
    {
        float cross2 = SFUTIL::cross(*std::next(it) - *it, *it2 - *it);
        float deltaThickness; // = minThickness - distance;
        if (cross2 < 0)       //internal thin point
        {
            deltaThickness = minThickness - fabs(distance);
        }
        else
        { //external gap
            deltaThickness = minClearance - fabs(distance);
        }
        if (deltaThickness < 0.f)
            return;

        //compute the normal vector to the line ?
        Vec2 normalToLine(l0.y - l1.y, l1.x - l0.x);
        float l = SFUTIL::length<float>(normalToLine);
        normalToLine /= l;
        normalToLine *= deltaThickness;
        if (cross2 < 0) // && distance > 0) //internal thin point
        {
            *it2 -= normalToLine;
        }
        else if (cross2 > 0) // && distance < 0)//distance sign is useless for this problem
        {                    //external gap
            *it2 += normalToLine;
        }
    }
}

bool delayedfixPinchPoints(std::list<Point> &line, float minThickness, float minClearance, float variance, std::vector<PointFix> &fixes)
{
    bool isChanged = false;
    for (auto it = line.begin(); it != std::prev(line.end()); ++it) //std::prev(std::prev(line.end())); ++it)
    {
        //float clearDist;
        for (auto it2 = line.begin(); it2 != line.end(); ++it2)
        {
            if (it2 == it || it2 == std::next(it) || it2 == std::prev(it) || it2 == std::next(std::next(it)))
                continue;
            Point l0 = *it;
            Point l1 = *std::next(it);
            Point p = *it2;
            //copypasta
            const Vec2 L = l1 - l0;
            const Vec2 P = p - l0;
            float dot = SFUTIL::dot(L, L);
            float range = L.x * P.x + L.y * P.y;
            range /= dot;
            if (range > 1.01f || range < -0.01f) //make slightly over for triangular pinch points '/\'
                continue;                        //point is not in range to be a pinch point
            float cross = SFUTIL::cross(P, L);
            float length = sqrtf(dot);
            float distance = cross / length;
            //distance = fabs(distance);

            //if(distance < minThickness)
            {
                float cross2 = SFUTIL::cross(*std::next(it) - *it, *it2 - *it);
                float deltaThickness; // = minThickness - distance;
                if (cross2 < 0)       //internal thin point
                {
                    deltaThickness = minThickness - fabs(distance);
                }
                else
                { //external gap
                    deltaThickness = minClearance - fabs(distance);
                }
                if (deltaThickness <= variance)
                    continue;
                deltaThickness *= 1.1f;//01f;
                //compute the normal vector to the line ?
                Vec2 normalToLine(l0.y - l1.y, l1.x - l0.x);
                float l = SFUTIL::length<float>(normalToLine);
                normalToLine /= l;
                normalToLine *= deltaThickness;
                if (cross2 < 0) // && distance > 0) //internal thin point
                {
                    normalToLine *= -1.f;
                }
                fixes.push_back({it2, normalToLine});
                isChanged = true;
//                    *it2 -= normalToLine;
//                }
//                else if (cross2 > 0) // && distance < 0)//distance sign is useless for this problem
//                {                    //external gap
//                    *it2 += normalToLine;
//                }
            }
            //End copypasta
        }
    }
    return isChanged;
}

//yay I am fixing it kind of... is missing the very first line though?
void fixPinchPoints(std::list<Point> &line, float minThickness, float minClearance)
{
    for (auto it = line.begin(); it != std::prev(line.end()); ++it)//std::prev(std::prev(line.end())); ++it)
    {
        //float clearDist;
        for (auto it2 = line.begin(); it2 != line.end(); ++it2)
        {
            if (it2 == it || it2 == std::next(it) || it2 == std::prev(it) || it2 == std::next(std::next(it)))
                continue;
            Point l0 = *it;
            Point l1 = *std::next(it);
            Point p = *it2;
//copypasta
            const Vec2 L = l1 - l0;
            const Vec2 P = p - l0;
            float dot = SFUTIL::dot(L, L);
            float range = L.x * P.x + L.y * P.y;
            range /= dot;
            if (range > 1.01f || range < -0.01f) //make slightly over for triangular pinch points '/\'
                continue;//point is not in range to be a pinch point
            float cross = SFUTIL::cross(P, L);
            float length = sqrtf(dot);
            float distance = cross / length;
            //distance = fabs(distance);

            //if(distance < minThickness)
            {
                float cross2 = SFUTIL::cross(*std::next(it) - *it, *it2 - *it);
                float deltaThickness;// = minThickness - distance;
                if (cross2 < 0)       //internal thin point
                {
                    deltaThickness = minThickness - fabs(distance);
                }
                else
                { //external gap
                    deltaThickness = minClearance - fabs(distance);
                }
                if(deltaThickness < 0.f) continue;

                //compute the normal vector to the line ?
                Vec2 normalToLine( l0.y - l1.y, l1.x - l0.x );
                float l = SFUTIL::length<float>(normalToLine);
                normalToLine /= l;
                normalToLine *= deltaThickness;
                if (cross2 < 0)// && distance > 0) //internal thin point
                {
                    *it2 -= normalToLine;
                }
                else if (cross2 > 0)// && distance < 0)//distance sign is useless for this problem
                { //external gap
                    *it2 += normalToLine;
                }
                
            }
//End copypasta
        }
    }
}

//how remove?
//if p is < l0 erase p-l0 else erase l1-p
//don't erase... just move to minThickNess or minClearance, we know the direction from our sin/cos calcs already...
void markPinchPoints(std::list<Point> &line, float minThickness, float minClearance, std::list<ItPair> &squeezePts, std::list<ItPair> &thinPts)
{
    for (auto it = std::next(line.begin()); it != std::prev(std::prev(line.end())); ++it)
    {
        float clearDist;
        for (auto it2 = line.begin(); it2 != line.end(); ++it2)
        {
            if (it2 == it || it2 == std::next(it) || it2 == std::prev(it) || it2 == std::next(std::next(it)))
                continue;
            if (clearDist = howCloseIsPointToLine(*it, *std::next(it), *it2))
            {
                float cross = SFUTIL::cross(*std::next(it) - *it, *it2 - *it);
                if (cross < 0.f)
                {//internal
                    if (clearDist < minThickness)
                    {
                        //remove ???
                        thinPts.push_back(ItPair(it2,it));
                    }
                }
                else if (clearDist > minClearance)
                {
                    //mark
                    squeezePts.push_back(ItPair(it2, it));
                }
            }
        }
    }
}

//subdivides line so each segment is not too long.
void subdivideLine(std::list<Point> &line, float maxLength)//or minCLearance
{
    for(auto it = line.begin(); it != std::prev(line.end()); ++it)
    {
        Point a = *it;
        Point b = *std::next(it);
        Vec2 v = b - a;
        float length = SFUTIL::length<float>(v);
        int numSubdivisions = (int)(length / maxLength) + 1;
        v = v / length;
        
        if(numSubdivisions > 1)
        {
            float subLength = length / numSubdivisions;
            for(int i = 1; i < numSubdivisions; ++i)
            {
                //line 
                Point p = a + v * (i * subLength);
                it = line.insert(std::next(it), p);
            }
        }
    }
}