#ifndef PLOTTERMANIPULATOR_HPP
#define PLOTTERMANIPULATOR_HPP

//#include <SFML/Graphics.hpp>
#include "../../sfml/GUI/Observer.hpp"
#include "../../sfml/GUI/QuickSFGui.hpp"
#include "PerlinPlotter.hpp"

//manipulates the plotter
class PlotterManipulator : public Observer
{
public:
    PlotterManipulator(PerlinPlotter &plotter, sf::Font &font, std::list<Point> &points);
    void render(sf::RenderTarget &target);
    bool handleEvent(sf::Event &event);
    std::list<Point> &m_points;
private:
    PerlinPlotter &r_plotter;
    Slider baseLinePeriodSlider;
    Slider baseLineVarianceSlider;
    Slider baseLineOffsetSlider;
    Slider varianceLinePeriodSlider;
    Slider varianceLineVarianceSlider;
    Slider varianceLineOffsetSlider;

    void onUpdate() override;
};

PlotterManipulator::PlotterManipulator(PerlinPlotter &plotter, sf::Font &font, std::list<Point> &points)
    : m_points(points),
    r_plotter(plotter), baseLinePeriodSlider(font), baseLineVarianceSlider(font), baseLineOffsetSlider(font),
        varianceLinePeriodSlider(font), varianceLineVarianceSlider(font), varianceLineOffsetSlider(font)
{
    baseLinePeriodSlider.setPosition(0.f, 10.f);
    baseLinePeriodSlider.setScale(200.f, 30.f);
    baseLinePeriodSlider.setRange(0.f, 50.f);
    baseLinePeriodSlider.setStep(0.1f);//1.f);
    baseLinePeriodSlider.setDefaultValue(plotter.getBaseLinePeriod());
    baseLinePeriodSlider.setLabel("Base Period");

    baseLineVarianceSlider.setPosition(210.f, 10.f);
    baseLineVarianceSlider.setScale(200.f, 30.f);
    baseLineVarianceSlider.setRange(0.f, 5.f);
    baseLineVarianceSlider.setDefaultValue(plotter.getBaseLineVariance());
    baseLineVarianceSlider.setLabel("Base Variance");

    baseLineOffsetSlider.setPosition(420.f, 10.f);
    baseLineOffsetSlider.setScale(200.f, 30.f);
    baseLineOffsetSlider.setRange(0.f, 255.f);
    baseLineOffsetSlider.setStep(5.f);
    baseLineOffsetSlider.setDefaultValue(plotter.getBaseLineOffset());
    baseLineOffsetSlider.setLabel("Base Offset");

    varianceLinePeriodSlider.setPosition(0.f, 50.f);
    varianceLinePeriodSlider.setScale(200.f, 30.f);
    varianceLinePeriodSlider.setRange(0.f, 50.f);
    varianceLinePeriodSlider.setStep(0.1f);//1.f);
    varianceLinePeriodSlider.setDefaultValue(plotter.getVarianceLinePeriod());
    varianceLinePeriodSlider.setLabel("Variance Period");

    varianceLineVarianceSlider.setPosition(210.f, 50.f);
    varianceLineVarianceSlider.setScale(200.f, 30.f);
    varianceLineVarianceSlider.setRange(0.f, 2.f);
    varianceLineVarianceSlider.setDefaultValue(plotter.getVarianceLineVariance());
    varianceLineVarianceSlider.setLabel("Variance Variance");

    varianceLineOffsetSlider.setPosition(420.f, 50.f);
    varianceLineOffsetSlider.setScale(200.f, 30.f);
    varianceLineOffsetSlider.setRange(0.f, 20.f);
    varianceLineOffsetSlider.setStep(0.1f);
    varianceLineOffsetSlider.setDefaultValue(plotter.getVarianceLineOffset());
    varianceLineOffsetSlider.setLabel("Variance Offset");

    baseLinePeriodSlider.addObserver(this);
    baseLineVarianceSlider.addObserver(this);
    baseLineOffsetSlider.addObserver(this);
    varianceLinePeriodSlider.addObserver(this);
    varianceLineVarianceSlider.addObserver(this);
    varianceLineOffsetSlider.addObserver(this);
}

void PlotterManipulator::onUpdate()
{
    r_plotter.set( baseLinePeriodSlider.getVal(), baseLineVarianceSlider.getVal(), baseLineOffsetSlider.getVal(), 
                    varianceLinePeriodSlider.getVal(), varianceLineVarianceSlider.getVal(), varianceLineOffsetSlider.getVal());
    r_plotter.plot(m_points, 1500, 10.f);
}//if I could just hand it the 'function' it would be easier...

void PlotterManipulator::render(sf::RenderTarget &target)
{
    target.draw(baseLinePeriodSlider);
    target.draw(baseLineVarianceSlider);
    target.draw(baseLineOffsetSlider);
    target.draw(varianceLinePeriodSlider);
    target.draw(varianceLineVarianceSlider);
    target.draw(varianceLineOffsetSlider);
}

bool PlotterManipulator::handleEvent(sf::Event &event)
{
    baseLinePeriodSlider.handleEvent(event);
    baseLineVarianceSlider.handleEvent(event);
    baseLineOffsetSlider.handleEvent(event);
    varianceLinePeriodSlider.handleEvent(event);
    varianceLineVarianceSlider.handleEvent(event);
    varianceLineOffsetSlider.handleEvent(event);

    if (event.type == sf::Event::MouseWheelMoved)
    {
        if (baseLinePeriodSlider.isActive() || baseLineVarianceSlider.isActive() || baseLineOffsetSlider.isActive() ||
            varianceLinePeriodSlider.isActive() || varianceLineVarianceSlider.isActive() || varianceLineOffsetSlider.isActive())
            return true;
    }
    return false;
}

#endif //PLOTTERMANIPULATOR_HPP