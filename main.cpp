#include <cmath>
#include <iostream>
#include <functional>
#include <fstream>

#include "surface.h"

auto sphere = [](Point center, double r) {
  return [=](double x, double y, double z) {
    return std::sqrt((x-center.x)*(x-center.x) + (y-center.y)*(y-center.y) + (z-center.z)*(z-center.z)) - r;
  };
};

int N_WALKERS = 200;
int N_STEPS = 15000;
double STEP_SIZE = 0.5;
double GRID_H = 0.06;

int main() {
  // Define the domain and grid spacing
  Interval x = {0,10};
  Interval y = {0,10};
  Interval z = {0,10};

  Surface surf = Surface(sphere({5,5,5}, 4.5), x, y, z, GRID_H);
  std::cout << "Surface created with " << surf.nPoints() << " points.\n";
  Point final_pos[N_WALKERS];

  for (int walker = 0; walker < N_WALKERS; ++walker) {
    // Simulate a random walk on the surface
    Point current = {5,0.5,5}; //start point
    // std::cout << "Debug: Starting point: " << current << '\n';
    // current = surf.project(current);
    // std::cout << "Debug: Projected starting point: " << current << '\n';

    for (int step = 0; step < N_STEPS; ++step) {
      // chose a random direction (up, down, left, right, forward, backward)
      int direction = rand() % 6;
      Point next = current;
      switch(direction) {
        case 0: next.x += STEP_SIZE; break; //right
        case 1: next.x -= STEP_SIZE; break; //left
        case 2: next.y += STEP_SIZE; break; //up
        case 3: next.y -= STEP_SIZE; break; //down
        case 4: next.z += STEP_SIZE; break; //forward
        case 5: next.z -= STEP_SIZE; break; //backward
      }
      // std::cout << "Debug: Chosen direction: " << direction << ", next point before projection: " << next << '\n';

      // project back to the surface
      next = surf.project(next);
      // std::cout << "Debug: Projected point: " << next << '\n';
      
      current = next;
    }

    final_pos[walker] = current;
  }

  std::cout << "Simulation completed.\n";

  // save final positions to file
  std::ofstream fout("final_positions.dat");
  for (int i = 0; i < N_WALKERS; ++i) {
    fout << final_pos[i] << '\n';
  }
  fout.close();

  std::cout << "Final positions saved.\n";

  return 0;
}