#include "Player.h"
#include "Tile.h"
#include "MainMenuState.h"
#include "DEFINITIONS.h"
#include <iostream>

Player::Player(GameDataRef data, std::vector<Level>& levels, int& currentLevel, sf::Vector2f wh) : _data(data), _levels(levels), _currentLevel(currentLevel)
{
	this->_speed = 300.0f;
	this->_jumpVelocity = 450.0f;
	this->_sprite.setTexture(this->_data->assetManager.getTexturesheet(PLAYER).getTexture(0));
	AssetManager::rescale(_sprite, wh);
	this->_sprite.setColor(sf::Color::Blue);
	this->init();
}

Player::~Player()
{

}


void Player::init()
{
	_jump = false;
	_falling = true;
	_jumping = false;
	_holdingJump = false; 
	_grounded = false;
	_finished = false;
	this->respawn();
}

void Player::update(float dt)
{
	if (_jump) {
		_jump = false;
		_grounded = false;
		_jumping = true;
		this->_velocity.y = -_jumpVelocity;
	}

	if (_falling && this->_velocity.y == 0.0f) {
		_grounded = true;
		_falling = false;
		_jumping = false;
	}else if ((_grounded || _jumping) && this->_velocity.y > 0.0f) {
		_grounded = false;
		_falling = true;
		_jumping = false;
	}else if (_grounded && this->_velocity.y < 0.0f) {
		_grounded = false;
		_falling = false;
		_jumping = true;
	}


	if (_falling){
		this->_velocity.y += this->_data->gravity.y * (2.5f) * dt;
	}
	else if(_jumping && !_holdingJump){
		this->_velocity.y += this->_data->gravity.y * (2.0f) * dt;
	}
	else {
		this->_velocity.y += this->_data->gravity.y * dt;
	}
	if (!_direction == 0) {
		this->_velocity.x = _speed * _direction;
	}
	if (!_grounded && this->_velocity.x != 0.0f) {
		this->_velocity.x -= this->_velocity.x * (0.1f);
	}
	else {
		this->_velocity.x -= this->_velocity.x * (0.35f);
	}


	//check collision for each tile in the collision map of the current level

	if (this->_velocity.y > 1000.0f) {
		this->_velocity.y = 1000.0f;
	}
	else if (this->_velocity.y < -1000.0f){
		this->_velocity.y = -1000.0f;
	}


	//collsison detection in 5 substeps with wall sliding
	sf::Vector2f oldpos;
	int num_steps = 1;

	for (int i = 0; i < num_steps; i++) {
		oldpos = sf::Vector2f(this->_position);
		this->_position.x += this->_velocity.x * (dt/num_steps);
		_sprite.setPosition(this->_position);
		bool collision = this->_levels.at(this->_currentLevel).collision(_sprite.getGlobalBounds());
		if (collision) {
			this->_position = oldpos;
			_sprite.setPosition(this->_position);
		}

		oldpos = sf::Vector2f(this->_position);
		this->_position.y += this->_velocity.y * (dt/num_steps);
		_sprite.setPosition(this->_position);
		collision = this->_levels.at(this->_currentLevel).collision(_sprite.getGlobalBounds());
		if (collision) {
			this->_position = oldpos;
			this->_velocity.y = 0;
			_sprite.setPosition(this->_position);
		}
	}

	this->setProgress(percentageOfLevelCompleted());

	//if the player goes under the map then they die;
	if (this->_levels.at(this->_currentLevel).collisionWithTile(this->_sprite.getGlobalBounds(), DEATH_TILE)) {
		this->die();
	}

	//if the next checkpoint is the of the level then when the player passes it they win finish
	if (this->_levels.at(this->_currentLevel).lastCheckpoint(_currentCheckpoint + 1)) {
		if (this->_position.x >= this->_levels.at(this->_currentLevel).getCheckpoint(_currentCheckpoint + 1).x) {
			//player beat the level.
			//fireworks and stop the player;
			this->finish();
		}
	}
	//if it is no the last check point then when the player passes it, it just sets that as the current checkpoint
	else if (this->_position.x >= this->_levels.at(this->_currentLevel).getCheckpoint(_currentCheckpoint + 1).x ) {
		_currentCheckpoint++;
	}

	this->_sprite.setPosition(this->_position);
}

void Player::draw(float dt)
{
	this->_data->window.draw(this->_sprite);
	this->_sprite.setColor(sf::Color::Blue);
}


void Player::jump()
{
	_holdingJump = true;
	if (_grounded) {		
		_jump = true;
	}
}

void Player::stopJumping()
{
	this->_holdingJump = false;
}

void Player::left()
{
	_direction = -1;
}

void Player::right()
{
	_direction = 1;
}

void Player::stop()
{
	_direction = 0;
}

void Player::die()
{
	if (!_finished) {
		_lives--;
		this->deactivate();
		if (_lives <= 0) {
			this->restart();
		}
		else {
			this->respawn();
		}
	}
}

void Player::respawn()
{
	this->_position = this->_levels.at(this->_currentLevel).getCheckpoint(this->_currentCheckpoint);
	this->_velocity = sf::Vector2f(0.0f, 0.0f);
	this->activate();
}

void Player::restart()
{
	this->_lives = this->_startingLives;
	this->_currentCheckpoint = 0;
	this->_finished = false;
	this->_timer.restart();
	this->respawn();
}

void Player::finish()
{
	//player has finished the level...
	this->_finished = true;
	//could start a clock and have the next level after like 2 secs;
	//and display you finished in overlay text or something;
	
}

bool Player::isFinished()
{
	return this->_finished;
}

void Player::setProgress(float progress)
{
	this->_progress = progress;
}

const float & Player::getProgress() const
{
	return this->_progress;
}

void Player::setColor(sf::Color color)
{
	this->_sprite.setColor(color);
}

bool Player::isAlive()
{
	return (_lives > 0);
}

float Player::percentageOfLevelCompleted()
{
	//find the position of the player and compare it to the position of the final checkpoint
	float start = this->_levels.at(this->_currentLevel).getCheckpoint(0).x;
	float end = this->_levels.at(this->_currentLevel).getFinishFlagPosition().x;
	float ratio = (this->_position.x - start) / (end - start);
	float percentage = ratio * 100.0f;
	if (percentage > 100.0f || percentage < 0.0f) {
		percentage = std::max(0.0f, std::min(percentage, 100.0f));
	}
	return  percentage;
}

const sf::Vector2f & Player::getPosition() const
{
	return this->_position;
}



