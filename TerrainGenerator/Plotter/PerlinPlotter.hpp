#include "Plotter.hpp"
#include "../PerlinNoise.h"

void minMax(float &min, float &max)
{
    if (min < max)
        return;
    float temp = min;
    min = max;
    max = temp;
    return;
}

inline float lerp(float a, float b, float t)
{
    return (a + (b - a) * t);
}

class PerlinPlotter : public Plotter
{
public:
    enum ParamType
    {
        PERIOD = 0,
        VARIANCE,
        OFFSET
    };

    PerlinPlotter(const Vec2 bounds);

    void randomize();
    void plot(std::list<Point> &points, uint numPts, float clearDist) override;
    void set(float basePeriod, float baseVariance, float baseOffset, float variancePeriod, float varianceVariance, float varianceOffset);
    const float getVarianceLinePeriod() { return m_varianceLineParameters[PERIOD]; }
    const float getVarianceLineVariance() { return m_varianceLineParameters[VARIANCE]; }
    const float getVarianceLineOffset() { return m_varianceLineParameters[OFFSET]; }
    const float getBaseLinePeriod() { return m_baseLineParameters[PERIOD]; }
    const float getBaseLineVariance() { return m_baseLineParameters[VARIANCE]; }
    const float getBaseLineOffset() { return m_baseLineParameters[OFFSET]; }
private:
    float m_varianceLineParameters[3];
    float m_baseLineParameters[3];
    float m_heightLineParameters[3];
};

PerlinPlotter::PerlinPlotter(const Vec2 bounds) : Plotter(bounds)
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
}

void PerlinPlotter::randomize()
{
    std::random_device random_device;
    std::mt19937 random_engine(random_device());
    std::uniform_real_distribution<float> variance_dist(0.2f, 0.7f); //normal dist...
    std::uniform_real_distribution<float> offset_dist(0.f, 255.f);
    std::uniform_real_distribution<float> uniform_dist; //0-1
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

void PerlinPlotter::plot(std::list<Point> &points, uint numPts, float clearDist)
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
    std::normal_distribution<float> normal_dist(0.5, .5 / 6.);//3.

    for (uint i = 0; i < numPts; ++i)
    { //x is a 'random number'
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
        float y;
        if(m_distribution == Distribution::UNIFORM) y = uniform_dist(random_engine);
        else y = normal_dist(random_engine);
        y = lerp(baseY, varianceY, y);
        Point p(baseX, y);

        bool collide = false;
        for (auto point : points)
        {
            if (SFUTIL::lengthSquared(p - point) < clearDist * clearDist)
            {
                collide = true;
                break;
            }
        }
        if (collide)
        {
            //--i;
            continue;
        }
        points.push_back(p);
    }
}

void PerlinPlotter::set(float basePeriod, float baseVariance, float baseOffset, float variancePeriod, float varianceVariance, float varianceOffset)
{
    m_baseLineParameters[PERIOD] = basePeriod;
    m_baseLineParameters[VARIANCE] = baseVariance;
    m_baseLineParameters[OFFSET] = baseOffset;

    m_varianceLineParameters[PERIOD] = variancePeriod;
    m_varianceLineParameters[VARIANCE] = varianceVariance;
    m_varianceLineParameters[OFFSET] = varianceOffset;

    //plot();
}