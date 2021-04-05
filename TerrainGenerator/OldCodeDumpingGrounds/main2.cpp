#include <SFML/Graphics.hpp>
#include <vector>
#include <unistd.h>
#include <list>
#include "../SFML_Functions/SFML_Functions.hpp"
#include <stack>
#include <cmath>
#include <fstream>

//TODO:
/********
 * BUGS:
 * Self-intersections from cuts, on the cut side, sometimes causes it to intersect
 * when we cut the line through the poly line, the side we take the points out and sometimes revers is fine,
 * the other side can get intersections when we just connect the 2 points, if they are not in a line
 * maybe the algorithm is just need work... ???
 * also... to do
 * work on not 'cutting' necessarily, especially if have to cut through half the screen
 * it causes really long jagged thin segments... looks like a cut
 * can be good for a channel maybe one day, with a mine shaft or something
 * but better to 'grow' the line there.  find closer points and grow it out towards it.
 * 
 * Make more readable duh
 * 
 * Otherwise seems to be fine.
 * 
 * Seeding the points properly is important too.
 * And also finetuning the result also...
 * remove jaggies, intersections, assert clear distance between walls -> remove outcrops
 * and then noisify
 */

//line walker... has issues...

//ANOTHER IDEA!!!
//generate random points on a plane...
//connect the points pseudorandomly
//ie closest to closest...
//when the 'path' you follows ends, and the next closest results in an intersection.
//Swap the points.. so that the last point is connected to the point after the intersection
//and the previous last point is the 'next' point and then continue in reverse order till reach same point before the intersection
//n line
//n+1 point intersects at i to i+1 line
//i goes to n to n-1 ... n-2 ... till i+1 then to n+1 // like a stack or list
//as always finding the 'closest' points, ie voronoi-ish the buffer is maximized??? so clear splace is bigger, not splinter triangles,,, tiny gaps 
//and can randomize so points are a 'gap' from each other.
//HMMMM
//turtle..

sf::VertexArray nextLine;
std::list<Point> points;
std::list<Point> line;
unsigned int width = 800;
unsigned int height = 600;
sf::CircleShape C;

void seed()
{
    //points.push_back({-1000.f,0.f});
    /*
    points.push_back({200.f, 100.f});
    points.push_back({100.f, 200.f});
    points.push_back({300.f, 400.f});
    points.push_back({500.f, 500.f});
    points.push_back({700.f, 300.f});
    points.push_back({600.f, 100.f});
    points.push_back({50.f, 550.f});
    */
   /*
    points.push_back({300.f, 200.f});
    points.push_back({400.f, 400.f});
    points.push_back({500.f, 400.f});
    points.push_back({600.f, 200.f});
    points.push_back({650.f, 200.f});
    points.push_back({700.f, 250.f});
    points.push_back({750.f, 220.f});
    points.push_back({50.f, 400.f});
    */
   /* 4 int case
    points.push_back({300.f, 300.f});
    points.push_back({350.f, 400.f});
    points.push_back({500.f, 300.f});
    points.push_back({550.f, 400.f});
    points.push_back({650.f, 300.f});
    points.push_back({700.f,350.f});
    points.push_back({50.f, 350.f});
    */
    for(uint i = 0; i<128; i++)//128
    {
        float x = rand()%width;//
        float y = /*rand()%height;*/(fabsf(x - (width/2))/width) * (float)(rand()%height);//f(x)
        Point pos(x,y);//rand()%width,rand()%(height/8)+(height/2));
        bool collide = false;
        for(auto &point : points)
        {
            Point delta = pos - point;
            if(delta.x*delta.x + delta.y*delta.y < 100.f)
            {
                collide = true;
                break;
            }
        }
        if(collide) continue;
        points.push_back(pos);
    }
    points.push_back({2000.f, 0.f});
    line.push_back({-1000.f,0.f});
    LOG(points.size()," points created");
}

void handleCollision(std::list<Point>::iterator i0,
                     std::list<Point>::iterator i1,
                     std::list<Point>::iterator in)
{
    //restructure the line
    //i goes to n to n-1 to n-2 ... to i+1 then add p at the end
    //std::stack<Point> pointStack;
    std::reverse(i1,in);
}

bool lessThan(std::list<Point>::iterator i0,
                     std::list<Point>::iterator i1)
{
    auto it = i0;
    while (true)//it != i1 && it != line.end())
    {
        if(it == i1) return true;
        if(it == line.end()) return false;
        ++it;
    }
    return true;
}

void init()
{
    line.push_back(points.front());
    points.erase(points.begin());
    line.push_back(points.front());
    points.erase(points.begin());
    line.push_back(points.front());
    points.erase(points.begin());
}

auto getNextAvailableClosestPoint()
{
    float closestDist = std::numeric_limits<float>::max();
    auto closestPoint = points.begin();
    for (auto it = points.begin(); it != points.end(); ++it)
    {
        Point delta = (*it) - line.back();
        float dist = delta.x * delta.x + delta.y * delta.y;
        if (dist < closestDist)
        {
            closestPoint = it;
            closestDist = dist;
        }
    }

    nextLine[0].position = line.back();
    nextLine[1].position = *closestPoint;

    return closestPoint;
}

void generateLine()
{
    if(points.size()==0) return;
    //find closest point in points.. append it to the line while points to remove...
    //while (points.size())
    {
        //find next available closest point
        float closestDist = std::numeric_limits<float>::max();
        /*auto closestPoint = points.begin();
        for(auto it = points.begin(); it != points.end(); ++it)
        {
            Point delta = (*it) - line.back();
            float dist = delta.x*delta.x + delta.y*delta.y;
            if (dist < closestDist)
            {
                closestPoint = it;
                closestDist = dist;
            }
        }*/

        auto closestPoint = getNextAvailableClosestPoint();
        Point delta = (*closestPoint) - line.back();
        float dist = delta.x * delta.x + delta.y * delta.y;
        //if(dist > 300*300 && line.size()>2) return;

        //generate a line from n to x and test if collides with existing polyline...
        //find the closest lineSegmentIntersectionPoint.... to n... gives you the line segment to swap
        bool collision = false;
        closestDist = std::numeric_limits<float>::max();
        auto closestLine = line.begin();
        //or sort them???
        struct itDist
        {
            float dist;
            std::list<Point>::iterator it;
        };
        std::vector<itDist> intersectors;

        for (auto it = line.begin(); it != line.end()/*std::prev(line.end())*/; ++it) //or - 2 ???
        {
            Point intersection;
            if (SFUTIL::lineSegsIntersect((*it), *std::next(it), line.back(), (*closestPoint), &intersection))
            {
                collision = true;
                Point delta = (*closestPoint) - intersection;//use dist2 function?
                float dist = delta.x * delta.x + delta.y * delta.y;
                if (dist < closestDist)
                {
                    closestLine = it;
                    closestDist = dist;
                }

                bool inserted = false;
                for(uint i = 0; i<intersectors.size(); ++i)//wtf?
                {//inserts before...
                    if(intersectors[i].dist > dist)
                    {
                        intersectors.insert(intersectors.begin()+i,{dist,it});//hmm
                        inserted = true;
                        break;
                    }
                }
                if(!inserted) intersectors.push_back({dist,it});
            }
        }
        if(collision)
        {
            std::list<Point> &LRef = line;
            std::list<Point> reorderedPoints;
            std::stack<Point> pointStack;
            for(uint i = 0; i < intersectors.size()/2; ++i)//if just one intersectin, just go to end case...
            {
                //Join i10 to i21 reverse order of cut and add at end...
                //it is when we connect the 2 points is collison...
                //Check for collision between where we join the line... and rest of line, after do the swapperoo and call the swap
                //function again...  Need to rewrite the code...
                //we reverse i11 -> i20 (inclusive) 1 less than i21 and splice it out
                //so we connect i10 to i21 <- the line to test for an intersection(s).  and call our 'cut' function recursively till no intersections found.
                auto i10 = intersectors[2*i].it;
                auto i11 = std::next(i10);
                //auto i12 = std::next(i11);
                auto i20 = intersectors[2*i+1].it;
                auto i21 = std::next(i20);

                bool isLess = lessThan(i21,i11);
                if(isLess) 
                    std::swap(i21,i11);
                //cut or splice... reverse first then splice to front or reorderedpoints
                //erase removes iterator to next. element...
                /*auto it = i11;
                while(it != i21)
                {
                    if(it == line.end()) throw;
                    pointStack.push(*it);
                    it = line.erase(it);
                }*/
                if(!isLess)
                std::reverse(i11,i21);
                reorderedPoints.splice(reorderedPoints.begin(), line, i11,i21);//i21, i11);//i11, i21);//i20, i11);
            }
            //if is odd do last case too
            
            if(intersectors.size()%2)
            {
                auto i11 = std::next( intersectors.back().it);
                //auto n = line.end();
                /*auto it = i11;
                while(it != line.end())
                {
                    pointStack.push(*it);
                    it = line.erase(it);
                }*/
                std::reverse(i11, line.end());
                reorderedPoints.splice(reorderedPoints.begin(), line, i11, line.end()); //n, i11);
            }

            //place back at end ...
            /*while (pointStack.size())
            {
                line.push_back(pointStack.top());
                pointStack.pop();
            }*/
            
            line.splice(line.end(), reorderedPoints);
            int f = line.size();
            //i10->i21
            //i11->p
              //  i20->i31
                //    i21->i10
                  //      i30->i41
            //for(uint i = 0; i<intersectors.size(); i++)
            //{//no. reverse every second one or so...... ???
              //  if(i == 0) std::reverse(std::next(intersectors[i].it), line.end());
                //else std::reverse(std::next(intersectors[i].it),std::next(intersectors[i-1].it));
            //}
            //LOG("Collision");
            //std::reverse(std::next(closestLine), line.end());//std::prev(line.end()));
            //this far is fine... but how else we handle the other collisions?
            //a big line over multiple linesegments... each with it's own index
            //do i just reverse them all and save them all?
            //continue;//???
        }

        line.push_back(*closestPoint);
        points.erase(closestPoint);
    }
    
    LOG(line.size(), " points linked", points.size(), " points left");
}

int main(int args, char** argv)
{
    int rand = time(NULL);//3;
    if(args == 2) rand = std::stoi(argv[1]);
    //srand(time(NULL));
    srand(rand);//time(NULL));

    sf::RenderWindow window(sf::VideoMode(width, height), "Line");
    sf::View view(window.getDefaultView());
    view.setSize(view.getSize().x,-view.getSize().y);
    window.setView(view);
    CameraController camera(window, view);

    C.setFillColor(sf::Color(80,80,128,128));
    C.setRadius(5.f);
    C.setOrigin(5.f,5.f);

    seed();
    //init();

    //sf::VertexArray V;
    //V.setPrimitiveType(sf::Points);
    std::vector<sf::CircleShape> shapes;
    for(auto &point : points)
    {
        C.setPosition(point);
        shapes.push_back(C);
    }
    //for(auto &point : points) V.append({point, sf::Color::Blue});

    nextLine.setPrimitiveType(sf::Lines);
    nextLine.resize(2);
    nextLine[0].color = nextLine[1].color = sf::Color::Red;

    for(int i = 0; i<53; i++) generateLine();
    sf::VertexArray L;
    L.setPrimitiveType(sf::LineStrip);
    for(auto &point : line) L.append({point, sf::Color::Green});

    sf::Event event;

    while (window.isOpen())
    {
        window.clear(sf::Color::White);
        window.draw(L);
        window.draw(nextLine);
        for(auto &s : shapes) window.draw(s);
        window.display();

        window.waitEvent(event);
        switch (event.type)
        {
        case sf::Event::KeyPressed:
            if (event.key.code == sf::Keyboard::Space)
            {
                
                generateLine();
                L.clear();
                for(auto &point : line) L.append({point, sf::Color::Green});
                getNextAvailableClosestPoint();
                break;
            }
        case sf::Event::Closed:
            window.close();
            break;
        default:
            break;
        }
        camera.handleEvent(event);
    }

    std::ofstream ofs("outfile");
    if(!ofs.is_open()) return -1;
    for(auto &point : line)
    {
        ofs << point.x << " " << point.y << std::endl;
    }
    ofs.close();

    return 0;
}

    /***
sf::VertexArray points;

sf::Vertex vertex;
sf::Transform rotation;
Point curPos;
int turnCtr;//no more than 3 consecutive right or left turns?
int turnWt;
//move forward move back, etc...

void advance()
{
    //float length = rand()%50 + 50.f;
    float buffer = 20.f;
    //float testLength = length + buffer;

    //Point newPoint = {length, 0.f};
    //Point testPoint = {testLength, 0.f};

    uint ctr = 0;
    uint collisionCtr = 0;
    int turn = 0;
    while (true)
    {
        
        vertex.color = sf::Color::Black;
        float length = rand() % 50 + 50.f;
        Point newPoint = {length, 0.f};
        float angle = rand()%180 - 90;

        if(angle > 0.f) turn = 1;
        else if(angle < 0.f) turn = -1;
        if(turnCtr > 1 && turn == 1) continue;
        else if(turnCtr < -1 && turn == -1) continue;

        sf::Transform newTransform = rotation;
        newTransform.rotate(angle);
        //newTransform.translate(curPos);

        newPoint = newTransform.transformPoint(newPoint) + curPos;
        //testPoint = currentHeading.transformPoint(testPoint); //rotate it... translate it

        //curPos
        bool collided = false;
        if(points.getVertexCount() > 2)
        for(uint i = 0; i < points.getVertexCount()-1; i++)
        {
            //buffer test too 
            //point to perpendicular distance of line...
            //and the line perp... of length buffer intersects the line too...
            //get equation of line?
            SFUTIL::line<float> l;
            SFUTIL::lineFromPoints<float>(points[i].position,points[i+1].position,l);
            // distance is given by... (| a*x1 + b*y1 + c |) / (sqrt( a*a + b*b))
            float dist = (l.a * newPoint.x + l.b * newPoint.y + l.c);
            dist *= dist;
            dist /= (l.a*l.a + l.b*l.b);

            //Point delta = newPoint - points[i].position;
            //if(delta.x*delta.x + delta.y*delta.y < buffer*buffer)
            if(dist < buffer * buffer)
            {
                collided = true;
                break;
            }

            if (SFUTIL::lineSegsIntersect<float>(points[i].position, points[i+1].position,
                                                curPos, newPoint))
            {
                vertex.color = sf::Color::Red;
                LOG("COLLIDED");
                collided = true;
                ++collisionCtr;
                break;
            }
        }
        if(collisionCtr > 100)
        {
            LOG("No options :(");
            points.resize(points.getVertexCount()-1);
            collisionCtr = 0;
            break;
        }
        if(collided) continue;

        //generate line...
        vertex.position = newPoint;
        points.append(vertex);

        rotation = newTransform;
        curPos = newPoint;
        turnCtr += turn;
        turnWt += turn;
        LOG(turnWt);

        ++ctr;
        if(ctr > 0) break;
    }
    
}

int main()
{
    turnCtr = 0;
    turnWt = 0;
    points.setPrimitiveType(sf::LineStrip);
    vertex.color = sf::Color::Black;
    vertex.position = curPos = {100.f,300.f};
    points.append(vertex);

    while(points.getVertexCount() < 100) advance();

    srand(time(NULL));

    unsigned int width = 800;
    unsigned int height = 600;
    sf::RenderWindow window(sf::VideoMode(width,height),"Line");
    sf::View view(window.getDefaultView());
    CameraController camera(window,view);

    sf::Event event;

    while (window.isOpen())
    {
        window.clear(sf::Color::White);
        window.draw(points);
        window.display();

        window.waitEvent(event);
        switch (event.type)
        {
        case sf::Event::KeyPressed:
            if(event.key.code == sf::Keyboard::Space)
            {
                advance();
                break;
            }
        case sf::Event::Closed:
            window.close();
            break;
        default:
            break;
        }
        camera.handleEvent(event);

    }
    
    return 0;
}*/