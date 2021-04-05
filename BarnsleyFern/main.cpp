#include <SFML/Graphics.hpp>

//barnsley's fern da wikipedia

/*
f(x,y) = |a b||x| + |e|
         |c d||y|   |f|
affine transformation

draw first point at origin 0,0
randomly apply one of the 4 coordinate transformations

*/

//constants
typedef float num;

struct BarnsleyTransform
{
    BarnsleyTransform(){}
    BarnsleyTransform(num A, num B, num C, num D, num E, num F) 
                        : a(A), b(B), c(C), d(D), e(E), f(F) {}
    num a,b,c,d,e,f;
    void transform(num &x, num &y);
};

void BarnsleyTransform::transform(num &x_out, num &y_out)
{
    num x = x_out;
    num y = y_out;
    x_out = a * x + b * y + e;
    y_out = c * x + d * y + f;
}

void init(BarnsleyTransform &B1, BarnsleyTransform &B2, BarnsleyTransform &B3, BarnsleyTransform &B4)
{
    B1.a = 0.00;
    B1.b = 0.00;
    B1.c = 0.00;
    B1.d = 0.16;
    B1.e = 0.00;
    B1.f = 0.00;

    B2.a = 0.85;
    B2.b = 0.04;
    B2.c =-0.04;
    B2.d = 0.85;
    B2.e = 0.00;
    B2.f = 1.60;

    B3.a = 0.20;
    B3.b =-0.26;
    B3.c = 0.23;
    B3.d = 0.22;
    B3.e = 0.00;
    B3.f = 1.60;

    B4.a =-0.15;
    B4.b = 0.28;
    B4.c = 0.26;
    B4.d = 0.24;
    B4.e = 0.00;
    B4.f = 0.44;
}

unsigned int width = 5000;
unsigned int height = 5000;

sf::Image image;

void drawPoint(num x, num y)
{
    //transform x y to image coords
    //x -2.182 -> 2.6558
    //y 0 -> 9.9983
    //so 10 high, and 10 wide works

    unsigned int xPixelCoord = (int)(x * (num)(width/10) + (width/2));
    unsigned int yPixelCoord = (int)(y * (num)(height/10));
    image.setPixel(xPixelCoord,yPixelCoord,sf::Color::Black);
}

int main()
{
    
    image.create(width,height,sf::Color::White);

    BarnsleyTransform B1,B2,B3,B4;
    init(B1,B2,B3,B4);

    int p1,p2,p3,p4;
    p1 = 1;
    p2 = 85;
    p3 = 7;
    p4 = 7;

    if(p1+p2+p3+p4 != 100) return 0;
    p2 += p1;
    p3 += p2;

    srand(time(NULL));
    int r;
    num x = 0.0;
    num y = 0.0;

    drawPoint(x,y);

    for(int i = 0; i<10000000; i++)
    {
        r = rand()%100;
        if(r <= p1) B1.transform(x,y);
        else if(r <= p2) B2.transform(x,y);
        else if(r <= p3) B3.transform(x,y);
        else B4.transform(x,y);
        drawPoint(x,y);
    }

    image.saveToFile("Fern.bmp");

    return 0;
}