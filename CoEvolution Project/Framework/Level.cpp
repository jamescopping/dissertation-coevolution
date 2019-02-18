#include "Level.h"
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include "DEFINITIONS.h"

Level::Level(GameDataRef data, std::string filePath, float time): _data(data), _timeToComplete(time)
{
	loadLevelFromTextFile(filePath);
}

void Level::loadLevelFromTextFile(std::string filePath)
{
	this->_tilemap = std::vector<Tile>();
	this->_checkpoint = std::vector<sf::Vector2f>();
	int i = 0, j = 0;
	this->_width = 0;
	this->_height = 0;
	std::ifstream file;
	std::string line;
	file.open(filePath);
	if (file.is_open()) {
		while (!file.eof()) {
			std::getline(file, line);
			std::stringstream ss(line);
			std::string token;

			while (std::getline(ss, token, ',')) {
				int tileID = std::stoi(token);
				sf::Sprite spriteTile;
				spriteTile.setTexture(this->_data->assetManager.getTexturesheet(TILES).getTexture(tileID));
				AssetManager::rescale(spriteTile, ZOOM_FACTOR);
				//change the width and height scaling
				sf::Vector2f pos(j*TILE_SIZE, i*TILE_SIZE);
				spriteTile.setPosition(pos);
				_tilemap.push_back(Tile(tileID, spriteTile, Tile::getIfSolid(tileID)));
				if (tileID == CHECKPOINT_TILE || tileID == FINISH_LINE_TILE) {
					_checkpoint.push_back(pos);
				}
				j++;
			}
			i++;
			if(this->_width == 0){
				this->_width = j;
			}
			j = 0;
		}
	}
	this->_height = i;
	file.close();
}

Tile * Level::tileAt(int i, int j)
{
	if (i < 0) i = 0;
	if (i > this->_width - 1) i = this->_width - 1;
	if (j < 0) j = 0;
	if (j > this->_height - 1) j = this->_height - 1;
	return &_tilemap.at((j * this->_width) + i);
}

void Level::draw()
{
	for (Tile& tile : _tilemap) {
		if (this->_data->camera.getCameraBox().intersects(tile.getSprite().getGlobalBounds())) {
			this->_data->window.draw(tile.getSprite());
		}
	}
}


bool Level::collision(const sf::FloatRect &rect)
{
	bool any_collision = false;
	for (Tile* t : this->getTilesInArea(rect)) {
		if (t->isSolid()) {
			any_collision = true;
			break;
		}
	}
	return any_collision;
}

std::vector<Tile*> Level::getTilesInArea(const sf::FloatRect &rect)
{
	int left_tile = int(rect.left / TILE_SIZE);
	int right_tile = int((rect.left + rect.width) / TILE_SIZE);
	int top_tile = int(rect.top / TILE_SIZE);
	int bottom_tile = int((rect.top + rect.height) / TILE_SIZE);

	std::vector<Tile*> tilesInArea = std::vector<Tile*>();

	for (int i = left_tile; i <= right_tile; i++)
	{
		for (int j = top_tile; j <= bottom_tile; j++)
		{
			tilesInArea.push_back(this->tileAt(i, j));
		}
	}
	return tilesInArea;

}

bool Level::collisionWithTile(const sf::FloatRect &rect, int tileID)
{
	for (Tile* t : this->getTilesInArea(rect)) {
		if (t->getTileID() == tileID) {
			return true;
		}
	}
	return false;
}

const sf::Vector2f& Level::getCheckpoint(int num) const 
{
	return _checkpoint.at(num);
}

bool Level::lastCheckpoint(int num)
{
	if (num == _checkpoint.size() - 1) {
		return true;
	}
	else {
		return false;
	}
}

const sf::Vector2f & Level::getFinishFlagPosition() const
{
	return this->_checkpoint.back();
}

const float & Level::getLevelTime() const
{
	return this->_timeToComplete;
}