#include "Resources.hpp"

Texture::Texture(const std::string &fileName)
{
    if(!m_texture.loadFromFile(fileName))
    {
        std::string error = "Was unable to load texture ";
        error += fileName;
        throw std::runtime_error(error.c_str());
    }
    m_texture.setRepeated(true);
}

Font::Font(const std::string &fileName)
{
    if (!m_font.loadFromFile(fileName))
    {
        std::string error = "Was unable to load font ";
        error += fileName;
        throw std::runtime_error(error.c_str());
    }
}