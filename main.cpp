#include <cmath>
#include <iostream>
#include <functional>
#include <fstream>
#include <random>
#include <filesystem>

#include "surface.h"

//convert double to string with 2 decimal places
auto to_string2 = [](double value) {
  std::ostringstream out;
  out << std::fixed << std::setprecision(2) << value;
  return out.str();
};

auto sphere = [](Point center, double r) {
  return [=](double x, double y, double z) {
    return std::sqrt((x-center.x)*(x-center.x) + (y-center.y)*(y-center.y) + (z-center.z)*(z-center.z)) - r;
  };
};

void simulate(Surface const& surf, Point startingPoint, double stepSize, int nSteps,
              bool snap = false, int nWalkers = 10000, std::string outputDir = "data") {

  Point walkers[nWalkers];
  for (int w = 0; w < nWalkers; ++w) {
    walkers[w] = startingPoint;
  }

  std::random_device dev;
  std::mt19937 rng(dev());
  std::uniform_int_distribution<std::mt19937::result_type> dist(0,5); // distribution in range [0,5]

  // create output directory recursively
  std::filesystem::create_directories(outputDir);
  std::ofstream fout;
  for (int step = 0; step < nSteps; ++step) {
    if ((step) % 10 == 0) {
      std::string filename = outputDir + "/step" + std::to_string(step) + ".dat";
      fout.open(filename);
    }

    for (int w = 0; w < nWalkers; ++w) {
      // chose a random direction (up, down, left, right, forward, backward)
      int direction = dist(rng);
      
      // log position every 10 steps
      if (step % 10 == 0) {
        fout << walkers[w] << '\n';
      }

      switch(direction) {
        case 0: walkers[w].x += stepSize; break; //right
        case 1: walkers[w].x -= stepSize; break; //left
        case 2: walkers[w].y += stepSize; break; //up
        case 3: walkers[w].y -= stepSize; break; //down
        case 4: walkers[w].z += stepSize; break; //forward
        case 5: walkers[w].z -= stepSize; break; //backward
      }

      // project back to the surface
      walkers[w] = surf.project(walkers[w]);

      // optionally, snap to nearest point
      walkers[w] = surf.snap(walkers[w]);
    }
    fout.close();
  }

  // Log a final time
  std::string filename = "stepsize=" + to_string2(stepSize) + "_step" + std::to_string(nSteps) + ".dat";
  fout.open(filename);
  for (int w = 0; w < nWalkers; ++w) {
    fout << walkers[w] << '\n';
  }
  fout.close();

  std::cout << "Simulation completed: " << nWalkers << " walkers, " << nSteps << " steps each, step size " << stepSize << ".\n";
}


// Default parameters
double STEP_SIZE = 2;
int N_STEPS = 10000;
bool SNAP = true;
int N_WALKERS = 10000;
double GRID_H = 0.06;

int main(int argc, char** argv) {
  // Show help message
  for (int i = 1; i < argc; ++i) {
    std::string arg = argv[i];
    if (arg == "-h" || arg == "--help") {
      std::cout << "Usage: " << argv[0] << " [STEP_SIZE] [N_STEPS] [N_WALKERS] [GRID_H]\n";
      std::cout << "  STEP_SIZE: Size of each step (default: 0.5)\n";
      std::cout << "  N_STEPS:   Number of steps for each walker (default: 1000)\n";
      std::cout << "  SNAP:      Whether to snap to surface or not (default: false)\n";
      std::cout << "  N_WALKERS: Number of walkers to simulate (default: 10000)\n";
      std::cout << "  GRID_H:    Grid spacing for surface construction (default: 0.06)\n";
      return 0;
    }
  }
  // Parse command line arguments
  if (argc > 1) STEP_SIZE = std::stod(argv[1]);
  if (argc > 2) N_STEPS = std::stoi(argv[2]);
  if (argc > 3) SNAP = std::stoi(argv[3]);
  if (argc > 4) N_WALKERS = std::stoi(argv[4]);
  if (argc > 5) GRID_H = std::stod(argv[5]);

  for (double size = 0.1; size <= STEP_SIZE; size += 0.1) {
    std::cout << "Running simulation with step size: " << size << "\n";
  // Define the domain and grid spacing
  Interval x = {0,10};
  Interval y = {0,10};
  Interval z = {0,10};

  Surface surf = Surface(sphere({5,5,5}, 4.5), x, y, z, GRID_H);
  std::cout << "Surface created with " << surf.nPoints() << " points.\n";

  Point right = {9.5, 5, 5};
    
    std::string outputDir;
    if (SNAP) 
      outputDir = "data/snap/stepSize=" + to_string2(size) + "_nWalkers=" + std::to_string(N_WALKERS);
    else
      outputDir = "data/nosnap/stepSize=" + to_string2(size) + "_nWalkers=" + std::to_string(N_WALKERS);
  
    simulate(surf, right, size, N_STEPS, SNAP, N_WALKERS, outputDir);
  }

  return 0;
}