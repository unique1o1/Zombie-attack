#include <iostream>
#include <SFML/Graphics.hpp>

#include "Player.h"
#include "ZombieArena.h"

#include "Zombie.h"
#include "TextureHolder.h"

#include "Bullet.h"
using namespace sf;
int main()

{
    TextureHolder holder;
    enum class State
    {
        PAUSED,
        LEVELING_UP,
        GAME_OVER,
        PLAYING
    };
    // Start with the GAME_OVER state
    State state = State::GAME_OVER;
    int numZombies;
    int numZombiesAlive;
    Zombie *zombies = nullptr;
    Vector2f resolution = Vector2f();

    resolution.x = VideoMode().getDesktopMode().width;
    resolution.y = VideoMode().getDesktopMode().height;
    RenderWindow window(VideoMode(resolution.x, resolution.y), "zombie attack", Style::Fullscreen);
    View mainView(FloatRect(0, 0, resolution.x, resolution.y));
    Clock clock;
    Time gameTimeTotal = clock.restart();

    Vector2f mouseWorldPosition;
    Vector2i mouseScreenPosition;
    VertexArray background;
    Texture textureBackground = TextureHolder::GetTexture("graphics/background_sheet.png");
    // 100 bullets should do
    Bullet bullets[100];
    int currentBullet = 0;
    int bulletsSpare = 24;
    int bulletsInClip = 6;
    int clipSize = 6;
    float fireRate = 1;
    // When was the fire button last pressed?
    Time lastPressed;
    Player player;
    IntRect arena;
    while (window.isOpen())
    {
        Event event;
        while (window.pollEvent(event))
        {
            // std::cout << "asdf";
            if (event.type == Event::KeyPressed)
            {
                if (event.key.code == Keyboard::Return && state == State::PLAYING)
                {
                    state = State::PAUSED;
                }
                else if (event.key.code == Keyboard::Return && state == State::PAUSED)
                {
                    state = State::PLAYING;
                    //reset the clock so there is no frame jump
                    // clock.restart();
                }
                else if (event.key.code == Keyboard::Return && state == State::GAME_OVER)
                {
                    state = State::LEVELING_UP;
                }
                if (state == State::PLAYING)
                { //reloading
                    if (event.key.code == Keyboard::R)
                    {
                        if (bulletsSpare >= clipSize)
                        {
                            // Plenty of bullets. Reload.
                            int temp = clipSize - bulletsInClip;
                            bulletsInClip = clipSize;
                            bulletsSpare -= temp;
                        }
                        else if (bulletsSpare > 0)
                        {
                            // Only few bullets left
                            bulletsInClip = bulletsSpare;
                            bulletsSpare = 0;
                        }
                        else
                        {
                            // More here soon?!
                        }
                    }
                }
            }
        } //end event polling
        if (Keyboard::isKeyPressed(Keyboard::Escape))
        {
            window.close();
        }

        // Handle controls while playing
        if (state == State::PLAYING)
        {
            // Handle the pressing and releasing of the WASD keys
            if (Keyboard::isKeyPressed(Keyboard::W))
            {
                player.moveUp();
            }
            else
            {
                player.stopUp();
            }

            if (Keyboard::isKeyPressed(Keyboard::S))
            {
                player.moveDown();
            }
            else
            {
                player.stopDown();
            }

            if (Keyboard::isKeyPressed(Keyboard::A))
            {
                player.moveLeft();
            }
            else
            {
                player.stopLeft();
            }

            if (Keyboard::isKeyPressed(Keyboard::D))
            {
                player.moveRight();
            }
            else
            {
                player.stopRight();
            }

            // Fire a bullet
            if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
            {

                if (gameTimeTotal.asMilliseconds() - lastPressed.asMilliseconds() > 500 && bulletsInClip > 0)
                {

                    // Pass the centre of the player
                    // and the centre of the cross-hair
                    // to the shoot function
                    bullets[currentBullet].shoot(
                        player.getCenter().x, player.getCenter().y,
                        mouseWorldPosition.x, mouseWorldPosition.y);

                    currentBullet++;
                    if (currentBullet > 99)
                    {
                        currentBullet = 0;
                    }
                    lastPressed = gameTimeTotal;

                    bulletsInClip--;
                }

            } // End fire a bullet

        } // End WASD while playing

        // Handle the levelling up state
        if (state == State::LEVELING_UP)
        {
            // Handle the player levelling up
            if (event.key.code == Keyboard::Num1)
            {
                state = State::PLAYING;
            }

            if (event.key.code == Keyboard::Num2)
            {
                state = State::PLAYING;
            }

            if (event.key.code == Keyboard::Num3)
            {
                state = State::PLAYING;
            }

            if (event.key.code == Keyboard::Num4)
            {
                state = State::PLAYING;
            }

            if (event.key.code == Keyboard::Num5)
            {
                state = State::PLAYING;
            }

            if (event.key.code == Keyboard::Num6)
            {
                state = State::PLAYING;
            }

            if (state == State::PLAYING)
            {
                // Prepare thelevel
                // We will modify the next two lines later
                arena.width = 1000;
                arena.height = 1000;
                arena.left = 0;
                arena.top = 0;

                int tileSize = createBackground(background, arena);

                // Spawn the player in the middle of the arena
                player.spawn(arena, resolution, tileSize);
                numZombies = 10;
                delete[] zombies;
                zombies = createHorde(numZombies, arena);
                numZombiesAlive = numZombies;

                // Reset the clock so there isn't a frame jump
                clock.restart();
            }
        } // End levelling up

        if (state == State::PLAYING)
        {
            // Update the delta time
            Time dt = clock.restart();
            // Update the total game time
            gameTimeTotal += dt;
            // Make a decimal fraction of 1 from the delta time
            float dtAsSeconds = dt.asSeconds();

            // Where is the mouse pointer

            mouseScreenPosition = Mouse::getPosition();

            // Convert mouse position to world coordinates of mainView
            mouseWorldPosition = window.mapPixelToCoords(
                Mouse::getPosition(), mainView);

            // Update the player
            player.update(dtAsSeconds, Mouse::getPosition());

            // Make a note of the players new position
            Vector2f playerPosition(player.getCenter());

            // Make the view centre around the player
            mainView.setCenter(player.getCenter());
            // Vector2f(resolution.x / 2, resolution.y / 2)
            for (int i = 0; i < numZombies; i++)
            {
                if (zombies[i].isAlive())
                {
                    zombies[i].update(dtAsSeconds, playerPosition);
                }
            }
            // Update any bullets that are in-flight
            for (int i = 0; i < 100; i++)
            {
                if (bullets[i].isInFlight())
                {
                    bullets[i].update(dtAsSeconds);
                }
            }
        } // End updating the scene

        /*
		**************
		Draw the scene
		**************
		*/

        if (state == State::PLAYING)
        {
            window.clear();

            // set the mainView to be displayed in the window
            // And draw everything related to it
            window.setView(mainView);
            window.draw(background, &textureBackground);
            for (int i = 0; i < numZombies; i++)
            {
                window.draw(zombies[i].getSprite());
            }

            for (int i = 0; i < 100; i++)
            {
                if (bullets[i].isInFlight())
                {
                    window.draw(bullets[i].getShape());
                }
            }

            // Draw the player
            window.draw(player.getSprite());
        }

        if (state == State::LEVELING_UP)
        {
        }

        if (state == State::PAUSED)
        {
        }

        if (state == State::GAME_OVER)
        {
        }

        window.display();
    }
    delete[] zombies;
    return 0;
}