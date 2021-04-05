#ifndef RESOURCES_HPP
#define RESOURCES_HPP

#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <string>

//loading and unloading our resources
//fonts and stuff//ie smart pointer
class Texture
{
public:
    Texture(const std::string &fileName);
    Texture(){}
    const sf::Texture& getTexture() const {return m_texture;}
private:
    sf::Texture m_texture;
};

class Font
{
public:
    Font(const std::string &fileName);
    ~Font(){}
    const sf::Font& getFont() const {return m_font;}
private:
    sf::Font m_font;
};

struct Context
{
    Context(Font &font_, Texture &texture_, Texture &bgTexture_, sf::RenderTarget &target_)
        :font(font_),texture(texture_),bgTexture(bgTexture_),target(target_){}
    const Font &font;
    const Texture &texture;
    const Texture &bgTexture;
    sf::RenderTarget &target;
};

#endif