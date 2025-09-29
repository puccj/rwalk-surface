#ifndef SURFACE_H
#define SURFACE_H

#include <vector>
#include <ostream>
#include <functional>

#include "utils.hpp"

//to do: template class T
class Surface {
  int _nPoints;
  Point* _data;
  std::function<double(double,double,double)> _phi = nullptr;
  double _h = 0;
  
 public:
  Surface(int nPoints = 0, Point data = Point{});
  Surface(int nPoints, Point* data);

  /**
   * @brief Creates a Surface object by sampling points near the zero level set of a scalar field function.
   *
   * This function evaluates the provided scalar field function `phi` over a 3D grid defined by the intervals `x`, `y`, and `z`
   * with spacing `h`. Points within a narrow band around the zero level set (where `phi` is close to zero) are collected
   * and used to construct a Surface object.
   *
   * @param phi A scalar field function of the form double(double x, double y, double z), typically representing an implicit surface.
   * @param x Interval specifying the minimum and maximum bounds in the x-direction.
   * @param y Interval specifying the minimum and maximum bounds in the y-direction.
   * @param z Interval specifying the minimum and maximum bounds in the z-direction.
   * @param h Grid spacing for sampling points in the domain.
   * @return Surface object containing points near the zero level set of `phi`.
   */
  Surface(std::function<double(double, double, double)> phi, Interval x, Interval y, Interval z, double h);

  Surface(const Surface &src);            //copy constructor
  Surface(Surface &&src);			            //move constructor
  Surface& operator=(const Surface &src); //copy assignment
  Surface& operator=(Surface &&src);	    //move assignment
  ~Surface();                             //destructor

  int nPoints() { return _nPoints; };
  Point operator[](int index) const { return _data[index]; };

  // Project point p onto the surface using the phi function provided at construction
  Point project(Point p); 

  // Snap p to the nearest point in the surface
  Point snap(Point p); 


  friend std::ostream& operator<<(std::ostream& os, const Surface& obj);
};

#endif  //SURFACE_H