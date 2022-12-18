#pragma once

#include "TinyXML/tinyxml.h"
#include "sdl.h"
#include <vector>

class Animation {
public:
	std::vector<SDL_Rect> frames, frames_flip;
	float currentFrame, speed;
	bool loop, flip, isPlaying;
	Sprite sprite; //in level.h

	Animation() {
		currentFrame = 0;
		isPlaying = true;
		flip = false;
		loop = true;
	}

	void tick(float time) {
		if (!isPlaying) return;
		currentFrame += speed * time;

		if (currentFrame > frames.size())
		{
			currentFrame -= frames.size();
			if (!loop)
			{
				isPlaying = false;
				return;
			}
		}

		int i = currentFrame;
		sprite.rect = frames[i];
		if (flip) sprite.rect = frames_flip[i];
	}
};

class AnimationManager {
public:
	std::string currentAnim;
	std::map<std::string, Animation> animList;

	AnimationManager() {}
	~AnimationManager() { animList.clear(); }

	void create(std::string name, SDL_Texture* &texture, int x, int y,
		int w, int h, int count, float speed, int step = 0, bool Loop = true) {

		Animation a;
		a.speed = speed;
		a.loop = Loop;
		a.sprite.texture = texture;

		for (int i = 0; i < count; i++) {
			a.frames.push_back({ x + i * step, y, w, h });
			a.frames_flip.push_back({ x + i * step + w, y, -w, h });
		}
		animList[name] = a;
		currentAnim = name;
	}

	// parsing from xml archive
	void loadFromXML(std::string fileName, SDL_Texture* &t)
	{
		TiXmlDocument animFile(fileName.c_str());

		animFile.LoadFile();

		TiXmlElement *head;
		head = animFile.FirstChildElement("sprites");

		TiXmlElement *animElement;
		animElement = head->FirstChildElement("animation");
		while (animElement)
		{
			Animation anim;
			currentAnim = animElement->Attribute("title");
			int delay = atoi(animElement->Attribute("delay"));
			anim.speed = 1.0 / delay; anim.sprite.texture = t;

			TiXmlElement *cut;
			cut = animElement->FirstChildElement("cut");
			while (cut)
			{
				int x = atoi(cut->Attribute("x"));
				int y = atoi(cut->Attribute("y"));
				int w = atoi(cut->Attribute("w"));
				int h = atoi(cut->Attribute("h"));

				anim.frames.push_back({x, y, w, h});
				anim.frames_flip.push_back({ x + w, y, -w, h });
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

	void draw(SDL_Renderer *window, int x = 0, int y = 0)
	{
		animList[currentAnim].sprite.pos = { x,y };

		SDL_SetTextureAlphaMod(animList[currentAnim].sprite.texture, 255);
		SDL_Rect dest = { animList[currentAnim].sprite.pos.x, animList[currentAnim].sprite.pos.y, animList[currentAnim].sprite.rect.w, animList[currentAnim].sprite.rect.h };
		SDL_RenderCopyEx(window, animList[currentAnim].sprite.texture, &animList[currentAnim].sprite.rect, &dest, 0.0, 0, SDL_FLIP_NONE);
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