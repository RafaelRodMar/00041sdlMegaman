#pragma once

#include <vector>
#include "Animation.h"
#include "level.h"
#include "sdl.h"

class Entity
{
public:
	float x, y, dx, dy, w, h;
	AnimationManager anim;
	std::vector<Object> obj;
	bool life, dir;
	float timer, timer_end;
	std::string Name;
	int Health;

	Entity(AnimationManager &A, int X, int Y)
	{
		anim = A;
		x = X;
		y = Y;
		dir = 0;

		life = true;
		timer = 0;
		timer_end = 0;
		dx = dy = 0;
	}

	virtual void update(float time) = 0;

	void draw(SDL_Renderer* window)
	{
		anim.draw(window, x, y);
	}

	SDL_FRect getRect()
	{
		return { x, y, w, h };
	}

	void option(std::string NAME, float SPEED = 0, int HEALTH = 10, std::string FIRST_ANIM = "")
	{
		Name = NAME;
		if (FIRST_ANIM != "") anim.set(FIRST_ANIM);
		w = anim.getW();
		h = anim.getH();
		dx = SPEED;
		Health = HEALTH;
	}

};
