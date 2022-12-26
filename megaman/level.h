#pragma once

#include <iostream>
#include <vector>

struct Tile {
	SDL_Texture* texture;
	SDL_Point pos;
	SDL_Rect rect;
};

struct Layer
{
	int opacity;
	std::vector<Tile> tiles;
};

class Level {
public:
	static const int levelHeight = 17;
	static const int levelWidth = 150;
	SDL_Texture* tile;

	std::string Tilemap[levelHeight] = {
	"000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000",
	"0                                                                                                                                                    0",
	"0                                                                                    w                                                               0",
	"0                   w                                  w                   w                                                                         0",
	"0                                      w                                       kk                                                                    0",
	"0                                                                             k  k    k    k                                                         0",
	"0                      c                                                      k      kkk  kkk  w                                                     0",
	"0                                                                       r     k       k    k                                                         0",
	"0                                                                      rr     k  k                                                                   0",
	"0                                                                     rrr      kk                                                                    0",
	"0               c    kckck                                           rrrr                                                                            0",
	"0                                      t0                           rrrrr                                                                            0",
	"0G                                     00              t0          rrrrrr            G                                                               0",
	"0           d    g       d             00              00         rrrrrrr                                                                            0",
	"PPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPP",
	"PPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPP",
	"PPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPP",
	};

	Level() {}
	~Level() {}

	void setTileset(std::string tileset) {
		tile = loadTexture(tileset, g_pRenderer);
	}

	void draw() {
		SDL_Rect srcTile, destTile;
		for (int i = 0; i < levelHeight; i++)
			for (int j = 0; j < levelWidth; j++)
			{
				if (Tilemap[i][j] == 'P')  srcTile = { 143 - 16 * 3, 112, 16, 16 };

				if (Tilemap[i][j] == 'k')  srcTile = { 143, 112, 16, 16 };

				if (Tilemap[i][j] == 'c')  srcTile = { 143 - 16, 112, 16, 16 };

				if (Tilemap[i][j] == 't')  srcTile = { 0, 47, 32, 95 - 47 };

				if (Tilemap[i][j] == 'g')  srcTile = { 0, 16 * 9 - 5, 3 * 16, 16 * 2 + 5 };

				if (Tilemap[i][j] == 'G')  srcTile = { 145, 222, 222 - 145, 255 - 222 };

				if (Tilemap[i][j] == 'd')  srcTile = { 0, 106, 74, 127 - 106 };

				if (Tilemap[i][j] == 'w')  srcTile = { 99, 224, 140 - 99, 255 - 224 };

				if (Tilemap[i][j] == 'r')  srcTile = { 143 - 32, 112, 16, 16 };

				if ((Tilemap[i][j] == ' ') || (Tilemap[i][j] == '0')) continue;

				destTile = { (int)(j * 16 - offsetX), (int)(i * 16 - offsetY), 16, 16 };
				SDL_SetTextureAlphaMod(tile, 255);
				SDL_RenderCopyEx(g_pRenderer, tile, &srcTile, &destTile, 0.0, 0, SDL_FLIP_NONE); //Load current frame on the buffer game.
			}
	}
};