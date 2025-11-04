#include "surface.h"
#include <limits>
#include <iostream>
#include <iomanip>
#include <functional>
#include <cmath>

Surface::Surface(int nPoints, Point data) :
                _nPoints{nPoints},
                _data{new Point[nPoints]} {
  std::fill(_data, _data + nPoints, data);
}

Surface::Surface(int nPoints, Point *data) :
                _nPoints{nPoints},
                _data{new Point[nPoints]} {
  std::copy(data, data + nPoints, _data);
}

Surface::Surface(std::function<double(double, double, double)> phi, Interval x, Interval y, Interval z, double h) :
                _nPoints{0},
                _data{nullptr},
                _phi{phi},
                _h{h} {

  int domainPoints = (x.max - x.min)*(y.max - y.min)*(z.max - z.min)/(h*h*h);
  Point* temp = new Point[domainPoints];

  double delta = 1.1 * sqrt(3) * h;

  for (double i = x.min; i < x.max; i += h) {
    for (double j = y.min; j < y.max; j += h) {
      for (double k = z.min; k < z.max; k += h) {
        double dist = phi(i,j,k);
        if (dist > -delta && dist < delta) {
          temp[_nPoints] = {i,j,k};
          ++_nPoints;
        }
      }
    }
  }

  _data = new Point[_nPoints];
  std::copy(temp, temp + _nPoints, _data);
  delete[] temp;
}

Surface::Surface(const Surface &src) : 
                _nPoints{src._nPoints},
                _data{new Point[src._nPoints]} {
    std::copy(src._data, src._data + src._nPoints, _data);
}

Surface::Surface(Surface &&src) : 
              _nPoints{src._nPoints},
              _data{src._data} {
  src._nPoints = 0;
  src._data = nullptr;
}

Surface &Surface::operator=(const Surface &src) {
  // Guard self assignment
  if (this == &src)
    return *this;

  if (_nPoints != src._nPoints) {           // resource in *this cannot be reused
    Point* temp = new Point[src._nPoints];  // allocate resource, if throws, do nothing
    delete[] _data;                         // release resource in *this
    _data = temp;
    _nPoints = src._nPoints;
  } 

  std::copy(src._data, src._data + src._nPoints, _data);
  return *this;
}

Surface &Surface::operator=(Surface &&src) {
  // Guard self assignment
  if (this == &src)
    return *this;

  delete[] _data;       // release resource in *this
  _data = src._data;
  _nPoints = src._nPoints;
  src._data = nullptr;  // leave src in valid state
  src._nPoints = 0;

  return *this;
}

Surface::~Surface() {
  delete[] _data;
}

Point Surface::project(Point p) const {
  if (_phi == nullptr || _h == 0) {
    throw std::runtime_error("Surface::project: phi function or h not defined. "
      "The surface needs to be constructed using a function to use project method.");
  }

  double x = p.x;
  double y = p.y;
  double z = p.z;

  double gradient[3];
  gradient[0] = (_phi(x+_h, y,    z   ) - _phi(x-_h, y,    z   )) / (2*_h);
  gradient[1] = (_phi(x,    y+_h, z   ) - _phi(x,    y-_h, z   )) / (2*_h);
  gradient[2] = (_phi(x,    y,    z+_h) - _phi(x,    y,    z-_h)) / (2*_h);

  // If gradient is zero, the point is already on the surface
  if (gradient[0] == 0 && gradient[1] == 0 && gradient[2] == 0) {
    return p;
  }

  double norm = std::sqrt(gradient[0]*gradient[0] + gradient[1]*gradient[1] + gradient[2]*gradient[2]);
  double dist = _phi(x, y, z);

  p.x = x - gradient[0]*dist/norm;
  p.y = y - gradient[1]*dist/norm;
  p.z = z - gradient[2]*dist/norm;

  return p;
}

Point Surface::snap(Point p) const {
  if (_nPoints == 0) {
    throw std::runtime_error("Surface::snap: surface has no points.");
  }

  // Approximate nearest neighbor search by snapping to the nearest grid point
  double x = (int)((p.x/_h)+0.5)*_h;
  double y = (int)((p.y/_h)+0.5)*_h;
  double z = (int)((p.z/_h)+0.5)*_h;

  return {x,y,z};
}

std::ostream &operator<<(std::ostream &os, const Surface &obj)
{
  //format: 3 cols of value: x y z
  for (int i = 0; i < obj._nPoints; ++i) {
    os << obj._data[i].x << ' ' << obj._data[i].y << ' ' << obj._data[i].z << '\n';
  }
  return os;
}