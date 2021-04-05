#ifndef POINT_PLOTTER_HPP
#define POINT_PLOTTER_HPP

#include "../PerlinNoise.h"
#include <vector>
#include "../../SFML_Functions/SFML_Functions.hpp"//for Point typedef :/
#include <chrono>
#include <random>
#include <SFML/System/Vector2.hpp>
#include <cmath>
#include <iostream>
#include <list>
#include <functional>

typedef sf::Vector2f Point;
typedef sf::Vector2f Vec2;

void minMax(float &min, float &max)
{
    if(min < max) return;
    float temp = min;
    min = max;
    max = temp;
    return;
}

inline float lerp(float a, float b, float t)
{
    return(a + (b - a) * t);
}

float baseGroundFunc(float x, float y = 0.f)
{
    return y;
}

enum ParamType
{
    PERIOD = 0,
    VARIANCE,
    OFFSET
};

float m_varianceLineParameters[3];
float m_baseLineParameters[3];
float m_heightLineParameters[3];

struct perlinNoiseFunctor
{

};
float pnf(float x)
{
    ;
}

float gx(float x, float fx = 0);

//inherit from it???
class Plotter
{
public:
    Plotter(Vec2 bounds, std::function<float(float)> fx_, std::function<float(float,float)> gx_ = baseGroundFunc);

    void randomize();
    void plot();
    void plot(std::list<Point> &points, uint numPts, float clearDist);
    void set(float basePeriod, float baseVariance, float baseOffset, float variancePeriod, float varianceVariance, float varianceOffset);
    const float getVarianceLinePeriod() { return m_varianceLineParameters[PERIOD]; }
    const float getVarianceLineVariance() { return m_varianceLineParameters[VARIANCE]; }
    const float getVarianceLineOffset() { return m_varianceLineParameters[OFFSET]; }
    const float getBaseLinePeriod() { return m_baseLineParameters[PERIOD]; }
    const float getBaseLineVariance() { return m_baseLineParameters[VARIANCE]; }
    const float getBaseLineOffset() { return m_baseLineParameters[OFFSET]; }
    const std::vector<Point> &getBaseLine() { return m_baseLine; }
    const std::vector<Point> &getVarianceLine() { return m_varianceLine; }
private:
    std::function<float(float)> fx;
    std::function<float(float)> gx;

    const Vec2 m_bounds;
    

    std::vector<Point> m_baseLine;
    std::vector<Point> m_varianceLine;
};

Plotter::Plotter(Vec2 bounds, std::function<float(float)> fx_, std::function<float(float)> gx_) : m_bounds(bounds), fx(fx_), gx(gx_)
{
    m_varianceLineParameters[PERIOD] = 1.f;
    m_varianceLineParameters[VARIANCE] = 2.f;
    m_varianceLineParameters[OFFSET] = 0.f;
    m_baseLineParameters[PERIOD] = 1.f;
    m_baseLineParameters[VARIANCE] = 1.f;
    m_baseLineParameters[OFFSET] = 0.f;
    m_heightLineParameters[PERIOD] = 1.f;
    m_heightLineParameters[VARIANCE] = 1.f;
    m_heightLineParameters[OFFSET] = 0.f;

    //plot();
}

void Plotter::randomize()
{
    std::random_device random_device;
    std::mt19937 random_engine(random_device());
    std::uniform_real_distribution<float> variance_dist(0.2f, 0.7f);//normal dist...
    std::uniform_real_distribution<float> offset_dist(0.f, 255.f);
    std::uniform_real_distribution<float> uniform_dist;//0-1
//    std::normal_distribution<REAL> norm_dist(0.5, .5 / 3.); //99.7% within 3 std deviations 0.5/4

    m_baseLineParameters[OFFSET] = offset_dist(random_engine);
    m_varianceLineParameters[OFFSET] = offset_dist(random_engine);
    m_heightLineParameters[OFFSET] = offset_dist(random_engine);

    m_baseLineParameters[VARIANCE] = variance_dist(random_engine);
    m_varianceLineParameters[VARIANCE] = variance_dist(random_engine);
    m_heightLineParameters[VARIANCE] = variance_dist(random_engine);

    m_baseLineParameters[PERIOD] = uniform_dist(random_engine) * 10 + 5.f;
    m_varianceLineParameters[PERIOD] = uniform_dist(random_engine) * 10 + 4.f;
    m_heightLineParameters[PERIOD] = uniform_dist(random_engine) * 4.f;
}

//just adding variance in y coordinates. ???
//we need it to fit within our x y coordinates
//plots at zero zero ... as y goes up...
void Plotter::plot()
{
    m_baseLine.clear();
    m_varianceLine.clear();

    PerlinNoise PN;

    float width = m_bounds.x;
    float height = m_bounds.y;

    for (uint i = 0; i < 1000; ++i)
    {                                            //x is a 'random number'
        float x = (float)i / 1000.f;             // x *= 2.f;
        float baseX = x * m_baseLineParameters[PERIOD];
        float varianceX = x * m_varianceLineParameters[PERIOD];
        float heightX = x * m_heightLineParameters[PERIOD];

        float baseY = PN.noise(baseX + m_baseLineParameters[OFFSET], 0, 0) * m_baseLineParameters[VARIANCE];
        float varianceY = PN.noise(varianceX + m_varianceLineParameters[OFFSET], 0, 0) * m_varianceLineParameters[VARIANCE];
        float heightVariance = PN.noise(heightX + m_heightLineParameters[OFFSET], 0, 0) * m_heightLineParameters[VARIANCE];
        heightVariance *= height;

        baseX *= width / m_baseLineParameters[PERIOD];
        baseY -= 0.5f * m_baseLineParameters[VARIANCE];
        baseY *= height; // * heightVariance;
        baseY += heightVariance;
        //baseY -= m_baseLineParameters[VARIANCE] * height;
        m_baseLine.push_back({baseX, baseY});

        //varianceX *= width / m_varianceLineParameters[PERIOD];
        varianceX = baseX;
        varianceY -= 0.5f * m_varianceLineParameters[VARIANCE];
        varianceY *= -height;
        varianceY += baseY;
        //std::cout<<"VarianceY "<<varianceY<<std::endl;
        m_varianceLine.push_back({varianceX, varianceY});
    }
}

//same code as plot above just some different things at the end
void Plotter::plot(std::list<Point> &points, uint numPts, float clearDist)
{
    m_baseLine.clear();
    m_varianceLine.clear();
    points.clear();

    //might as well put it in a list so not have to worry about it at all
    PerlinNoise PN;

    float width = m_bounds.x;
    float height = m_bounds.y;

    std::random_device random_device;
    std::mt19937 random_engine(random_device());
    std::uniform_real_distribution<float> x_dist(0.f, width); //normal dist...
    std::uniform_real_distribution<float> uniform_dist;

    for (uint i = 0; i < numPts; ++i)
    {                                //x is a 'random number'
        float x = uniform_dist(random_engine);

        float baseX = x * m_baseLineParameters[PERIOD];
        float varianceX = x * m_varianceLineParameters[PERIOD];
        float heightX = x * m_heightLineParameters[PERIOD];

        float baseY = PN.noise(baseX + m_baseLineParameters[OFFSET], 0, 0) * m_baseLineParameters[VARIANCE];
        float varianceY = PN.noise(varianceX + m_varianceLineParameters[OFFSET], 0, 0) * m_varianceLineParameters[VARIANCE];
        float heightVariance = PN.noise(heightX + m_heightLineParameters[OFFSET], 0, 0) * m_heightLineParameters[VARIANCE];
        heightVariance *= height;

        baseX *= width / m_baseLineParameters[PERIOD];
        baseY -= 0.5f * m_baseLineParameters[VARIANCE];
        baseY *= height; // * heightVariance;
        baseY += heightVariance;
        m_baseLine.push_back({baseX, baseY});

        varianceY -= 0.5f * m_varianceLineParameters[VARIANCE];
        varianceY *= -height;
        varianceY += baseY;
        m_varianceLine.push_back({baseX, varianceY});

        /*
        float minY = baseY;
        float maxY = varianceY;
        minMax(minY, maxY);*/

        //generate random point
        float y = uniform_dist(random_engine);
        y = lerp(baseY, varianceY, y);
        Point p(baseX,y);

        bool collide = false;
        for(auto point : points)
        {
            if(SFUTIL::lengthSquared(p-point) < clearDist * clearDist)
            {
                collide = true;
                break;
            }
        }
        if(collide)
        {
            //--i;
            continue;
        }
        points.push_back(p);
    }
}

void Plotter::set(float basePeriod, float baseVariance, float baseOffset, float variancePeriod, float varianceVariance, float varianceOffset)
{
    m_baseLineParameters[PERIOD] = basePeriod;
    m_baseLineParameters[VARIANCE] = baseVariance;
    m_baseLineParameters[OFFSET] = baseOffset;

    m_varianceLineParameters[PERIOD] = variancePeriod;
    m_varianceLineParameters[VARIANCE] = varianceVariance;
    m_varianceLineParameters[OFFSET] = varianceOffset;

    //plot();
}

//for instance
/*
class ValleyPlotter : public Plotter
{
    for (uint i = 0; i < 128; i++) //128
    {
        float x = rand() % width;                                                              //
        float y = -1.f * (fabsf(x - (width / 2)) / width) * (float)(rand() % height) + height; //f(x)
        Point pos(x, y);                                                                       //rand()%width,rand()%(height/8)+(height/2));
        bool collide = false;
        for (auto &point : points)
        {
            Point delta = pos - point;
            if (delta.x * delta.x + delta.y * delta.y < 100.f)
            {
                collide = true;
                break;
            }
        }
        if (collide)
            continue;
        points.push_back(pos);
    }
}*/

/******
//class it?  sure then can store our 'variables'
class Plotter
{
public:
    Plotter() : m_numPoints(512), m_variance(300.f) {}
    void plot(unsigned int w, unsigned int h, std::vector<Point> &points);
    void plot();
    void variance(std::vector<Point> &points);
    float offset[3];//0-1 ranges... for the function
    unsigned int width, height;
private:
    unsigned int m_numPoints;
    float m_variance;
    std::vector<Point> *p_points;
    //float m_offset;
};

// i need to understand this noise function better. give it x,y,z coordinates and see how the line changes.
// view it. then do it 2x
void Plotter::plot(unsigned int w, unsigned int h, std::vector<Point> &points)
{
    width = w;
    height = h;
    p_points = &points;
    plot();
}

//variance or delta height a function of perlin noise ???
void Plotter::plot()
{
    p_points->clear();

    PerlinNoise PN;

    //std::random_device random_device;
    //std::mt19937 random_engine(random_device());
    //std::uniform_real_distribution<float> uniform_dist(0.0, 1.0);
    //std::normal_distribution<REAL> norm_dist(0.5, .5/3.); //99.7% within 3 std deviations 0.5/4
    //try the different distributions see what looks better...

    for (uint i = 0; i < m_numPoints; ++i)
    { //x is a 'random number'
        float x = (float)i / (float)m_numPoints;// x *= 2.f;
        float y = PN.noise(x+offset[0], x+offset[1], x+offset[2]);

        //y *= 5;
        //y -= int(y);

        x *= width; //not liking,, lose the periodicity more want to scale our seed
        y *= height;

        p_points->push_back({x, y});
    }
}

void Plotter::variance(std::vector<Point> &points)
{
    PerlinNoise PN;
    for (auto &p : *p_points)
    {
        float x = p.x / (float)width;
        //faster period?
        x *= 2.5f;
        float y = PN.noise(0, x + offset[0], x);
        y *= m_variance;
        y -= m_variance/3.f;
        y += p.y;
        points.push_back({p.x,y});
    }
}****/

#endif //POINT_PLOTTER_HPP