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
	std::vector<Layer> layers;
	int width, height, tileWidth, tileHeight;

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

	Level() {
		loadFromString();
	}
	~Level() {}

	//load map from string Tilemap
	bool loadFromString() {
		width = levelWidth;
		height = levelHeight;
		tileWidth = tileHeight = 16;
		Layer layer;

		for (int i = 0; i < height; i++) {
			for (int j = 0; j < width; j++) {
				if ((Tilemap[i][j] == ' ') || (Tilemap[i][j] == '0')) continue;
				Tile cell;
				cell.texture = tile;
				if (Tilemap[i][j] == 'P')  cell.rect = { 143 - 16 * 3, 112, 16, 16 };
				if (Tilemap[i][j] == 'k')  cell.rect = { 143, 112, 16, 16 };
				if (Tilemap[i][j] == 'c')  cell.rect = { 143 - 16, 112, 16, 16 };
				if (Tilemap[i][j] == 't')  cell.rect = { 0, 47, 32, 95 - 47 };
				if (Tilemap[i][j] == 'g')  cell.rect = { 0, 16 * 9 - 5, 3 * 16, 16 * 2 + 5 };
				if (Tilemap[i][j] == 'G')  cell.rect = { 145, 222, 222 - 145, 255 - 222 };
				if (Tilemap[i][j] == 'd')  cell.rect = { 0, 106, 74, 127 - 106 };
				if (Tilemap[i][j] == 'w')  cell.rect = { 99, 224, 140 - 99, 255 - 224 };
				if (Tilemap[i][j] == 'r')  cell.rect = { 143 - 32, 112, 16, 16 };
				cell.pos = { j * tileWidth , i * tileHeight };
				layer.tiles.push_back(cell);
			}
		}

		layers.push_back(layer);
		return true;
	}

	void setTileset(std::string tileset) {
		tile = loadTexture(tileset, g_pRenderer);
	}
	
	void draw() {
		SDL_Rect destTile;
		for (int layer = 0; layer < layers.size(); layer++) {
			for (int cell = 0; cell < layers[layer].tiles.size(); cell++) {
				destTile = { layers[layer].tiles[cell].pos.x - (int)offsetX, (int)layers[layer].tiles[cell].pos.y - (int)offsetY, tileWidth, tileHeight };
				SDL_SetTextureAlphaMod(tile, 255);
				SDL_RenderCopyEx(g_pRenderer, tile, &layers[layer].tiles[cell].rect, &destTile, 0.0, 0, SDL_FLIP_NONE);
			}
		}
	}
};