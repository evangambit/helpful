#pragma once

struct Color {
	Color() {}
	Color(double r, double g, double b) : r(r), g(g), b(b) {}
	double r, g, b;
	Color operator*(const Color& c) const {
		return Color(r * c.r, g * c.g, b * c.b);
	}
	Color operator+(const Color& c) const {
		return Color(r + c.r, g + c.g, b + c.b);
	}
	Color operator-(const Color& c) const {
		return Color(r - c.r, g - c.g, b - c.b);
	}
	Color operator/(const Color& c) const {
		return Color(r / c.r, g / c.g, b / c.b);
	}

	Color operator*(const double s) const {
		return Color(r * s, g * s, b * s);
	}
	Color operator+(const double s) const {
		return Color(r + s, g + s, b + s);
	}
	Color operator-(const double s) const {
		return Color(r - s, g - s, b - s);
	}
	Color operator/(const double s) const {
		return Color(r / s, g / s, b / s);
	}

	bool operator==(const Color& c) const {
		return (r == c.r) && (g == c.g) && (b == c.b);
	}

	Color& operator+=(const Color& c) {
		r += c.r;
		g += c.g;
		b += c.b;
		return *this;
	}
	Color& operator-=(const Color& c) {
		r -= c.r;
		g -= c.g;
		b -= c.b;
		return *this;
	}
	Color& operator*=(const Color& c) {
		r *= c.r;
		g *= c.g;
		b *= c.b;
		return *this;
	}
	Color& operator/=(const Color& c) {
		r /= c.r;
		g /= c.g;
		b /= c.b;
		return *this;
	}

	Color& operator+=(const double s) {
		r += s;
		g += s;
		b += s;
		return *this;
	}
	Color& operator-=(const double s) {
		r -= s;
		g -= s;
		b -= s;
		return *this;
	}
	Color& operator*=(const double s) {
		r *= s;
		g *= s;
		b *= s;
		return *this;
	}
	Color& operator/=(const double s) {
		r /= s;
		g /= s;
		b /= s;
		return *this;
	}
};

std::ostream& operator<<(std::ostream& ostream, const Color& c) {
	return ostream << "rgb(" << c.r << ", " << c.g << ", " << c.b << ")";
}
