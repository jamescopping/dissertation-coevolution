#include "GameObjectManager.h"

GameObjectManager::~GameObjectManager()
{
	clearEntities();
}

void GameObjectManager::addEntity(IEntity * e)
{
	this->_entities.push_back(e);
}


void GameObjectManager::clearEntities()
{
	this->_entities.clear();
}


void GameObjectManager::draw(float dt)
{
	for (IEntity* e : _entities) {
		if (e->isActive()) {
			e->draw(dt);
		}
	}
}

void GameObjectManager::update(float dt)
{
	for(int i = 0; i < (int)_entities.size(); i++){
		if (_entities[i]->isActive()) {
			_entities[i]->update(dt);
		}/*
		else {
			//deactivate / remove the entity;
			_entities.erase(_entities.begin() + i);
		}*/
	}
}

