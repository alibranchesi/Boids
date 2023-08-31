#ifndef SHOAL_HPP
#define SHOAL_HPP

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

struct Boid {
  sf::Vector2f p;
  sf::Vector2f v;
  float angle{0.};
};

class Shoal {
 private:
  double d_b;
  double d;
  double d_k;

  bool separation;
  bool alignement;
  bool cohesion;
  bool kill;
  bool walls;
  bool cm;

  float delta_t;
  float velocity;
  int tx_imageCount;
  int tx_currentImage;
  float tx_totalTime;
  float tx_switchTime;

  sf::Texture boidTexture;
  sf::Texture predatorTexture;
  sf::Texture cmTexture;
  sf::Clock textureClock;

  std::default_random_engine gen; 
  float windowSizeX;
  float windowSizeY;
  unsigned int b_start;
  float ViewAngle;
  Boid boid;
  Boid predator;

  sf::RectangleShape canvas_b;
  sf::RectangleShape canvas_p;
  sf::RectangleShape canvas_cm;
  sf::Vector2f boids_CM;

 public:
  std::vector<Boid> boids;
  std::vector<Boid> predators;

  void InitShoal(float, float, float, int);
  void UpdateBools(bool, bool, bool, bool, bool, bool);
  void InitCanvas();

  void update();
  void render(sf::RenderWindow &window);

  void moveBoids();

  void movePredators();
  void VCorrectionBoids();
  void move_bouncing();
  void move_pacman();
  void move_separation();
  void move_alignement();
  void move_cohesion();
  void move_escape();
  void move_bouncingPredator();
  void move_pacmanPredator();
  void move_hunting();
  void VCorrectionPredator();
  void killing();

  void clearPredators();
  long unsigned int getBoidsSize() const;
  long unsigned int getPredatorsSize() const;

  void updateAnimation();
  void updateTexture();

  bool nearby(Boid const &, Boid const &, double);
  bool visible_b(int b_i, int b_j);
  bool visible_p(int p_i, int p_j);

  void spawnBoids(float, float);
  void spawnPredator(float, float);
  void spawnCustomBoid(float, float, float, float);
  void spawnCustomPredator(float, float, float, float);

  void InitBoids();
  void InitPredator();
  void RandomBoids();
};

#endif