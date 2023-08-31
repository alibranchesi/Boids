#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "doctest.h"
#include "shoal.hpp"
#include "simulation.hpp"
#include "vector.hpp"

bool approx_eq(sf::Vector2f const& a, sf::Vector2f const& b) {
  return a.x == doctest::Approx(b.x) && a.y == doctest::Approx(b.y);
}

TEST_CASE("Testing Boids Proximity Functions") {
  Shoal shoal;
  shoal.InitShoal(0, 1000, 700, 0);

  shoal.spawnCustomBoid(0., 400., -1., 2.);
  shoal.spawnCustomBoid(80., 400., 1., 4.);
  shoal.spawnCustomBoid(160., 400., 1., 0.);
  shoal.spawnCustomBoid(180., 400., -1., 0.);

  SUBCASE("Testing Nearby") {
    CHECK(shoal.nearby(shoal.boids[0], shoal.boids[1], 200.) == true);
    CHECK(shoal.nearby(shoal.boids[0], shoal.boids[2], 100.) == false);
    CHECK(shoal.nearby(shoal.boids[0], shoal.boids[0], 1.) == true);
  }

  SUBCASE("Testing Visible") {
    CHECK(shoal.visible_b(0, 1) == false);
    CHECK(shoal.visible_b(1, 0) == false);
    CHECK(shoal.visible_b(0, 2) == false);
    CHECK(shoal.visible_b(2, 3) == true);
    CHECK(shoal.visible_b(3, 2) == true);
  }
}

TEST_CASE("Testing Predators Proximity Functions") {
  Shoal shoal;
  shoal.InitShoal(0, 1000, 700, 0);

  shoal.spawnCustomPredator(160., 500., -3., 2.);
  shoal.spawnCustomPredator(0., 400., 1., 0.);
  shoal.spawnCustomPredator(170., 500., 1., 0.);
  shoal.spawnCustomBoid(800., 500., 2., -1.);
  shoal.spawnCustomBoid(240., 500., 0., 0.);
  shoal.spawnCustomBoid(193., 500., -4., 3.);

  SUBCASE("Testing Nearby") {
    CHECK(shoal.nearby(shoal.predators[1], shoal.predators[2], 100.) == false);
    CHECK(shoal.nearby(shoal.predators[1], shoal.boids[1], 100.) == false);
    CHECK(shoal.nearby(shoal.predators[1], shoal.boids[0], 101.) == false);
    CHECK(shoal.nearby(shoal.predators[0], shoal.boids[0], 100.) == false);
    CHECK(shoal.nearby(shoal.predators[2], shoal.boids[2], 100.) == true);
  }

  SUBCASE("Testing Visible") {
    CHECK(shoal.visible_p(0, 1) == false);
    CHECK(shoal.visible_p(1, 0) == true);
    CHECK(shoal.visible_p(2, 1) == true);
    CHECK(shoal.visible_p(1, 2) == true);
  }
}

TEST_CASE("Testing Boids Movement Funcion") {
  Shoal shoal;
  shoal.InitShoal(0, 1000, 700, 0);

  SUBCASE("Testing Boids Separation Funcion") {
    shoal.spawnCustomBoid(400., 400., 0., 0.);
    shoal.spawnCustomBoid(395., 405., 0., 0.);
    shoal.spawnCustomBoid(400., 420., 0., 0.);

    shoal.move_separation();
    shoal.move_bouncing();

    CHECK(shoal.boids[0].p.x > 400.);
    CHECK(shoal.boids[0].p.y < 400.);
    CHECK(shoal.boids[1].p.x < 395.);
    CHECK(shoal.boids[1].p.y > 405.);
    CHECK(approx_eq(shoal.boids[2].p, {400., 420}));

    shoal.boids.clear();
  }

  SUBCASE("Testing Boids alignment Function") {
    shoal.spawnCustomBoid(400., 400., 1., 0.);
    shoal.spawnCustomBoid(400., 410., 1., 0.);
    shoal.spawnCustomBoid(400., 420., -1., 0.);
    shoal.spawnCustomBoid(400., 430., 0., 1.);

    shoal.move_alignement();

    CHECK(shoal.boids[0].v.x < 1.);
    CHECK(shoal.boids[2].v.x > -1.);
    CHECK(shoal.boids[2].v.y > 0.);
    CHECK(shoal.boids[3].v.x > 0.);

    shoal.boids.clear();
  }

  SUBCASE("Testing Boids Cohesive Function") {
    shoal.spawnCustomBoid(400., 400., 1., 0.);
    shoal.spawnCustomBoid(500., 400., 1., 0.);
    shoal.spawnCustomBoid(400., 570., 0., -1.);

    shoal.move_cohesive();

    CHECK(shoal.boids[0].p.x > 1.);
    CHECK(approx_eq(shoal.boids[1].v, sf::Vector2f(1., 0.)));
    CHECK(approx_eq(shoal.boids[2].v, sf::Vector2f(0., -1.)));

    shoal.boids.clear();
  }

  SUBCASE("Testing Boids Escape Function") {
    shoal.spawnCustomBoid(400., 400., 0., 0.);
    shoal.spawnCustomBoid(600., 400., 0., 0.);
    shoal.spawnCustomPredator(500., 400., 1., 0.);

    shoal.move_escape();

    CHECK(shoal.boids[0].v.x < 0.);
    CHECK(shoal.boids[1].v.x > 0.);
  }
}

TEST_CASE("Testing Predator Hunting Functions") {
  Shoal shoal;
  shoal.InitShoal(0, 1000, 700, 0);

  SUBCASE("Testing Predator Move Hunting Function") {
    shoal.spawnCustomBoid(400., 400., 0., 0.);
    shoal.spawnCustomBoid(540., 400., 0., 0.);
    shoal.spawnCustomPredator(420., 400., 1., 0.);

    shoal.move_hunting();

    CHECK(shoal.predators[0].v.x > 1.);
    CHECK(shoal.predators[0].v.y == doctest::Approx(0.));

    shoal.boids.clear();
    shoal.predators.clear();
  }

  SUBCASE("Testing Killing") {
    shoal.spawnCustomBoid(385., 400., 0., 0.);
    shoal.spawnCustomBoid(400., 400., 0., 0.);
    shoal.spawnCustomBoid(420., 400., 0., 0.);
    shoal.spawnCustomPredator(380., 400., 1., 0.);

    shoal.killing();

    CHECK(shoal.getBoidsSize() == 2);
    CHECK(approx_eq(shoal.boids[1].p, {420., 400}));
  }
}

TEST_CASE("Testing Vectors' Operators Overloading") {
  sf::Vector2f v_0(1., 0.);
  sf::Vector2f v_1(1., 1.);
  sf::Vector2f v_2(0., -1.);
  float a{2};
  float b{-2};

  SUBCASE("Testing Operator +") {
    CHECK(approx_eq((v_0 + v_2), sf::Vector2f(1., -1.)));
    CHECK(approx_eq((v_1 + v_2), sf::Vector2f(1., 0.)));
  }
  SUBCASE("Testing Operator -") {
    CHECK(approx_eq((v_1 - v_1), sf::Vector2f(0., 0.)));
    CHECK(approx_eq((v_0 - v_2), sf::Vector2f(1., 1.)));
  }
  SUBCASE("Testing Operator *") {
    CHECK(approx_eq((v_1 * v_2), sf::Vector2f(0., -1.)));
    CHECK(approx_eq((v_1 * a), sf::Vector2f(2., 2.)));
  }
  SUBCASE("Testing Operator /") {
    CHECK(approx_eq((v_2 / v_1), sf::Vector2f(0., -1.)));
    CHECK(approx_eq((v_0 / b), sf::Vector2f(-0.5, 0.)));
    CHECK(approx_eq((a / v_1), sf::Vector2f(2., 2.)));
  }
}

TEST_CASE("Testing Vectors' Additional Functions") {
  sf::Vector2f v_0(1., 0.);
  sf::Vector2f v_1(1., 1.);
  sf::Vector2f v_2(-1., 1.);
  sf::Vector2f v_3(-7., -3.);
  sf::Vector2f v_4(2., -1.);

  SUBCASE("Testing Norm Function") {
    CHECK(norm(v_0) == doctest::Approx(1.));
    CHECK(norm(v_3) == doctest::Approx(std::sqrt(58)));
    CHECK(norm(v_2) == doctest::Approx(std::sqrt(2)));
  }

  SUBCASE("Testing GetRotation Function") {
    float r_3 = 180. + std::atan(3. / 7.) * (180 / (std::atan(1) * 4));
    float r_4 = -std::atan(1. / 2.) * (180 / (std::atan(1) * 4));

    CHECK(getRotation(v_0) == doctest::Approx(0.));
    CHECK(getRotation(v_3) == doctest::Approx(r_3));
    CHECK(getRotation(v_4) == doctest::Approx(r_4));
    CHECK(getRotation(v_2) == doctest::Approx(135.));
  }
}
