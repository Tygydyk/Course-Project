#include <SFML/Graphics.hpp>
#include "map.h"
#include "view.h"
#include <iostream>
#include <sstream>
#include <list>

using namespace sf;

class Entity {
public:
	float x, y, dx, dy, speed;
	int width, hight, health;
	bool life, isMove;
	Texture texture;
	Sprite sprite;
	String name; // каждый объект будет иметь своё имя
	Entity(Image & image, float X, float Y, int Width, int Hight, String Name) {
		x = X; y = Y; width = Width; hight = Hight; 
		speed = 0; health = 100; dx = 0; dy = 0;
		life = true; isMove = false;
		texture.loadFromImage(image);
		sprite.setTexture(texture);
		sprite.setOrigin(width / 2, hight / 2);
	}

	virtual void update(float time) = 0;
};



class Player : public Entity {
public:
	enum { left, right, up, down, stay } state;
	int playerScore;
	Player(Image & image, float X, float Y, int Width, int Hight, String Name) : Entity(image, X, Y, Width, Hight, Name) {
		playerScore = 0;
		state = stay;
		sprite.setTextureRect(IntRect(0, 310, width, hight));
	}

	void control() {
		if (Keyboard::isKeyPressed(Keyboard::Left)) {
			state = left;
			speed = 0.1;
		}

		if (Keyboard::isKeyPressed(Keyboard::Right)) {
			state = right;
			speed = 0.1;
		}

		if (Keyboard::isKeyPressed(Keyboard::Up)) {
			state = up;
			speed = 0.1;
		}

		if (Keyboard::isKeyPressed(Keyboard::Down)) {
			state = down;
			speed = 0.1;
		}
	}



	void update(float time)
	{
		control(); //функция управления персонажем
		switch (state) //тут делаются различные действия в зависимости от состояния
		{
		case right:dx = speed; break; //состояние идти вправо
		case left:dx = -speed; break; //состояние идти влево
		case up: dy = -speed; break; //будет состояние поднятия наверх (например по лестнице)
		case down: dy = speed; break; //будет состояние во время спуска персонажа (например по лестнице)
		case stay: break; //и здесь тоже		
		}
		x += dx * time;
		checkCollisionWithMap(dx, 0);//обрабатываем столкновение по Х
		y += dy * time;
		checkCollisionWithMap(0, dy);//обрабатываем столкновение по Y
		sprite.setPosition(x + width / 2, y + hight / 2); //задаем позицию спрайта в место его центра
		if (health <= 0) { life = false; }
		if (!isMove) { speed = 0; }
		//if (!onGround) { dy = dy + 0.0015*time; }//убираем и будем всегда притягивать к земле
		if (life) { getPlayerCoordinateforView(x, y); }
		//dy = dy + 0.0015*time;//постоянно притягиваемся к земле
	}



	void checkCollisionWithMap(float Dx, float Dy) //ф ция для столкновений с картой
	{
		for (int i = y / 60; i < (y + hight) / 60; i++) //проходимся по элементам карты
			for (int j = x / 60; j<(x + width) / 60; j++)
			{
				if (TileMap[i][j] == '0') //если элемент наш тайл земли
				{
					if (Dy>0) { y = i * 60 - hight;  dy = 0; } //по Y вниз=>идем в пол
					if (Dy<0) { y = i * 60 + 60;  dy = 0; } //столкновение с верхними краями карты
					if (Dx>0) { x = j * 60 - width; } //с правым краем карты
					if (Dx<0) { x = j * 60 + 60; } // с левым краем карты
				}
			}
	}



	float getplayercoordinateX() {
		return x;
	}
	float getplayercoordinateY() {
		return y;
	}



	void interactionWithMap()
	{

		for (int i = y / 60; i < (y + hight) / 60; i++)
			for (int j = x / 60; j<(x + width) / 60; j++)
			{
				if (TileMap[i][j] == '0')
				{
					if (dy>0)
					{
						y = i * 60 - hight;
					}
					if (dy<0)
					{
						y = i * 60 + 60;
					}
					if (dx>0)
					{
						x = j * 60 - width;
					}
					if (dx < 0)
					{
						x = j * 60 + 60;
					}
				}
			}
	}

};



class Enemy : public Entity {
public:
	Enemy(Image & image, float X, float Y, int Width, int Hight, String Name) : Entity(image, X, Y, Width, Hight, Name) {
		if (name == "EasyEnemy") {
			dx = 0.1;
			sprite.setTextureRect(IntRect(0, 185, width, hight));
		}
	}


	void checkCollisionWithMap(float Dx, float Dy) //ф ция для столкновений с картой
	{
		for (int i = y / 60; i < (y + hight) / 60; i++) 
			for (int j = x / 60; j<(x + width) / 60; j++)
			{
				if (TileMap[i][j] == '0') 
				{
					if (Dy>0) { y = i * 60 - hight;  dy = 0; } 
					if (Dy<0) { y = i * 60 + 60;  dy = 0; } 
					if (Dx>0) { x = j * 60 - width; } 
					if (Dx<0) { x = j * 60 + 60; } 
				}
			}
	}

	void update(float time)
	{
		if (name == "EasyEnemy") 
		{
			checkCollisionWithMap(dx, 0); //обрабатываем столкновение по Х
			x += dx * time;
			sprite.setPosition(x + width / 2, y + hight / 2); //задаем позицию спрайта в место его центра
			if (health <= 0) { life = false; }
		}
	}

};



int main()
{
	//randomMapGenerate();

	RenderWindow window(VideoMode(1400, 800), "SFML");
	view.reset(FloatRect(0, 0, 1400, 800));

	Font font;
	font.loadFromFile("CyrilicOld.ttf");
	Text text("", font, 20);
	text.setFillColor(Color::Red);
	text.setStyle(Text::Bold);

	Image map_image;
	map_image.loadFromFile("images/new_map.png");
	Texture map;
	map.loadFromImage(map_image);
	Sprite s_map;
	s_map.setTexture(map);


	std::list<Entity*> entities;
	std::list<Entity*>::iterator it;


	Image myTank;
	myTank.loadFromFile("images/tank.png");

	Player p(myTank, 680, 350, 60, 58, "Grisha");

	Image easyEnemyImage;
	easyEnemyImage.loadFromFile("images/tank.png");

	/////////////////////////////Рисуем карту/////////////////////
	for (int i = 0; i < HEIGHT_MAP; i++)
		for (int j = 0; j < WIDTH_MAP; j++)
		{
			if (TileMap[i][j] == ' ')  s_map.setTextureRect(IntRect(0, 0, 60, 60));
			if ((TileMap[i][j] == '0')) s_map.setTextureRect(IntRect(64, 0, 60, 60));
			s_map.setPosition(j * 60, i * 60);

			window.draw(s_map);
		}
	
	int randomElementX = 0;//случайный элемент по горизонтали
	int randomElementY = 0;//случ эл-т по вертикали

		int countEnemy = 3;//количество камней 1

		while (countEnemy > 0) {
			randomElementX = 1 + rand() % (WIDTH_MAP - 1); //рандомное по иксу от 1 до ширина карты-1, чтобы не получать числа бордюра карты
			randomElementY = 1 + rand() % (HEIGHT_MAP - 1); //по игреку так же
			if (TileMap[randomElementY][randomElementX] == ' ') {
				srand(time(0)); //если встретили символ пробел, 
				countEnemy--; //создали врага=>счетчик будет "текущий минус 1"
				entities.push_back(new Enemy(easyEnemyImage, 1 + rand() % (WIDTH_MAP - 1), 1 + rand() % (HEIGHT_MAP - 1), 60, 58, "EasyEnemy"));
			}
		}

	float currentFrame = 0;
	Clock clock;
	Clock gameTimeClock;//переменная игрового времени, будем здесь хранить время игры 
	int gameTime = 0;//объявили игровое время, инициализировали.
	while (window.isOpen())
	{

		float time = clock.getElapsedTime().asMicroseconds();

		if (p.life) gameTime = gameTimeClock.getElapsedTime().asSeconds(); else { view.move(0.8, 0); }//игровое время в секундах идёт вперед, пока жив игрок, перезагружать как time его не надо. оно не обновляет логику игры
		clock.restart();
		time = time / 800;


		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}

		p.update(time);
		//for (it = entities.begin(); it != entities.end(); it++) { (*it)->update(time); }
		window.setView(view);
		window.clear();


		
		std::ostringstream playerHealthString, gameTimeString;    // объявили переменную здоровья и времени
		playerHealthString << p.health; gameTimeString << gameTime;		//формируем строку
		text.setString("Здоровье: " + playerHealthString.str() + "\nВремя игры: " + gameTimeString.str());//задаем строку тексту и вызываем сформированную выше строку методом .str()

		text.setPosition(view.getCenter().x + 365, view.getCenter().y - 400);//задаем позицию текста, отступая от центра камеры
		window.draw(text);//рисую этот текст

		window.draw(p.sprite);
		for (it = entities.begin(); it != entities.end(); it++) {
			window.draw((*it)->sprite); //рисуем entities объекты (сейчас это только враги)
		}

		window.display();
	}

	return 0;
}
