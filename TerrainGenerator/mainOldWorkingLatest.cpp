#include "LineGenerator.hpp"
#include "PerlinNoise.h"
#include "fstream"
#include "../sfml/GUI/Observer.hpp"
#include "../sfml/GUI/QuickSFGui.hpp"

unsigned int width = 800;
unsigned int height = 600;
std::vector<Point> points;

void minMax(double &min, double &max)
{
    if (min < max)
        return;
    double temp = min;
    min = max;
    max = temp;
    return;
}

//made it 2x as long????s
//plots a bunch of random points onto a vector of Point Points
double variance = 0.5;
void plot(unsigned int width, unsigned int height, std::vector<Point> &points)
{
    points.clear();
/***********************************
    for(uint i = 0; i<64; ++i)
    {
        float x = rand()%1000;
        float y = rand() % 1000;
        x /= 1000.f;
        y /= 1000.f;
        x*=width;
        y*=height;
        Point pos(x,y);

        bool collide = false;
        for (auto &point : points)
        {
            Point delta = pos - point;
            if (delta.x * delta.x + delta.y * delta.y < 2500.f) //25.f)//100.f)
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

        points.push_back({x,y});
    }
***************************/
    
    PerlinNoise PN;

    double rand1 = rand() % 100;
    rand1 /= 100;
    double rand2 = rand() % 100;
    rand2 /= 100;

    float craterRad = rand() % 100; //200;
    craterRad /= 100.f;
    craterRad = 100.f + craterRad * 400.f;
    float craterXStart = rand() % 100;
    craterXStart /= 100.f;
    //0-2xwidth
    craterXStart *= 4*width;// - 2*craterRad);
    float craterDepth = rand()%400 + 200;

    //upper Line f(x)
    //lower Line f(x = y)
//smoothish shapes ok, too smooth???
    //gives a number in range 0 - 1 at the 'x'
    //seed top rowline
    //scale, offset, amplitude, etc...
    for (uint i = 0; i < 10000/*256*/; ++i)
    {
        /*double variance = rand() % 100;
        variance /= 200;// 0 - 0.2
        variance -= 0.25;//give it a range from -0.1 to 0.1*/
/*
        double v2 = rand() % 100;
        v2 /= 200;  // 0 - 0.2
        v2 -= 0.25; //give it a range from -0.1 to 0.1
*/
//make it more points in crater area by a fudge factor 
//int fudge = rand() % 10;

        double x = rand() % 200;//200;
        //if(fudge > 7) x = 35 + (rand()%30);
        x /= 50;//100;

        double ymin = PN.noise(x , x + rand1, x + rand2);
        double ymax = PN.noise(x + variance, x + rand1 + variance, x + rand2 + variance);
ymin = 0;
ymax = 100;//height;
        //double ymin = PN.noise(x, rand1, 0.1);// + variance, rand2 - variance);
        //double ymax = PN.noise(x , rand1 + variance, variance);//2 + variance, rand1 - variance);

        minMax(ymin, ymax);

        double h = rand() % 100;
        h /= 100; //0-1 range
        double y = ymin + h * ((ymax - ymin)*2.f);///2.f);//halfing the variance here????
        y *= 1.4f;

        x *= width;//not liking,, lose the periodicity more want to scale our seed
        y *= height;

        //std::swap(x,y);

        Point pos(x,y);
        bool collide = false;
        for (auto &point : points)
        {
            Point delta = pos - point;
            if (delta.x * delta.x + delta.y * delta.y < 800)//2500)//50.f)//25.f)//100.f)
            {
                collide = true;
                break;
            }
        }
        if (collide)
            continue;

        //add a crater... :( broke the code...)
        //x 600 - 900 drop it 200 let's see how looks.


        //if (pos.x > 700 && pos.x < 1300)
        if(pos.x > craterXStart && pos.x < craterXStart + 2*craterRad)
        {
            pos.y += craterDepth * ((pos.x - (craterXStart + craterRad)) * (pos.x - (craterXStart + craterRad)) / (craterRad * craterRad)) - craterDepth;
            //pos.y += 500.f * ((pos.x - 1000)*(pos.x - 1000) / 90000) - 500.f;//SO MUCH ERROR PROPAGATION HERE F*
            //pos.y -= sqrtf(90000 - (pos.x - 1000) * (pos.x - 1000));

            for (auto &point : points)
            {
                Point delta = pos - point;
                //if(delta.x == 0.f && delta.y == 0.f) throw;
                if (delta.x * delta.x + delta.y * delta.y < 50.f) //25.f)//100.f)
                {
                    collide = true;
                    break;
                }
            }
            if (collide)
                continue;
        }

        //add a mountain? // fabsf function for height
        /*if (pos.x > 700 && pos.x < 1300)
        {
            pos.y -= 150.f - 0.5f*fabsf(pos.x - 1000.f);
            //pos.y -= sqrtf(90000 - (pos.x - 1000) * (pos.x - 1000));
        }*/
        //adding a channel doesn't work too welll, better to add it afterwards artificially
        //if(pos.x > 400.f && pos.x < 480.f) pos.y -= 800.f;

        points.push_back(pos);//{x, y});
    }

} //256 points given...

void seed()
{
    /*
    points.push_back({0.f, 0.f});
    points.push_back({0.f, 100.f});
    points.push_back({100.f, 150.f});
    points.push_back({200.f, 300.f});
    points.push_back({250.f, 200.f});
    points.push_back({250.f, 140.f});
    points.push_back({400.f, 300.f});
    points.push_back({450.f, 400.f});
    points.push_back({500.f, 600.f});
    points.push_back({550.f, 560.f});
    points.push_back({0.f, 600.f});*/
/*
    for (uint i = 0; i < 128; i++)
    {
        float x = (rand() % (width));
        float y = (rand() % (height));
        Point pos(x, y); //rand()%width,rand()%(height/8)+(height/2));
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
    }*/

    /* 15
   for(uint i = 0; i < 32; i++)
   {
       float x = 100 * (rand() % (width/100));
       float y = 100 * (rand() % (height / 100));
       Point pos(x, y); //rand()%width,rand()%(height/8)+(height/2));
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
   }*/
    // rand 9 seed fail here
    
    for (uint i = 0; i < 512; i++) //128
    {
        /*
        float r = (rand() % 100);
        r /= 2;//0-10
        r += 150;
        
        float x = rand() % (int)(r*r);//width;
        
        float y = sqrtf(r*r - x);//rand() % height;
        x = sqrtf(x);
        int b = rand() % 2;
        if(b==1) x *= -1;
        b = rand() % 2;
        if(b==1) y *= -1;
        y += 200;
        x += 200;
        x = rand() % width;
        y = rand() % height;*/
        //y/=4.f;
        float x = rand() % width;
        //float y = ((width/2 - fabsf(x - width/2)) / width) * (float)(rand() % height);
        //y /= 16;
        //float y = rand() % 100;
        //float x = rand() % 5000;//width;                                                                 //
        float y = /*rand()%height;*/ (fabsf(x - (width / 2)) / width) * (float)(rand() % height); //f(x)
        Point pos(x, y);                                                                          //rand()%width,rand()%(height/8)+(height/2));
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
    points.push_back({2000.f,1000.f});
    //points.push_back({2000.f, -1500.f});
    //points.push_back({22000.f, 0.f});
    
    LOG(points.size(), " points created");
}

LineGenerator LG;

struct varianceFunct : public Observer
{
    varianceFunct(Slider &varSlider):slider(varSlider){slider.addObserver(this);}
    void onUpdate() override
    {
        variance = slider.getVal();
        points.clear();
        plot(width, height, points);
        points.push_back({5000.f, 0.f});
        LG.generate(points);
        LG.advance();
    }
    Slider &slider;
};

struct funct : public Observer
{
    funct(Slider &smooth, Slider &sliver):smoo(smooth),sliv(sliver){smoo.addObserver(this);sliv.addObserver(this);}
    void onUpdate() override
    {
        float smooth = smoo.getVal();
        float sliver = sliv.getVal();
        LG.setSmoothSliver(smooth,sliver);
        LG.resetSmoothness();
        LG.advance();
    }
    Slider &smoo, &sliv;
};

struct resetFunc : public Observer
{
    resetFunc(Subject &subject){subject.addObserver(this);}
    void onUpdate() override
    {
        points.clear();
        plot(width, height, points);
        points.push_back({5000.f, 0.f});
        LG.generate(points);
        LG.advance();
    }
};

struct tinyFunc : public Observer
{
    tinyFunc(Slider &subject):s(subject) { subject.addObserver(this); }
    void onUpdate() override
    {
        LG.setTinySegs(s.getVal());
        LG.resetSmoothness();
        LG.advance();
    }
    Slider &s;
};

int main(int args, char **argv)
{
    int rand = time(NULL); //3;
    if (args == 2)
        rand = std::stoi(argv[1]);
    
    srand(rand); //time(NULL));

    sf::RenderWindow window(sf::VideoMode(width, height), "Line");
    sf::View view(window.getDefaultView());
    view.setSize(view.getSize().x, -view.getSize().y);
    view.zoom(2.f);
    view.setCenter(width,height/2);
    window.setView(view);
    sf::View defaultView = window.getDefaultView();
    CameraController camera(window, view);

    sf::Font font;
    font.loadFromFile("../Moonlander/Soft Elegance.ttf");
    Slider varianceSlider(font);
    varianceSlider.setPosition(400.f,50.f);
    varianceSlider.setScale(200.f,30.f);
    varianceSlider.setRange(0.f,0.5f);
    varianceSlider.setDefaultValue(variance);
    sf::Text varText;
    varText.setFont(font);
    varText.setString("Variance");
    varText.setPosition(400.f, 50.f);
    varText.setFillColor(sf::Color(255, 255, 255, 128));
    //float sliverValue = 500.f;
    //float smoothValue = 100.f;
    sf::Text smoothText;
    smoothText.setFont(font);
    smoothText.setString("Smoothness");
    smoothText.setPosition(10.f,10.f);
    smoothText.setFillColor(sf::Color(255, 255, 255, 128));
    sf::Text sliverText;
    sliverText.setFont(font);
    sliverText.setString("Slivers");
    sliverText.setPosition(10.f, 50.f);
    sliverText.setFillColor(sf::Color(255, 255, 255, 128));
    Slider smoothSlider(font);
    smoothSlider.setPosition(10.f,10.f);
    smoothSlider.setScale(200.f,30.f);
    smoothSlider.setRange(0,2000);//500);
    smoothSlider.setDefaultValue(100.f);
    Slider sliverSlider(font);
    sliverSlider.setPosition(10.f, 50.f);
    sliverSlider.setScale(200.f, 30.f);
    sliverSlider.setRange(0, 5000);//1000);
    sliverSlider.setDefaultValue(500.f);
    Button resetButton(font);
    resetButton.setPosition(400.f,90.f);
    resetButton.setScale(80.f,30.f);
    resetButton.setLabel("RESET");
    Slider tinySegsSlider(font);
    tinySegsSlider.setPosition(10.f, 90.f);
    tinySegsSlider.setScale(200.f, 30.f);
    tinySegsSlider.setRange(0.f,50.f);
    tinySegsSlider.setDefaultValue(10.f);
    sf::Text tinyText;
    tinyText.setFont(font);
    tinyText.setString("Min Length");
    tinyText.setPosition(10.f, 90.f);
    tinyText.setFillColor(sf::Color(255,255,255,128));

    //seed();
    plot(width,height,points);
    points.push_back({5000.f, 0.f});

    LG.generate(points);
    LG.advance();
    //LineGenerator LG(points);
    funct f(smoothSlider,sliverSlider);
    resetFunc resetf(resetButton);
    tinyFunc tinyf(tinySegsSlider);
    varianceFunct varfunct(varianceSlider);

    sf::Event event;

    while (window.isOpen())
    {
        window.clear(sf::Color::Black);
        LG.render(window);
        window.setView(defaultView);//window.getDefaultView());//wrong view... the resize size
        window.draw(smoothSlider);
        window.draw(sliverSlider);
        window.draw(smoothText);
        window.draw(sliverText);
        window.draw(resetButton);
        window.draw(tinySegsSlider);
        window.draw(tinyText);
        window.draw(varText);
        window.draw(varianceSlider);
        window.setView(view);
        window.display();

        window.waitEvent(event);
        switch (event.type)
        {
        case sf::Event::KeyPressed:
            if (event.key.code == sf::Keyboard::Space)
            {
                LG.advance();
                break;
            }
        case sf::Event::Closed:
            window.close();
            break;
        case sf::Event::Resized:
        {
            defaultView.setSize(event.size.width,event.size.height);
            defaultView.setCenter(defaultView.getSize()/2.f);
        }
        default:
            break;
        }
        resetButton.handleEvent(event);
        tinySegsSlider.handleEvent(event);
        smoothSlider.handleEvent(event);
        sliverSlider.handleEvent(event);//if handle wheel, don't want to ...
        varianceSlider.handleEvent(event);
        if(event.type == sf::Event::MouseWheelMoved && (smoothSlider.isActive() || sliverSlider.isActive() || tinySegsSlider.isActive() || varianceSlider.isActive())) continue;
        camera.handleEvent(event);// <- some sort of state maching herer?
    }

    return 0;
}