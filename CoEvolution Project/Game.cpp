#include "Game.h"
#include "SplashState.h"

Game::Game(int width, int height, std::string title)
{
	this->_data->window.create(sf::VideoMode(width, height), title);
	this->_data->window.setVerticalSyncEnabled(true);
	this->_data->stateMachine.PushState(StateRef(new SplashState(this->_data)));
	this->_data->camera = Camera(&(this->_data->window), this->_data->window.getSize(), sf::Vector2f(this->_data->window.getSize().x/2, this->_data->window.getSize().y/2));
	this->Run();
}

void Game::Run()
{
	float newTime, frameTime, interpolation;

	float currentTime = this->_clock.getElapsedTime().asSeconds();
	float accumulator = 0.0f;

	while (this->_data->window.isOpen()) {
		this->_data->stateMachine.ProcessStateChanges();

		newTime = this->_clock.getElapsedTime().asSeconds();

		frameTime = newTime - currentTime;
		if (frameTime > 0.25f) {
			frameTime = 0.25f;
		}
		currentTime = newTime;
		accumulator += frameTime;

		while (accumulator >= dt) {
			this->_data->stateMachine.GetAvtiveState()->HandleEvents();
			this->_data->stateMachine.GetAvtiveState()->Update(dt);

			accumulator -= dt;
		}
		interpolation = accumulator / dt;
		this->_data->stateMachine.GetAvtiveState()->Draw(interpolation);
	}
}
