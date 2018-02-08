#pragma once

#include <SFML/Graphics.hpp>
#include <map>

using namespace sf;

class TextureHolder
{
  public:
	TextureHolder();
	static Texture &GetTexture(std::string const &filename);

  private:
	//create a map object for holding key and value pair for sfml texture
	std::map<std::string, sf::Texture> m_Textures;

	// A pointer of the same type as the class itself
	// the one and only instance
	static TextureHolder *m_s_Instance;
};
