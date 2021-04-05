#ifndef TRIANGLESHAPE_HPP
#define TRIANGLESHAPE_HPP

#include <SFML/Graphics.hpp>
#include "../SFML_Functions/SFML_Functions.hpp"

struct TriangleShape : public sf::Transformable, public sf::Drawable
{
public:
    TriangleShape();
    bool collides(const SFUTIL::AABBf &aabb) { return aabb.intersects(m_aabb); }
    bool collidesLine(const Point &a, const Point &b);
    void update();
    SFUTIL::AABBf &getAABB() { return m_aabb; }

private:
    std::vector<sf::Vertex> m_vertices;
    std::vector<sf::Vector2f> m_transformedPoints;
    void draw(sf::RenderTarget &target, sf::RenderStates states) const override;
    SFUTIL::AABBf m_aabb;
};

#endif