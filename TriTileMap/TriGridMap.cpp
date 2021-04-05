#include "TriGridMap.hpp"
#include <cmath>
#include <fstream>

#include <iostream>

//utilities...
const float x = 1.f / tanf(M_PI / 3.f);//half width

const float cos30n = cosf(-1.f*M_PI/6.f);
const float sin30n = sinf(-1.f*M_PI/6.f);
const float cos90 = 0.f;
const float sin90 = 1.f;
const float cos30 = cosf(M_PI/6.f);
const float sin30 = sinf(M_PI/6.f);

const sf::Vector2f unitVector(1.f,0.f);

sf::Vector2f rotate(const sf::Vector2f &v, const float cosAng, const float sinAng)
{
	sf::Vector2f V;
	V.x = v.x * cosAng - v.y * sinAng;
	V.y = v.x * sinAng + v.y * cosAng;
	return V;
}

float vecLength(const sf::Vector2f &v)
{
    return sqrtf(v.x*v.x + v.y*v.y);
}

const sf::Vector2f AVector = rotate(unitVector,cos90,sin90);
const sf::Vector2f BVector = rotate(unitVector,cos30n,sin30n);
const sf::Vector2f CVector = rotate(unitVector,cos30,sin30);

const sf::Vector2f ATVector = rotate(AVector,cos90,sin90);
const sf::Vector2f BTVector = rotate(BVector,cos90,sin90);
const sf::Vector2f CTVector = rotate(CVector,cos90,sin90);

unsigned int Coordinate::distanceTo(const Coordinate &C) const
{
    return abs(a - C.a) + abs(b - C.b) + abs(c - C.c);
}

std::ostream &operator<<(std::ostream &os, const TileData &T)
{
    return os<<T.isActive<<" ";
}

std::istream &operator>>(std::istream &is, TileData &T)
{
    return is>>T.isActive;
}

TileShape::TileShape() : p_texture(nullptr)
{
    setColor(sf::Color(200,120,140,128));
	
	//up triangle
	m_shape[0].position = {0.f,0.f};
	m_shape[1].position = {x,1.f};
	m_shape[2].position = {2*x,0.f};
	//down triangle
    /*m_shape[3].position = {2*x,0.f};
	m_shape[4].position = {x,1.f};
	m_shape[5].position = {3*x,1.f};
    */
	m_shape[3].position = {0.f,0.f};
	m_shape[4].position = {x,-1.f};
	m_shape[5].position = {2*x,0.f};
}

void TileShape::setTexture(const sf::Texture &texture)
{
    for (auto &V : m_shape)
    {
        V.color = sf::Color::White;
    }
    p_texture = &texture;
    for (int i = 0; i < 6; i++)
        m_shape[i].texCoords = getTransform().transformPoint(m_shape[i].position) *64.f;
}

void TileShape::moveToCoordinate(const Coordinate &C, float scaleFactor)
{
    amUp = C.isUp();
    if(amUp)
	{
		setPosition(float(C.b+C.c)*x*scaleFactor, float(C.a)*scaleFactor);
    }
	else
	{
		setPosition(float(C.b+C.c)*x*scaleFactor, float(C.a + 1)*scaleFactor);
    }
}

void TileShape::draw(sf::RenderTarget &target)//, sf::RenderStates states)// const
{
    sf::RenderStates states;
	states.transform = getTransform();
    if (p_texture)
    {
        states.texture = p_texture;
        float scaleFactor = target.getSize().x / target.getView().getSize().x;
        if (amUp)
            for (std::size_t i = 0; i < 3; i++)
                m_shape[i].texCoords = getTransform().transformPoint(m_shape[i].position) * scaleFactor;
        else
            for (std::size_t i = 3; i < 6; i++)
                m_shape[i].texCoords = getTransform().transformPoint(m_shape[i].position) * scaleFactor;
    }
    if(amUp) target.draw(&m_shape[0],3,sf::Triangles,states);
	else target.draw(&m_shape[3],3,sf::Triangles,states);
}

GridMap::GridMap(unsigned int width, unsigned int height, float scale)
                : m_width(width), m_height(height), m_offset_x(0), m_offset_y(0)
                , i_min(0), i_max(m_width), j_min(0), j_max(m_height)
                , m_scale(scale)
{
    m_map.resize(m_height * m_width);
    setUpGridLines();
}

void GridMap::setup(unsigned int width, unsigned int height, float scale)
{
    m_width = width;
    m_height = height;
    m_scale = scale;
    m_map.resize(m_height * m_width);
    i_max = m_width;
    i_min = 0;
    j_min = 0;
    j_max = m_height;
    m_offset_x = m_offset_y = 0;
    setUpGridLines();
}

bool GridMap::saveToFile(const std::string &fileName)
{
    std::ofstream outFile(fileName);
    if(!outFile) return false;

    outFile << m_width << " " <<m_height << " " << m_scale<<" ";

    for(auto &T : m_map)
        outFile<<T;
    
    outFile.close();

    return true;
}

bool GridMap::loadFromFile(const std::string &fileName)
{
    std::ifstream inFile(fileName);
    if (!inFile) return false;

    inFile >> m_width >> m_height >> m_scale;
    m_map.resize(m_height * m_width);

    for(unsigned int i = 0; i < m_width; i++)
    {
        for(unsigned int j = 0; j < m_height; j++)
        {
            inFile >> m_map[i * m_height + j];
        }
    }

    i_min = 0;
    i_max = m_width;
    j_min = 0;
    j_max = m_height;
    m_offset_x = m_offset_y = 0;

    inFile.close();

    setUpGridLines();

    return true;
}

//convert ABC to IJ or XY ???

void GridMap::setUpGridLines()
{
    int numALines = m_height + 1;
    int numBLines = m_height/2 + m_width/2 + 1;
    int numCLines = numBLines;

    m_gridLines.resize(2*(numALines+numBLines+numCLines));
	for(auto &V : m_gridLines) V.color = sf::Color::Blue;

    float lineLength = m_width*m_height*m_scale;//arbitrarily big but same factor size
    sf::Vector2f scaledOffset = sf::Vector2f(m_offset_x * m_scale,m_offset_y * m_scale);

    for(int i = 0; i<numALines; i++)
	{
		m_gridLines[2*i].position
            = AVector * (float)(i) * m_scale
            + ATVector * -lineLength 
            + scaledOffset;
		m_gridLines[2*i+1].position
            = AVector * (float)(i) * m_scale
            + ATVector * lineLength 
            + scaledOffset;
	}
	for(int i = 0; i<numBLines; i++)
	{
		float f = i - (int)m_height/2;
		m_gridLines[2*(i+numALines)].position
            = BVector * f * m_scale
            + BTVector * -lineLength
            + scaledOffset;
		m_gridLines[2*(i+numALines)+1].position
            = BVector * f * m_scale
            + BTVector * lineLength
            + scaledOffset;
	}
	for(int i = 0; i<numCLines; i++)
	{
		float f = i+1;// - (int)m_height/2;
		m_gridLines[2*(i+numALines+numBLines)].position
            = CVector * f * m_scale
            + CTVector * -lineLength
            + scaledOffset;
		m_gridLines[2*(i+numALines+numBLines)+1].position
            = CVector * f * m_scale
            + CTVector * lineLength
            + scaledOffset;
	}
}

void GridMap::display(sf::RenderTarget &target, TileShape &tileShape)
{
    tileShape.setScale(m_scale, m_scale);
    Coordinate C;
    for(unsigned int j = j_min; j < j_max; j++)
    {
        C.a = j + m_offset_y;
        C.b = C.a/2 + m_offset_x;
        C.c = -C.b - 1 + m_offset_x + i_min;//2 wrongs make right

        for(unsigned int i = i_min; i < i_max; i++)
        {
            if(m_map[j*m_width + i].isActive)
            {
                tileShape.moveToCoordinate(C, m_scale);
                tileShape.draw(target);
            }
            C.b++;//i proportional to b + c
        }
    }
}

void GridMap::drawGridLines(sf::RenderTarget &target)
{
	target.draw(&m_gridLines[0],m_gridLines.size(),sf::Lines);
}

void GridMap::setBounds(float x_min, float x_max, float y_min, float y_max)
{
    std::cout<<"x is "<<x<<std::endl;
    std::cout<<i_min<<" "<<i_max<<" "<<j_min<<" "<<j_max<<std::endl;
    if(x_min < x*m_scale) i_min = 0;
    else
    {
        i_min = int(x_min / (x*m_scale));// -1;
    }
    if(y_min < 0.f) j_min = 0;
    else
    {
        j_min = y_min / m_scale;
    }
    if(x_max > m_scale * x  * m_width) i_max = m_width;
    else
    {
        i_max = int(x_max / (x*m_scale)) + 2;
    }
    if(y_max > m_scale * m_height) j_max = m_height;
    else
    {
        j_max = int(y_max / m_scale) + 1;
    }
    std::cout<<i_min<<" "<<i_max<<" "<<j_min<<" "<<j_max<<std::endl;
}

sf::Vector2f GridMap::getSize()
{
    if(m_width == 0 || m_height == 0) return sf::Vector2f(0,0);
    float sx = (m_width-1) * x * m_scale;
    float sy = m_height * m_scale;
    return sf::Vector2f(sx,sy);
}

void GridMap::getBounds(sf::Vector2f &min, sf::Vector2f &max)
{
    min.x = i_min * x * m_scale;//x = half-width??? rename
    max.x = (i_max - 1) * x * m_scale;
    min.y = j_min * m_scale;
    max.y = j_max * m_scale;
}

bool GridMap::isValidCoordinate(const Coordinate &C)
{
    return
        ((unsigned int)(C.a - m_offset_y) < m_height)
        && ((unsigned int)(C.b + C.c + 1 - m_offset_x) < m_width);
}

void GridMap::convertPosToCoordinate(const sf::Vector2f &pos, Coordinate &C)
{
    //only rotate the x coordinates....
    sf::Vector2f scaledPos = pos / m_scale;
    C.a = floorf(-1.f *(scaledPos.x * cos90 - scaledPos.y * sin90));
    C.b = floorf(scaledPos.x * cos30n - scaledPos.y * sin30n);
    C.c = floorf(scaledPos.x * cos30 - scaledPos.y * sin30);
}

void GridMap::toggleTile(const Coordinate &C)
{
    if (!isValidCoordinate(C))
    {
        //std::cout<<"out of bounds "<<C.a<<" "<<C.b<<" "<<C.c<<std::endl;
		return;
    }
//convert to i j ???
    int j = C.a - m_offset_y;
	int i = C.b + C.c + 1 - m_offset_x;

    m_map[j*m_width + i].toggle();
}

bool GridMap::checkTile(Coordinate &C)
{
    if (!isValidCoordinate(C))
		return false;
    int j = C.a - m_offset_y;
	int i = C.b + C.c + 1 - m_offset_x;
	return m_map[j*m_width + i].isActive;
}

bool GridMap::checkTile(const sf::Vector2f &pos)
{
    Coordinate C;
    convertPosToCoordinate(pos,C);
    return checkTile(C);
}

//casts line till we hit something, fills a result
//what tile did we hit?
//is it below us...
//I can separate this out...
//we do the ray cast, gives origin coordinate and end coordinate, and our dx DX values
bool GridMap::checkLine(const sf::Vector2f &p1, const sf::Vector2f &p2, Coordinate &hit)
{
	//DX = distance ray travels between gridlines
	//dx = distance from origin to first gridline
    auto getdxDX = [](const sf::Vector2f &origin, const sf::Vector2f &ray, const float cosAng, const float sinAng, float &dx, float &DX, bool &up)
	{
		sf::Vector2f p1 = rotate(origin, cosAng, sinAng);
//		p1.x = origin.x * cosAng - origin.y * sinAng;
//		p1.y = origin.x * sinAng + origin.y * cosAng;
		sf::Vector2f rayX = rotate(ray, cosAng, sinAng);
//		rayX.x = ray.x * cosAng - ray.y * sinAng;
//		rayX.y = ray.x * sinAng + ray.y * cosAng;

		DX = sqrtf(1.f + rayX.y * rayX.y / (rayX.x * rayX.x));
	//DX = fabsf(1 / rayX.y);
		float x0 = fmodf(p1.x,1.f);
		if(x0 < 0) x0 += 1.f;
		if(rayX.x > 0.f)
		{
			up = true;
			dx = (1.f - x0)*DX;
		}
		else
		{
			up = false;
			dx = x0*DX;
		}
	};

    Coordinate curCoordinate;
    Coordinate endCoordinate;
    convertPosToCoordinate(p1,curCoordinate);
    if(checkTile(curCoordinate))
    {
        hit = curCoordinate;
        return true;
    }
    convertPosToCoordinate(p2,endCoordinate);
    if(checkTile(endCoordinate))
    {
        hit = endCoordinate;
        return true;
    }

    toggleTile(curCoordinate);

    //check if 'adjacent' ie d == 2 before doing 'ray cast'
    unsigned int distance = curCoordinate.distanceTo(endCoordinate);
    if(distance == 2)
    {
        //check middle coordinate, don't need to do a ray cast
        if(curCoordinate.a == endCoordinate.a)
        {//moves right left
            if(endCoordinate.b > curCoordinate.b)//to the right
            {
                if(curCoordinate.isUp())
                    curCoordinate.c++;
                else
                    curCoordinate.b++;
            }
            else
            {
                if(curCoordinate.isUp())
                    curCoordinate.b--;
                else
                    curCoordinate.c--;
            }
        }
        else if(curCoordinate.b == endCoordinate.b)
        {
            if(endCoordinate.a > curCoordinate.a)//goes up
            {
                if(curCoordinate.isUp())
                    curCoordinate.c++;
                else 
                    curCoordinate.a++;
            }
            else
            {
                if(curCoordinate.isUp())
                    curCoordinate.a--;
                else
                    curCoordinate.c--;
            }
        }
        else //c == c
        {
            if(endCoordinate.a > curCoordinate.a)//goes up
            {
                if(curCoordinate.isUp())
                    curCoordinate.b++;
                else
                    curCoordinate.a++;
            }
            else
            {
                if(curCoordinate.isUp())
                    curCoordinate.a--;
                else
                    curCoordinate.b--;
            }
        }
        //toggleTile(curCoordinate);
        if(checkTile(curCoordinate))
        {
            hit = curCoordinate;
            return true;
        }
        return false;
    }

    sf::Vector2f rayVector = p2 - p1;
	rayVector /= m_scale;//distance = 1*m_scale between grid lines
    sf::Vector2f origin = p1/m_scale;

    float da, DA, db, DB, dc, DC;
    bool dirA, dirB, dirC;

    getdxDX(origin,rayVector,cos90,sin90,da,DA,dirA);
	getdxDX(origin,rayVector,cos30,sin30,db,DB,dirB);
	getdxDX(origin,rayVector,cos30n,sin30n,dc,DC,dirC);

    float lineLength = vecLength(rayVector);

    float nextA = da;
	float nextB = db;
	float nextC = dc;

    float t = 0;

    while(t < lineLength)
    {
        if(nextA < nextB && nextA < nextC)
        {
            if(!dirA) curCoordinate.a++;
            else curCoordinate.a--;
            t = nextA;
            nextA += DA;
        }
        else if(nextB < nextA && nextB < nextC)
        {
            if(dirB) curCoordinate.b++;
            else curCoordinate.b--;
            t = nextB;
            nextB += DB;
        }
        else
        {
            if(dirC) curCoordinate.c++;
            else curCoordinate.c--;
            t = nextC;
            nextC += DC;
        }
        toggleTile(curCoordinate);
        if(checkTile(curCoordinate))
        {
            hit = curCoordinate;
            return true;
        }
    }
    return false;
}

//returns length till it hits something and coordinate ray ends up at (optionally)
float GridMap::rayCast(const sf::Vector2f &p1, const sf::Vector2f &rayVector, float maxLength, Coordinate *C)
{
    //hmmm
    auto getdxDX = [](const sf::Vector2f &origin, const sf::Vector2f &ray, const float cosAng, const float sinAng, float &dx, float &DX, bool &up) {
        sf::Vector2f p1 = rotate(origin, cosAng, sinAng);
        sf::Vector2f rayX = rotate(ray, cosAng, sinAng);
        
        DX = sqrtf(1.f + rayX.y * rayX.y / (rayX.x * rayX.x));

        float x0 = fmodf(p1.x, 1.f);
        if (x0 < 0)
            x0 += 1.f;
        if (rayX.x > 0.f)
        {
            up = true;
            dx = (1.f - x0) * DX;
        }
        else
        {
            up = false;
            dx = x0 * DX;
        }
    };

    float length = 0.f;

    //sf::Vector2f rayVector = dir / m_scale; //distance = 1*m_scale between grid lines
    sf::Vector2f origin = p1 / m_scale;

    Coordinate curCoordinate;
    convertPosToCoordinate(p1, curCoordinate);
    if (checkTile(curCoordinate))
    {
        if(C) *C = curCoordinate;
        return length;
    }

    float da, DA, db, DB, dc, DC;
    bool dirA, dirB, dirC;

    getdxDX(origin, rayVector, cos90, sin90, da, DA, dirA);
    getdxDX(origin, rayVector, cos30, sin30, db, DB, dirB);
    getdxDX(origin, rayVector, cos30n, sin30n, dc, DC, dirC);

    float nextA = da;
    float nextB = db;
    float nextC = dc;

    //the 'walk
    float scaledMax = maxLength / m_scale;
    while (length < scaledMax)
    {
        if (nextA < nextB && nextA < nextC)
        {
            if (!dirA)
                curCoordinate.a++;
            else
                curCoordinate.a--;
            length = nextA;
            nextA += DA;
        }
        else if (nextB < nextA && nextB < nextC)
        {
            if (dirB)
                curCoordinate.b++;
            else
                curCoordinate.b--;
            length = nextB;
            nextB += DB;
        }
        else
        {
            if (dirC)
                curCoordinate.c++;
            else
                curCoordinate.c--;
            length = nextC;
            nextC += DC;
        }
        if (checkTile(curCoordinate) || !isValidCoordinate(curCoordinate))
        {
            if(!isValidCoordinate(curCoordinate)) return maxLength;
            if(C) *C = curCoordinate;
            length *= m_scale;
            if(length > maxLength) return maxLength;
            return length;
        }
    }

    if(C) *C = curCoordinate;
    return maxLength;
}

void GridMap::clear()
{
    for(auto &T : m_map) T.isActive = true;
}

/*
void GridMap::getBounds(const sf::Vertex *points, const unsigned int numPts)
{
    if(numPts==0) return;
    //int A_min, A_max, B_min, B_max, C_min, C_max;
    int lower_i, upper_i, lower_j, upper_j;
    //int j = C.a - m_offset_y;
	//int i = C.b + C.c + 1 - m_offset_x;
    Coordinate C;
    convertPosToCoordinate(points[0].position,C);
    lower_j = upper_j = C.a;
    lower_i = upper_i = C.b + C.c + 1;

    for(unsigned int i = 1; i<numPts;i++)
    {
        convertPosToCoordinate(points[i].position,C);
        if(C.a < lower_j) lower_j = C.a;
        else if(C.a > upper_j) upper_j = C.a;
        if(C.b + C.c < lower_i) lower_i = C.b + C.c - 1;
        else if(C.b + C.c + 1 > upper_i) upper_i = C.b + C.c + 1;
    }

    if(lower_i < 0) lower_i = 0;
    if(lower_j < 0) lower_j = 0;
    if(upper_i >= m_width) upper_i = m_width-1;
    if(upper_j >= m_height) upper_j = m_height-1;

    for(int j = lower_j; j <= upper_j; j++)
    {
        for(int i = lower_i; i<= upper_i; i++)
            m_map[j*m_width + i].toggle();
    }
    
    A_max = A_min = C.a;
    B_max = B_min = C.b;
    C_max = C_min = C.c;
    for(unsigned int i = 1; i<points.size();i++)
    {
        convertPosToCoordinate(points[0],C);
        if(C.a < A_min) A_min = C.a;
        else if(C.a > A_max) A_max = C.a;
        if(C.b < B_min) B_min = C.b;
        else if(C.b > B_max) B_max = C.b;
        if(C.c < C_min) C_min = C.c;
        else if(C.c > C_max) C_max = C.c;
    }
}
*/