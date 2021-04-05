#include "Plotter.hpp"

class ValleyPlotter : public Plotter
{
public:
    ValleyPlotter (const Vec2 bounds);
    void plot(std::list<Point> &points, uint numPts, float clearDist) override;
};

ValleyPlotter::ValleyPlotter(const Vec2 bounds) : Plotter(bounds)
{
}

void ValleyPlotter::plot(std::list<Point> &points, uint numPts, float clearDist)
{
    float width = m_bounds.x;
    float height = m_bounds.y/2;

    for (uint i = 0; i < numPts; i++) //128
    {
        float x = rand() % (int)width;                                                              //
        float y = -1.f * (fabsf(x - (width / 2)) / width) * (float)(rand() % (int)height) + height; //f(x)
        Point pos(x, y);                                                                       //rand()%width,rand()%(height/8)+(height/2));
        bool collide = false;
        for (auto &point : points)
        {
            Point delta = pos - point;
            if (delta.x * delta.x + delta.y * delta.y < clearDist * clearDist)
            {
                collide = true;
                break;
            }
        }
        if (collide)
            continue;
        points.push_back(pos);
    }
}

class SemiCirclePlotter : public Plotter
{
public:
    SemiCirclePlotter(const Vec2 bounds);
    void plot(std::list<Point> &points, uint numPts, float clearDist) override;
};

SemiCirclePlotter::SemiCirclePlotter(const Vec2 bounds) : Plotter(bounds)
{
}

void SemiCirclePlotter::plot(std::list<Point> &points, uint numPts, float clearDist)
{
    float width = m_bounds.x;
    float height = m_bounds.y / 2;

    for (uint i = 0; i < numPts; i++) //128
    {
        float x = rand() % (int)width;                                                              //
        float y_max = -sqrtf((width/2.f)*(width/2.f) - (x - (width / 2))* (x - (width / 2)));
        float y = rand () % 1000;
        y /= 1000.f;
        y *= y_max;
        y /= 2.f;

        y = rand () % 50;
//        float y = -1.f * (fabsf(x - (width / 2)) / width) * (float)(rand() % (int)height) + height; //f(x)
        Point pos(x, y);                                                                            //rand()%width,rand()%(height/8)+(height/2));
        bool collide = false;
        for (auto &point : points)
        {
            Point delta = pos - point;
            if (delta.x * delta.x + delta.y * delta.y < clearDist * clearDist)
            {
                collide = true;
                break;
            }
        }
        if (collide)
            continue;
        points.push_back(pos);
    }
}