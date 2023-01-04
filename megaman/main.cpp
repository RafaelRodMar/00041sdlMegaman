#include<SDL.h>
#include<SDL_image.h>
#include<iostream>
#include<string>
#include<list>
#include<time.h>

SDL_Window* g_pWindow = 0;
SDL_Renderer* g_pRenderer = 0;

SDL_Texture* loadTexture(std::string fileName, SDL_Renderer* pRenderer)
{
	SDL_Surface* pTempSurface = IMG_Load(fileName.c_str()); //create Surface with contents of fileName

	if (pTempSurface == 0) //exception handle load error
	{
		std::cout << "error loading file: " << fileName << std::endl;
		std::cout << SDL_GetError() << " " << fileName << std::endl;
		return false;
	}

	SDL_Texture* pTexture = SDL_CreateTextureFromSurface(pRenderer, pTempSurface); //Pass Surface to Texture
	SDL_FreeSurface(pTempSurface); //Delete Surface

	if (pTexture == 0) //exception handle transiction Surfacte to Texture
	{
		std::cout << "error creating Texture of file: " << fileName << std::endl;
		return false;
	}

	return pTexture; //save texture.
}

void drawFrameScl(SDL_Texture* textureMap, int x, int y, int srcWidth, int srcHeight, int destWidth, int destHeight, SDL_Renderer *pRenderer, double angle, int alpha, SDL_RendererFlip flip)
{
	SDL_Rect srcRect; //source rectangle
	SDL_Rect destRect; //destination rectangle

	srcRect.x = srcWidth;
	srcRect.y = srcHeight;
	srcRect.w = srcWidth;
	destRect.w = destWidth;
	srcRect.h = srcHeight;
	destRect.h = destHeight;
	destRect.x = x;
	destRect.y = y;

	SDL_SetTextureAlphaMod(textureMap, alpha);
	SDL_RenderCopyEx(pRenderer, textureMap, &srcRect, &destRect, angle, 0, flip); //Load current frame on the buffer game.
}

SDL_Rect getRectFromFRect(SDL_FRect fr) {
	SDL_Rect r;
	r.x = fr.x; r.y = fr.y;
	r.h = fr.h; r.w = fr.w;
	return r;
}

// keyboard specific
const Uint8* m_keystates;

bool isKeyDown(SDL_Scancode key)
{
	if (m_keystates != 0)
	{
		if (m_keystates[key] == 1)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	return false;
}

bool isRunning = true;

float offsetX = 0, offsetY = 0;
#include "levelv3.h"
#include "animation.h"
Level lvl;

class Entity {
public:
	float dx, dy;
	SDL_FRect rect; //real position
	Tile sprite;
	bool onGround;
	float currentFrame;
	bool life = true;
	bool flip = false;
	std::vector<Object> obj;

	Entity(std::string name, std::string image, int x, int y) {
		sprite.texture = loadTexture(image, g_pRenderer);

		if (name == "PLAYER")
		{
			rect = { 100,50, 16,16 };
			dx = dy = 0.1;
		}
		else //ENEMY
		{
			rect = { (float)x, (float)y, (float)16, (float)16 };
			dx = 0.05;
		}

		currentFrame = 0;
	}

	virtual void update(float num) = 0;
	virtual void Collision(int num) = 0;

	void draw() {
		SDL_Rect dest = { sprite.pos.x, sprite.pos.y, 16, 16 };
		SDL_SetTextureAlphaMod(sprite.texture, 255);
		SDL_RenderCopyEx(g_pRenderer, sprite.texture, &sprite.rect, &dest, 0.0, 0, flip == false ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL); //Load current frame on the buffer game.
	}
};

class Entity2 {
public:
	std::string Name;
	float dx, dy;
	SDL_FRect rect; //real position
	AnimationManager anim;
	//Tile sprite;
	bool onGround;
	float currentFrame;
	bool life = true;
	bool flip = false;
	float timer, timer_end;
	int Health;
	std::vector<Object> obj;

	Entity2(AnimationManager &A, int X, int Y) {
		anim = A;
		rect.x = X;
		rect.y = Y;
		flip = false;

		life = true;
		timer = timer_end = 0;
		dx = dy = 0;
	}

	virtual void update(float num) = 0;
	virtual void Collision(int num) = 0;

	void option(std::string NAME, float SPEED = 0, int HEALTH = 10, std::string FIRST_ANIM = "")
	{
		Name = NAME;
		if (FIRST_ANIM != "") anim.set(FIRST_ANIM);
		rect.w = anim.getW();
		rect.h = anim.getH();
		dx = SPEED;
		Health = HEALTH;
	}

	void draw(SDL_Renderer* &window) {
		anim.draw(window, rect.x - offsetX, rect.y - offsetY);
	}

	//void draw() {
	//	SDL_Rect dest = { sprite.pos.x, sprite.pos.y, 16, 16 };
	//	SDL_SetTextureAlphaMod(sprite.texture, 255);
	//	SDL_RenderCopyEx(g_pRenderer, sprite.texture, &sprite.rect, &dest, 0.0, 0, flip == false ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL); //Load current frame on the buffer game.
	//}
};

class Player : public Entity {
public:

	Player(std::string name, std::string image, Level &lev, int x, int y) : Entity(name, image, x, y) {
		obj = lev.GetAllObjects();
	};

	void keyboard() {
		if (isKeyDown(SDL_SCANCODE_LEFT)) dx = -0.1;
		if (isKeyDown(SDL_SCANCODE_RIGHT)) dx = 0.1;
		if (isKeyDown(SDL_SCANCODE_UP))
		{
			if (onGround)
			{
				dy = -0.35;
				onGround = false;
			}
		}
	}

	void update(float time)
	{
		keyboard();

		rect.x += dx * time;
		Collision(0);


		if (!onGround) dy = dy + 0.0005*time;
		rect.y += dy * time;
		onGround = false;
		Collision(1);


		currentFrame += time * 0.005;
		if (currentFrame > 3) currentFrame -= 3;

		sprite.rect = { 112 + 31 * int(currentFrame), 144, 16, 16 };
		if (dx > 0) flip = false;
		if (dx < 0) flip = true;
		if (dx == 0) sprite.rect = { 80, 144, 16, 16 };

		sprite.pos.x = rect.x - offsetX;
		sprite.pos.y = rect.y - offsetY;

		dx = 0;
	}

	void Collision(int num)
	{
		for (int i = 0; i < obj.size(); i++)
			if(SDL_HasIntersection(&getRectFromFRect(rect), &getRectFromFRect(obj[i].rect)))
			{
				if (obj[i].name == "solid")
				{
					//std::cout << "choque con solid" << std::endl;
					if (dy > 0 && num == 1) { rect.y = obj[i].rect.y - rect.h;  dy = 0; onGround = true; }
					if (dy < 0 && num == 1) { rect.y = obj[i].rect.y + obj[i].rect.h;   dy = 0; }
					if (dx > 0 && num == 0) { rect.x = obj[i].rect.x - rect.w; }
					if (dx < 0 && num == 0) { rect.x = obj[i].rect.x + obj[i].rect.w; }
				}

				if (obj[i].name == "SlopeLeft")
				{
					SDL_FRect r = obj[i].rect;
					int y0 = (rect.x + rect.w / 2 - r.x) * r.h / r.w + r.y - rect.h;
					if (rect.y > y0)
						if (rect.x + rect.w / 2 > r.x)
						{
							rect.y = y0; dy = 0; onGround = true;
						}
				}

				if (obj[i].name == "SlopeRight")
				{
					//std::cout << "collision with sloperight" << std::endl;
					SDL_FRect r = obj[i].rect;
					int y0 = -(rect.x + rect.w / 2 - r.x) * r.h / r.w + r.y + r.h - rect.h;
					if (rect.y > y0)
						if (rect.x + rect.w / 2 < r.x + r.w)
						{
							rect.y = y0; dy = 0; onGround = true;
						}
				}

			}
	}

};

class Bullet :public Entity2{
public:

	Bullet(AnimationManager &a, Level &lev, int x, int y, bool dir) :Entity2(a, x, y)
	{
		option("Bullet", 0.3, 10, "move");

		if (dir) dx = -0.3;
		obj = lev.GetObjects("solid");
	}

	void update(float time)
	{
		rect.x += dx * time;

		for (int i = 0; i < obj.size(); i++)
			if (SDL_HasIntersection(&getRectFromFRect(rect), &getRectFromFRect(obj[i].rect)))
			{
				Health = 0;
			}

		if (Health <= 0) {
			anim.set("explode"); dx = 0;
			if (anim.isPlaying() == false) life = false;
		}

		anim.tick(time);
	}

	void Collision(int num){}

};

class Player2 : public Entity2 {
public:
	enum { stay, walk, duck, jump, climb, swim } STATE;
	bool onLadder, shoot, hit;

	Player2(AnimationManager &a, Level &lev, int x, int y) : Entity2(a, x, y) {
		option("Player", 0, 100, "stay");
		STATE = stay; hit = false;
		obj = lev.GetAllObjects();
	};

	void keyboard() {

		if (isKeyDown(SDL_SCANCODE_LEFT))
		{
			flip = true;
			if (STATE != duck) dx = -0.1;
			if (STATE == stay) STATE = walk;
		}

		if (isKeyDown(SDL_SCANCODE_RIGHT))
		{
			flip = false;
			if (STATE != duck) dx = 0.1;
			if (STATE == stay) STATE = walk;
		}

		if (isKeyDown(SDL_SCANCODE_UP))
		{
			if (onLadder) STATE = climb;
			if (STATE == stay || STATE == walk) { dy = -0.27; STATE = jump; anim.play("jump"); }
			if (STATE == climb) dy = -0.05;
			if (STATE == climb) if (isKeyDown(SDL_SCANCODE_LEFT) || isKeyDown(SDL_SCANCODE_RIGHT)) STATE = stay;
		}

		if (isKeyDown(SDL_SCANCODE_DOWN))
		{
			if (STATE == stay || STATE == walk) { STATE = duck; dx = 0; }
			if (STATE == climb) dy = 0.05;
		}

		if (isKeyDown(SDL_SCANCODE_SPACE))
		{
			shoot = true;
		}

		//check if no key pressed
		if (!(isKeyDown(SDL_SCANCODE_RIGHT) || isKeyDown(SDL_SCANCODE_LEFT)))
		{
			dx = 0;
			if (STATE == walk) STATE = stay;
		}

		if (!(isKeyDown(SDL_SCANCODE_UP) || isKeyDown(SDL_SCANCODE_DOWN)))
		{
			if (STATE == climb) dy = 0;
		}

		if (!isKeyDown(SDL_SCANCODE_DOWN))
		{
			if (STATE == duck) { STATE = stay; }
		}

		if (!isKeyDown(SDL_SCANCODE_SPACE))
		{
			shoot = false;
		}

		//key["R"] = key["L"] = key["Up"] = key["Down"] = key["Space"] = false;
	}

	void Animation(float time)
	{
		if (STATE == stay) anim.set("stay");
		if (STATE == walk) anim.set("walk");
		if (STATE == jump) anim.set("jump");
		if (STATE == duck) anim.set("duck");
		if (STATE == climb) { anim.set("climb"); anim.pause(); if (dy != 0) anim.play(); }

		if (shoot) {
			anim.set("shoot");
			if (STATE == walk) anim.set("shootAndWalk");
		}

		if (hit) {
			timer += time;
			if (timer > 1000) { hit = false; timer = 0; }
			anim.set("hit");
		}

		if (flip) anim.flip();

		anim.tick(time);
	}

	void update(float time)
	{
		keyboard();

		Animation(time);

		if (STATE == climb) if (!onLadder) STATE = stay;
		if (STATE != climb) dy += 0.0005*time;
		onLadder = false;

		rect.x += dx * time;
		Collision(0);

		rect.y += dy * time;
		Collision(1);
	}

	void Collision(int num)
	{
		for (int i = 0; i < obj.size(); i++)
			if (SDL_HasIntersection(&getRectFromFRect(rect), &getRectFromFRect(obj[i].rect)))
			{
				if (obj[i].name == "solid")
				{
					//std::cout << "choque con solid" << std::endl;
					if (dy > 0 && num == 1) { rect.y = obj[i].rect.y - rect.h;  dy = 0; STATE = stay; }
					if (dy < 0 && num == 1) { rect.y = obj[i].rect.y + obj[i].rect.h;   dy = 0; }
					if (dx > 0 && num == 0) { rect.x = obj[i].rect.x - rect.w; }
					if (dx < 0 && num == 0) { rect.x = obj[i].rect.x + obj[i].rect.w; }
				}

				if (obj[i].name == "SlopeLeft")
				{
					SDL_FRect r = obj[i].rect;
					int y0 = (rect.x + rect.w / 2 - r.x) * r.h / r.w + r.y - rect.h;
					if (rect.y > y0)
						if (rect.x + rect.w / 2 > r.x)
						{
							rect.y = y0; dy = 0; STATE = stay;
						}
				}

				if (obj[i].name == "SlopeRight")
				{
					//std::cout << "collision with sloperight" << std::endl;
					SDL_FRect r = obj[i].rect;
					int y0 = -(rect.x + rect.w / 2 - r.x) * r.h / r.w + r.y + r.h - rect.h;
					if (rect.y > y0)
						if (rect.x + rect.w / 2 < r.x + r.w)
						{
							rect.y = y0; dy = 0; STATE = stay;
						}
				}

			}
	}

};

class Enemy : public Entity {
public:

	Enemy(std::string name, std::string image, int x, int y) : Entity(name, image, x, y) {};

	void update(float time)
	{
		rect.x += dx * time;

		Collision(0);


		currentFrame += time * 0.005;
		if (currentFrame >= 2) currentFrame -= 2;

		sprite.rect = { 18 * int(currentFrame), 0, 16, 16 };
		if (!life) sprite.rect = { 58, 0, 16, 16 };

		sprite.pos.x = rect.x - offsetX;
		sprite.pos.y = rect.y - offsetY;

	}


	void Collision(int num)
	{
	}
};

const int FPS = 60;
const int DELAY_TIME = 1000.0f / FPS;

int main(int argc, char* args[])
{
	// attempt to initialize SDL
	if (SDL_Init(SDL_INIT_EVERYTHING) == 0)
	{
		int flags = 0;

		std::cout << "SDL init success\n";
		// init the window
		g_pWindow = SDL_CreateWindow("Mario platformer", 100, 100,
			400, 250, flags);
		if (g_pWindow != 0) // window init success
		{
			std::cout << "window creation success\n";
			g_pRenderer = SDL_CreateRenderer(g_pWindow, -1, 0);
			if (g_pRenderer != 0) // renderer init success
			{
				std::cout << "renderer creation success\n";
				SDL_SetRenderDrawColor(g_pRenderer,
					255, 255, 255, 255);
			}
			else
			{
				std::cout << "renderer init fail\n";
				return false; // renderer init fail
			}
		}
		else
		{
			std::cout << "window init fail\n";
			return false; // window init fail
		}
	}
	else
	{
		std::cout << "SDL init fail\n";
		return false; // SDL init fail
	}

	//scenery
	lvl.loadFromFile("files/Level1.tmx");

	//load animations
	SDL_Texture* megaman_t = loadTexture("files/images/megaman.png", g_pRenderer);
	AnimationManager anim;
	anim.loadFromXML("files/anim_megaman.xml", megaman_t);
	anim.animList["jump"].loop = 0;

	AnimationManager anim2;
	SDL_Texture* bullet_t = loadTexture("files/images/bullet.png", g_pRenderer);
	anim2.create("move", bullet_t, 7, 10, 8, 8, 1, 0);
	anim2.create("explode", bullet_t, 27, 7, 18, 18, 4, 0.01, 29, false);

	//player and enemy
	Player Mario("PLAYER", "mario_tileset.png", lvl, 0, 0);
	Enemy enemy("ENEMY", "mario_tileset.png", 48 * 16, 13 * 16);

	//player entity2
	Object pl = lvl.GetObject("player");
	pl.rect.x = 100;
	pl.rect.y = 50;
	Player2 Mario2(anim, lvl, pl.rect.x, pl.rect.y);

	//
	std::list<Entity2*> entities;
	std::list<Entity2*>::iterator it;

	srand(time(NULL));

	Uint32 frameStart, frameTime;
	
	while (isRunning)
	{
		frameStart = SDL_GetTicks(); //tiempo inicial

		//input handler
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
			case SDL_QUIT:
				isRunning = false;
				break;

			case SDL_KEYDOWN:
				m_keystates = SDL_GetKeyboardState(0);
				if (isKeyDown(SDL_SCANCODE_SPACE))
				{
					entities.push_back(new Bullet(anim2, lvl, Mario2.rect.x + 18, Mario2.rect.y + 18, Mario2.flip));
				}
				break;

			case SDL_KEYUP:
				m_keystates = SDL_GetKeyboardState(0);
				break;

			default:
				break;
			}
		}

		//update entities
		for (it = entities.begin(); it != entities.end();) {
			Entity2 *b = *it;
			b->update(20);
			if (b->life == false) { it = entities.erase(it); delete b; }
			else it++;
		}

		//update
		Mario.update(20);
		enemy.update(20);
		Mario2.update(20);

		if(SDL_HasIntersection(&getRectFromFRect(Mario.rect), &getRectFromFRect(enemy.rect)))
		{
			if (enemy.life)
			{
				if (Mario.dy > 0) { enemy.dx = 0; Mario.dy = -0.2; enemy.life = false; }
				else SDL_SetTextureColorMod(Mario.sprite.texture, 255, 0, 0);
			}
		}

		//camera focus on Mario
		//if (Mario.rect.x > 200) offsetX = (int)(Mario.rect.x - 200);
		//if (Mario.rect.y > 200) offsetY = (int)(Mario.rect.y - 200);
		//std::cout << Mario.rect.x << "," << Mario.rect.y << std::endl;

		//camera focus on Megaman
		if (Mario2.rect.x > 200) offsetX = (int)(Mario2.rect.x - 200);
		if (Mario2.rect.y > 100) offsetY = (int)(Mario2.rect.y - 100);

		//draw
		SDL_SetRenderDrawColor(g_pRenderer, 107, 140, 255, 255);
		SDL_RenderClear(g_pRenderer);

		//draw background
		lvl.draw();

		//draw entities
		for (it = entities.begin(); it != entities.end(); it++)
			(*it)->draw(g_pRenderer);

		//draw Mario and enemy
		Mario.draw();
		enemy.draw();
		Mario2.draw(g_pRenderer);

		SDL_RenderPresent(g_pRenderer); // draw to the screen

		frameTime = SDL_GetTicks() - frameStart; //tiempo final - tiempo inicial

		if (frameTime < DELAY_TIME)
		{
			//con tiempo fijo el retraso es 1000 / 60 = 16,66
			//procesar handleEvents, update y render tarda 1, y hay que esperar 15
			//cout << "frameTime : " << frameTime << "  delay : " << (int)(DELAY_TIME - frameTime) << endl;
			SDL_Delay((int)(DELAY_TIME - frameTime)); //esperamos hasta completar los 60 fps
		}
	}
	
	std::cout << "game closing...\n";
	SDL_DestroyTexture(Mario.sprite.texture);
	SDL_DestroyTexture(enemy.sprite.texture);
	SDL_DestroyTexture(megaman_t);
	SDL_DestroyTexture(lvl.tilesetImage);

	return 0;
}