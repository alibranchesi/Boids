#include "simulation.hpp"

#include "shoal.hpp"

template <typename T>
void checkInput(T &input, T max) {
  std::cin >> input;

  while (!std::cin.good() || input < 0 || input > max) {
    if (!std::cin.good() || input < 0) {
      throw std::runtime_error{"ERROR: wrong input! Try again: "};
      std::cin.clear();
      std::cin.ignore(1000, '\n');
    } else {
      throw std::runtime_error{"The value is too high. Try again: "};
      std::cin.clear();
      std::cin.ignore(1000, '\n');
    }
    std::cin >> input;
  }
  std::cin.ignore(1000, '\n');
}

Simulation::Simulation() {
  InitVariables();
  InitWindow();
}

void Simulation::InitVariables() {
  float v;
  std::cout << "Set the speed of the boids (a number from 0 to 10): ";
  checkInput(v, 10.f);

  int n;
  std::cout << "Enter the initial number of boids (an integer from 0 to 200): ";
  checkInput(n, 800);

  int Max_P;
  std::cout << "Enter the maximum number of predators allowed: ";
  checkInput(Max_P, 10);
  max_p = Max_P;

  separation = false;
  alignement = false;
  cohesion = false;
  walls = false;
  kill = true;
  cm = false;

  time = sf::seconds(0.2f);
  mouseHeld = false;
  endSimulation = false;
  w_x = 1000.;
  w_y = 700.;

  if (!font.loadFromFile("DejaVuSans-Bold.ttf")) {
    throw std::runtime_error{"Fatal Error!"};
  }

  prompt_nb.setFont(font);
  prompt_nb.setCharacterSize(10);
  prompt_nb.setFillColor(sf::Color::Blue);

  prompt_np.setFont(font);
  prompt_np.setCharacterSize(10);
  prompt_np.setFillColor(sf::Color::Blue);

  prompt_s.setFont(font);
  prompt_s.setCharacterSize(10);
  prompt_s.setFillColor(sf::Color::Blue);

  prompt_a.setFont(font);
  prompt_a.setCharacterSize(10);
  prompt_a.setFillColor(sf::Color::Blue);

  prompt_c.setFont(font);
  prompt_c.setCharacterSize(10);
  prompt_c.setFillColor(sf::Color::Blue);

  prompt_k.setFont(font);
  prompt_k.setCharacterSize(10);
  prompt_k.setFillColor(sf::Color::Blue);

  prompt_w.setFont(font);
  prompt_w.setCharacterSize(10);
  prompt_w.setFillColor(sf::Color::Red);

  prompt_m.setFont(font);
  prompt_m.setCharacterSize(10);
  prompt_m.setFillColor(sf::Color::Red);

  shoal.InitShoal(v, w_x, w_y, n);
}

void Simulation::InitWindow() {
  videoMode.height = w_y;
  videoMode.width = w_x;
  videoMode.getDesktopMode();
  window.create(videoMode, "Boids simulation",
                sf::Style::Titlebar | sf::Style::Close);
  window.setPosition({100, 60});
  window.setFramerateLimit(60);
}

bool Simulation::running() const { return window.isOpen(); }

bool Simulation::getEndSimulation() const { return endSimulation; }

void Simulation::pollEvents() {
  while (window.pollEvent(ev)) {
    switch (ev.type) {
      case sf::Event::Closed:
        window.close();
        break;

      case sf::Event::KeyPressed:

        switch (ev.key.code) {
          case sf::Keyboard::Escape:
            window.close();
            break;

          case sf::Keyboard::S:
            if (separation) {
              separation = false;
            }

            else {
              separation = true;
            }

            break;

          case sf::Keyboard::X:
            if (alignement) {
              alignement = false;

            }

            else {
              alignement = true;
            }
            break;

          case sf::Keyboard::C:
            if (cohesion) {
              cohesion = false;
            }

            else {
              cohesion = true;
            }
            break;

          case sf::Keyboard::K:
            if (kill) {
              kill = false;
            }

            else {
              kill = true;
            }
            break;

          case sf::Keyboard::P:
            shoal.clearPredators();
            break;

          case sf::Keyboard::W:
            if (walls) {
              walls = false;
              break;
            }

            else {
              walls = true;
              break;
            }

          case sf::Keyboard::M:
            if (cm) {
              cm = false;
              break;
            }

            else {
              cm = true;
              break;
            }

          default:

            break;
        }

      default:
        break;
    }

    shoal.UpdateBools(separation, alignement, cohesion, kill, walls, cm);
  }
}

void Simulation::updateMousePosition() {
  mousePosWindow = sf::Mouse::getPosition(window);
  mousePosView = window.mapPixelToCoords(mousePosWindow);
}

void Simulation::checkSpawnBoids() {
  if (ev.mouseButton.button == sf::Mouse::Left && shoal.getBoidsSize() < 800) {
    if (clock.restart() >= time) {
      shoal.spawnBoids(ev.mouseButton.x, ev.mouseButton.y);
    }
  }
}

void Simulation::checkSpawnPredator() {
  if (ev.mouseButton.button == sf::Mouse::Right &&
      shoal.getPredatorsSize() < 10) {
    if (shoal.getPredatorsSize() < max_p) {
      if (clock.restart() >= time) {
        shoal.spawnPredator(ev.mouseButton.x, ev.mouseButton.y);
      }
    }
  }
}

void Simulation::update() {
  pollEvents();

  if (endSimulation == false) {
    window.clear(sf::Color::Cyan);
    updateMousePosition();
    checkSpawnBoids();
    checkSpawnPredator();
    shoal.update();
    shoal.render(window);
    renderTerminal();
    window.display();
  }
}

void Simulation::renderTerminal() {
  std::ostringstream number_b;
  number_b << std::fixed << std::setprecision(1) << shoal.getBoidsSize();
  prompt_nb.setPosition(5., 5.);
  prompt_nb.setString("Boids simulation\nNumber of boids: " + number_b.str());

  prompt_s.setPosition(5., 43.);
  if (separation) {
    prompt_s.setString("Separation (S): true");
  }

  else {
    prompt_s.setString("Separation (S): false");
  }

  prompt_a.setPosition(5., 56.);
  if (alignement) {
    prompt_a.setString("Alignement (X): true");
  }

  else {
    prompt_a.setString("Alignement (X): false");
  }

  prompt_c.setPosition(5., 69.);
  if (cohesion) {
    prompt_c.setString("cohesion (C): true");
  }

  else {
    prompt_c.setString("cohesion (C): false");
  }

  if (shoal.getPredatorsSize() >= max_p) {
    prompt_np.setPosition(5., 93.);
    prompt_np.setString("Number of predators: MAX");
  }

  else {
    std::ostringstream number_p;
    number_p << std::fixed << std::setprecision(1) << shoal.getPredatorsSize();
    prompt_np.setPosition(5., 93.);
    prompt_np.setString("Number of predators: " + number_p.str());
  }

  prompt_k.setPosition(5., 118.);
  if (kill) {
    prompt_k.setString("Killing (K): true");
  }

  else {
    prompt_k.setString("Killing (K): false");
  }

  prompt_w.setPosition(window.getSize().x - 99, 5.);
  if (walls) {
    prompt_w.setString("Walls (W): true");
  }

  else {
    prompt_w.setString("Walls (W): false");
  }

  prompt_m.setPosition(window.getSize().x - 160, 18.);
  if (cm) {
    prompt_m.setString("Center of Mass (M): true");
  }

  else {
    prompt_m.setString("Center of Mass (M): false");
  }

  window.draw(prompt_nb);
  window.draw(prompt_s);
  window.draw(prompt_a);
  window.draw(prompt_c);
  window.draw(prompt_np);
  window.draw(prompt_k);
  window.draw(prompt_w);
  window.draw(prompt_m);
}
