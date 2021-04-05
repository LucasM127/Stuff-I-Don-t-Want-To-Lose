#ifndef POLYTRIANGULATOR_HPP
#define POLYTRIANGULATOR_HPP

#include <SFML/Graphics/Vertex.hpp>

#include "Polygon.hpp"

#include <list>

struct Merge
{
    unsigned int verticeID;
    std::list<LSeg>::iterator lineAbove;
    std::list<LSeg>::iterator lineBelow;
};

class PolyTriangulator
{
public:
    PolyTriangulator(Polygon &_P);
    //void sweep(std::vector<sf::Vertex>& lines);
    //void sweep2(std::vector<sf::Vertex>& lines);
    //void sweep3(std::vector<Trap>& trapezoids);
    void sweep4(std::vector<int> &numTriPPoly);
    void init();
    void triangulateMonotonePolygon(const std::vector<unsigned int> &monotonePolygon, const std::vector<bool> &isTopChain);

    const std::vector<Trap>& getTraps(){return m_trapezoids;}
    const std::vector<sf::Vertex>& getLines(){return m_lines;}
    const std::vector<std::vector<unsigned int>> &getMonotonePolygons(){return m_monotonePolygons;}
    const std::vector<Vertice> &getVertices() {return vertices;}
    const std::vector<Triangle> &getTriangles() {return m_triangles;}

  private:
    void handleSplit(const Vertice &V, std::list<Edge>::iterator &nextEdge);
    void handleMerge(const Vertice &V, std::list<Edge>::iterator &nextEdge);
    void handleContinue(const Vertice &V, std::list<Edge>::iterator &nextEdge);

    void getTrap(const Edge &edge, float x);
    //void addDiagonal(const Vertice &leftV, const Vertice &rightV);
    void addDiagonal(Edge &edge, const Vertice &nextV);
    std::list<Edge>::iterator merge(const std::list<Edge>::iterator& topEdge, const std::list<Edge>::iterator& bottomEdge, const Vertice &V);
    //void monotonate();
    const Polygon &P;

    std::vector<unsigned int> pIDs;
    std::vector<Vertice> vertices;
    std::vector<LSeg> lsegs;
    std::vector<bool> isReflex;

    std::vector<Trap> m_trapezoids;
    std::vector<sf::Vertex> m_lines;
    std::vector<std::vector< unsigned int > > m_monotonePolygons;//sorted left to right...
    std::vector<std::vector<bool> > amTopChain;
    std::vector<Triangle> m_triangles;

    std::list<LSeg> curLSegs;
    std::list<Edge> curEdges;
};

#endif