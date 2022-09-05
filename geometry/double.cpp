constexpr double EPS = 1e-10;

bool zero(double x) {
	return abs(x) <= EPS;
}

// CORNER: point = (0, 0)
struct point {
	double x, y;
	
	point(double x=0, double y=0): x(x), y(y) {}
	
	point operator+(point rhs) { return point(x+rhs.x, y+rhs.y); }
	point operator-(point rhs) { return point(x-rhs.x, y-rhs.y); }
	point operator*(double k) { return point(x*k, y*k); }
	point operator/(double k) { return point(x/k, y/k); }
	double operator*(point rhs) { return x*rhs.x + y*rhs.y; }
	double operator^(point rhs) { return x*rhs.y - y*rhs.x; }

	point rotated(point p, point polar) { return point(*this^polar,*this*polar); }
	point rotated(point p, double ang) { return rotated(p, point(sin(ang),cos(ang))); }
	double norm2() { return *this * *this; }
	double norm() { return sqrt(norm2()); }

	bool operator<(const point& rhs) const {
		return x < rhs.x - EPS || (zero(x-rhs.x) && y < rhs.y - EPS);
	}
};

const point ccw90(1, 0), cw90(-1, 0);

// angular comparison in [0, 2pi)
// smallest is (1, 0)
// CORNER CASE: a, b == (0, 0)
bool ang_cmp(point a, point b) {
	auto quad = [](point p) -> bool {
		// 0 if ang in [0, pi), 1 if in [pi, 2pi)
		return p.y < 0 || (p.y == 0 && p.x < 0);
	};
	using tup = tuple<bool, double>;
	return tup{quad(a), 0} < tup{quad(b), a^b};
}

double dist2(point p, point q) { // squared distance
    return (p - q)*(p - q);
}

double dist(point p, point q) {
    return sqrt(dist2(p, q));
}

double area2(point a, point b, point c) { // two times signed area of triangle abc
	return (b - a) ^ (c - a);
}

bool left(point a, point b, point c) {
	return area2(a, b, c) > EPS; // counterclockwise
}

bool right(point a, point b, point c) {
	return area2(a, b, c) < -EPS; // clockwise
}

bool collinear(point a, point b, point c) {
	return zero(area2(a,b,c));
}

int parallel(point a, point b) {
	if((a ^ b) != 0) return 0;
	return (a.x>0) == (b.x>0) && (a.y > 0) == (b.y > 0) ? 1 : -1;
}

// CORNER: a == b
struct segment {
	point a, b;

	segment(point a=point(), point b=point()): a(a), b(b) {}

	//MAINTAIN a < b FOR SHAMOS-HOEY
	//segment(point a=point(), point b=point()): a(min(a,b)), b(max(a,b)) {}
	
	segment sorted() {
		return segment(min(a,b), max(a,b));
	}

	bool contains(point p) {
		double lx = a.x, ux = b.x, ly = a.y, uy = b.y;
		if(lx > ux) swap(lx, ux); 
		if(ly > uy) swap(ly, uy);
		return collinear(a, b, p)
			&& lx-EPS < p.x && p.x < ux+EPS
			&& ly-EPS < p.y && p.y < uy+EPS;
	}

	bool intersects(segment rhs) {
		segment lhs = *this;
		auto sign = [](int x) { if(zero(x)) return 0; return x > 0 ? 1 : -1; };
		if(sign(area2(lhs.a, lhs.b, rhs.a))*sign(area2(lhs.a, lhs.b, rhs.b)) == 1) return false;
		swap(lhs, rhs);
		if(sign(area2(lhs.a, lhs.b, rhs.a))*sign(area2(lhs.a, lhs.b, rhs.b)) == 1) return false;
		return true;
	}

	point v() { return b - a; }

};

bool parallel(segment r, segment s) {
	return parallel(r.v(), s.v());
}

point line_intersection(segment r, segment s) {
	if(parallel(r, s)) return point(HUGE_VAL, HUGE_VAL);
	point vr = r.v(), vs = s.v();
	double cr = vr ^ r.a, cs = vs ^ s.a;
	return (vs*cr - vr*cs) / (vr ^ vs);
}

point proj(segment r, point p) {
	auto [a, b] = r;
	p = p - a;
	point v = b-a;
	return a + v*((p*v)/(v*v));
}