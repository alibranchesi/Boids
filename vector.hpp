#ifndef VECTOR_HPP
#define VECTOR_HPP

#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <algorithm>
#include <cassert>
#include <chrono>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <random>
#include <stdexcept>
#include <vector>

inline void operator+=(sf::Vector2f& a, sf::Vector2f const& b) {
  a.x += b.x;
  a.y += b.y;
}

inline void operator-=(sf::Vector2f& a, sf::Vector2f const& b) {
  a.x -= b.x;
  a.y -= b.y;
}

inline void operator*=(sf::Vector2f& a, sf::Vector2f const& b) {
  a.x *= b.x;
  a.y *= b.y;
}

inline sf::Vector2f operator*(sf::Vector2f const& a, sf::Vector2f const& b) {
  sf::Vector2f result(a);
  result *= b;

  return result;
}

inline void operator*=(sf::Vector2f& a, float const& b) {
  a.x *= b;
  a.y *= b;
}

inline sf::Vector2f operator*(sf::Vector2f const& a, float const& b) {
  sf::Vector2f result(a);
  result *= b;

  return result;
}

inline void operator/=(sf::Vector2f& a, sf::Vector2f const& b) {
  assert(b.x != 0 && b.y != 0);
  a.x /= b.x;
  a.y /= b.y;
}

inline sf::Vector2f operator/(sf::Vector2f const& a, sf::Vector2f const& b) {
  sf::Vector2f result(a);
  result /= b;

  return result;
}

inline void operator/=(sf::Vector2f& a, float const& b) {
  assert(b != 0);
  a.x /= b;
  a.y /= b;
}

inline sf::Vector2f operator/(sf::Vector2f const& a, float const& b) {
  sf::Vector2f result(a);
  result /= b;

  return result;
}

inline sf::Vector2f operator/(float const& a, sf::Vector2f const& b) {
  sf::Vector2f result(0., 0.);
  result.x = a / b.x;
  result.y = a / b.y;
  return result;
}

inline float norm(sf::Vector2f const& a) {
  return std::sqrt(a.x * a.x + a.y * a.y);
}

inline float getRotation(sf::Vector2f const& ds) {
  float rotation{};

  if (ds.x >= 0.) {
    rotation = std::atan(ds.y / ds.x) * (180 / 3.1415926);
  }

  if (ds.x < 0.) {
    rotation = 180 + std::atan(ds.y / ds.x) * (180 / 3.1415926);
  }

  if (ds == sf::Vector2f(0., 0.)) {
    rotation = 0.;
  }

  return rotation;
}

// for_each algorithm to calculate the cm every time that the vector boid
// changes(depends on the number of the boids, the lambda takes them all and the
// result is the sum divided by the size)
inline sf::Vector2f CalculateCM(std::vector<Boid>::iterator init, int size) {
  sf::Vector2f sum;
  std::for_each(init, init + size, [&](Boid const& init) { sum += init.p; });

  return sum / size;
}

#endif
