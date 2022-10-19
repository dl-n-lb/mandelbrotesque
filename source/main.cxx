#include <stdio.h>
#include <complex>
#include <chrono>
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

template<class Fractal, bool IsGPU = false>
class FractalViewer {
public:
  FractalViewer(f64 top_left_x, f64 top_left_y, f64 x_scale, f64 y_scale) : 
                tlx(top_left_x), tly(top_left_y), 
                scale_x(x_scale), scale_y(y_scale), f() {}
  
  auto getRenderer() {
    return [this](RaylibWindow* win) {
      win->getRenderer(RED);
      for (u32 y = 0; y < 600; ++y) {        
        for (u32 x = 0; x < 800; ++x) {
          f64 re = tlx + ((f64)x / 800.0) * scale_x;
          f64 im = tly + ((f64)y / 600.0) * scale_y;
          complex64 z = {re, im};
          auto color = (std::norm(f.iterate(z, 10)) > 4) ? RAYWHITE : BLACK;
          DrawPixel(x, y, color);
        }
      }    
    };
  }
  
  arr<800*600, Color> bake() {  
    arr<800*600, Color> pixels;
    for (u32 y = 0; y < 600; ++y) {        
      for (u32 x = 0; x < 800; ++x) {
        f64 re = tlx + ((f64)x / 800.0) * scale_x;
        f64 im = tly + ((f64)y / 600.0) * scale_y;
        complex64 z = {re, im};
        auto color = (std::abs(f.iterate(z, 100)) < 4) ? RAYWHITE : BLACK;
        pixels[800*y + x] = color;
      }
    }    
    return pixels;
  } 
  
  auto getRendererBaked() {
    auto pixels = bake();
    return [this, pixels] (RaylibWindow* win) {
      win->getRenderer(RED);
      for (u32 y = 0; y < 600; ++y) {
        for (u32 x = 0; x < 800; ++x) {
          DrawPixel(x, y, pixels[y * 800 + x]);
        }
      }
    };
  }
  
private:
  f64 scale_x, scale_y, tlx, tly;
  Fractal f;
};

const static u32 width = 800;
const static u32 height = 600;

int main(int, char**) {
  RaylibWindow window{width, height, "Fractal Viewer"};
  SetTraceLogLevel(LOG_ERROR);
  
  using Mandelbrot = Fractal<mandel_iter>;
  
  FractalViewer<Mandelbrot> mandel_viewer{0, 0, 0, 0};
  
  const auto viewer_fn = mandel_viewer.getRendererBaked();
  
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
