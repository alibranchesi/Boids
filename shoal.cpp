#include "shoal.hpp"

#include "simulation.hpp"
#include "vector.hpp"

void Shoal::InitShoal(float v, float w_x, float w_y, int b_s) {
  gen.seed(std::chrono::system_clock::now().time_since_epoch().count());

  delta_t = 0.5;
  velocity = v;
  windowSizeX = w_x;
  windowSizeY = w_y;
  b_start = b_s;

  d = 145.f;   // min distance
  d_b = 10.f;  // separation
  d_k = 25.f;  // killing distance

  separation = true;
  cohesion = false;
  alignement = false;
  kill = true;
  walls = false;
  cm = true;

  ViewAngle = 150;

  InitCanvas();
  RandomBoids();
}

void Shoal::InitCanvas() {
  if (!boidTexture.loadFromFile("12fish.png")) {
    throw std::runtime_error{"Fatal Error!"};
  }

  if (!predatorTexture.loadFromFile("12sharks.png")) {
    throw std::runtime_error{"Fatal Error!"};
  }

  if (!cmTexture.loadFromFile("CmMarker.png")) {
    throw std::runtime_error{"Fatal Error!"};
  }

  tx_imageCount = 12;
  tx_switchTime = 0.02;
  tx_currentImage = 1;

  canvas_b.setTexture(&boidTexture);
  canvas_b.setTextureRect(sf::IntRect(0, 0, boidTexture.getSize().x,
                                      boidTexture.getSize().y / tx_imageCount));
  canvas_b.setPosition(0.f, 0.f);
  canvas_b.setSize(sf::Vector2f(14.f, 8.f));
  canvas_b.setOrigin(7.f, 4.f);

  canvas_p.setTexture(&predatorTexture);
  canvas_p.setTextureRect(
      sf::IntRect(0, 0, predatorTexture.getSize().x,
                  predatorTexture.getSize().y / tx_imageCount));
  canvas_p.setPosition(0., 0.);
  canvas_p.setSize(sf::Vector2f(40.f, 24.f));
  canvas_p.setOrigin(20.f, 12.f);

  canvas_cm.setSize(sf::Vector2f(30.f, 30.f));
  canvas_cm.setOrigin(5.f, 5.f);
  canvas_cm.setTexture(&cmTexture);
  canvas_cm.setPosition(0.f, 0.f);
}

void Shoal::RandomBoids() {
  for (unsigned int i{}; i < static_cast<unsigned int>(b_start); i++) {
    int x = static_cast<int>(gen() % (1000));
    int y = static_cast<int>(gen() % (700 - 30));

    if (y < 20) {
      y = 25;
    }

    boid.p = sf::Vector2f(x, y);

    std::uniform_real_distribution<float> distribution(-3.14, 3.14);
    float angle = distribution(gen);
    boid.v = {std::cos(angle) * (velocity), std::sin(angle) * (velocity)};
    boids.push_back(boid);
  }
}

void Shoal::UpdateBools(bool s, bool a, bool c, bool k, bool w, bool m) {
  separation = s;
  alignement = a;
  cohesion = c;
  kill = k;
  walls = w;
  cm = m;
}

void Shoal::update() {
  moveBoids();
  movePredators();
  updateAnimation();
}

void Shoal::render(sf::RenderWindow &window) {
  for (auto &it : boids) {
    canvas_b.setPosition(it.p);
    canvas_b.setRotation(it.angle);
    window.draw(canvas_b);
  }

  for (auto &it : predators) {
    canvas_p.setPosition(it.p);
    canvas_p.setRotation(it.angle);
    window.draw(canvas_p);
  }

  if (cm) {
    canvas_cm.setPosition(boids_CM);
    window.draw(canvas_cm);
  }
}

void Shoal::clearPredators() { predators.clear(); };

long unsigned int Shoal::getBoidsSize() const {
  long unsigned int size = boids.size();
  return size;
};

long unsigned int Shoal::getPredatorsSize() const {
  long unsigned int size = predators.size();
  return size;
};

void Shoal::move_pacman() {
  std::vector<sf::Vector2f> FinState;

  std::for_each(boids.begin(), boids.end(), [&](Boid &b_i) {
    sf::Vector2f const InitState{b_i.p};
    if (b_i.p.y > windowSizeY + 10.f) {
      b_i.p = sf::Vector2f(b_i.p.x, 0.f - 10.f);
    }
    if (b_i.p.y < -15.f) {
      b_i.p = sf::Vector2f(b_i.p.x, windowSizeY);
    }

    if (b_i.p.x > windowSizeX + 10.f) {
      b_i.p = sf::Vector2f(0.f - 10.f, b_i.p.y);
    }

    if (b_i.p.x < -15.f) {
      b_i.p = sf::Vector2f(windowSizeX, b_i.p.y);
    }

    b_i.p += b_i.v * delta_t;

    sf::Vector2f const ds = b_i.p - InitState;

    b_i.angle = getRotation(ds);
  });
}

void Shoal::move_bouncing() {
  std::for_each(boids.begin(), boids.end(), [&](Boid &b_i) {
    sf::Vector2f const InitState{b_i.p};

    if (b_i.p.y > windowSizeY - 10.f) {
      b_i.p = sf::Vector2f(b_i.p.x, windowSizeY - 11.f);
      b_i.v.y *= -1;
    }

    if (b_i.p.y < 5.f) {
      b_i.p = sf::Vector2f(b_i.p.x, 6.f);
      b_i.v.y *= -1;
    }

    if (b_i.p.x > windowSizeX - 10.f) {
      b_i.v.x *= -1;
      b_i.p = sf::Vector2f(windowSizeX - 11.f, (b_i.p.y));
    }

    if (b_i.p.x < 5.f) {
      b_i.v.x *= -1;
      b_i.p = sf::Vector2f(6.f, (b_i.p.y));
    }

    b_i.p += b_i.v * delta_t;

    sf::Vector2f const ds = b_i.p - InitState;

    b_i.angle = getRotation(ds);
  });
}

void Shoal::VCorrectionBoids() {
  std::for_each(boids.begin(), boids.end(), [&](Boid &b_i) {
    if (norm(b_i.v) > velocity * 1.3) {
      b_i.v *= 0.9;
    }
    if (norm(b_i.v) < 0.75 * velocity) {
      b_i.v *= 1.2;
    }
  });
}

void Shoal::move_separation() {
  for (long unsigned int i{}; i < boids.size(); i++) {
    sf::Vector2f sep =
        std::accumulate(boids.begin(), boids.end(), sf::Vector2f{0, 0},
                        [&](sf::Vector2f sum, Boid const &jeppetto) {
                          {
                            if (nearby(boids[i], jeppetto, d_b)) {
                              return sum + (jeppetto.p - boids[i].p) * (-0.045);
                            } else {
                              return sum;
                            }
                          }
                        });
    boids[i].v += sep;
  }
}

void Shoal::move_alignement() {
  for (long unsigned int i{}; i < boids.size(); i++) {
    int c{};
    sf::Vector2f alignement =
        std::accumulate(boids.begin(), boids.end(), sf::Vector2f{0.f, 0.f},
                        [&](sf::Vector2f sum, Boid &al) {
                          if (nearby(boids[i], al, d)) {
                            c++;
                            return sum + al.v;
                          } else {
                            return sum;
                          }
                        });
    if (c > 0) {
      boids[i].v += ((alignement / c) - boids[i].v) * 0.007;
    };
  }
}

void Shoal::move_cohesion() {
  for (long unsigned int i{}; i < boids.size(); i++) {
    sf::Vector2f sumX{0, 0};
    int counter{};

    for (long unsigned int j{}; j < boids.size(); j++) {
      if (nearby(boids[i], boids[j], d) && i != j) {
        if (visible_b(i, j)) {
          sumX += boids[j].p;
          ++counter;
        }
      }
    }

    if (counter > 0) {
      boids[i].v += ((sumX / counter) - boids[i].p) * 0.01;
    }
  }
}

void Shoal::move_escape() {
  std::for_each(predators.begin(), predators.end(), [&](Boid const &p_i) {
    std::for_each(boids.begin(), boids.end(), [&](Boid &b_i) {
      if (nearby(p_i, b_i, d)) {
        b_i.v += (p_i.p - b_i.p) * (-0.005);
      }
    });
  });
}

void Shoal::move_bouncingPredator() {
  std::for_each(predators.begin(), predators.end(), [&](Boid &p_i) {
    sf::Vector2f const InitState = p_i.p;

    if (p_i.p.y > windowSizeY - 30.f) {
      p_i.p = sf::Vector2f((p_i.p.x), windowSizeY - 31.f);
      p_i.v.y *= -1;
    }

    if (p_i.p.y < 20.f) {
      p_i.p = sf::Vector2f((p_i.p.x), 21.f);
      p_i.v.y *= -1;
    }

    if (p_i.p.x > windowSizeX - 30.f) {
      p_i.v.x *= -1;
      p_i.p = sf::Vector2f(windowSizeX - 31.f, (p_i.p.y));
    }

    if (p_i.p.x < 20.f) {
      p_i.v.x *= -1;
      p_i.p = sf::Vector2f(21.f, (p_i.p.y));
    }

    p_i.p += p_i.v * delta_t;
    sf::Vector2f const ds = p_i.p - InitState;
    p_i.angle = getRotation(ds);
  });
}

void Shoal::move_pacmanPredator() {  // ALGORITMO
  for (long unsigned int i{}; i < predators.size(); i++) {
    sf::Vector2f const InitState{predators[i].p};

    if (predators[i].p.y > windowSizeY + 10.f) {
      predators[i].p = sf::Vector2f(predators[i].p.x, 0.f - 15.f);
    }

    if (predators[i].p.y < -15.f) {
      predators[i].p = sf::Vector2f(predators[i].p.x, windowSizeY);
    }

    if (predators[i].p.x > windowSizeX) {
      predators[i].p = sf::Vector2f(0.f - 15.f, predators[i].p.y);
    }

    if (predators[i].p.x < -15.f) {
      predators[i].p = sf::Vector2f(windowSizeX, predators[i].p.y);
    }

    predators[i].p += predators[i].v * delta_t;
    sf::Vector2f const ds = predators[i].p - InitState;
    predators[i].angle = getRotation(ds);
  }
}

void Shoal::killing() {  // ALGORITMO
  for (long unsigned int i{}; i < predators.size(); i++) {
    for (long unsigned int j{}; j < boids.size(); j++) {
      if (nearby(predators[i], boids[j], d_k)) {
        if (visible_p(i,j)) {
          boids.erase(boids.begin() + j);
        }
      }
    }
  }
}

void Shoal::move_hunting() {  // ALGORITMO
  for (long unsigned int i{}; i < predators.size(); i++) {
    sf::Vector2f sumX{0, 0};
    int counter{};

    for (long unsigned int j{}; j < boids.size(); j++) {
      if (nearby(predators[i], boids[j], d) && i != j) {
        if (visible_p(i,j)) {
          sumX += boids[j].p;
          ++counter;
        }
      }
    }

    if (counter > 0) {
      sf::Vector2f CM_distance = ((sumX / counter) - predators[i].p);
      predators[i].v += CM_distance * 0.01;
    }
  }
}
void Shoal::VCorrectionPredator() {
  std::for_each(predators.begin(), predators.end(), [=](Boid &p_i) {
    if (norm(p_i.v) > velocity * 1.3) {
      p_i.v *= 0.9;
    }
    if (norm(p_i.v) < 0.75 * velocity) {
      p_i.v *= 1.2;
    }
  });
}

void Shoal::moveBoids() {
  VCorrectionBoids();

  if (separation) {
    move_separation();
  }

  if (alignement) {
    move_alignement();
  }

  if (cohesion) {
    move_cohesion();
  }

  move_escape();

  (walls) ? move_bouncing() : move_pacman();

  if (cm) {
    boids_CM = sf::Vector2f(CalculateCM(boids.begin(), boids.size()));
  }
}

void Shoal::movePredators() {
  VCorrectionPredator();
  if (kill) {
    killing();
  }
  (walls) ? move_bouncingPredator() : move_pacmanPredator();
}

void Shoal::spawnBoids(float mouse_x, float mouse_y) {
  boid.p = sf::Vector2f(mouse_x, mouse_y);
  std::uniform_real_distribution<float> distribution(-3.14, 3.14);
  float angle = distribution(gen);
  boid.v = {std::cos(angle) * (velocity), std::sin(angle) * (velocity)};
  boids.push_back(boid);
}

void Shoal::spawnPredator(float mouse_x, float mouse_y) {
  predator.p = sf::Vector2f(mouse_x, mouse_y);
  std::uniform_real_distribution<float> distribution(-3.14, 3.14);
  float angle = distribution(gen);

  predator.v = {std::cos(angle) * (velocity), std::sin(angle) * (velocity)};
  predators.push_back(predator);
}

void Shoal::updateAnimation() {
  tx_totalTime = textureClock.getElapsedTime().asSeconds();

  if (tx_totalTime >= tx_switchTime) {
    tx_currentImage += 1;

    if (tx_currentImage > tx_imageCount) {
      tx_currentImage = 1;
    }

    textureClock.restart();

    updateTexture();
  }
}

void Shoal::updateTexture() {
  sf::IntRect uvRect_b = canvas_b.getTextureRect();
  sf::IntRect uvRect_p = canvas_p.getTextureRect();
  uvRect_b.top = (tx_currentImage - 1) * uvRect_b.height;
  uvRect_p.top = (tx_currentImage - 1) * uvRect_p.height;

  for (long unsigned int i{}; i < boids.size(); i++) {
    canvas_b.setTextureRect(uvRect_b);
  }

  for (long unsigned int i{}; i < predators.size(); i++) {
    canvas_p.setTextureRect(uvRect_p);
  }
}

bool Shoal::visible_b(int b_i, int b_j) {
  float ds_angle = getRotation(boids[b_j].p - boids[b_i].p);
  float relative_angle = ds_angle - getRotation(boids[b_i].v);

  return std::abs(relative_angle) < ViewAngle / 2;
}

bool Shoal::visible_p(int p_i, int b_j) {
  float ds_angle = getRotation(boids[b_j].p - predators[p_i].p);
  float relative_angle = ds_angle - getRotation(predators[p_i].v);

  return std::abs(relative_angle) < ViewAngle / 2;
}

bool Shoal::nearby(Boid const &x1, Boid const &x2, double d_int) {
  double distance = norm(x2.p - x1.p);

  return distance < d_int;
}

void Shoal::spawnCustomBoid(float x, float y, float v_x, float v_y) {
  boid.p = sf::Vector2f(x, y);
  boid.v = sf::Vector2f(v_x, v_y);
  boid.angle = getRotation(boid.v);
  boids.push_back(boid);
}

void Shoal::spawnCustomPredator(float x, float y, float v_x, float v_y) {
  predator.p = sf::Vector2f(x, y);
  predator.v = sf::Vector2f{v_x, v_y};
  predator.angle = getRotation(predator.v);
  predators.push_back(predator);
}