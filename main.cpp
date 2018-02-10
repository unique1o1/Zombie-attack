#include <iostream>
#include <SFML/Graphics.hpp>

#include "Player.h"
#include "ZombieArena.h"

#include "Zombie.h"
#include "TextureHolder.h"
#include "Pickup.h"
#include <sstream>

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
    Sprite spritemouse;
    Texture textureCrossHair = TextureHolder::GetTexture("graphics/crosshair.png");

    resolution.x = VideoMode().getDesktopMode().width;
    resolution.y = VideoMode().getDesktopMode().height;
    RenderWindow window(VideoMode(resolution.x, resolution.y), "zombie attack", Style::Fullscreen);
    window.setMouseCursorVisible(false);
    spritemouse.setTexture(textureCrossHair);
    spritemouse.setOrigin(25, 25);

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
    // Create a couple of pickups
    Pickup healthPickup(1);
    Pickup ammoPickup(2);

    // About the game
    int score = 0;
    int hiScore = 0;
    //Game texts

	// For the home/game over screen
	Sprite spriteGameOver;
	Texture textureGameOver = TextureHolder::GetTexture("graphics/background.png");
	spriteGameOver.setTexture(textureGameOver);
	spriteGameOver.setPosition(0, 0);

	// Create a view for the HUD
	View hudView(sf::FloatRect(0, 0, resolution.x, resolution.y));

	// Create a sprite for the ammo icon
	Sprite spriteAmmoIcon;
	Texture textureAmmoIcon = TextureHolder::GetTexture("graphics/ammo_icon.png");
	spriteAmmoIcon.setTexture(textureAmmoIcon);
	spriteAmmoIcon.setPosition(20, 980);

	// Load the font
	Font font;
	font.loadFromFile("fonts/zombiecontrol.ttf");

	// Paused
	Text pausedText;
	pausedText.setFont(font);
	pausedText.setCharacterSize(155);
	pausedText.setPosition(400, 400);
	pausedText.setString("Press Enter \nto continue");

	// Game Over
	Text gameOverText;
	gameOverText.setFont(font);
	gameOverText.setCharacterSize(125);
	
	gameOverText.setPosition(250, 850);
	gameOverText.setString("Press Enter to play");

	// Levelling up
	Text levelUpText;
	levelUpText.setFont(font);
	levelUpText.setCharacterSize(80);

	levelUpText.setPosition(150, 250);
	std::stringstream levelUpStream;
	levelUpStream <<
		"1- Increased rate of fire" <<
		"\n2- Increased clip size(next reload)" <<
		"\n3- Increased max health" <<
		"\n4- Increased run speed" <<
		"\n5- More and better health pickups" <<
		"\n6- More and better ammo pickups";
	levelUpText.setString(levelUpStream.str());

	// Ammo
	Text ammoText;
	ammoText.setFont(font);
	ammoText.setCharacterSize(55);

	ammoText.setPosition(200, 980);

	// Score
	Text scoreText;
	scoreText.setFont(font);
	scoreText.setCharacterSize(55);
	
	scoreText.setPosition(20, 0);

	// Hi Score
	Text hiScoreText;
	hiScoreText.setFont(font);
	hiScoreText.setCharacterSize(55);
	
	hiScoreText.setPosition(1400, 0);
	std::stringstream s;
	s << "Hi Score:" << hiScore;
	hiScoreText.setString(s.str());

	// Zombies remaining
	Text zombiesRemainingText;
	zombiesRemainingText.setFont(font);
	zombiesRemainingText.setCharacterSize(55);

	zombiesRemainingText.setPosition(1500, 980);
	zombiesRemainingText.setString("Zombies: 100");

	// Wave number
	int wave = 0;
	Text waveNumberText;
	waveNumberText.setFont(font);
	waveNumberText.setCharacterSize(55);
	
	waveNumberText.setPosition(1250, 980);
	waveNumberText.setString("Wave: 0");

	// Health bar
	RectangleShape healthBar;
	healthBar.setPosition(450, 980);
		
	// When did we last update the HUD?
	int framesSinceLastHUDUpdate = 0;

	// How often (in frames) should we update the HUD
	int fpsMeasurementFrameInterval = 500;
    //game text end
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

                if (gameTimeTotal.asMilliseconds() - lastPressed.asMilliseconds() > 300 && bulletsInClip > 0)
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
                arena.width = 1920;
                arena.height = 1080;
                arena.left = 0;
                arena.top = 0;

                int tileSize = createBackground(background, arena);

                // Spawn the player in the middle of the arena
                player.spawn(arena, resolution, tileSize);
                numZombies = 10;
                delete[] zombies;
                zombies = createHorde(numZombies, arena);
                numZombiesAlive = numZombies;
                healthPickup.setArena(arena);
                ammoPickup.setArena(arena);
                // Reset the clock so there isn't a frame jump
                clock.restart();
            }
        } // End levelling up
          //update
        if (state == State::PLAYING)
        {
            // Update the delta time
            Time dt = clock.restart();
            // Update the total game time
            gameTimeTotal += dt;
            // Make a decimal fraction of 1 from the delta time
            float dtAsSeconds = dt.asSeconds();

            // Update the player
            player.update(dtAsSeconds, Mouse::getPosition());
            // Where is the mouse pointer

            mouseScreenPosition = Mouse::getPosition();

            // Convert mouse position to world coordinates of mainView
            mouseWorldPosition = window.mapPixelToCoords(
                Mouse::getPosition(), mainView);
            spritemouse.setPosition(mouseWorldPosition);


            // Make a note of the players new position
            Vector2f playerPosition(player.getCenter());

            // Make the view centre around the player
            mainView.setCenter(1920/2,1080/2);
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

            // Update the pickups
            healthPickup.update(dtAsSeconds);
            ammoPickup.update(dtAsSeconds);
            for (int i = 0; i < 100; i++)
            {
                for (int j = 0; j < numZombies; j++)
                {
                    if (bullets[i].isInFlight() &&
                        zombies[j].isAlive())
                    {
                        if (bullets[i].getPosition().intersects(zombies[j].getPosition()))
                        {
                            // Stop the bullet
                            bullets[i].stop();

                            // Register the hit and see if it was a kill
                            if (zombies[j].hit())
                            {
                                // Not just a hit but a kill too
                                score += 10;
                                if (score >= hiScore)
                                {
                                    hiScore = score;
                                }

                                numZombiesAlive--;

                                // When all the zombies are dead (again)
                                if (numZombiesAlive == 0)
                                {
                                    state = State::LEVELING_UP;
                                }
                            }
                        }
                    }
                }
            } // End zombie being shot

            //Have any zombies touched the player
            for (int i = 0; i < numZombies; i++)
            {
                if (player.getPosition().intersects(zombies[i].getPosition()) && zombies[i].isAlive())
                {

                    if (player.hit(gameTimeTotal))
                    {
                        // More here later
                    }

                    if (player.getHealth() <= 0)
                    {
                        state = State::GAME_OVER;
                    }
                }
            } // End player touched

            // Has the player touched health pickup
            if (player.getPosition().intersects(healthPickup.getPosition()) && healthPickup.isSpawned())
            {
                player.increaseHealthLevel(healthPickup.gotIt());
            }

            // Has the player touched ammo pickup
            if (player.getPosition().intersects(ammoPickup.getPosition()) && ammoPickup.isSpawned())
            {
                bulletsSpare += ammoPickup.gotIt();
            }

            	healthBar.setSize(Vector2f(player.getHealth() * 3, 70));

			// Increment the number of frames since the last HUD calculation
			framesSinceLastHUDUpdate++;
			// Calculate FPS every fpsMeasurementFrameInterval frames
			if (framesSinceLastHUDUpdate > fpsMeasurementFrameInterval)
			{

				// Update game HUD text
				std::stringstream ssAmmo;
				std::stringstream ssScore;
				std::stringstream ssHiScore;
				std::stringstream ssWave;
				std::stringstream ssZombiesAlive;

				// Update the ammo text
				ssAmmo << bulletsInClip << "/" << bulletsSpare;
				ammoText.setString(ssAmmo.str());

				// Update the score text
				ssScore << "Score:" << score;
				scoreText.setString(ssScore.str());

				// Update the high score text
				ssHiScore << "Hi Score:" << hiScore;
				hiScoreText.setString(ssHiScore.str());

				// Update the wave
				ssWave << "Wave:" << wave;
				waveNumberText.setString(ssWave.str());

				// Update the high score text
				ssZombiesAlive << "Zombies:" << numZombiesAlive;
				zombiesRemainingText.setString(ssZombiesAlive.str());

				framesSinceLastHUDUpdate = 0;
			}// End HUD update

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
            if (ammoPickup.isSpawned())
            {
                window.draw(ammoPickup.getSprite());
            }
            if (healthPickup.isSpawned())
            {
                window.draw(healthPickup.getSprite());
            }
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
            // Draw the pick-ups, if currently spawned

            // Draw the player
            window.draw(player.getSprite());
            window.draw(spritemouse);
   		// Switch to the HUD view
			// window.setView(hudView);

			// Draw all the HUD elements
			window.draw(spriteAmmoIcon);
			window.draw(ammoText);
			window.draw(scoreText);
			window.draw(hiScoreText);
			window.draw(healthBar);
			window.draw(waveNumberText);
			window.draw(zombiesRemainingText);
		}

		if (state == State::LEVELING_UP)
		{
			window.draw(spriteGameOver);
			window.draw(levelUpText);
		}

		if (state == State::PAUSED)
		{
			window.draw(pausedText);
		}

		if (state == State::GAME_OVER)
		{
			window.draw(spriteGameOver);
			window.draw(gameOverText);
			window.draw(scoreText);
			window.draw(hiScoreText);
		}


        window.display();
    }
    delete[] zombies;
    return 0;
}
