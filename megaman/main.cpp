#include<SDL.h>
#include<SDL_image.h>
#include<iostream>
#include<vector>
#include<list>
#include<string>
#include<time.h>

SDL_Window* g_pWindow = 0;
SDL_Renderer* g_pRenderer = 0;

SDL_Point getTextureSize(SDL_Texture *texture) {
	SDL_Point size;
	SDL_QueryTexture(texture, NULL, NULL, &size.x, &size.y);
	return size;
}

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

#include "level.h"
#include "animation.h"
#include "player.h"

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
		g_pWindow = SDL_CreateWindow("Megaman platformer", 100, 100,
			450, 280, flags);
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

	//load the level
	Level lvl;
	lvl.LoadFromFile("files/level1.tmx");

	//load player animations
	SDL_Texture* megaman_t;
	AnimationManager anim;
	anim.loadFromXML("files/anim_megaman.xml", megaman_t);
	anim.animList["jump"].loop = 0;

	//create player
	Object pl = lvl.GetObject("player");
	PLAYER Mario(anim, lvl, pl.rect.x, pl.rect.y);

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
				break;

			case SDL_KEYUP:
				m_keystates = SDL_GetKeyboardState(0);
				break;

			default:
				break;
			}
		}

		if (isKeyDown(SDL_SCANCODE_ESCAPE)) isRunning = false;
		if (isKeyDown(SDL_SCANCODE_LEFT)) lvl.offsetX++;
		if (isKeyDown(SDL_SCANCODE_RIGHT)) lvl.offsetX--;
		if (isKeyDown(SDL_SCANCODE_UP)) lvl.offsetY++;
		if (isKeyDown(SDL_SCANCODE_DOWN)) lvl.offsetY--;

		//update
		Mario.update(25);

		//draw
		SDL_SetRenderDrawColor(g_pRenderer, 107, 140, 255, 255);
		SDL_RenderClear(g_pRenderer);

		lvl.Draw(g_pRenderer);

		Mario.draw(g_pRenderer);

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

	SDL_Quit();
	return 0;
}