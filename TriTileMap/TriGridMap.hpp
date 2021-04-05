#ifndef TRIGRIDMAP_HPP
#define TRIGRIDMAP_HPP

#include <SFML/Graphics.hpp>
//#include <ostream>

enum GridAxis
{
    A = 1, // -- axis
    B = 2, // / axis
    C = 3 // \ axis
};

struct Coordinate
{
    int a, b, c;
    inline bool isUp() const {return !((a+b+c)%2);}
    inline bool operator==(const Coordinate &C) const {return a==C.a && b==C.b && c==C.c;}
    unsigned int distanceTo(const Coordinate &C) const;
};

struct TileData
{
    TileData():isActive(true){}
    bool isActive;
    inline void toggle(){isActive = !isActive;}
};

std::ostream &operator<<(std::ostream &os, const TileData &T);
std::istream& operator>>(std::istream &is, TileData &T);

class TileShape : public sf::Transformable//, public sf::Drawable
{
public:
	TileShape();
	void setColor(const sf::Color color){for(auto &V : m_shape) V.color = color;}
    void setTexture(const sf::Texture &texture);
	void moveToCoordinate(const Coordinate &C, float scaleFactor = 1.f);
    void draw(sf::RenderTarget &target);// const override;
private:
	sf::Vertex m_shape[6];
	
	bool amUp;
    const sf::Texture *p_texture;
};

class GridMap
{
public:
    GridMap(unsigned int width = 0, unsigned int height = 0, float scale = 1.f);
    bool loadFromFile(const std::string &fileName);
    bool saveToFile(const std::string &fileName);
    void setup(unsigned int width, unsigned int height, float scale);
    void display(sf::RenderTarget &target, TileShape &tileShape);
    void drawGridLines(sf::RenderTarget &target);
    void toggleTile(const Coordinate &C);//to a temporary???
    void setBounds(float x_min, float x_max, float y_min, float y_max);
    void convertPosToCoordinate(const sf::Vector2f &pos, Coordinate &C);
    bool checkTile(const sf::Vector2f &pos);
    bool checkTile(Coordinate &C);
    bool isValidCoordinate(const Coordinate &C);
    bool checkLine(const sf::Vector2f &p1, const sf::Vector2f &p2, Coordinate &hit);
    void clear();
    void getBounds(sf::Vector2f &min, sf::Vector2f &max);
    sf::Vector2f getSize();
    float getScale(){return m_scale;}
    //void getBounds(const sf::Vertex *points, const unsigned int numPts);
    float rayCast(const sf::Vector2f &p1, const sf::Vector2f &dir, float maxLength, Coordinate *C = nullptr);//or if it hits something?  
private:
    //TileShape m_tileShape;
    unsigned int m_width, m_height;
    unsigned int m_offset_x, m_offset_y;
    unsigned int i_min, i_max, j_min, j_max;//bounds
    float m_scale;
    std::vector<TileData> m_map;
    std::vector<sf::Vertex> m_gridLines;

    void setUpGridLines();
};

#endif //TRIGRIDMAP_HPP