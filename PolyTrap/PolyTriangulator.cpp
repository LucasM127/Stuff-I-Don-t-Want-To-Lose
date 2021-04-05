#include "PolyTriangulator.hpp"
#include <algorithm>

#include <iostream>

PolyTriangulator::PolyTriangulator(Polygon &_P) : P(_P)
{
    init();
}

unsigned int last(unsigned int i, unsigned int n)
{
    return i == 0 ? n - 1 : i - 1;
}

unsigned int next(unsigned int i, unsigned int n)
{
    return (i + 1) % n;
}

void PolyTriangulator::init()
{
    pIDs.clear();
    vertices.clear();
    lsegs.clear();
    m_trapezoids.clear();
    m_lines.clear();

    unsigned int n = P.size();

    for(const auto &p : P)
    std::cout<<p.x<<" "<<p.y<<std::endl;

    //Sort our polygon vertices by increasing x then y
    {

        pIDs.resize(P.size());
        for (unsigned int i = 0; i < n; i++)
            pIDs[i] = i;

        auto isLessThan = [&](int a, int b) -> bool {
            if (isLess(P[a].x, P[b].x))//(P[a].x < P[b].x)
                return true;
            if (areEqual(P[a].x, P[b].x))//(P[a].x == P[b].x)
            {
                if (isLess(P[a].y, P[b].y))//(P[a].y < P[b].y)
                    return true;
            }
            return false;
        };
        std::sort(pIDs.begin(), pIDs.end(), isLessThan);

        for(auto id : pIDs) std::cout<<id<<" ";
        std::cout<<"pids"<<std::endl;
    }

    //form our lsegs array, so lsegs[i] = line from P[i] to P[i+1]
    {

        LSeg lseg;
        Point a, b;

        for (unsigned int i = 0; i < P.size(); ++i)
        {
            a = P[i];
            b = P[(i + 1) % n];
            if (isLess(a.x, b.x))//(a.x < b.x)
            {
                lseg.left = a;
                lseg.right = b;
            }
            else if (areEqual(a.x, b.x))//(a.x == b.x)
            {
                if (isLess(a.y, b.y))//(a.y < b.y)
                {
                    lseg.left = a;
                    lseg.right = b;
                }
                else
                {
                    lseg.left = b;
                    lseg.right = a;
                }
            }
            else
            {
                lseg.left = b;
                lseg.right = a;
            }
            lsegs.push_back(lseg);
        }
    }

    //form our Vertices array, is each vertice convex? etc... uses lsegs list.
    {

        Vertice V;
        unsigned int counter = 0;
        unsigned int nextID, lastID, curID;
        Point curPt, lastPt, nextPt;
        while (true)
        {
            curID = pIDs[counter];
            curPt = P[curID];
            lastID = (curID == 0) ? (n - 1) : (curID - 1);
            lastPt = P[lastID];
            nextID = (curID + 1) % n;
            nextPt = P[nextID];

            //V.bottom = V.top = curID;
            V.top = curID;
            V.x = curPt.x;

            //Split, Merge, Continue?
            if (isLess(lastPt.x, curPt.x) && isLess(nextPt.x, curPt.x))//(lastPt.x < curPt.x && nextPt.x < curPt.x)
            {
                V.type = Vertice::Type::MERGE;
            }
            else if (isGreater(lastPt.x, curPt.x) && isGreater(nextPt.x, curPt.x))//(lastPt.x > curPt.x && nextPt.x > curPt.x)
            {
                V.type = Vertice::Type::SPLIT;
            }
            else if (areEqual(lastPt.x, curPt.x) || areEqual(nextPt.x, curPt.x))//(lastPt.x == curPt.x || nextPt.x == curPt.x)
            {
                V.type = Vertice::Type::CONTINUE;
                if (areEqual(lastPt.x, curPt.x)) //(lastPt.x == curPt.x)
                {
                    if(isGreater(lastPt.y, curPt.y))//(lastPt.y > curPt.y)
                    {
                        if(nextPt.x > curPt.x)//L
                            V.type = Vertice::Type::SPLIT;
                    }
                    else if(isLess(lastPt.y, curPt.y))//(lastPt.y < curPt.y)
                    {
                        if(nextPt.x < curPt.x)//-|
                            V.type = Vertice::Type::MERGE;
                    }
                }
                else if (areEqual(nextPt.x, curPt.x)) //(nextPt.x == curPt.x)
                {
                    if(isGreater(nextPt.y, curPt.y))//(nextPt.y > curPt.y)
                    {
                        if(lastPt.x > curPt.x)
                            V.type = Vertice::Type::SPLIT;
                    }
                    else if(isLess(nextPt.y, curPt.y))//(nextPt.y < curPt.y)
                    {
                        if(lastPt.x < curPt.x)
                            V.type = Vertice::Type::MERGE;
                    }
                }
            }
            else
            {
                V.type = Vertice::Type::CONTINUE;
            }

            nextID = (nextID == 0) ? (n - 1) : (nextID - 1);

            if (V.type == Vertice::Type::CONTINUE)
            {
                //segA = id of segment to the left
                //segB = id of segment to the right
                if (isLess(lastPt.x, curPt.x))//(lastPt.x < curPt.x)
                {
                    V.segA = lastID;
                    V.segB = nextID;
                }
                else if (isGreater(lastPt.x, curPt.x))//(lastPt.x > curPt.x)
                {
                    V.segB = lastID;
                    V.segA = nextID;
                }
                else if (isLess(nextPt.x, curPt.x))//(nextPt.x < curPt.x)
                {
                    V.segB = lastID;
                    V.segA = nextID;
                }
                else if (isGreater(nextPt.x, curPt.x))//(nextPt.x > curPt.x)
                {
                    V.segA = lastID;
                    V.segB = nextID;
                }
                else throw(std::runtime_error("Duplicate point!"));
            }
            else //SPLIT or MERGE, segA on top, segB below
            {
                if (areEqual(lastPt.x, curPt.x))//(lastPt.x == curPt.x)//floatpoints are equal???
                {
                    if(isGreater(lastPt.y, curPt.y))//(lastPt.y > curPt.y)//last pt is higher
                    {
                        V.segA = lastID;
                        V.segB = nextID;
                    }
                    else if(isLess(lastPt.y, curPt.y))//(lastPt.y < curPt.y)
                    {
                        V.segB = lastID;
                        V.segA = nextID;
                    }
                    else throw(std::runtime_error("Duplicate point!"));
                }
                else if (areEqual(nextPt.x, curPt.x)) //(nextPt.x == curPt.x)
                {
                    if(isGreater(nextPt.y, curPt.y))//(nextPt.y > curPt.y)
                    {
                        V.segB = lastID;
                        V.segA = nextID;
                    }
                    else
                    {
                        V.segA = lastID;
                        V.segB = nextID;
                    }
                }
                else
                {
                    float testx; //shouldn't need a test case... as same side for a b
                    if (fabsf(lastPt.x - curPt.x) > fabsf(nextPt.x - curPt.x))
                        testx = nextPt.x;
                    else
                        testx = lastPt.x;
                    float lastYInt = getYIntValAtX(testx, lsegs[lastID]);
                    float nextYInt = getYIntValAtX(testx, lsegs[nextID]);
                    if (lastYInt > nextYInt)
                    {
                        V.segA = lastID;
                        V.segB = nextID;
                    }
                    else
                    {
                        V.segA = nextID;
                        V.segB = lastID;
                    }
                }
            }

            std::cout << "made vertice sega segb " << V.segA << " " << V.segB << std::endl;

            V.id = vertices.size();
            vertices.push_back(V);

            if (V.type == Vertice::Type::MERGE)
                lsegs[lastID].rVertice = lsegs[nextID].rVertice = V.id;
            else if (V.type == Vertice::Type::SPLIT)
                lsegs[lastID].lVertice = lsegs[nextID].lVertice = V.id;
            else
                lsegs[V.segA].rVertice = lsegs[V.segB].lVertice = V.id;

            counter++;
            if (counter >= n)
                break;
        }
    }
}

/**** trying vertical lines *******
void PolyTriangulator::init()
{
    pIDs.clear();
    vertices.clear();
    lsegs.clear();
    m_trapezoids.clear();
    m_lines.clear();

    unsigned int n = P.size();

    //Sort our polygon vertices by increasing x then y
    {

        pIDs.resize(P.size());
        for (unsigned int i = 0; i < n; i++)
            pIDs[i] = i;

        auto isLessThan = [&](int a, int b) -> bool {
            if (P[a].x < P[b].x)
                return true;
            if (P[a].x == P[b].x)
            {
                if (P[a].y < P[b].y)
                    return true;
            }
            return false;
        };
        std::sort(pIDs.begin(), pIDs.end(), isLessThan);
    }

    //form our lsegs array, so lsegs[i] = line from P[i] to P[i+1]
    {

        LSeg lseg;
        Point a, b;

        for (unsigned int i = 0; i < P.size(); ++i)
        {
            a = P[i];
            b = P[(i + 1) % n];
            if (a.x < b.x)
            {
                lseg.left = a;
                lseg.right = b;
            }
            else
            {
                lseg.left = b;
                lseg.right = a;
            }
            lsegs.push_back(lseg);
        }
    }

    //form our Vertices array, is each vertice convex? etc... uses lsegs list.
    {

        Vertice V;
        unsigned int counter = 0;
        unsigned int nextID, lastID, curID;
        
        Point curPt, lastPt, nextPt;
        while (true)
        {
            curID = pIDs[counter];
            curPt = P[curID];
            lastID = last(curID, n);
            lastPt = P[lastID];
            nextID = next(curID, n);
            nextPt = P[nextID];

            V.bottom = V.top = curID;
            V.x = curPt.x;

            //Split, Merge, Continue?
            if(counter == 0) {V.type = Vertice::Type::SPLIT;}
            else if(counter == n-1) {V.type = Vertice::Type::MERGE;}
            else if(lastPt.x == curPt.x || nextPt.x == curPt.x)
            {
                Point nextOrdered = P[pIDs[counter + 1]];
                Point lastOrdered = P[pIDs[counter - 1]];
                if(lastPt.x == nextPt.x)
                {
                    V.type = Vertice::Type::CONTINUE;
                }
                else if(nextOrdered.x == curPt.x && ( nextOrdered == nextPt || nextOrdered == lastPt ))
                {//split L or continue _|
                    if( (nextPt.x > curPt.x && lastPt.x == curPt.x)
                        || (lastPt.x > curPt.x && nextPt.x == curPt.x))
                        V.type = Vertice::Type::SPLIT;
                    else V.type = Vertice::Type::CONTINUE;
                }
                else if (lastOrdered.x == curPt.x && (lastOrdered == nextPt || lastOrdered == lastPt))
                {//merge -| or continue |- (top)
                    if( (nextPt.x < curPt.x && lastPt.x == curPt.x)
                        || (lastPt.x < curPt.x && nextPt.x == curPt.x))
                        V.type = Vertice::Type::MERGE;
                    else V.type = Vertice::Type::CONTINUE;
                }
                else V.type = Vertice::Type::CONTINUE;
            }
            else if (lastPt.x < curPt.x && nextPt.x < curPt.x)//note = change
            {
                V.type = Vertice::Type::MERGE;
            }
            else if (lastPt.x > curPt.x && nextPt.x > curPt.x)
            {
                V.type = Vertice::Type::SPLIT;
            }
            else
            {
                V.type = Vertice::Type::CONTINUE;
            }

            nextID = last(nextID, n);

            if (V.type == Vertice::Type::CONTINUE)
            {
                //segA = id of segment to the left
                //segB = id of segment to the right
                if (lastPt.x <= curPt.x)//added =
                {
                    V.segA = lastID;
                    V.segB = nextID;
                }
                else
                {
                    V.segA = nextID;
                    V.segB = lastID;
                }
            }
            else
            {
                //segA = id of segment on top
                //segB = id of segment below
                float testx; //shouldn't need a test case... as same side for a b
                if (fabsf(lastPt.x - curPt.x) > fabsf(nextPt.x - curPt.x))
                    testx = nextPt.x;
                else
                    testx = lastPt.x;
                float lastYInt = getYIntValAtX(testx, lsegs[lastID]);
                float nextYInt = getYIntValAtX(testx, lsegs[nextID]);
                if (lastYInt > nextYInt)
                {
                    V.segA = lastID;
                    V.segB = nextID;
                }
                else
                {
                    V.segA = nextID;
                    V.segB = lastID;
                }

                if(lastPt.x == curPt.x || nextPt.x == curPt.x)//last is irrelevant...
                {
                    if(lastPt.x == curPt.x)
                    {
                        if(lastPt.y > curPt.y)
                        {
                            V.segA = lastID;
                            V.segB = nextID;
                        }
                        else
                        {
                            V.segA = nextID;
                            V.segB = lastID;
                        }
                    }
                    else
                    {
                        if (nextPt.y > curPt.y)
                        {
                            V.segA = nextID;
                            V.segB = lastID;
                        }
                        else
                        {
                            V.segA = lastID;
                            V.segB = nextID;
                        }
                    }
                }
            }

            std::cout << "made vertice "<<counter<<" sega segb " << V.segA << " " << V.segB;//<< std::endl;
            if(V.type == Vertice::Type::CONTINUE) std::cout<<" continue "<<std::endl;
            if(V.type == Vertice::Type::MERGE) std::cout<<" merge "<<std::endl;
            if(V.type == Vertice::Type::SPLIT) std::cout<<" split "<<std::endl;

            V.id = vertices.size();
            vertices.push_back(V);

            if (V.type == Vertice::Type::MERGE)
                lsegs[lastID].rVertice = lsegs[nextID].rVertice = V.id;
            else if (V.type == Vertice::Type::SPLIT)
                lsegs[lastID].lVertice = lsegs[nextID].lVertice = V.id;
            else
                lsegs[V.segA].rVertice = lsegs[V.segB].lVertice = V.id;

            counter++;
            if (counter >= n)
                break;
        }
    }
}
*******/
/*****
void PolyTriangulator::init()
{
    pIDs.clear();
    vertices.clear();
    lsegs.clear();
    m_trapezoids.clear();
    m_lines.clear();

    unsigned int n = P.size();

    for(unsigned int i = 0; i<P.size(); i++)
    {
        if(isConvex(P[i == 0 ? n - 1 : i - 1],P[i],P[(i+1)%n])) isReflex.push_back(false);
        else isReflex.push_back(true);
        std::cout<<"P "<<i<<" : ";
        if(isReflex.back()) std::cout<<"reflex";
        else std::cout<<"concave";
        std::cout<<std::endl;
    }

    //Sort our polygon vertices by increasing x then y
    {

    pIDs.resize(P.size());
    for(unsigned int i = 0; i<n;i++) pIDs[i] = i;

    auto isLessThan = [&](int a, int b)->bool
    {
        if(P[a].x < P[b].x) return true;
        if(P[a].x == P[b].x)
        {
            if(P[a].y < P[b].y) return true;
        }
        return false;
    };
    std::sort(pIDs.begin(), pIDs.end(), isLessThan);

    }

    //form our lsegs array, so lsegs[i] = line from P[i] to P[i+1]
    {

    LSeg lseg;
    Point a, b;

    for(unsigned int i = 0; i<P.size(); ++i)
    {
        a = P[i];
        b = P[(i+1)%n];
        if(a.x < b.x)
        {
            lseg.left = a;
            lseg.right = b;
        }
        else
        {
            lseg.left = b;
            lseg.right = a;
        }
        lsegs.push_back(lseg);
    }

    }

    //form our Vertices array, is each vertice convex? etc... uses lsegs list.
    {

    Vertice V;
    unsigned int counter = 0;
    unsigned int nextID, lastID, curID;
    Point curPt, lastPt, nextPt;
    while(true)
    {
        curID = pIDs[counter];
        curPt = P[curID];
        lastID = (curID == 0) ? (n - 1) : (curID - 1);
        lastPt = P[lastID];
        nextID = (curID + 1) % n;
        nextPt = P[nextID];

        V.bottom = V.top = curID;
        V.x = curPt.x;

        //test vertical line segment vertices
        if(curPt.x == lastPt.x)
        {
            V.top = lastID;
            lastID = (lastID == 0) ? (n - 1) : (lastID - 1);
            lastPt = P[lastID];
            counter++;
        }
        else if(curPt.x == nextPt.x)
        {
            V.top = nextID;
            nextID = (nextID + 1) % n;
            nextPt = P[nextID];
            counter++;
        }

        //V.isConvex = isConvex(lastPt,curPt,nextPt);

        //Split, Merge, Continue?
        if(lastPt.x < curPt.x && nextPt.x < curPt.x)
        {
            V.type = Vertice::Type::MERGE;
        }
        else if(lastPt.x > curPt.x && nextPt.x > curPt.x)
        {
            V.type = Vertice::Type::SPLIT;
        }
        else
        {
            V.type = Vertice::Type::CONTINUE;
        }

        nextID = (nextID == 0) ? (n-1) : (nextID -1);

        if(V.type == Vertice::Type::CONTINUE)
        {
            //segA = id of segment to the left
            //segB = id of segment to the right
            if(lastPt.x < curPt.x)
            {
                V.segA = lastID;
                V.segB = nextID;
            }
            else
            {
                V.segA = nextID;
                V.segB = lastID;
            }
        }
        else
        {
            //segA = id of segment on top
            //segB = id of segment below
            float testx;//shouldn't need a test case... as same side for a b
            if( fabsf( lastPt.x - curPt.x ) > fabsf( nextPt.x - curPt.x) )
                testx = nextPt.x;
            else testx = lastPt.x;
            float lastYInt = getYIntValAtX(testx, lsegs[lastID]);
            float nextYInt = getYIntValAtX(testx, lsegs[nextID]);
            if(lastYInt > nextYInt)
            {
                V.segA = lastID;
                V.segB = nextID;
            }
            else
            {
                V.segA = nextID;
                V.segB = lastID;
            }
        }

        std::cout<<"made vertice sega segb "<<V.segA<<" "<<V.segB<<std::endl;

        V.id = vertices.size();
        vertices.push_back(V);

        if(V.type == Vertice::Type::MERGE)
            lsegs[lastID].rVertice = lsegs[nextID].rVertice = V.id;
        else if(V.type == Vertice::Type::SPLIT)
            lsegs[lastID].lVertice = lsegs[nextID].lVertice = V.id;
        else
            lsegs[ V.segA ].rVertice = lsegs[ V.segB ].lVertice = V.id;

        counter++;
        if(counter >= n) break;
    }

    }
}
***/

//mmhmm
void PolyTriangulator::sweep4( std::vector<int> &numTriPPoly)
{
    curEdges.clear();
    m_monotonePolygons.clear();
    m_triangles.clear();
    amTopChain.clear();

    auto getNextEdge = [&](const Vertice &V) -> auto
    {
        float x = V.x;
        float insertedY = P[V.top].y;
        std::list<Edge>::iterator it;
        float curEdgeBottomY;
        for (it = curEdges.begin(); it != curEdges.end(); ++it)
        {
            /*float curEdgeBottomY;
            if( (int)((lsegs[(*it).bottomLSeg].left.x+0.0002)*1000) == (int)((lsegs[(*it).bottomLSeg].right.x+0.0002)*1000) )
            {
                curEdgeBottomY = lowerOf(lsegs[(*it).bottomLSeg].left.y, lsegs[(*it).bottomLSeg].left.y);
            }
            else*/
            //what if it is a vertical segment???
            if(areEqual(lsegs[(*it).bottomLSeg].left.x,lsegs[(*it).bottomLSeg].right.x))
                curEdgeBottomY = fminf(lsegs[(*it).bottomLSeg].left.y,lsegs[(*it).bottomLSeg].right.y);
            else curEdgeBottomY = getYIntValAtX(x, lsegs[(*it).bottomLSeg]);//this is my issue!

            //if ((int)((insertedY+0.0002) * 1000) < (int)((curEdgeBottomY+0.0002) * 1000))//hmm or equal, first above or first same
            if(isLess(insertedY, curEdgeBottomY))
                return it; //now can't be inside at all...
        }
        return it;
    };

    std::list<Edge>::iterator nextEdge;

    for(auto &V : vertices)
    {
        nextEdge = getNextEdge(V);

        switch(V.type)
        {
            case Vertice::Type::SPLIT:
                handleSplit(V, nextEdge);
                break;
            case Vertice::Type::MERGE:
                handleMerge(V, nextEdge);
                break;
            case Vertice::Type::CONTINUE:
                handleContinue(V, nextEdge);
                break;
            default:
                break;
        }
    }

    std::cout<<std::endl;
    std::cout<<"Made "<<m_monotonePolygons.size()<<" monotone polygons with vertices :"<<std::endl;
    for(unsigned int i = 0; i<m_monotonePolygons.size(); i++)
    {
        std::cout<<"MP"<<i<<": ";
        for(unsigned int j = 0; j< m_monotonePolygons[i].size(); ++j)
        {
            std::cout<<m_monotonePolygons[i][j]<<"(";
            if(amTopChain[i][j]) std::cout<<"T";
            else std::cout<<"B";
            std::cout<<") ";
        }
        //for(auto Vid : m_monotonePolygons[i]) std::cout<<Vid<<" ";
        std::cout<<std::endl;
    }

    unsigned int ctr = 0;
    unsigned int lastctr = 0;
    Triangle T;
    std::cout<<"and triangulating"<<std::endl;
    for(unsigned int i = 0; i<m_monotonePolygons.size(); ++i)
    //int i = 5;
    {
        std::cout<<"MP"<<i<<":"<<std::endl;
        triangulateMonotonePolygon(m_monotonePolygons[i],amTopChain[i]);
        for (;ctr < m_triangles.size(); ctr++)
        {
            T = m_triangles[ctr];
            std::cout << "Tri" << ctr << ": " << T.a << " " << T.b << " " << T.c << std::endl;
        }
        numTriPPoly.push_back(ctr - lastctr);
        lastctr = ctr;
    }
}

/***** new attempt fail?
void PolyTriangulator::handleSplit(const Vertice &V, std::list<Edge>::iterator &nextEdge)
{
    Edge E;
    E.vertice = V.id;
    E.topLSeg = V.segA;
    E.bottomLSeg = V.segB;
    E.m_monotonePoly = m_monotonePolygons.size();

    if (nextEdge == curEdges.begin())
    {
        std::cout << "split new insert bottom" << std::endl;
        m_monotonePolygons.push_back(std::vector<unsigned int>());
        m_monotonePolygons[E.m_monotonePoly].push_back(V.id);
        curEdges.insert(nextEdge, E);
        return;
    }

    nextEdge = std::prev(nextEdge);

    float edgeTopY = getYIntValAtX(V.x, lsegs[(*nextEdge).topLSeg]);

    if (P[V.bottom].y > edgeTopY)
    {
        std::cout << "Split new insert above " << (*nextEdge).bottomLSeg << " " << (*nextEdge).topLSeg << " of " << E.bottomLSeg << " " << E.topLSeg << std::endl;
        m_monotonePolygons.push_back(std::vector<unsigned int>());
        m_monotonePolygons[E.m_monotonePoly].push_back(V.id);
        curEdges.insert(std::next(nextEdge), E);
        return;
    }

    //splits nextEdge
    //a merge to a continue, just continues the two edges on, no new edges made, no edges removed
    if (vertices[(*nextEdge).vertice].type == Vertice::Type::MERGE) //just continue, is the edge above... by definition
    {
        auto bottomEdge = std::prev(nextEdge);
        m_monotonePolygons[(*nextEdge).m_monotonePoly].push_back(V.id);
        m_monotonePolygons[(*bottomEdge).m_monotonePoly].push_back(V.id);
        return;
    }

    //splits from existing continue edge, so put back last edge too...
    //but do we insert above or below???
    bool insertBelow;
    if((*nextEdge).vertice == lsegs[(*nextEdge).topLSeg].lVertice)
        insertBelow = false;
    else
        insertBelow = true;
    
    m_monotonePolygons.push_back(std::vector<unsigned int>());
    m_monotonePolygons[E.m_monotonePoly].push_back((*nextEdge).vertice); //and the last one
    m_monotonePolygons[E.m_monotonePoly].push_back(V.id);
    m_monotonePolygons[(*nextEdge).m_monotonePoly].push_back(V.id); //to the split one too...

    std::cout << "splitting at x " << V.x << " edge " << (*nextEdge).bottomLSeg << " " << (*nextEdge).topLSeg;
    
    if (insertBelow)
    {
        unsigned int tempLSeg = (*nextEdge).bottomLSeg;
        (*nextEdge).vertice = V.id;
        (*nextEdge).bottomLSeg = E.topLSeg; //no good...
        E.topLSeg = E.bottomLSeg;
        E.bottomLSeg = tempLSeg;
        curEdges.insert(nextEdge, E);
    }
    else
    {
        unsigned int tempLSeg = (*nextEdge).topLSeg;
        (*nextEdge).vertice = V.id;
        (*nextEdge).topLSeg = E.bottomLSeg;//vsegb
        E.bottomLSeg = E.topLSeg;//vsega
        E.topLSeg = tempLSeg;
        curEdges.insert(std::next(nextEdge), E); //above
    }
    std::cout << " becoming 2 edges e1 " << E.bottomLSeg << " " << E.topLSeg << " e2 " << (*nextEdge).bottomLSeg << " " << (*nextEdge).topLSeg << std::endl;
}

void PolyTriangulator::handleMerge(const Vertice &V, std::list<Edge>::iterator &nextEdge)
{
    auto topEdge = std::prev(nextEdge);

    if (vertices[(*topEdge).vertice].type == Vertice::Type::MERGE)//so the one above was a merge itself...
    {//need to merge it together with other one, is it below or above?
        //merge the 2 together to new top edge
        if(topEdge == curEdges.begin()) throw std::runtime_error("TopEdge is Begin!");
        auto bottomEdge = std::prev(topEdge); //how do i know?
        //if(topTopEdge == curEdges.end()) throw std::runtime_error("TopTop is end!");
        topEdge = merge(topEdge, bottomEdge, V);//adds V.id to both chains//removes other edge
    }//hmmm???

    //remove it is isolated merge...
    if ((*topEdge).topLSeg == V.segA)
    {
        std::cout<<"Removing merged edge "<<V.segB<<" "<<V.segA<<" at x "<<V.x<<std::endl;
        m_monotonePolygons[(*topEdge).m_monotonePoly].push_back(V.id);
        curEdges.erase(topEdge);
        return;
    }

    //is a inside merge of a top and a bottom edge, top edge is already premerged

    if(topEdge == curEdges.begin()) throw std::runtime_error("TopEdge is begin!");
    auto bottomEdge = std::prev(topEdge);//only if is not a continue!!!
    //merge with below last
    if (vertices[(*bottomEdge).vertice].type == Vertice::Type::MERGE)
    {
        if(bottomEdge == curEdges.begin()) throw std::runtime_error("BottomEdge is begin!");
        auto nextDown = std::prev(bottomEdge); //below bottom
        bottomEdge = merge(bottomEdge, nextDown, V);
    }

    //continue the two edges to next event
    if (vertices[(*bottomEdge).vertice].type != Vertice::Type::MERGE)
        m_monotonePolygons[(*bottomEdge).m_monotonePoly].push_back(V.id);
    if (vertices[(*topEdge).vertice].type != Vertice::Type::MERGE)
        m_monotonePolygons[(*topEdge).m_monotonePoly].push_back(V.id);

    (*topEdge).vertice = V.id;
    (*bottomEdge).vertice = V.id;
}

void PolyTriangulator::handleContinue(const Vertice &V, std::list<Edge>::iterator &nextEdge)
{
    if(nextEdge == curEdges.begin()) throw std::runtime_error("NextEdge is begin in continue!");
    auto curEdge = std::prev(nextEdge);

    if(vertices[(*curEdge).vertice].type == Vertice::Type::MERGE)
    {
        std::list<Edge>::iterator topEdge, bottomEdge;
        if ((*curEdge).topLSeg == V.segA) //lines up with top
        {
            topEdge = curEdge;
            bottomEdge = std::prev(curEdge);
        }
        else
        {
            bottomEdge = curEdge;
            topEdge = std::next(curEdge);
        }
        curEdge = merge(topEdge,bottomEdge,V);
    }

    if ((*curEdge).topLSeg == V.segA)
    {
        std::cout << "CONTINUING topseg " << (*curEdge).topLSeg << " to " << V.segB << std::endl;
        (*curEdge).topLSeg = V.segB;
    }
    else
    {
        std::cout << "CONTINUING btmseg " << (*curEdge).bottomLSeg << " to " << V.segB << " with V " << V.top << std::endl;
        (*curEdge).bottomLSeg = V.segB;
    }
    (*curEdge).vertice = V.id;
    if (vertices[(*curEdge).vertice].type != Vertice::Type::MERGE)
        m_monotonePolygons[(*curEdge).m_monotonePoly].push_back(V.id);
}
***/


std::list<Edge>::iterator PolyTriangulator::merge(const std::list<Edge>::iterator& topEdge, const std::list<Edge>::iterator& bottomEdge, const Vertice &V)
{
    m_monotonePolygons[(*topEdge).m_monotonePoly].push_back(V.id);
    m_monotonePolygons[(*bottomEdge).m_monotonePoly].push_back(V.id);

    bool removeTop;
    //merge the two edges at the next vertice.
    std::cout << "Merging " << (*bottomEdge).bottomLSeg << " " << (*bottomEdge).topLSeg << " " << (*topEdge).bottomLSeg << " " << (*topEdge).topLSeg;
    std::cout << " to " << (*bottomEdge).bottomLSeg << " " << (*topEdge).topLSeg << std::endl;
    if(V.type == Vertice::Type::CONTINUE)//left
    {
        if(V.segA == (*topEdge).topLSeg) removeTop = true;
        else removeTop = false;
    }
    else //merge only, split is handled by itself
    {
        if(V.segB == (*topEdge).topLSeg) removeTop = true;
        else removeTop = false;
//        if(V.segA == (*bottomEdge).bottomLSeg) removeTop = false;
    }

    if(removeTop)
    {
        (*bottomEdge).topLSeg = (*topEdge).topLSeg;
        (*bottomEdge).vertice = V.id;
        curEdges.erase(topEdge);
        return bottomEdge;
    }
    else
    {
        (*topEdge).bottomLSeg = (*bottomEdge).bottomLSeg;
        (*topEdge).vertice = V.id;
        curEdges.erase(bottomEdge);
        return topEdge;
    }
}

void PolyTriangulator::addDiagonal(Edge &edge, const Vertice &V)
{
    /*
    Point left, right;
    if(P[vertices[edge.vertice].top].y < P[V.bottom].y)
    {
        left = P[vertices[edge.vertice].top];
        right = P[V.bottom];
    }
    else
    {
        left = P[vertices[edge.vertice].bottom];
        right = P[V.top];
    }*/
    std::cout<<"adding diagonal"<<std::endl;
    //m_monotonePolygons[edge.m_monotonePoly].push_back(V.id);
    if(edge.m_sharedPoly != -1)
    {
        m_monotonePolygons[edge.m_sharedPoly].push_back(V.id);
        edge.m_sharedPoly = -1;
    }
}
/*
void PolyTriangulator::addDiagonal(const Vertice &leftV, const Vertice &rightV)
{
    Point left, right;
    if(P[leftV.top].y < P[rightV.bottom].y)
    {
        left = P[leftV.top];
        right = P[rightV.bottom];
    }
    else
    {
        left = P[leftV.bottom];
        right = P[rightV.top];
    }
    std::cout<<"add diagonal from ";
    sf::Vertex V;
    V.color = sf::Color::Magenta;
    V.position = left;//P[vertices[leftV].id];
    std::cout<<V.position.x<<" "<<V.position.y<<" to ";
    m_lines.push_back(V);
    V.position = right;//P[vertices[rightV].id];
    m_lines.push_back(V);
    std::cout<<V.position.x<<" "<<V.position.y<<std::endl;
}*/

void PolyTriangulator::getTrap (const Edge &edge, float x)
{
    Trap T;
    float x_last, y_top, y_bottom;
    //float lasty_bot, lasty_top;
    x_last = vertices[edge.vertice].x;
    if(areEqual(x_last,x)) return;
    y_top = getYIntValAtX(x_last, lsegs[edge.topLSeg]);
    y_bottom = getYIntValAtX(x_last, lsegs[edge.bottomLSeg]);
    T.topLeft.x = T.bottomLeft.x = x_last;
    T.topLeft.y = y_top;
    T.bottomLeft.y = y_bottom;
    //lasty_bot = y_bottom;
    //lasty_top = y_top;
    y_top = getYIntValAtX(x, lsegs[edge.topLSeg]);
    y_bottom = getYIntValAtX(x, lsegs[edge.bottomLSeg]);
    T.topRight.x = T.bottomRight.x = x;
    T.topRight.y = y_top;
    T.bottomRight.y = y_bottom;
    m_trapezoids.push_back(T);
    //std::cout<<"trap made: x "<<x_last<<" y "<<lasty_bot<<" "<<lasty_top<<" to x "<<x<<" y "<<y_bottom<<" "<<y_top<<std::endl;
}

void PolyTriangulator::handleSplit(const Vertice &V, std::list<Edge>::iterator &nextEdge)
{
    Edge E;
    E.vertice = V.id;
    E.topLSeg = V.segA;
    E.bottomLSeg = V.segB;
    E.m_monotonePoly = m_monotonePolygons.size();

    if (nextEdge == curEdges.begin())
    {
        std::cout << "split new insert bottom" << std::endl;
        m_monotonePolygons.push_back(std::vector<unsigned int>());
        m_monotonePolygons[E.m_monotonePoly].push_back(V.id);
        amTopChain.push_back(std::vector<bool>());
        amTopChain[E.m_monotonePoly].push_back(false); //first and last
        curEdges.insert(nextEdge, E);
        return;
    }

    nextEdge = std::prev(nextEdge);

    float edgeTopY = getYIntValAtX(V.x, lsegs[(*nextEdge).topLSeg]);

    if (P[V.top].y > edgeTopY)//(P[V.bottom].y > edgeTopY)
    {
        std::cout << "Split new insert above " << (*nextEdge).bottomLSeg << " " << (*nextEdge).topLSeg << " of " << E.bottomLSeg << " " << E.topLSeg << std::endl;
        m_monotonePolygons.push_back(std::vector<unsigned int>());
        m_monotonePolygons[E.m_monotonePoly].push_back(V.id);
        amTopChain.push_back(std::vector<bool>());
        amTopChain[E.m_monotonePoly].push_back(false); //first and last
        curEdges.insert(std::next(nextEdge), E);
        return;
    }

    //else splitting the existing edge...
    getTrap((*nextEdge), V.x);

    bool insertAbove;
    if(vertices[(*nextEdge).vertice].type == Vertice::Type::CONTINUE)
    {
        if ((*nextEdge).topLSeg == vertices[(*nextEdge).vertice].segB)
            insertAbove = true;
        else insertAbove = false;
    }
    else if(vertices[(*nextEdge).vertice].type == Vertice::Type::SPLIT)
    {
        //above or below...
        if ((*nextEdge).bottomLSeg == vertices[(*nextEdge).vertice].segA)//last top
            insertAbove = false;
        else insertAbove = true;
    }
    else//split to merge already taken care of
    {
        insertAbove = true;
    }
    //if(vertices[(*nextEdge).vertice].type == Vertice::Type::SPLIT) insertAbove = false;
    //if(vertices[V.id].type == Vertice::Type::SPLIT) insertAbove=false;

    if (vertices[(*nextEdge).vertice].type == Vertice::Type::MERGE) //merge to continue...
    {
        E.m_monotonePoly = (*nextEdge).m_sharedPoly;
    }
    else
    {
        m_monotonePolygons.push_back(std::vector<unsigned int>());
        m_monotonePolygons[E.m_monotonePoly].push_back((*nextEdge).vertice);
        amTopChain.push_back(std::vector<bool>());
        amTopChain[E.m_monotonePoly].push_back(false); //start
    }

    m_monotonePolygons[(*nextEdge).m_monotonePoly].push_back(V.id);
    m_monotonePolygons[E.m_monotonePoly].push_back(V.id);
    
    if(insertAbove)
    {
        amTopChain[(*nextEdge).m_monotonePoly].push_back(true);
        amTopChain[E.m_monotonePoly].push_back(false);
    }
    else
    {
        amTopChain[(*nextEdge).m_monotonePoly].push_back(false);
        amTopChain[E.m_monotonePoly].push_back(true);
    }

    std::cout << "splitting " << (*nextEdge).bottomLSeg << " " << (*nextEdge).topLSeg; //below or above?
    if(insertAbove)
    {
        //if(vertices[V.id].type == Vertice::Type::SPLIT) //std::cout<<" to last split ";
        std::cout <<" above ";
        unsigned int tempLSeg = (*nextEdge).topLSeg;
        (*nextEdge).vertice = V.id;
        (*nextEdge).topLSeg = E.bottomLSeg; //vsegb
        E.bottomLSeg = E.topLSeg;           //vsega
        E.topLSeg = tempLSeg;
        curEdges.insert(std::next(nextEdge), E); //above
        std::cout << " to " << (*nextEdge).bottomLSeg << " " << (*nextEdge).topLSeg << " " << E.bottomLSeg << " " << E.topLSeg << std::endl;
    }
    else
    {
        std::cout << " below ";
        unsigned int tempLSeg = (*nextEdge).bottomLSeg;
        (*nextEdge).vertice = V.id;
        (*nextEdge).bottomLSeg = E.topLSeg;
        E.topLSeg = E.bottomLSeg;
        E.bottomLSeg = tempLSeg;
        curEdges.insert(nextEdge, E);
        std::cout << " to " << " " << E.bottomLSeg << " " << E.topLSeg << (*nextEdge).bottomLSeg << " " << (*nextEdge).topLSeg << std::endl;
    }
}
   
void PolyTriangulator::handleMerge(const Vertice &V, std::list<Edge>::iterator &nextEdge)
{
    auto edge = std::prev(nextEdge);
    getTrap((*edge), V.x);

    if( vertices[(*edge).vertice].type == Vertice::Type::MERGE)//last was a merge... line to right!
    {
        //remove bottom edge if on top// has to.
        m_monotonePolygons[(*edge).m_monotonePoly].push_back(V.id);
        amTopChain[(*edge).m_monotonePoly].push_back(false);
        (*edge).m_monotonePoly = (*edge).m_sharedPoly;
    }

    if((*edge).topLSeg == V.segA)//isolated merge
    {
        std::cout << "Removing merged edge " << V.segB << " " << V.segA << " at x " << V.x << std::endl;
        m_monotonePolygons[(*edge).m_monotonePoly].push_back(V.id);
        amTopChain[(*edge).m_monotonePoly].push_back(false);//last of chain
//        amTopChain[(*edge).m_monotonePoly].push_back(true); //last of chain
        curEdges.erase(edge);
        return;
    }

    //an internal merge, of topedge (edge) and bottom edge
    if(edge == curEdges.begin())
    {
        getTrap(*edge, V.x);
        throw std::runtime_error("Internal Merge TopEdge is begin!");
    }
    auto bottomEdge = std::prev(edge);
    getTrap((*bottomEdge), V.x);

    if (vertices[(*bottomEdge).vertice].type == Vertice::Type::MERGE)
    {
        //removes the top edge below a merge
        m_monotonePolygons[(*bottomEdge).m_sharedPoly].push_back(V.id);
        amTopChain[(*bottomEdge).m_sharedPoly].push_back(false); //last of chain
//        amTopChain[(*bottomEdge).m_sharedPoly].push_back(true); //last of chain
    }
    
    std::cout<<"Merging"<<std::endl;

    //remove the top edge, and put it as shared of bottom edge
    //merge the 2 edges to 1 edge
    (*bottomEdge).topLSeg = (*edge).topLSeg;
    (*bottomEdge).m_sharedPoly = (*edge).m_monotonePoly;
    (*bottomEdge).vertice = V.id;
    m_monotonePolygons[(*bottomEdge).m_monotonePoly].push_back(V.id);
    m_monotonePolygons[(*edge).m_monotonePoly].push_back(V.id);
    amTopChain[(*bottomEdge).m_monotonePoly].push_back(true);
    amTopChain[(*edge).m_monotonePoly].push_back(false);
    curEdges.erase(edge);
}

void PolyTriangulator::handleContinue(const Vertice &V, std::list<Edge>::iterator &nextEdge)
{
    if(nextEdge == curEdges.begin()) throw std::runtime_error("NextEdge is begin in continue!");
    auto curEdge = std::prev(nextEdge);
    getTrap((*curEdge), V.x);

    if (vertices[(*curEdge).vertice].type == Vertice::Type::MERGE)
    {
        if((*curEdge).topLSeg == V.segA)//next V is on top, remove the top edge already done
        {
            m_monotonePolygons[(*curEdge).m_sharedPoly].push_back(V.id);
            amTopChain[(*curEdge).m_sharedPoly].push_back(false);//last of chain
//            amTopChain[(*curEdge).m_sharedPoly].push_back(true); //last of chain
        }
        else //removing the 'bottom' edge, swap shared to current...
        {
            m_monotonePolygons[(*curEdge).m_monotonePoly].push_back(V.id);
            amTopChain[(*curEdge).m_monotonePoly].push_back(false);//last of chain
//            amTopChain[(*curEdge).m_monotonePoly].push_back(true); //last of chain
            (*curEdge).m_monotonePoly = (*curEdge).m_sharedPoly;
        }
    }

    if ((*curEdge).topLSeg == V.segA)
    {
        std::cout << "CONTINUING topseg " << (*curEdge).topLSeg << " to " << V.segB << std::endl;
        (*curEdge).topLSeg = V.segB;
        amTopChain[(*curEdge).m_monotonePoly].push_back(true);
    }
    else
    {
        std::cout << "CONTINUING btmseg " << (*curEdge).bottomLSeg << " to " << V.segB << " with V " << V.top << std::endl;
        (*curEdge).bottomLSeg = V.segB;
        amTopChain[(*curEdge).m_monotonePoly].push_back(false);
    }

    (*curEdge).vertice = V.id;
    m_monotonePolygons[(*curEdge).m_monotonePoly].push_back(V.id);
}

    /*****
void PolyTriangulator::handleContinue(const Vertice &V, std::list<Edge>::iterator &nextEdge)
{
    Edge &edge = *std::prev(nextEdge);

    getTrap(edge, V.x);
    if(vertices[edge.vertice].type == Vertice::Type::MERGE)
    {
        std::list<Edge>::iterator topEdge, bottomEdge;
        addDiagonal(vertices[edge.vertice], V);
        //neeed to find top or bottom?
        if(edge.topLSeg == V.segA)//lines up with top
        {
            topEdge = std::prev(nextEdge);
            bottomEdge = std::prev(topEdge);
        }
        else
        {
            bottomEdge = std::prev(nextEdge);
            topEdge = nextEdge;
        }
        merge(topEdge,bottomEdge,V);
    }

    if (edge.topLSeg == V.segA)
    {
        std::cout << "CONTINUING topseg " << edge.topLSeg << " to " << V.segB << std::endl;
        edge.topLSeg = V.segB;
    }
    else
    {
        std::cout << "CONTINUING btmseg " << edge.bottomLSeg << " to " << V.segB << " with V " << V.top << std::endl;
        edge.bottomLSeg = V.segB;
    }
    edge.vertice = V.id;
    if(vertices[edge.vertice].type != Vertice::Type::MERGE)
        m_monotonePolygons[edge.m_monotonePoly].push_back(V.id);
}

void PolyTriangulator::handleMerge(const Vertice &V, std::list<Edge>::iterator &nextEdge)
{
    Edge E;
    E.vertice = V.id;
    E.topLSeg = V.segA;
    E.bottomLSeg = V.segB;
    E.m_sharedPoly = -1;//do we even use the E though? not really, not inserting...

    nextEdge = std::prev(nextEdge);

    //merge with above last which is itself now merging
    if (vertices[(*nextEdge).vertice].type == Vertice::Type::MERGE)
    {
        addDiagonal((*nextEdge), V);
        //nextEdge is just above V here, (including V), so top is above nextEdge
        //auto topEdge = std::next(nextEdge);
        //nextEdge = merge(topEdge, nextEdge, V);// top bottom V
    }//should be ok here

    //is an isolated merge, just remove it, no effect in future
    if ((*nextEdge).topLSeg == E.topLSeg)
    {
        std::cout<<"Removing merged edge "<<E.bottomLSeg<<" "<<E.topLSeg<<" at x "<<V.x<<std::endl;
        //m_monotonePolygons[(*nextEdge).m_monotonePoly].push_back(V.id);
        getTrap((*nextEdge), V.x);
        curEdges.erase(nextEdge);
        return;
    }

    auto bottomEdge = std::prev(nextEdge);
    getTrap((*nextEdge), V.x);
    getTrap((*bottomEdge), V.x);

    //merge with below last
    if (vertices[(*bottomEdge).vertice].type == Vertice::Type::MERGE)
    {
        addDiagonal((*bottomEdge),V);
        //auto nextDown = std::prev(bottomEdge);//below bottom
        //bottomEdge = merge(bottomEdge, nextDown, V);
        //addDiagonal(vertices[(*bottomEdge).vertice], V);
    }

    //just continue the 2 edges to next event. no erasing...
    //no sharing needed either then too

//    std::cout << "Merging " << (*bottomEdge).bottomLSeg << " " << (*bottomEdge).topLSeg << " " << (*nextEdge).bottomLSeg << " " << (*nextEdge).topLSeg;
//    (*bottomEdge).topLSeg = (*nextEdge).topLSeg;
//    (*bottomEdge).vertice = V.id;
//    std::cout << " to " << (*bottomEdge).bottomLSeg << " " << (*bottomEdge).topLSeg << std::endl;
    (*nextEdge).vertice = V.id;
    (*bottomEdge).vertice = V.id;
//    if (vertices[(*bottomEdge).vertice].type != Vertice::Type::MERGE)
//        m_monotonePolygons[(*bottomEdge).m_monotonePoly].push_back(V.id);
//    if (vertices[(*nextEdge).vertice].type != Vertice::Type::MERGE)
//        m_monotonePolygons[(*nextEdge).m_monotonePoly].push_back(V.id);
//oh my word this is overly complicated,
    //if((*nextEdge).vertice)
//    (*bottomEdge).m_sharedPoly = (*nextEdge).m_monotonePoly;
    curEdges.erase(nextEdge);//is gone, but we still can continue the list, but how???//shitty shitty shitty... the edge erased is not the edge here.
    
}

void PolyTriangulator::handleSplit(const Vertice &V, std::list<Edge>::iterator &nextEdge)
{
    Edge E;
    E.vertice = V.id;
    E.topLSeg = V.segA;
    E.bottomLSeg = V.segB;
    E.m_sharedPoly = -1;//inserting it

    //new edge, create new edge and insert vertice is good
    if (nextEdge == curEdges.begin())
    {
        std::cout << "split new insert bottom" <<std::endl;
        E.m_monotonePoly = m_monotonePolygons.size();
        m_monotonePolygons.push_back(std::vector<unsigned int>());
        m_monotonePolygons[E.m_monotonePoly].push_back(V.id);
        curEdges.insert(nextEdge, E);
        return;
    }

    nextEdge = std::prev(nextEdge);

    float edgeTopY = getYIntValAtX(V.x, lsegs[(*nextEdge).topLSeg]);

    //same here
    if (P[V.bottom].y > edgeTopY)
    {
        std::cout << "Split new insert above " << (*nextEdge).bottomLSeg << " " << (*nextEdge).topLSeg << " of " << E.bottomLSeg << " " << E.topLSeg << std::endl;
        E.m_monotonePoly = m_monotonePolygons.size();
        m_monotonePolygons.push_back(std::vector<unsigned int>());
        m_monotonePolygons[E.m_monotonePoly].push_back(V.id);
        curEdges.insert(std::next(nextEdge), E);
        return;
    }

    //splits an existing edge, nextedge
    getTrap((*nextEdge), V.x);
    addDiagonal((*nextEdge), V);

    //a merge to a continue, just continues the two edges on, no new edges made, no edges removed
    if(vertices[(*nextEdge).vertice].type == Vertice::Type::MERGE)//just continue, is the edge above... by definition
    {
        auto bottomEdge = std::prev(nextEdge);
        m_monotonePolygons[(*nextEdge).m_monotonePoly].push_back(V.id);
        m_monotonePolygons[(*bottomEdge).m_monotonePoly].push_back(V.id);
        return;
    }

    //splits from existing continue edge, so put back last edge too...
    //but do we insert above or below???
    bool insertBelow;
    if((*nextEdge).bottomLSeg == V.segA)//continues on above, insert new below
        insertBelow = true;
    else insertBelow = false;
    E.m_monotonePoly = m_monotonePolygons.size();
    m_monotonePolygons.push_back(std::vector<unsigned int>());
    m_monotonePolygons[E.m_monotonePoly].push_back((*nextEdge).vertice);//and the last one
    m_monotonePolygons[E.m_monotonePoly].push_back(V.id);
    m_monotonePolygons[(*nextEdge).m_monotonePoly].push_back(V.id);//to the split one too...

    std::cout<<"splitting at x "<<V.x<<" edge "<<(*nextEdge).bottomLSeg<<" "<<(*nextEdge).topLSeg;
    unsigned int tempLSeg = (*nextEdge).bottomLSeg;
    (*nextEdge).vertice = V.id;
    (*nextEdge).bottomLSeg = E.topLSeg;
    E.topLSeg = E.bottomLSeg;
    E.bottomLSeg = tempLSeg;
    if(insertBelow) curEdges.insert(nextEdge, E);
    else curEdges.insert(std::next(nextEdge), E);//above
    std::cout << " becoming 2 edges e1 " << E.bottomLSeg << " " << E.topLSeg << " e2 " << (*nextEdge).bottomLSeg << " " << (*nextEdge).topLSeg << std::endl;
}
***/
    /***************
void PolyTriangulator::sweep2(std::vector<sf::Vertex> &emitLines)
{
    std::list<Edge> edges;//or trap faces
    //Edge E;

    auto addLine = [&](float x, float y1, float y2) 
    {
        /*sf::Vertex V;
        V.color = sf::Color::Green;
        V.position.x = x;
        V.position.y = y1;
        emitLines.push_back(V);
        V.position.y = y2;
        emitLines.push_back(V);*   /
    };

    //CONTINUE
    auto replace = [&](const Vertice &V)
    {
        //replace an edge with another edge...
        float x = V.x;
        bool found = false;
        float bottomYInt, topYInt;

        for(auto &edge : edges)
        {
            if(edge.topLSeg == V.segA)
            {
                edge.topLSeg = V.segB;
                topYInt = P[V.top].y;
                bottomYInt = getYIntValAtX(x,lsegs[edge.bottomLSeg]);
                found = true;
            }
            if(edge.bottomLSeg == V.segA)
            {
                edge.bottomLSeg = V.segB;
                bottomYInt = P[V.bottom].y;
                topYInt = getYIntValAtX(x, lsegs[edge.topLSeg]);
                found = true;
            }
            if(found)
            {
                std::cout<<"replacing segment "<<V.segA<<" with "<<V.segB<<" at "<<V.x<<std::endl;
                if(vertices[edge.vertice].type == Vertice::Type::MERGE)
                {
                    //diagonal to it from this vertice...
                    sf::Vertex vx;
                    vx.color = sf::Color::Red;
                    vx.position.x = x;
                    vx.position.y = P[V.top].y;//unsure of this stuff here...
                    emitLines.push_back(vx);
                    vx.position.x = vertices[edge.vertice].x;
                    vx.position.y = P[vertices[edge.vertice].top].y;
                    emitLines.push_back(vx);
                }
                edge.vertice = V.id;
                addLine(x, bottomYInt, topYInt);
                return;
            }
        }
    };

    //MERGE - diagonal to right, future
    auto remove = [&](const Vertice &V)
    {
        float x = V.x;

        for( auto it = edges.begin(); it != edges.end(); ++it)
        {
            Edge &curEdge = (*it);
            //float bottomYInt = getYIntValAtX(x, lsegs[curEdge.bottomLSeg]); //bottom
            //float topYInt = getYIntValAtX(x, lsegs[curEdge.topLSeg]);

            if (curEdge.bottomLSeg == V.segB)//should not need this anyways...
            {
                std::cout<<"removing segs "<<curEdge.topLSeg<<" "<<curEdge.bottomLSeg<<" at V.x"<<std::endl;
                edges.erase(it);
                return;
            }
            if(curEdge.topLSeg == V.segB)//is between, joins! ???
            {//merge the two edges together, and remove one
                Edge &nextEdge = (*std::next(it)); //top edge
                std::cout<<"removing segs "<<curEdge.topLSeg<<" "<<nextEdge.bottomLSeg<<" at V.x"<<std::endl;
                
                nextEdge.bottomLSeg = curEdge.bottomLSeg;
                nextEdge.vertice = V.id;

                float bottomYInt = getYIntValAtX(x, lsegs[curEdge.bottomLSeg]); //bottom
                float topYInt = getYIntValAtX(x, lsegs[nextEdge.topLSeg]);
                addLine(x, bottomYInt, topYInt);

                std::cout<<"leaving segs "<<nextEdge.topLSeg<<" "<<nextEdge.bottomLSeg<<" at V.x"<<std::endl;
                
                edges.erase(it);
                return;
            }
        }
    };

    //only insert edges when split convex vertice
    auto insert = [&](const Vertice &V)
    {
        //float insertedBottom = P[vertices[E.vertice].bottom].y;//bottom of split vertice
        float insertedY = P[V.top].y;
        float x = V.x;

        Edge E;
        E.topLSeg = V.segA;
        E.bottomLSeg = V.segB;
        E.vertice = V.id;

        std::list<Edge>::iterator it;
        for( it = edges.begin(); it != edges.end(); ++it )
        {
            Edge &curEdge = (*it);
            float bottomYInt = getYIntValAtX( x, lsegs[curEdge.bottomLSeg]);//bottom
            float topYInt = getYIntValAtX( x, lsegs[curEdge.topLSeg]);
            
            //a splitter, draw line to last vertice
            if(insertedY > bottomYInt && insertedY < topYInt)//insert inside, split the edge
            {
                std::cout<<"inserting segs "<<E.topLSeg<<" "<<E.bottomLSeg<<" at "<<V.x<<std::endl;
                //copy pasta//////////////
                sf::Vertex vx;
                vx.color = sf::Color::Red;
                vx.position.x = x;
                vx.position.y = P[V.bottom].y; //unsure of this stuff here...
                emitLines.push_back(vx);
                vx.position.x = vertices[curEdge.vertice].x;
                vx.position.y = P[vertices[curEdge.vertice].top].y;
                emitLines.push_back(vx);
                ///////////////

                unsigned int tempLSeg = curEdge.bottomLSeg;
                curEdge.bottomLSeg = E.topLSeg;
                E.topLSeg = E.bottomLSeg;
                E.bottomLSeg = tempLSeg;
                curEdge.vertice = E.vertice;

                std::cout<<"splitting so top to bottom is "<<curEdge.topLSeg<<" "<<curEdge.bottomLSeg<<" "<<E.topLSeg<<" "<<E.bottomLSeg<<std::endl;
                //draw the lines...
                addLine(x, bottomYInt, topYInt);//one line 2 events...
                edges.insert( it, E );
                return;
            }
            //was convex, exterior, not a splitter...
            if( insertedY < bottomYInt )//insert totally sorted 0 - high
            {
                std::cout<<"inserting segs "<<E.topLSeg<<" "<<E.bottomLSeg<<" at "<<V.x<<std::endl;
                edges.insert( it, E );
                return;
            }
        }
        std::cout<<"inserting segs "<<E.topLSeg<<" "<<E.bottomLSeg<<" at "<<V.x<<std::endl;
        edges.insert( it, E );
    };

    auto insertV = [&](const Vertice &V)
    {
        switch(V.type)
        {
            case Vertice::Type::SPLIT:
            {//doesn't matter if isconvex or not
                //create new edge and put it in the list...?
                insert(V);
                break;
            }
            case Vertice::Type::MERGE:
            {
                remove(V);
                break;
            }
            case Vertice::Type::CONTINUE:
            {
                //need to find it and replace it
                replace(V);
            }
            default:
                break;
        }
    };

    int counter = 0;
    for(Vertice &V : vertices)
    {
        insertV(V);
        std::cout<<counter;
        switch(V.type)
        {
            case Vertice::Type::MERGE:
                std::cout<<" merge ";
                break;
            case Vertice::Type::SPLIT:
                std::cout<<" split ";
                break;
            case Vertice::Type::CONTINUE:
                std::cout<<" cont ";
                break;
            default:
                break;
        }
        std::cout<<edges.size()<<std::endl;
        counter++;
    }
}

void PolyTriangulator::sweep(std::vector<sf::Vertex> &emitLines)
{
    sf::Vertex V;
    V.color = sf::Color::Green;
    auto addLine = [&](float x, float y1, float y2) {
        V.position.x = x;
        V.position.y = y1;
        emitLines.push_back(V);
        V.position.y = y2;
        emitLines.push_back(V);
    };

    auto insertLSeg = [&](LSeg &lseg)->auto
    {//reason they use a binary tree here...
        float y_int;

        for(auto it = curLSegs.begin(); it != curLSegs.end(); ++it)
        {
            LSeg &curLSeg = (*it);
            y_int = getYIntValAtX(lseg.left.x,curLSeg);

            if(curLSeg.left.y > y_int)//insertafter and done. or if no insert, push back
            {
                return curLSegs.insert(std::next(it),lseg);
                //return;
            }//increasing y order insertion...
        }//not inserted anywhere? then push back
        //curLSegs.push_back(lseg);
        return curLSegs.insert(curLSegs.end(),lseg);
    };

    unsigned int n = vertices.size();
    unsigned int counter = 0;
    std::vector<Vertice> curVertices;
    Merge lastMerge;
    //bool merged = false;
    float x_at;

    while(true)
    {//go through the vertices
        curVertices.clear();
        x_at = vertices[counter].x;
        
        //list of current valid vertices...
        while(true)
        {
            Vertice curV = vertices[counter];
            std::cout<<"Vertice at x "<<x_at;
            if(curV.isConvex) std::cout<<" is convex"<<std::endl;
            else std::cout<<" is concave"<<std::endl;

            if(curV.type == Vertice::Type::SPLIT)
            {
                insertLSeg( lsegs[curV.segA] );
                insertLSeg( lsegs[curV.segB] );
            }
            else if(curV.type == Vertice::Type::CONTINUE)
            {
                insertLSeg( lsegs[curV.segB] );
            }

            curVertices.push_back(vertices[counter]);
            if(counter < n -1)
            {
                if(vertices[counter+1].x == vertices[counter].x)
                    counter++;
                else
                    break;
            }
            else break;
        }

        for (auto &V : curVertices)
        {
            float x_at = curVertices[0].x;
            float y_above = 12.f;
            float y_below = 0.f; //bounds
            float y_int;
            for (auto &lseg : curLSegs) //only need to test for the line above and below!... is sorted... so rewrite it here
            {
                y_int = getYIntValAtX(x_at, lseg);
                if (y_int > P[V.top].y && y_above > y_int)
                    y_above = y_int;
                else if (y_int < P[V.bottom].y && y_below < y_int)
                    y_below = y_int;
            } //want the closest one though... is sorted... hmmmmmmm
            addLine(x_at, P[V.top].y, y_above);
            addLine(x_at, P[V.bottom].y, y_below);
        }

        for(auto it = curLSegs.begin(); it != curLSegs.end(); ++it)
        {
            if((*it).right.x == x_at)
            {
                if(vertices[(*it).rVertice].type == Vertice::Type::MERGE)
                {
                    lastMerge.lineBelow = std::prev(it);
                    it = curLSegs.erase(it);
                    it = curLSegs.erase(it);
                    lastMerge.lineAbove = it;
                    //merged = true;
                }
                else it = curLSegs.erase(it);
                it--;
            }
        }

        counter++;
        if(counter >= n) break;
    }
}
***/
    /***************** working prototype, just /* things
void PolyTriangulator::sweep3(std::vector<Trap> &trapezoids)
{
    auto getTrap = [&](const Edge &edge, float x) {
        Trap T;
        float x_last, y_top, y_bottom;
        //float lasty_bot, lasty_top;
        x_last = vertices[edge.vertice].x;
        y_top = getYIntValAtX(x_last, lsegs[edge.topLSeg]);
        y_bottom = getYIntValAtX(x_last, lsegs[edge.bottomLSeg]);
        T.topLeft.x = T.bottomLeft.x = x_last;
        T.topLeft.y = y_top;
        T.bottomLeft.y = y_bottom;
        //lasty_bot = y_bottom;
        //lasty_top = y_top;
        y_top = getYIntValAtX(x, lsegs[edge.topLSeg]);
        y_bottom = getYIntValAtX(x, lsegs[edge.bottomLSeg]);
        T.topRight.x = T.bottomRight.x = x;
        T.topRight.y = y_top;
        T.bottomRight.y = y_bottom;
        trapezoids.push_back(T);
        //std::cout<<"trap made: x "<<x_last<<" y "<<lasty_bot<<" "<<lasty_top<<" to x "<<x<<" y "<<y_bottom<<" "<<y_top<<std::endl;
    };

    std::list<Edge> curEdges;

    auto getNextEdge = [&](const Edge &edge) -> auto
    {
        float x = vertices[edge.vertice].x;
        float edgeTopY = P[vertices[edge.vertice].top].y;
        std::list<Edge>::iterator it;
        for (it = curEdges.begin(); it != curEdges.end(); ++it)
        {
            float curEdgeBottomY = getYIntValAtX(x, lsegs[(*it).bottomLSeg]);
            curEdgeBottomY += 0.0002;
            edgeTopY += 0.0002;
            if ((int)(edgeTopY * 1000) < (int)(curEdgeBottomY * 1000))
                return it; //now can't be inside at all...
            //float curEdgeTopY = getYIntValAtX(x, lsegs[(*it).topLSeg]);
            //if((int)(edgeTopY*1000) < (int)(curEdgeTopY*1000)) return it;//std::prev(it);//hmm
        }
        return it;
    };

    Edge E;
    Trap T;
    //float y_top, y_bottom;//for outputing our trapezoids....
    //float x_last, x_current;
    float x_current;
    std::list<Edge>::iterator nextEdge;
    for (auto &V : vertices)
    {
        x_current = V.x;
        //fill our edge struct
        E.vertice = V.id;
        switch (V.type)
        {
        case Vertice::Type::SPLIT:
        case Vertice::Type::MERGE:
        {
            E.topLSeg = V.segA;
            E.bottomLSeg = V.segB;
        }
        break;
        case Vertice::Type::CONTINUE:
        {

            /*if (lsegs[V.segA].right.y > lsegs[V.segB].left.y)
            {
                E.topLSeg = V.segA;
                E.bottomLSeg = V.segB;
            }
            else
            {
                E.topLSeg = V.segB;
                E.bottomLSeg = V.segA;
            }*   /
        }
        default:
            break;
        }

        //find it?
        //if(V.type == Vertice::Type::CONTINUE);
        nextEdge = getNextEdge(E); //oh my word... but if is continue, won't work, need the vertice point
        /  *if(curEdges.size() == 0)
        {
            curEdges.insert(nextEdge, E);
            continue;
        }*  /

        switch (V.type)
        {
        //insert the edge before or split the edge in 2
        case Vertice::Type::SPLIT:
        { //rewrite again and again...

            //E.top < foundEdge.bottom
            /  *if(nextEdge == curEdges.end())//could be inside though...
            {
                std::cout<<"Split new insert at top "<<E.bottomLSeg<<" "<<E.topLSeg<<std::endl;
                curEdges.insert(nextEdge, E);
                break;
            }*  /

            if (nextEdge == curEdges.begin()) //that's right as the first edge would be totally over it... so that is the one
            {
                std::cout << "Split new insert at bottom " << E.bottomLSeg << " " << E.topLSeg << std::endl;
                curEdges.insert(nextEdge, E);
                break;
            } //std::prev would crash this one

            nextEdge = std::prev(nextEdge);

            //now the vertice can either be inside this one, or above it.  test if is above?

            //float edgeBottomY = getYIntValAtX(V.x, lsegs[(*nextEdge).bottomLSeg]);
            float edgeTopY = getYIntValAtX(V.x, lsegs[(*nextEdge).topLSeg]);

            if (P[V.bottom].y > edgeTopY)
            {
                std::cout << "Split new insert above " << (*nextEdge).bottomLSeg << " " << (*nextEdge).topLSeg << " of " << E.bottomLSeg << " " << E.topLSeg << std::endl;
                curEdges.insert(std::next(nextEdge), E); //insert above
                break;
            } //dbl check
            //SHOULD BE INSIDE NOW...
            /  *            if(P[V.bottom].y > edgeTopY || P[V.top].y < edgeBottomY)
            {
                std::cout<<"not inside?"<<std::endl;
                return;
            }
*  /
            getTrap((*nextEdge), x_current);

            std::cout << "splitting at x " << x_current << " edge " << (*nextEdge).bottomLSeg << " " << (*nextEdge).topLSeg;

            unsigned int tempLSeg = (*nextEdge).bottomLSeg;
            (*nextEdge).vertice = V.id;
            (*nextEdge).bottomLSeg = E.topLSeg;
            E.topLSeg = E.bottomLSeg;
            E.bottomLSeg = tempLSeg;
            curEdges.insert(nextEdge, E);
            std::cout << " becoming 2 edges e1 " << E.bottomLSeg << " " << E.topLSeg << " e2 " << (*nextEdge).bottomLSeg << " " << (*nextEdge).topLSeg << std::endl;
        }
        break;
        case Vertice::Type::MERGE:
        {
            std::cout << "at MERGE " << curEdges.size() << " edges at x " << x_current << std::endl;
            nextEdge = std::prev(nextEdge); //this one is either it or on the bottom of

            std::cout << "Next edge is " << (*nextEdge).bottomLSeg << " " << (*nextEdge).topLSeg << std::endl;
            std::cout << "with E " << E.bottomLSeg << " " << E.topLSeg << std::endl;

            //if((*nextEdge).bottomLSeg != E.topLSeg)
            if ((*nextEdge).topLSeg == E.topLSeg)
            {
                auto matchedEdge = nextEdge; //std::prev(nextEdge);
                getTrap((*matchedEdge), x_current);
                curEdges.erase(matchedEdge);
                break;
            }

            auto bottomEdge = std::prev(nextEdge);
            getTrap((*nextEdge), x_current);
            getTrap((*bottomEdge), x_current);

            std::cout << "Merging " << (*bottomEdge).bottomLSeg << " " << (*bottomEdge).topLSeg << " " << (*nextEdge).bottomLSeg << " " << (*nextEdge).topLSeg;

            (*bottomEdge).topLSeg = (*nextEdge).topLSeg;
            (*bottomEdge).vertice = V.id;
            std::cout << " to " << (*bottomEdge).bottomLSeg << " " << (*bottomEdge).topLSeg << std::endl;
            curEdges.erase(nextEdge);
        }
        break;
        case Vertice::Type::CONTINUE:
            //SHITTY SHIT SHIT SHIT...
            {
                nextEdge = std::prev(nextEdge);

                Edge &edge = *nextEdge;

                getTrap(edge, x_current);

                if (edge.topLSeg == V.segA)
                {
                    std::cout << "CONTINUING seg " << edge.topLSeg << " to " << V.segB << std::endl;
                    edge.topLSeg = V.segB;
                }
                else
                {
                    std::cout << "CONTINUINGasf seg " << edge.bottomLSeg << " to " << V.segB << " with V " << V.top << std::endl;
                    edge.bottomLSeg = V.segB;
                }
                edge.vertice = V.id;
                break;
            }
        }
    }
}
***/