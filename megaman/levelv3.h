#pragma once

#include <iostream>
#include <vector>
#include <map>
#include "TinyXML/tinyxml.h"

struct Tile {
	SDL_Texture* texture;
	SDL_Point pos;
	SDL_Rect rect;
};

struct Object
{
	int GetPropertyInt(std::string name);
	float GetPropertyFloat(std::string name);
	std::string GetPropertyString(std::string name);

	std::string name;
	std::string type;
	SDL_FRect rect;
	std::map<std::string, std::string> properties;

	Tile sprite;

	SDL_Rect getRect() {
		SDL_Rect r = { rect.x, rect.y, rect.w, rect.h };
		return r;
	}
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
	SDL_Texture* tilesetImage;
	SDL_Point GetTextureSize(SDL_Texture* t);
	std::vector<Layer> layers;
	int width, height, tileWidth, tileHeight;
	int firstTileID;

	bool loadFromFile(std::string filename);

	Level() {}
	~Level() {}

	void draw() {
		SDL_Rect destTile;
		for (int layer = 0; layer < layers.size(); layer++) {
			for (int cell = 0; cell < layers[layer].tiles.size(); cell++) {
				destTile = { layers[layer].tiles[cell].pos.x - (int)offsetX, (int)layers[layer].tiles[cell].pos.y - (int)offsetY, tileWidth, tileHeight };
				SDL_SetTextureAlphaMod(tilesetImage, 255);
				SDL_RenderCopyEx(g_pRenderer, tilesetImage, &layers[layer].tiles[cell].rect, &destTile, 0.0, 0, SDL_FLIP_NONE);
			}
		}
	}
};

SDL_Point Level::GetTextureSize(SDL_Texture* t)
{
	SDL_Point size;
	SDL_QueryTexture(t, NULL, NULL, &size.x, &size.y);
	return size;
}

bool Level::loadFromFile(std::string filename)
{
	TiXmlDocument levelFile(filename.c_str());

	// try to open xml document
	if (!levelFile.LoadFile())
	{
		std::cout << "Loading level \"" << filename << "\" failed." << std::endl;
		return false;
	}

	// parse the "map" tag
	TiXmlElement *map;
	map = levelFile.FirstChildElement("map");

	// <map version="1.0" orientation="orthogonal"
	// width="10" height="10" tilewidth="34" tileheight="34">
	width = atoi(map->Attribute("width"));
	height = atoi(map->Attribute("height"));
	tileWidth = atoi(map->Attribute("tilewidth"));
	tileHeight = atoi(map->Attribute("tileheight"));

	// parse the "tileset" tag found into the "map" tag
	TiXmlElement *tilesetElement;
	tilesetElement = map->FirstChildElement("tileset");
	firstTileID = atoi(tilesetElement->Attribute("firstgid"));

	// parse source image for tileset
	TiXmlElement *image;
	image = tilesetElement->FirstChildElement("image");
	std::string imagepath = image->Attribute("source");

	// load the image
	SDL_Surface* img = IMG_Load(imagepath.c_str());

	if (img == 0)
	{
		std::cout << "Failed to load tile sheet." << std::endl;
		std::cout << SDL_GetError() << " " << imagepath << std::endl;
		return false;
	}

	tilesetImage = SDL_CreateTextureFromSurface(g_pRenderer, img);
	SDL_FreeSurface(img);

	if (tilesetImage == 0)
	{
		std::cout << "error creating texture of file" << imagepath << std::endl;
		return false;
	}

	SDL_Point size = GetTextureSize(tilesetImage);

	// get number of rows and columns of the texture by tile size
	int columns = size.x / tileWidth;
	int rows = size.y / tileHeight;

	// get the rect for all the tiles (TextureRect)
	std::vector<SDL_Rect> subRects;

	for (int y = 0; y < rows; y++)
		for (int x = 0; x < columns; x++)
		{
			SDL_Rect rect;

			rect.y = y * tileHeight;
			rect.h = tileHeight;
			rect.x = x * tileWidth;
			rect.w = tileWidth;

			subRects.push_back(rect);
		}

	// parse the layers
	TiXmlElement *layerElement;
	layerElement = map->FirstChildElement("layer");
	while (layerElement)
	{
		//new layer
		Layer layer;

		// set the opacity of the layers
		if (layerElement->Attribute("opacity") != NULL)
		{
			float opacity = strtod(layerElement->Attribute("opacity"), NULL);
			layer.opacity = 255 * opacity;
		}
		else
		{
			layer.opacity = 255;
		}

		// process data of the layer
		TiXmlElement *layerDataElement;
		layerDataElement = layerElement->FirstChildElement("data");

		if (layerDataElement == NULL)
		{
			std::cout << "Bad map. No layer information found." << std::endl;
		}

		// tile data
		TiXmlElement *tileElement;
		tileElement = layerDataElement->FirstChildElement("tile");

		if (tileElement == NULL)
		{
			std::cout << "Bad map. No tile information found." << std::endl;
			return false;
		}

		int x = 0;
		int y = 0;

		while (tileElement)
		{
			int tileGID = atoi(tileElement->Attribute("gid"));
			int subRectToUse = tileGID - firstTileID;

			// set the subrect of the texture for every tile
			if (subRectToUse >= 0)
			{
				Tile sprite;
				sprite.texture = tilesetImage;
				sprite.rect = subRects[subRectToUse];
				sprite.pos = { x * tileWidth, y * tileHeight };

				layer.tiles.push_back(sprite);
			}

			tileElement = tileElement->NextSiblingElement("tile");

			x++;
			if (x >= width)
			{
				x = 0;
				y++;
				if (y >= height)
					y = 0;
			}
		}

		layers.push_back(layer);

		layerElement = layerElement->NextSiblingElement("layer");
	}

	return true;
}
