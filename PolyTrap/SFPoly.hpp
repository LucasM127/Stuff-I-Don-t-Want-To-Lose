#ifndef SFPOLY_HPP
#define SFPOLY_HPP

#include <SFML/Graphics.hpp>
#include "Polygon.hpp"

class sfPoly : public sf::Drawable
{
  public:
    sfPoly();
    void set(Polygon &P);
  private:
    std::vector<sf::Vertex> m_vertices;
    void draw(sf::RenderTarget &target, sf::RenderStates states) const override;
};

void convertTraps(const std::vector<Trap> &traps, std::vector<sf::Vertex> &trapShapes);

void getTriangles(std::vector<sf::Vertex> &drawnTriangles, const std::vector<Triangle> &triangles, const Polygon &P, std::vector<int> &numTriPPoly);
void getMonotonePolygons(std::vector<Polygon> &polys, const std::vector<Vertice> &vertices, const std::vector<std::vector<unsigned int>> &monotoneIDs, const Polygon &P);

#endif