#include <iostream>
#include <SFML/Graphics.hpp>
using namespace sf;
#include "Player.h"

int main()
{
    Vector2f resolution = Vector2f();

    resolution.x = VideoMode().getDesktopMode().width;
    resolution.y = VideoMode().getDesktopMode().height;
    RenderWindow(VideoMode(resolution.x, resolution.y), "zombie attack", Style::Fullscreen);
    View mainview(FloatRect(0, 0, resolution.x, resolution.y));
    Clock clock;
    time gameTimeTotal;
    Vector2f mouseWorldPosition;
    Vector2f mouseScreenPosition;
    Player player;
    IntRect areana;
    while (window.isOpen())
    {
    }
    return 0;
}