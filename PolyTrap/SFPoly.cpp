#include "SFPoly.hpp"

sfPoly::sfPoly()
{
}

void sfPoly::set(Polygon &P)
{
    m_vertices.clear();
    sf::Vertex V;

    V.color = sf::Color::Black;
    for (auto p : P)
    {
        V.position = p;
        m_vertices.push_back(V);
    }
    V.position = P[0];
    m_vertices.push_back(V);
}

void sfPoly::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
    target.draw(&m_vertices[0], m_vertices.size(), sf::LineStrip);
}

void convertTraps(const std::vector<Trap> &traps, std::vector<sf::Vertex> &trapShapes)
{
    sf::Vertex V;
    
    for(auto &T : traps)
    {
        int R = rand() % 256;//128 + rand() % 128;
        int G = rand() % 256; //128 + rand() % 128;
        int B = rand() % 256; //128 + rand() % 128;
        V.color = sf::Color(R,G,B,128);

        V.position = T.bottomLeft;
        trapShapes.push_back(V);
        V.position = T.topLeft;
        trapShapes.push_back(V);
        V.position = T.topRight;
        trapShapes.push_back(V);
        //triangle 2
        V.position = T.bottomLeft;
        trapShapes.push_back(V);
        V.position = T.topRight;
        trapShapes.push_back(V);
        V.position = T.bottomRight;
        trapShapes.push_back(V);
    }
}

void getMonotonePolygons(std::vector<Polygon> &polys, const std::vector<Vertice> &vertices, const std::vector<std::vector<unsigned int>> &monotoneIDs, const Polygon &P)
{
    for(const auto &polyIDs : monotoneIDs)
    {
        Polygon monoP;

        for(const auto &id : polyIDs)
        {
            monoP.push_back( P[ vertices[id].top ] );
        }

        polys.push_back(monoP);
    }
}

void setColor(int &color, int &R, int &G, int &B)
{
    if(color == 0){R = 0; G = 0; B = 160;}
    else if(color == 1){R = 160; G = 0; B = 0;}
    else if(color == 2){R = 0; G = 160; B = 0;}
    //else if(color == 3){R = 160; G = 0; B = 160;}
    //else if(color == 4){R = 0; G = 160; B = 160;}
    else if(color == 3){R = 160; G = 160; B = 0;}
}//purple remove and yellow rgb 

void getTriangles(std::vector<sf::Vertex> &drawnTriangles, const std::vector<Triangle> &triangles, const Polygon &P, std::vector<int> &numTriPPoly)
{
    int color = 0;
    int R, G, B;
    sf::Vertex V;

    setColor(color,R,G,B);

    //int R = rand() % 256; //128 + rand() % 128;
    //int G = rand() % 256; //128 + rand() % 128;
    //int B = rand() % 256; //128 + rand() % 128;
    V.color = sf::Color(R, G, B, 128);
    unsigned int ctr = 0;
    unsigned int i = 0;
    for(const auto &T : triangles)
    {
        int rVar = rand() % 96;
        int gVar = rand() % 96;
        int bVar = rand() % 96;
        V.color = sf::Color(R+rVar, G+gVar, B+bVar, 128);
        V.position = P[T.a];
        drawnTriangles.push_back(V);
        V.position = P[T.b];
        drawnTriangles.push_back(V);
        V.position = P[T.c];
        drawnTriangles.push_back(V);
        ctr++;
        if(ctr == numTriPPoly[i])
        {
            i++;
            ctr = 0;
            color = (color+1 > 3) ? 0 : color + 1;
            setColor(color,R,G,B);
            //V.color = sf::Color(R, G, B, 128);
        }
    }
}