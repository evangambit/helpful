#pragma once

#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <cmath>
#include <functional>
#include <string>

#include "Color.hpp"

void bmp_write(const std::string& filename, const uint64_t height, const uint64_t width, uint8_t *data) {
	FILE *f;
	unsigned char *img = NULL;
	size_t filesize = 54 + 3 * width + height;

	uint8_t fileHeader[14] = {
    'B','M',  // Always "BM"
    static_cast<uint8_t>(filesize),     static_cast<uint8_t>(filesize>>8),
    static_cast<uint8_t>(filesize>>16), static_cast<uint8_t>(filesize>>24),
    0,0, 0,0, 54,0,0,0
  };
	uint8_t infoHeader[40] = {
    40,0,0,0,
    static_cast<uint8_t>(width),      static_cast<uint8_t>(width>>8),
    static_cast<uint8_t>(width>>16),  static_cast<uint8_t>(width>>24),
    static_cast<uint8_t>(height),     static_cast<uint8_t>(height>>8),
    static_cast<uint8_t>(height>>16), static_cast<uint8_t>(height>>24),
    1, 0,        // Number of planes (always 1)
    24, 0,       // Bits per pixel (1, 4, 8, 16, or 24)
    0, 0, 0, 0,  // No compression
    // The rest of the values are unused since we're not compressing.
  };

  // These values are ignored... might as well be zero.
	uint8_t pad[3] = {0,0,0};

	f = fopen(filename.c_str(), "wb");
	fwrite(fileHeader,1,14,f);
	fwrite(infoHeader,1,40,f);
  for (size_t y = height - 1; y < height; --y) {
      fwrite(data+(width*y*3), 3, width, f);
      // Every line is zero-padded to a multiple of 4 bytes (i.e.
      // a multiple of 4 pixels using 3 bytes per pixel).
      uint8_t wmod4 = (width * 3) % 4;
	    fwrite(pad, 1, (4 - wmod4) % 4, f);
	}

	fclose(f);
}

struct Image {
	Image(const uint64_t height, const uint64_t width)
	: width_(width), height_(height) {
    if (height_ * width_ == 0) {
      pixels_ = nullptr;
    } else {
  		pixels_ = new Color[height * width];
    }
	}
	Image(const Image& image) : height_(image.height_), width_(image.width_) {
		if (height_ * width_ == 0) {
      pixels_ = nullptr;
    } else {
      pixels_ = new Color[height_ * width_];
    }
	}
	Image& operator=(const Image& img) {
		delete[] pixels_;
		height_ = img.height_;
		width_ = img.width_;
    if (height_ * width_ == 0) {
      pixels_ = nullptr;
    } else {
      pixels_ = new Color[height_ * width_];
      std::memcpy(pixels_, img.pixels_, img.height_ * img.width_ * sizeof(Color));
    }
		return *this;
	}
	~Image() {
		delete[] pixels_;
	}
	Color operator()(uint64_t y, uint64_t x) const {
		return pixels_[y * width_ + x];
	}
	Color& operator()(uint64_t y, uint64_t x) {
		return pixels_[y * width_ + x];
	}
	Image operator+(const Image& img) const {
		Image r(height_, width_);
		for (uint64_t i = 0; i < height_ * width_; ++i) {
			r.pixels_[i] = pixels_[i] + img.pixels_[i];
		}
		return r;
	}
  Image operator-(const Image& img) const {
    Image r(height_, width_);
    for (uint64_t i = 0; i < height_ * width_; ++i) {
      r.pixels_[i] = pixels_[i] - img.pixels_[i];
    }
    return r;
  }
  Image operator*(const Image& img) const {
    Image r(height_, width_);
    for (uint64_t i = 0; i < height_ * width_; ++i) {
      r.pixels_[i] = pixels_[i] * img.pixels_[i];
    }
    return r;
  }
  Image operator/(const Image& img) const {
    Image r(height_, width_);
    for (uint64_t i = 0; i < height_ * width_; ++i) {
      r.pixels_[i] = pixels_[i] / img.pixels_[i];
    }
    return r;
  }
  Image operator+(const Color& color) const {
    Image r(height_, width_);
    for (uint64_t i = 0; i < height_ * width_; ++i) {
      r.pixels_[i] = pixels_[i] + color;
    }
    return r;
  }
  Image operator-(const Color& color) const {
    Image r(height_, width_);
    for (uint64_t i = 0; i < height_ * width_; ++i) {
      r.pixels_[i] = pixels_[i] - color;
    }
    return r;
  }
  Image operator*(const Color& color) const {
    Image r(height_, width_);
    for (uint64_t i = 0; i < height_ * width_; ++i) {
      r.pixels_[i] = pixels_[i] * color;
    }
    return r;
  }
  Image operator/(const Color& color) const {
    Image r(height_, width_);
    for (uint64_t i = 0; i < height_ * width_; ++i) {
      r.pixels_[i] = pixels_[i] / color;
    }
    return r;
  }
  Image operator+(double s) const {
  return *this + Color(s, s, s);
  }
  Image operator-(double s) const {
    return *this - Color(s, s, s);
  }
  Image operator*(double s) const {
    return *this * Color(s, s, s);
  }
  Image operator/(double s) const {
    return *this / Color(s, s, s);
  }
	Image& operator+=(double s) {
		for (uint64_t i = 0; i < height_ * width_; ++i) {
			pixels_[i] += s;
		}
		return *this;
	}
  Image& operator-=(double s) {
    for (uint64_t i = 0; i < height_ * width_; ++i) {
      pixels_[i] -= s;
    }
    return *this;
  }
	Image& operator*=(double s) {
		for (uint64_t i = 0; i < height_ * width_; ++i) {
			pixels_[i] *= s;
		}
		return *this;
	}
  Image& operator/=(double s) {
    for (uint64_t i = 0; i < height_ * width_; ++i) {
      pixels_[i] /= s;
    }
    return *this;
  }
  Image& operator+=(const Image& img) {
    if (img.height_ != height_) {
      std::cout << height_ << " != " << img.height_ << std::endl;
      throw std::runtime_error("Images have mismatching heights");
    }
    if (img.width_ != width_) {
      std::cout << width_ << " != " << img.width_ << std::endl;
      throw std::runtime_error("Images have mismatching widths");
    }
    for (uint64_t i = 0; i < height_ * width_; ++i) {
      pixels_[i] += img.pixels_[i];
    }
    return *this;
  }
  Image& operator-=(const Image& img) {
    if (img.height_ != height_) {
      std::cout << height_ << " != " << img.height_ << std::endl;
      throw std::runtime_error("Images have mismatching heights");
    }
    if (img.width_ != width_) {
      std::cout << width_ << " != " << img.width_ << std::endl;
      throw std::runtime_error("Images have mismatching widths");
    }
    for (uint64_t i = 0; i < height_ * width_; ++i) {
      pixels_[i] -= img.pixels_[i];
    }
    return *this;
  }
  Image& operator*=(const Image& img) {
    if (img.height_ != height_) {
      std::cout << height_ << " != " << img.height_ << std::endl;
      throw std::runtime_error("Images have mismatching heights");
    }
    if (img.width_ != width_) {
      std::cout << width_ << " != " << img.width_ << std::endl;
      throw std::runtime_error("Images have mismatching widths");
    }
    for (uint64_t i = 0; i < height_ * width_; ++i) {
      pixels_[i] *= img.pixels_[i];
    }
    return *this;
  }
  Image& operator/=(const Image& img) {
    if (img.height_ != height_) {
      std::cout << height_ << " != " << img.height_ << std::endl;
      throw std::runtime_error("Images have mismatching heights");
    }
    if (img.width_ != width_) {
      std::cout << width_ << " != " << img.width_ << std::endl;
      throw std::runtime_error("Images have mismatching widths");
    }
    for (uint64_t i = 0; i < height_ * width_; ++i) {
      pixels_[i] /= img.pixels_[i];
    }
    return *this;
  }
	Image log() const {
		Image r(height_, width_);
		for (uint64_t i = 0; i < height_ * width_; ++i) {
			r.pixels_[i].r = std::log(pixels_[i].r);
			r.pixels_[i].g = std::log(pixels_[i].g);
			r.pixels_[i].b = std::log(pixels_[i].b);
		}
		return r;
	}
	Image& log_() {
		for (uint64_t i = 0; i < height_ * width_; ++i) {
			pixels_[i].r = std::log(pixels_[i].r);
			pixels_[i].g = std::log(pixels_[i].g);
			pixels_[i].b = std::log(pixels_[i].b);
		}
		return *this;
	}
	Image clip(double low, double high) const {
		Image r(height_, width_);
		for (uint64_t i = 0; i < height_ * width_; ++i) {
			Color c = pixels_[i];
			r.pixels_[i].r = fminf(high, fmaxf(low, c.r));
			r.pixels_[i].g = fminf(high, fmaxf(low, c.g));
			r.pixels_[i].b = fminf(high, fmaxf(low, c.b));
		}
		return r;
	}
	Image& clip_(double low, double high) {
		for (uint64_t i = 0; i < height_ * width_; ++i) {
			Color c = pixels_[i];
			pixels_[i].r = fminf(high, fmaxf(low, c.r));
			pixels_[i].g = fminf(high, fmaxf(low, c.g));
			pixels_[i].b = fminf(high, fmaxf(low, c.b));
		}
		return *this;
	}
	void save(const std::string& filename) {
		uint8_t *bytes = new uint8_t[3 * width_ * height_];
		for (uint64_t i = 0; i < width_ * height_; ++i) {
			bytes[3*i+0] = fmaxf(0.0, fminf(255.0, pixels_[i].b * 255.0));
			bytes[3*i+1] = fmaxf(0.0, fminf(255.0, pixels_[i].g * 255.0));
			bytes[3*i+2] = fmaxf(0.0, fminf(255.0, pixels_[i].r * 255.0));
		}
		bmp_write(filename, height_, width_, bytes);
		delete[] bytes;
	}

  Image box_blur(const size_t h, const size_t w) const {
  	const Image& img = *this;
    Image sum(img.height_, img.width_);
    sum(0, 0) = img(0, 0);
    for(size_t y = 1; y < img.height_; y++) {
      sum(y, 0) = sum(y - 1, 0) + img(y, 0);
    }
    Color total;
    for(size_t x = 1; x < img.width_; x++) {
      total = img(0, x);
      sum(0, x) = sum(0, x - 1) + total;
      for(size_t y = 1; y < img.height_; y++) {
        total += img(y, x);
        sum(y, x) = sum(y, x - 1) + total;
      }
    }
    
    double area = (2 * h + 1) * (2 * w + 1);
    const size_t a = img.height_ - 1;
    const size_t b = img.width_ - 1;
    
    Image rtn(img.height_, img.width_);
    for(size_t y = 0; y < img.height_; y++) {
      for(size_t x = 0; x < img.width_; x++) {
        if(y + h < img.height_ && y - h - 1 < img.height_ && x + w < img.width_ && x - w - 1 < img.width_) {
          rtn(y, x) = (sum(y + h, x + w) + sum(y - h - 1, x - w - 1) - sum(y + h, x - w - 1) - sum(y - h - 1, x + w)) / area;
        }
        else if(y + h >= img.height_) {
          if(x + w >= img.width_) {
            rtn(y, x) = (sum(a, b) + sum(y - h - 1, x - w - 1) - sum(y - h - 1, b) - sum(a, x - w - 1)) / ((a - y + h + 1) * (b - x + w + 1));
          }
          else if(x - w - 1 >= img.width_) {
            rtn(y, x) = (sum(a, x + w) - sum(y - h - 1, x + w)) / ((a - y + h + 1) * (x + w + 1));
          }
          else {
            rtn(y, x) = (sum(a, x + w) + sum(y - h - 1, x - w - 1) - sum(a, x - w - 1) - sum(y - h - 1, x + w)) / ((a - y + h + 1) * (2 * w + 1));
          }
        }
        else if(y - h - 1 >= img.height_) {
          if(x + w >= img.width_) {
            rtn(y, x) = (sum(y + h, b) - sum(y + h, x - w - 1)) / ((y + h + 1) * (b - x + w + 1));
          }
          else if(x - w - 1 >= img.width_) {
            rtn(y, x) = sum(y + h, x + w) / ((y + h + 1) * (x + w + 1));
          }
          else {
            rtn(y, x) = (sum(y + h, x + w) - sum(y + h, x - w - 1)) / ((y + h + 1) * (2 * w + 1));
          }
        }
        else if(x + w >= img.width_) {
          rtn(y, x) = (sum(y + h, b) + sum(y - h - 1, x - w - 1) - sum(y + h, x - w - 1) - sum(y - h - 1, b)) / ((2 * h + 1) * (b - x + w + 1));
        }
        else if(x - w - 1 >= img.width_) {
          rtn(y, x) = (sum(y + h, x + w) - sum(y - h - 1, x + w)) / ((2 * h + 1) * (x + w + 1));
        }
      }
    }
    return rtn;
  }

  Image resize(size_t h, size_t w, std::function<double(double)> function) const {
    if(h == height_ && w == width_) {
      return *this;
    }
    if(h <= height_ && w <= width_) {
      Image rtn(h, w);
      const double boxHeight = double(height_) / h;
      const double boxWidth = double(width_) / w;
      
      for(size_t y = 0; y < h; y++) {
        for(size_t x = 0; x < w; x++) {
          rtn(y, x) = avgValueInBox(double(y * height_) / h, double(x * width_) / w, boxHeight, boxWidth);
        }
      }
      return rtn;
    }
    else if(h >= height_ && w >= width_) {
      // grow
      Image rtn(h, w);
      for(size_t y = 0; y < h; y++) {
        for(size_t x = 0; x < w; x++) {
          rtn(y, x) = interpolate(
            (y + .5) * height_ / h - .5,
            (x + .5) * width_ / w - .5,
            function
          );
        }
      }
      return rtn;
    }
    else {
      Image rtn(h, w);
      // else if one shrinks and one grows
      if(height_ > h) {
        rtn = resize(h, width_).resize(h, w);
      }
      else {
        rtn = resize(height_, w).resize(h, w);
      }
      return rtn;
    }
  }
  
  Image resize(size_t h, size_t w) const {
    return resize(h, w, [] (double v) -> double {
      return v;
    });
  }

  Image& fill_(const Color& color) {
    std::fill_n(pixels_, width_ * height_, color);
    return *this;
  }

 private:

	Color avgValueInBox(const double y1, const double x1, const double h, const double w) const {
    double x2 = x1 + w;
    double y2 = y1 + h;
    if(x2 >= width_) {
      x2 = width_ -  .0001;
    }
    if(y2 >= height_) {
      y2 = height_ - .0001;
    }
    const double a = ceil(x1) - x1;
    const double b = x2 - floor(x2);
    const double c = ceil(y1) - y1;
    const double d = y2 - floor(y2);
    
    Color sum = Color(0,0,0);
    
    const int y1int = int(y1);
    const int y2int = int(y2);
    const int x1int = int(x1);
    const int x2int = int(x2);
    
    // corners
    sum += (*this)(y1int, x1int) * (a * c); // upper left hand corner
    sum += (*this)(y1int, x2int) * (b * c); // upper right hand corner
    sum += (*this)(y2int, x1int) * (a * d); // lower left hand corner
    sum += (*this)(y2int, x2int) * (b * d); // lower right hand corner
    
    // left and right sides
    for(size_t y = ceil(y1); y < floor(y2); y++) {
      sum += (*this)(y, int(x1)) * a;
      sum += (*this)(y, int(x2)) * b;
    }
    
    // top and bottom sides
    for(size_t x = ceil(x1); x < floor(x2); x++) {
      sum += (*this)(y1int, x) * c;
      sum += (*this)(y2int, x) * d;
    }
    
    // tiles contained completely within the rectangle
    for(size_t x = ceil(x1); x < floor(x2); x++) {
      for(size_t y = ceil(y1); y < floor(y2); y++) {
        sum += (*this)(y, x);
      }
    }
    
    return sum / (w * h);
  }

  Color interpolate(double y, double x, std::function<double(double)> function) const {
    if(x < -.5 || x > width_ - .5) {
      return Color(0,0,0); // NAN
    }
    else if(y < -.5 || y > height_ - .5) {
      return Color(0,0,0); // NAN
    }

    size_t left = floor(x);
    size_t top = floor(y);
    double dx = function(x - left);
    double dy =  function(y - top);
    
    
    if(x < 0) {
      if(y < 0) {
        return (*this)(0, 0);
      }
      else if(y > height_ - 1) {
        return (*this)(height_ - 1, 0);
      }
      return (*this)(top, 0) * (1 - dy) + (*this)(top + 1, 0) * dy;
    }
    else if(x > width_ - 1) {
      if(y < 0) {
        return (*this)(0, width_ - 1);
      }
      else if(y > height_ - 1) {
        return (*this)(height_ - 1, width_ - 1);
      }
      return (*this)(top, width_ - 1) * (1 - dy) + (*this)(top + 1, width_ - 1) * dy;
    }
    
    if(y < 0) {
      return (*this)(0, left) * (1 - dx) + (*this)(0, left + 1) * dx;
    }
    else if(y > height_ - 1) {
      return (*this)(height_ - 1, left) * (1 - dx) + (*this)(height_ - 1, left + 1) * dx;
    }
    
    return ((*this)(top, left) * (1 - dx) + (*this)(top, left + 1) * dx) * (1 - dy) + ((*this)(top + 1, left) * (1 - dx) + (*this)(top + 1, left + 1) * dx) * dy;
  }

 public:

	Color *pixels_;
	uint64_t width_, height_;
};
