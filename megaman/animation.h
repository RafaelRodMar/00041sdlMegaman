#pragma once

#include "TinyXML/tinyxml.h"
#include <vector>
#include "sdl.h"

class Animation
{
public:
	std::vector<SDL_Rect> frames;
	float currentFrame, speed;
	bool loop, flip, isPlaying;
	Tile sprite;

	Animation()
	{
		currentFrame = 0;
		isPlaying = true;
		flip = false;
		loop = true;
	}

	void tick(float time)
	{
		if (!isPlaying) return;

		currentFrame += speed * time;

		if (currentFrame > frames.size()) {
			currentFrame -= frames.size();
			if (!loop) { isPlaying = false; return; }
		}

		int i = currentFrame;
		sprite.rect = { frames[i].x, frames[i].y, frames[i].w, frames[i].h };
	}

};



class AnimationManager
{

public:
	std::string currentAnim;
	std::map<std::string, Animation> animList;

	AnimationManager()
	{}

	~AnimationManager()
	{
		animList.clear();
	}

	//create the animation
	void create(std::string name, SDL_Texture* texture, int x, int y, int w, int h, int count, float speed, int step = 0, bool Loop = true)
	{
		Animation a;
		a.speed = speed;
		a.loop = Loop;
		a.sprite.texture = texture;

		for (int i = 0; i < count; i++)
		{
			a.frames.push_back({ (x + i * step), y, w, h });
		}
		animList[name] = a;
		currentAnim = name;
	}

	//parse the archive XML
	void loadFromXML(std::string fileName, SDL_Texture* &t)
	{
		std::cout << "Anim file: " << fileName << std::endl;
		TiXmlDocument animFile(fileName.c_str());

		animFile.LoadFile();

		TiXmlElement *head;
		head = animFile.FirstChildElement("sprites");
		std::cout << "Sprites found" << std::endl;

		TiXmlElement *animElement;
		animElement = head->FirstChildElement("animation");
		std::cout << "Animation found" << std::endl;
		while (animElement)
		{
			Animation anim;
			currentAnim = animElement->Attribute("title");
			int delay = atoi(animElement->Attribute("delay"));
			anim.speed = 1.0 / delay;
			anim.sprite.texture = t;
			std::cout << currentAnim << std::endl;

			TiXmlElement *cut;
			cut = animElement->FirstChildElement("cut");
			while (cut)
			{
				int x = atoi(cut->Attribute("x"));
				int y = atoi(cut->Attribute("y"));
				int w = atoi(cut->Attribute("w"));
				int h = atoi(cut->Attribute("h"));

				anim.frames.push_back({ x, y, w, h });
				std::cout << "Frame: " << x << "," << y << " // " << w << "," << h << std::endl;
				cut = cut->NextSiblingElement("cut");
			}

			animList[currentAnim] = anim;
			animElement = animElement->NextSiblingElement("animation");
		}
	}

	void set(std::string name)
	{
		currentAnim = name;
		animList[currentAnim].flip = 0;
	}

	void draw(SDL_Renderer* &window, int x = 0, int y = 0)
	{
		animList[currentAnim].sprite.pos = { x,y };
		Tile sp = animList[currentAnim].sprite;
		SDL_SetTextureAlphaMod(sp.texture, 255);
		SDL_Rect dest = { sp.pos.x, sp.pos.y, sp.rect.w, sp.rect.h };
		SDL_RenderCopyEx(window, sp.texture, &sp.rect, &dest, 0.0, 0, animList[currentAnim].flip == false ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL);
	}

	void flip(bool b = 1) { animList[currentAnim].flip = b; }

	void tick(float time) { animList[currentAnim].tick(time); }

	void pause() { animList[currentAnim].isPlaying = false; }

	void play() { animList[currentAnim].isPlaying = true; }

	void play(std::string name) { animList[name].isPlaying = true; }

	bool isPlaying() { return animList[currentAnim].isPlaying; }

	float getH() { return animList[currentAnim].frames[0].h; }

	float getW() { return animList[currentAnim].frames[0].w; }

};

