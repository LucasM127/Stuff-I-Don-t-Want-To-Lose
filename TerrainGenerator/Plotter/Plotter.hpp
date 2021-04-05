#ifndef PLOTTER_HPP
#define PLOTTER_HPP

#include "../../SFML_Functions/SFML_Functions.hpp"
#include <random>
#include <list>

typedef sf::Vector2f Point;
typedef sf::Vector2f Vec2;

class Plotter
{
public:
    Plotter(const Vec2 bounds);
    virtual ~Plotter(){}

    enum Distribution
    {
        UNIFORM,
        NORMAL
    };

    virtual void plot(std::list<Point> &points, uint numPts, float clearDist) = 0;
    void setDistribution(Distribution distribution){m_distribution = distribution;}
    
    const std::vector<Point> &getBaseLine() { return m_baseLine; }
    const std::vector<Point> &getVarianceLine() { return m_varianceLine; }

protected:
    const Vec2 m_bounds;
    Distribution m_distribution;
    std::mt19937 m_random_engine;
// for debugging purposes ???
    std::vector<Point> m_baseLine;
    std::vector<Point> m_varianceLine;
};

Plotter::Plotter(const Vec2 bounds) : m_bounds(bounds), m_distribution(UNIFORM)
{
    std::random_device random_device;
    m_random_engine.seed(random_device());
}

#endif //PLOTTER_HPP