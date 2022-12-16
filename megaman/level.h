#pragma once

#include "sdl.h"
#include <string>
#include <vector>
#include <map>
#include <iostream>
#include "TinyXML/tinyxml.h"

struct Sprite {
	SDL_Texture* texture;
	SDL_Rect rect;
	SDL_Point pos;
};

struct Object {
	int GetPropertyInt(std::string name);
	float GetPropertyFloat(std::string name);
	std::string GetPropertyString(std::string name);

	std::string name;
	std::string type;
	SDL_FRect rect;
	std::map<std::string, std::string> properties;

	Sprite sprite;
};

struct Layer {
	int opacity;
	std::vector<Sprite> tiles;
};

class Level {
public:
	bool LoadFromFile(std::string filename);
	Object GetObject(std::string name);
	std::vector<Object> GetObjects(std::string name);
	std::vector<Object> GetAllObjects();
	void Draw(SDL_Renderer *window);
	SDL_Point GetTileSize();

private:
	int width, height, tileWidth, tileHeight;
	int firstTileID;
	SDL_FRect drawingBounds;
	SDL_Texture* tilesetImage;
	std::vector<Object> objects;
	std::vector<Layer> layers;
};

///////////////////////////////////////
///////////////////////////////////////
///////////////////////////////////////

int Object::GetPropertyInt(std::string name)
{
	return atoi(properties[name].c_str());
}

float Object::GetPropertyFloat(std::string name)
{
	return strtod(properties[name].c_str(), NULL);
}

std::string Object::GetPropertyString(std::string name)
{
	return properties[name];
}

bool Level::LoadFromFile(std::string filename)
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

	SDL_Point size = getTextureSize(tilesetImage);

	// get number of rows and columns
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
				Sprite sprite;
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

	// parse the objects
	TiXmlElement *objectGroupElement;

	if (map->FirstChildElement("objectgroup") != NULL)
	{
		objectGroupElement = map->FirstChildElement("objectgroup");
		while (objectGroupElement)
		{
			// <object> tag
			TiXmlElement *objectElement;
			objectElement = objectGroupElement->FirstChildElement("object");

			while (objectElement)
			{
				// get the attributes of the objects
				std::string objectType;
				if (objectElement->Attribute("type") != NULL)
				{
					objectType = objectElement->Attribute("type");
				}
				std::string objectName;
				if (objectElement->Attribute("name") != NULL)
				{
					objectName = objectElement->Attribute("name");
				}
				int x = atoi(objectElement->Attribute("x"));
				int y = atoi(objectElement->Attribute("y"));

				int width, height;

				Sprite sprite;
				sprite.texture = tilesetImage;
				sprite.rect = { 0,0,0,0 };
				sprite.pos = { x, y };

				if (objectElement->Attribute("width") != NULL)
				{
					width = atoi(objectElement->Attribute("width"));
					height = atoi(objectElement->Attribute("height"));
				}
				else
				{
					width = subRects[atoi(objectElement->Attribute("gid")) - firstTileID].w;
					height = subRects[atoi(objectElement->Attribute("gid")) - firstTileID].h;
					sprite.rect = subRects[atoi(objectElement->Attribute("gid")) - firstTileID];
				}

				// create the object
				Object object;
				object.name = objectName;
				object.type = objectType;
				object.sprite = sprite;

				SDL_FRect objectRect;
				objectRect.y = y;
				objectRect.x = x;
				objectRect.h = height;
				objectRect.w = width;
				object.rect = objectRect;

				// set properties of the object
				TiXmlElement *properties;
				properties = objectElement->FirstChildElement("properties");
				if (properties != NULL)
				{
					TiXmlElement *prop;
					prop = properties->FirstChildElement("property");
					if (prop != NULL)
					{
						while (prop)
						{
							std::string propertyName = prop->Attribute("name");
							std::string propertyValue = prop->Attribute("value");

							object.properties[propertyName] = propertyValue;

							prop = prop->NextSiblingElement("property");
						}
					}
				}


				objects.push_back(object);

				objectElement = objectElement->NextSiblingElement("object");
			}
			objectGroupElement = objectGroupElement->NextSiblingElement("objectgroup");
		}
	}
	else
	{
		std::cout << "No object layers found..." << std::endl;
	}

	return true;
}

Object Level::GetObject(std::string name)
{
	// get object by name
	for (int i = 0; i < objects.size(); i++)
		if (objects[i].name == name)
			return objects[i];
}

std::vector<Object> Level::GetObjects(std::string name)
{
	// get all the objects with the same name
	std::vector<Object> vec;
	for (int i = 0; i < objects.size(); i++)
		if (objects[i].name == name)
			vec.push_back(objects[i]);

	return vec;
}


std::vector<Object> Level::GetAllObjects()
{
	return objects;
};


SDL_Point Level::GetTileSize()
{
	return { tileWidth, tileHeight };
}

void Level::Draw(SDL_Renderer *window)
{
	// draw the tiles of every layer
	for (int layer = 0; layer < layers.size(); layer++)
		for (int tile = 0; tile < layers[layer].tiles.size(); tile++) {
			//window.draw(layers[layer].tiles[tile]);
			Sprite sp = layers[layer].tiles[tile];
			SDL_SetTextureAlphaMod(sp.texture, 255);
			SDL_Rect dest = { sp.pos.x, sp.pos.y, sp.rect.w, sp.rect.h };
			SDL_RenderCopyEx(window, sp.texture, &sp.rect, &dest, 0.0, 0, SDL_FLIP_NONE);
		}
}