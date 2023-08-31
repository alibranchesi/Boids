#include "shoal.hpp"
#include "simulation.hpp"

int main()

{
  std::cout
      << "\n\nHOW TO USE\n\nAfter these instructions you will be prompted to "
         "enter: "
         "\n-the initial speed of boids and predators (it must be a positive "
         "value from 0 to 10, we suggest 5)\n-the initial number of boids (it "
         "must be a "
         "positive integer from 0 to 200, we suggest 50)\n-the maximum number "
         "of predators "
         "(it must be a positive integer from 0 to 10, we suggest "
         "2).\n\nDuring the "
         "simulation you will be allowed to able and disable some "
         "functions:\n-press S for the separation rule\n-press X for the "
         "alignement rule\n-press C for the cohesion rule\n-press P to remove "
         "all the "
         "predators on the screen\n-press K to allow sharks to eat "
         "fish\n-press W "
         "to switch mode (acquarium VS periodical border "
         "conditions)\n-press M to show the center of mass "
         "of the shoal\n\nDuring the simulation you will also be able to spawn "
         "boids and predators with your mouse:\n-to spawn a boid click with "
         "the "
         "left button on the window (as long as the number of active fishes is "
         "less then or\n"
         "equal to 800!)"
         "\n-to spawn a predator click with the right "
         "button on the "
         "window (as long as the number of active predators is less"
         "\nthen or equal to "
         "10!)\n\n------------------------------------------------------"
         "-------------------"
         "------\n\n\n";

  Simulation simulation;

  while (simulation.running() && !simulation.getEndSimulation()) {
    simulation.update();
  }
}