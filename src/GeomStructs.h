#ifndef __DEFINE_GEOMSTRUCTS__
#define __DEFINE_GEOMSTRUCTS__

// TODO: So far Vec3, Vec2 are essentially the same,
// so in future, we should make an abstract class
// for multi-dimensional vectors


struct Vec3{
	float x;
	float y;
	float z;

	// default and parameterised constructor
	Vec3(float x=0, float y=0, float z=0) : x(x), y(y), z(z){	}

	// Functions
	void normalise(void);
	float length(void);
	float dot(const Vec3&);
	Vec3 cross(const Vec3& v);
		
	// Assignment operator
	Vec3& operator=(const Vec3& a){
		x = a.x;
		y = a.y;
		z = a.z;
		return *this;
	}

	// Boolean operators;
	// equality is component-wise,
	// greater and less than compares magnitude
	bool operator==(const Vec3& a) const{
		return (x == a.x && y == a.y && z == a.z);
	}

	bool operator>(const Vec3& a) const{
		return ((x*x + y*y + z*z) > (a.x*a.x + a.y*a.y + a.z*a.z));
	}

	bool operator<(const Vec3& a) const{
		return ((x*x + y*y + z*z) < (a.x*a.x + a.y*a.y + a.z*a.z));
	}

	bool operator>=(const Vec3& a) const{
		return ((x*x + y*y + z*z) >= (a.x*a.x + a.y*a.y + a.z*a.z));
	}

	bool operator<=(const Vec3& a) const{
		return ((x*x + y*y + z*z) <= (a.x*a.x + a.y*a.y + a.z*a.z));
	}

	// Arithmatic operators
	Vec3 operator+(const Vec3& a) const{
		return Vec3(x+a.x, y+a.y, z+a.z);
	}

	Vec3 operator-(const Vec3& a) const{
		return Vec3(x-a.x, y-a.y, z-a.z);
	}

	Vec3 operator/(const Vec3& a) const{
		return Vec3(x/a.x, y/a.y, z/a.z);
	}

	Vec3 operator*(const Vec3& a) const{
		return Vec3(x*a.x, y*a.y, z*a.z);
	}

	Vec3 operator+(const float& a) const{
		return Vec3(x+a, y+a, z+a);
	}

	Vec3 operator-(const float& a) const{
		return Vec3(x-a, y-a, z-a);
	}

	Vec3 operator/(const float& a) const{
		return Vec3(x/a, y/a, z/a);
	}

	Vec3 operator*(const float& a) const{
		return Vec3(x*a, y*a, z*a);
	}

};

struct Vec2{
	float x;
	float y;

	// default + parameterized constructor
	Vec2(float x=0, float y=0) : x(x), y(y){	}
	
	// Assignment
	Vec2& operator=(const Vec2& a){
		x=a.x;
		y=a.y;
		return *this;
	}

	// Boolean;
	// equality is component-wise,
	// greater and less than compares magnitude
	bool operator==(const Vec2& a) const{
		return (x == a.x && y == a.y);
	}

	bool operator>(const Vec2& a) const{
		return ((x*x + y*y) > (a.x*a.x + a.y*a.y));
	}

	bool operator<(const Vec2& a) const{
		return ((x*x + y*y) < (a.x*a.x + a.y*a.y));
	}

	bool operator>=(const Vec2& a) const{
		return ((x*x + y*y) >= (a.x*a.x + a.y*a.y));
	}

	bool operator<=(const Vec2& a) const{
		return ((x*x + y*y) <= (a.x*a.x + a.y*a.y));
	}

	// Arithmatic
	Vec2 operator+(const Vec2& a) const{
		return Vec2(a.x+x, a.y+y);
	}

	Vec2 operator-(const Vec2& a) const{
		return Vec2(a.x-x, a.y-y);
	}

	Vec2 operator/(const Vec2& a) const{
		return Vec2(a.x/x, a.y/y);
	}

	Vec2 operator*(const Vec2& a) const{
		return Vec2(a.x*x, a.y*y);
	}
};

struct RGBA {
	float r;
	float g;
	float b;
	float a;

	// default + parameterized constructor
	RGBA(float r=0, float g=0, float b=0, float a=0) : r(r), g(g), b(b), a(a){	}
	
	// Assignment
	RGBA& operator=(const RGBA& c){
		r=c.r;
		g=c.g;
		b=c.b;
		a=c.a;
		return *this;
	}

	// Boolean;
	// equality is component-wise (excludes alpha),
	// greater and less than compares intensity (excludes alpha)
	bool operator==(const RGBA& c) const{
		return (r == c.r && g == c.g && b == c.b);
	}

	bool operator>(const RGBA& c) const{
		return ((r + g + b) > (c.r + c.g + c.b));
	}

	bool operator<(const RGBA& c) const{
		return ((r + g + b) < (c.r + c.g + c.b));
	}

	bool operator>=(const RGBA& c) const{
		return ((r + g + b) >= (c.r + c.g + c.b));
	}

	bool operator<=(const RGBA& c) const{
		return ((r + g + b) <= (c.r + c.g + c.b));
	}

	// Arithmatic (excludes alpha)
	RGBA operator+(const RGBA& c) const{
		return RGBA(c.r+r, c.g+g, c.b+b);
	}

	RGBA operator-(const RGBA& c) const{
		return RGBA(c.r-r, c.g-g, c.b-b);
	}

	RGBA operator/(const RGBA& c) const{
		return RGBA(c.r/r, c.g/g, c.b/b);
	}

	RGBA operator*(const RGBA& c) const{
		return RGBA(c.r*r, c.g*g, c.b*b);
	}
};

struct UVcoord {
	float u;
	float v;
};

struct Vert3D {
	Vec3 vertex;
	Vec3 normal;
	UVcoord uv_coord;
};

#endif


