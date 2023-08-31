#ifndef SIMULATION_HPP
#define SIMULATION_HPP

#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <chrono>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <random>
#include <stdexcept>
#include <vector>

#include "shoal.hpp"

class Simulation {
 private:
 //variables and window
  sf::Event ev;
  sf::Font font;
  sf::Text prompt_nb;
  sf::Text prompt_np;
  sf::Text prompt_s;
  sf::Text prompt_a;
  sf::Text prompt_c;
  sf::Text prompt_k;
  sf::Text prompt_w;
  sf::Text prompt_m;

 //mouse position visible
  sf::Vector2i mousePosWindow;
  sf::Vector2f mousePosView;

  //predators and velocity
  unsigned int max_p; 
  float w_x;
  float w_y;
  
  //moving rules
  bool separation;  
  bool alignement;
  bool cohesion;
  bool kill;
  bool walls;
  bool cm;

  bool boolkeyPressed;
  bool mouseHeld;
  bool endSimulation;

  //animations
  sf::Time time;
  sf::Clock clock;
  Shoal shoal;
  std::vector<sf::RectangleShape> boids;
  std::vector<sf::RectangleShape> predators;
  sf::VideoMode videoMode;
  sf::RenderWindow window;

 public:
  void InitVariables();
  void InitWindow();

  Simulation();

  //accessor, they take the private variables
  bool running() const;
  bool getEndSimulation() const;

 //moving functions, controll the boids and predators' movements
  void pollEvents();
  void updateMousePosition();

  void updateBoids();
  void updatePredator();
  void update();

  void renderTerminal();
  void render();

  void checkSpawnBoids();
  void checkSpawnPredator();
};

#endif