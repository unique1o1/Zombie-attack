#include <iostream>
#include "TextureHolder.h"
#include <assert.h>

using namespace sf;
using namespace std;

TextureHolder *TextureHolder::m_s_Instance = nullptr;

TextureHolder::TextureHolder()
{
	assert(m_s_Instance == nullptr);
	m_s_Instance = this;
}

sf::Texture &TextureHolder::GetTexture(std::string const &filename)
{
	// Get a reference to m_Textures using m_S_Instance
	auto &m = m_s_Instance->m_Textures;
	// auto is the equivalent of map<string, Texture>

	auto keyValuePair = m.find(filename);
	// auto is equivelant of map<string, Texture>::iterator

	if (keyValuePair != m.end())
	{
		return keyValuePair->second;
	}
	else
	{
		auto &texture = m[filename];
		texture.loadFromFile(filename);

		// Return the texture to the calling code
		return texture;
	}
}