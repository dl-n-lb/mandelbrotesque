#include <stdio.h>
#include <complex>
#include <chrono>
#include <iostream>
#include "types/type_alias.hxx"
#define RAYLIB_WINDOW_IMPL
#define RAYLIB_RENDERER_IMPL
#include "graphics/raylib_window/raylib_window.hxx"

using complex64 = std::complex<f64>;


// TODO:
// SOME WAY TO USE SIMD WITH THE POW AND LOG FUNCTIONS
// (CUSTOM IMPLEMENTATION??)
// BUT IT NEEDS TO BE KEEPING VECTORIZATION

// NOTE:
// top doesnt work (deleted destructor since it assumes its a type)
// which doesnt work with complex (why does complex have destructor??)
// using csimd = nsimd_t<complex64, 4>;
// NOTE:
// this is undefined behaviour since std::complex is only defined over
// float, double and long double
using complex64v4 = std::complex<f64v4>;

struct coeff_t {
  complex64 coeff;
  bool is_relative; // will add the coeff to the 'c' coordinate if true
};

template<typename complex_type, size_t N>
class PolynomialFractal {
public:
  PolynomialFractal(arr<N, coeff_t> coeffs) : _coeffs(coeffs) {}
  
  complex_type iterate(complex_type z) {
    complex_type ret;
    for(f64 n = N; n >= 0; --n) {
      ret += _coeffs[N - n] * std::pow(z, n);
    }
    return ret;
  }
  
  bool inSet(complex_type z1, u64 iterations) {
    complex_type z = z1;
    f64 B = std::max(std::abs(z1), std::pow(2, 1.0/(N-1)));
    for(u64 i = 0; i < iterations; ++i) {
      z = iterate(z);
    }
    return std::abs(z) < B;
  }
  
  f64 smoothSet(complex_type z1, u64 iterations) {
    complex_type z = z1;
    f64 B = std::max(std::abs(z1), std::pow(2, 1.0/(N-1)));
    for (u64 i = 0; i < iterations; ++i) {
      z = iterate(z);
    }
    f64 sn = iterations - std::log(std::log(std::abs(z)) / std::log(B)) /
                          std::log(N);
    return sn;
  }
private:
  arr<N, coeff_t> _coeffs;
};

template<class Fractal, bool isGpuRendered = false>
class FractalViewer {
public: 
  FractalViewer(complex64 top_left, complex64 dims)
             : _top_left(top_left), _dims(dims) {}  
  
  auto getRenderer() {
    return [this] (RaylibWindow* win) {
      // lifetime thing
      auto renderer = win->getRenderer(RED);
      for (u32 y = 0; y < win->getHeight(); ++y){
        for (u32 x = 0; x < win->getWidth(); ++x) {
          
        }
      }
    };
  }
  
private:
  complex64 _top_left;
  complex64 _dims;
  Fractal frac;
};

const static u32 width = 800;
const static u32 height = 600;

// TODO:
// ALLOW MOVING AND SCALING THE SET
// DRAWING WITH GPU IS MUCH QUICKER
// SOME WAY TO BLIT TO SCREEN RATHER THAN DRAWING PIXELS 1 BY 1
// COLORS
// MORE ACCURATE METHODS (INIGO QUILEZ)

int main(int, char**) {
  RaylibWindow window{width, height, "Fractal Viewer"};
  SetTraceLogLevel(LOG_ERROR);
  
  // NOTE:
  // top doesnt work (deleted destructor since it assumes its a type)
  // which doesnt work with complex (why does complex have destructor??)
  using csimd = nsimd_t<complex64, 4>;
  // NOTE:
  // this is undefined behaviour since std::complex is only defined over
  // float, double and long double
  using simdc = std::complex<f64v4>;

  // THIS IS WHAT ASSIGMNENT MIGHT LOOK LIKE
  // NEED TO WRAP ?
  simdc a;
  f64v4 re;
  re[0] = 1;
  f64v4 im;
  im[0] = 1;
  a.real(re);
  a.imag(im);
  
  std::cout << a.real()[0] << " " << a.imag()[0] << std::endl;
  
  auto start = std::chrono::system_clock::now();
  u64 frames = 0;
  while(!window.shouldClose()) {
    ++frames;
    window.Update(viewer_fn);
  }
  
  auto dur =  std::chrono::duration_cast<std::chrono::seconds>
    (std::chrono::system_clock::now() - start).count();
  
  printf("Average FPS %lf\n", (f64)frames/dur);
  
  return 0;
}
