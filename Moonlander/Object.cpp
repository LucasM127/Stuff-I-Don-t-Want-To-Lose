#include "Object.hpp"

Object::Object(ObjectType type) : objectType(type)
{}

const std::vector<Point> &Object::getPoints() const
{
    return m_points;
}

void Object::update(sf::Transform &transform)
{
    for(uint i = 0; i < m_points.size(); ++i)
        m_points[i] = transform.transformPoint(m_transformedPoints[i]);
    m_aabb.resize(&m_transformedPoints[0],m_transformedPoints.size());
}

Object* const Object::collides(const Object &other)
{
    if(m_aabb.intersects(other.m_aabb))
        return collidesSpecific(other);
    return nullptr;
}

LineObject::LineObject(Point &a, Point &b) : Object(ObjectType::Line)
{
    m_points.resize(2);
    m_points[0] = a;
    m_points[1] = b;
}

Object* const LineObject::collidesSpecific(const Object &other)
{
    if(other.objectType == ObjectType::Line)
    {
        if( SFUTIL::doLineSegsIntersect(
            m_points[0],
            m_points[1],
            other.getPoints()[0],
            other.getPoints()[1] ) )
            return &other;
    }

    return nullptr;
}