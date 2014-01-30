#ifndef VEC_HPP
#define VEC_HPP

#include <cmath>
#include <complex>
#include <functional>
#ifndef M_PI
#define M_PI 3.1415926535
#endif

struct vec2;
struct vec3;

struct vec2 {

    // Constructors
    vec2():
        x(0.f), y(0.f)
    {}
    vec2(float X, float Y):
         x(X), y(Y)
    {}
    vec2(const vec3&);

    // Logical Operators
    bool operator==(const vec2 &A) {

        return (x == A.x && y == A.y ? true : false);

    }
    bool operator!=(const vec2 &A) {

        return (x != A.x || y != A.y ? true : false);

    }
    bool operator>(const vec2 &A) {

        return (x > A.x && y > A.y ? true : false);

    }
    bool operator<(const vec2 &A) {

        return (x < A.x && y < A.y ? true : false);

    }
    bool operator>=(const vec2 &A) {

        return (x >= A.x && y >= A.y ? true : false);

    }
    bool operator<=(const vec2 &A) {

        return (x <= A.x && y <= A.y ? true : false);

    }

    // Mathematical Operators
    vec2 operator+(const vec2 &A) {

        return vec2(x + A.x, y + A.y);

    }
    vec2 operator+(float A) {

        return vec2(x + A, y + A);

    }
    vec2& operator+=(const vec2 &A) {

        x+= A.x, y+= A.y;
        return (*this);

    }
    vec2& operator+=(float A) {

        x+= A, y+= A;
        return (*this);

    }

    vec2 operator*(float A) {

        return vec2(x * A, y * A);

    }

    vec2 operator-(const vec2 &A) {

        return vec2(x - A.x, y - A.y);

    }
    vec2 operator-(float A) {

        return vec2(x - A, y - A);

    }
    vec2 operator-() {

        return vec2(-x, -y);

    }
    vec2& operator-=(const vec2 &A) {

        x-= A.x, y-= A.y;
        return (*this);

    }
    vec2& operator-=(float A) {

        x-= A, y-= A;
        return (*this);

    }

    vec2 operator/(float A) {

        return vec2(x / A, y / A);

    }

    vec2 operator*(const vec2 &A) {

        return vec2(x * A.x, y * A.y);

    }

    float x, y;

};

struct vec3 {

    // Constructors
    vec3():
         x(0.f), y(0.f), z(0.f)
    {}
    vec3(float X, float Y, float Z):
         x(X), y(Y), z(Z)
    {}
    vec3(const vec2&);

    // Logical Operators
    bool operator==(const vec3 &A) {

        return (x == A.x && y == A.y && z == A.z ? true : false);

    }
    bool operator!=(const vec3 &A) {

        return (x != A.x || y != A.y || z != A.z ? true : false);

    }
    bool operator>(const vec3 &A) {

        return (x > A.x && y > A.y && z > A.z ? true : false);

    }
    bool operator<(const vec3 &A) {

        return (x < A.x && y < A.y && z < A.z ? true : false);

    }
    bool operator>=(const vec3 &A) {

        return (x >= A.x && y >= A.y && z >= A.z ? true : false);

    }
    bool operator<=(const vec3 &A) {

        return (x <= A.x && y <= A.y && z <= A.z ? true : false);

    }

    // Mathematical Operators
    vec3 operator+(const vec3 &A) {

        return vec3(x + A.x, y + A.y, z + A.z);

    }
    vec3 operator+(float A) {

        return vec3(x + A, y + A, z + A);

    }
    vec3& operator+=(const vec3 &A) {

        x+= A.x, y+= A.y, z+= A.z;
        return (*this);

    }
    vec3& operator+=(float A) {

        x+= A, y+= A, z+= A;
        return (*this);

    }

    vec3 operator-(const vec3 &A) {

        return vec3(x - A.x, y - A.y, z - A.z);

    }
    vec3 operator-(float A) {

        return vec3(x - A, y - A, z - A);

    }
    vec3 operator-() {

        return vec3(-x, -y, -z);

    }
    vec3& operator-=(const vec3 &A) {

        x-= A.x, y-= A.y, z-= A.z;
        return (*this);

    }
    vec3& operator-=(float A) {

        x-= A, y-= A, z-= A;
        return (*this);

    }

    vec3 operator/(float A) {

        return vec3(x / A, y / A, z / A);

    }

    float x, y, z;

};

// Allows us to convert between 2D and 3D vectors.
vec2::vec2(const vec3 &A):
           x(A.x), y(A.z)
{}
vec3::vec3(const vec2 &A):
           x(A.x), y(0.f), z(A.y)
{}

// Additional "Euclidean Vector" functions.
float dot(vec3 A, vec3 B) {

    return (A.x * B.x + A.y * B.y + A.z * B.z);

}
float dot(vec2 A, vec2 B) {

    return (A.x * B.x + A.y * B.y);

}

float sqr(float A) { return A * A; }

float distance(vec2 A, vec2 B) {

    vec2 result = (A - B);
    return sqrt(std::abs(sqr(result.x) + sqr(result.y)));

}
float distance(vec3 A, vec3 B) {

    vec3 result(A - B);
    return sqrt(std::abs(sqr(result.x) + sqr(result.y) + sqr(result.z)));

}
template<typename t>
short sign(t A) {

    return (A == 0 ? 0 : (A < 0 ? -1 : 1));

}
float length(vec2 A) {

    return sqrt(std::abs(sqr(A.x) + sqr(A.y)));

}
float length(vec3 A) {

    return sqrt(std::abs(sqr(A.x) + sqr(A.y) + sqr(A.z)));

}

vec2 normalise(vec2 A) {

    return (A / length(A));

}
vec3 normalise(vec3 A) {

    return (A / length(A));

}

// Radian/Degree conversions.
float deg2rad(float deg) {

    return deg * M_PI / 180;

}
float rad2deg(float rad) {

    return rad / M_PI * 180;

}
float angBetween(vec2 Dir1, vec2 Dir2) { // Return the angle in degrees between 2 direction vectors.

    // acos == cos^-1
    return rad2deg(std::acos(dot(Dir1, Dir2) / (length(Dir1) * length(Dir2))));

}
float angBetweenN(vec2 Dir1, vec2 Dir2) {

    return rad2deg(std::fmod(std::atan2((Dir2.y - Dir1.y), (Dir2.x - Dir1.x)), (2 * M_PI))); // No idea how this works.

}

#endif // VEC_HPP
