all: main
main: TextureHolder.o CreateHorde.o Zombie.o createBackground.o Player.o Bullet.o Pickup.o main.o 
	g++ --std=c++11 -o main main.o Player.o TextureHolder.o Zombie.o CreateHorde.o Pickup.o createBackground.o Bullet.o  -lsfml-graphics -lsfml-window -lsfml-system

main.o: main.cpp 
	g++ --std=c++11 -c main.cpp
Player.o: Player.cpp
	g++ --std=c++11 -c Player.cpp
createBackground.o: createBackground.cpp
	g++ --std=c++11 -c createBackground.cpp
Zombie.o: Zombie.cpp
	g++ --std=c++11 -c Zombie.cpp 
CreateHorde.o: CreateHorde.cpp
	g++ --std=c++11 -c CreateHorde.cpp
TextureHolder.o: TextureHolder.cpp
	g++ --std=c++11 -c TextureHolder.cpp

Bullet.o: Bullet.cpp
	g++ --std=c++11 -c Bullet.cpp

Pickup.o: Pickup.cpp
	g++ --std=c++11 -c Pickup.cpp
clean:
	rm -rf *o main
	