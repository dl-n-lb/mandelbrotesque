#include <stdio.h>
#include <complex>
#include "types/type_alias.hxx"
#define RAYLIB_WINDOW_IMPL
#define RAYLIB_RENDERER_IMPL
#include "graphics/raylib_window/raylib_window.hxx"

using complex64 = std::complex<f64>;

template<auto IterationFn>
class Fractal{
public:
  Fractal() {} 
  // TODO: accept different # of args
  complex64 iterate(complex64 c, u32 iter_max) {
    complex64 z = c;
    for (u32 i = 0; i < iter_max; ++i) {
      z = IterationFn(z, c);
    }

    return z;
  }
};

complex64 mandel_iter(complex64 z, complex64 c) {
  return std::pow(z, 2) + c;
}

class FractalViewer {
public:
  FractalViewer(f64 x_scale, f64 y_scale) : 
                scale_x(x_scale), scale_y(y_scale) {}
private:
  f64 scale_x, scale_y;
};

const static u32 width = 800;
const static u32 height = 600;

int main(int, char**) {
  printf("Hello World");
  RaylibWindow window{width, height, "Fractal Viewer"};
  
  const auto update_fn = [](RaylibWindow* win){};
  
  while(!window.shouldCloseouldClose()) {
    window.Update(update_fn);
  }
  
  printf("Goodbye!");
  
  return 0;
}
