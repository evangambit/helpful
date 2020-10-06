// rm prog; g++ main.cpp -std=c++11 -o prog; ./prog

#include <iostream>

#include "mmap.hpp"
#include "Image.hpp"

double runiform() {
	return double(rand()) / RAND_MAX;
}

double interpolate(double t) {
	return 0.5 - std::cos(M_PI * t) / 2.0;
}

int main(int argc, char *argv[]) {
	// Demonstrate MMappedFile
	MMappedFile<char> file("main.cpp");
	std::cout << file.size() << std::endl;
	for (size_t i = 0; i < file.size(); ++i) {
		std::cout << file[i];
	}

	// Demonstrate Image by making Perlin Noise
	Image img(512, 512);
	img.fill_(Color(0, 0, 0));
	for (size_t octave = 1; octave < 9; ++octave) {
		size_t s = 1 << octave;
		Image layer(s, s);
		for (size_t y = 0; y < layer.height_; ++y) {
			for (size_t x = 0; x < layer.width_; ++x) {
				layer(y, x) = Color(
					runiform(),
					runiform(),
					runiform()
				);
			}
		}
		img += layer.resize(img.height_, img.width_, interpolate) / s;
	}
	img.save("perlin.png");

	return 0;
}