#ifndef OBJECT_HPP
#define OBJECT_HPP

//Perhaps a bit premature though...

//collidable object physics...
#include "../SFML_Functions/SFML_Functions.hpp"

enum class ObjectType
{
    Circle,
    Line,
    LineSeg,
    Polygon
};

class Object
{
public:
    Object(ObjectType type);
    Object* const collides(const Object &other);
    const ObjectType objectType;
    const std::vector<Point> &getPoints() const;
    void update(sf::Transform &transform);
protected:
    SFUTIL::AABBf m_aabb;
    virtual Object* const collidesSpecific(const Object &other) { return nullptr; }
    std::vector<Point> m_points;
    std::vector<Point> m_transformedPoints;
};

class LineObject : public Object
{
public:
    LineObject(Point &a, Point &b);
private:
    Object* const collidesSpecific(const Object &other) override;
};

#endif