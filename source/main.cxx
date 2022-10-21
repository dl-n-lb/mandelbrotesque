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
// addition may work but things like std::norm will be broken
// almost definitely
using complex64v4 = std::complex<f64v4>;

struct coeff_t {
  complex64 coeff;
  f64 c_factor; // will add the coeff to the 'c' coordinate if true
};

template<typename complex_type, size_t N>
class PolynomialFractal {
public:
  PolynomialFractal(arr<N, coeff_t> coeffs) : _coeffs(coeffs) {}
  
  complex_type iterate(complex_type z, complex_type c) {
    complex_type ret;
    for(f64 n = N; n >= 0; --n) {
      ret += (_coeffs[N - n].coeff + _coeffs[N-n].c_factor*c) * std::pow(z, n);
    }
    return ret;
  }
  
  bool inSet(complex_type z1, u64 iterations) {
    complex_type z = z1;
    f64 B = std::max(std::abs(z1), std::pow(2, 1.0/(N-1)));
    for(u64 i = 0; i < iterations && std::abs(z) < B; ++i) {
      z = iterate(z, z1);
    }
    return std::abs(z) < B;
  }
  
  f64 smoothSet(complex_type z1, u64 iterations) {
    complex_type z = z1;
    f64 B = std::max(std::abs(z1), std::pow(2, 1.0/(N-1)));
    for (u64 i = 0; i < iterations; ++i) {
      z = iterate(z, z1);
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
  FractalViewer(complex64 top_left, complex64 dims, Fractal frac)
             : _top_left(top_left), _dims(dims), _frac(frac) {}  
  
  auto getRenderer() {
    return [this] (RaylibWindow* win) {
      // lifetime thing
      auto renderer = win->getRenderer(RED);
      u32 winw = win->getWidth();
      u32 winh = win->getHeight();
      for (u32 y = 0; y < winh; ++y){
        for (u32 x = 0; x < winw; ++x) {
          complex64 coord = _top_left + complex64{_dims.real() * (f64)x / winw,
                                                 _dims.imag() * (f64)y / winh};
          DrawPixel(x, y, (_frac.inSet(coord, 100) ? BLACK : WHITE));
        }
      }
    };
  }
  
  auto getBakedRenderer(RaylibWindow* win) {
    vec<f64> inSet;
    u32 winw = win->getWidth();
    u32 winh = win->getHeight();
    for (u32 y = 0; y < winh; ++y){
      for (u32 x = 0; x < winw; ++x) {
        complex64 coord = _top_left + complex64{_dims.real() * (f64)x / winw,
                                               _dims.imag() * (f64)y / winh};
        inSet.push_back(_frac.inSet(coord, 100) ? 1.0 : 0);
      }
    }
    return [this, inSet] (RaylibWindow* win) {
      auto renderer = win->getRenderer(RED);
      u32 winw = win->getWidth();
      u32 winh = win->getHeight();
      for (u32 y = 0; y < winh; ++y){
        for (u32 x = 0; x < winw; ++x) {
          complex64 coord = _top_left + complex64{_dims.real() * (f64)x / winw,
                                                 _dims.imag() * (f64)y / winh};
          DrawPixel(x, y, (inSet[x + winw * y] == 1) ? WHITE : BLACK);
        }
      }
    };
  }
  
private:
  complex64 _top_left;
  complex64 _dims;
  Fractal _frac;
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

  PolynomialFractal<complex64, 2> frac = {{coeff_t{0, 0}, coeff_t{0, 0}}};
  
  FractalViewer mandel_view = {complex64{-1.5, -1}, complex64{3, 2}, frac};  
  
  auto viewer_fn = mandel_view.getBakedRenderer(&window);
  
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
