#include "PolyTriangulator.hpp"

//http://www.personal.kent.edu/%7Ermuhamma/Compgeometry/MyCG/PolyPart/polyPartition.htm

#include <iostream>
//#include <stack>

bool triangleHasArea(const Point &a, const Point &b, const Point &c)
{
    return (a.x * (b.y - c.y) + b.x * (c.y - a.y) + c.x * (a.y - b.y)) != 0.f;
}

//bool isToTheLeftOf(const Vector &RHS) const { return (x * RHS.y - y * RHS.x) < 0.0f; }
bool turnsLeft(const Point &a, const Point &b, const Point &c)
{
    sf::Vector2f v1 = b - a;
    sf::Vector2f v2 = c - a;
    return (v1.x * v2.y - v1.y * v2.x) <= 0.0f;//<=
}

void PolyTriangulator::triangulateMonotonePolygon(const std::vector<unsigned int> &monotonePolygon, const std::vector<bool> &isTopChain)
{
    {
    std::list<unsigned int> ActiveList;
    //the P[] id values...
    unsigned int Pid;
    Pid = vertices[monotonePolygon[0]].top;//for now... will be changing this later to be better
    ActiveList.push_back(Pid);
    Pid = vertices[monotonePolygon[1]].top;
    ActiveList.push_back(Pid);

    bool activeChain = isTopChain[1];
    bool pChain;
    Triangle T;
    std::list<unsigned int>::iterator curPt;

    for(unsigned int i = 2; i < monotonePolygon.size(); ++i)
    {
        Pid = vertices[monotonePolygon[i]].top;
        pChain = isTopChain[i];
        if(pChain != activeChain)
        {
            while(ActiveList.size()>1)
            {
                //output triangle...
                curPt = ActiveList.begin();
                T.a = (*curPt);
                T.b = (*std::next(curPt));
                T.c = Pid;
                if(triangleHasArea(P[T.a],P[T.b],P[T.c])) m_triangles.push_back(T);
                ActiveList.erase(curPt);
            }
            ActiveList.push_back(Pid);
            activeChain = pChain;
        }
        else
        {
            curPt = ActiveList.end();
            curPt = std::prev(curPt);
            bool conv = turnsLeft(P[(*curPt)], P[(*std::prev(curPt))], P[Pid]);
            if(activeChain) conv = !conv;
            while (conv && ActiveList.size() > 1)
            {
                T.a = (*curPt);
                T.b = (*std::prev(curPt));
                T.c = Pid;
                if(triangleHasArea(P[T.a],P[T.b],P[T.c])) m_triangles.push_back(T);
                ActiveList.erase(curPt);
                curPt = ActiveList.end();
                curPt = std::prev(curPt);
                conv = turnsLeft(P[(*curPt)], P[(*std::prev(curPt))], P[Pid]);
                if(activeChain) conv = !conv;
            }
            ActiveList.push_back(Pid);
        }
    }
    }
}