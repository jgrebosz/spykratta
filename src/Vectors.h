///////////////////////////////////////////////////////////////////////////
//// 28/01/2008 E.Farnea --> modified from original version by A.Latina ///
////                                                                    ///
//// Treatment of 2D and 3D vectors                                     ///
///////////////////////////////////////////////////////////////////////////
#ifndef VECTOR_CLASS
#define VECTOR_CLASS

#include <iostream>
#include <sstream>
#include <cstring>
#include <cstdlib>
#include <cmath>

using namespace std;

class Vector2D;
class Vector3D;

class Vector3D
{
public:
    Vector3D(double _x = 0, double _y = 0, double _z = 0) : x(_x), y(_y), z(_z) {};
    // copy constructor
    Vector3D(const Vector3D& orig);
    ~Vector3D() {};

private:
    double x, y, z;

    // components
public:
    inline double X()
    {
        return x;
    };
    inline double Y()
    {
        return y;
    };
    inline double Z()
    {
        return z;
    };

public:
    friend Vector3D   operator + (const Vector3D &a, const Vector3D &b)
    {
        return Vector3D(a.x + b.x, a.y + b.y, a.z + b.z);
    };
    friend Vector3D   operator - (const Vector3D &a, const Vector3D &b)
    {
        return Vector3D(a.x - b.x, a.y - b.y, a.z - b.z);
    };
    friend Vector3D   operator - (const Vector3D &a)
    {
        return Vector3D(-a.x, -a.y, -a.z);
    };
    friend double     operator * (const Vector3D &a, const Vector3D &b)
    {
        return a.x * b.x + a.y * b.y + a.z * b.z;
    };
    friend Vector3D   operator * (const Vector3D &a, double d)
    {
        return Vector3D(a.x * d, a.y * d, a.z * d);
    };
    friend Vector3D   operator * (double d, const Vector3D &a)
    {
        return Vector3D(a.x * d, a.y * d, a.z * d);
    };
    friend Vector3D   operator / (const Vector3D &a, double d)
    {
        return Vector3D(a.x / d, a.y / d, a.z / d);
    };
    friend Vector3D   operator ^(const Vector3D &a, const Vector3D &b)
    {
        return Vector3D(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
    };

public:
    const Vector3D &operator  = (const Vector3D &v)
    {
        x = v.x;
        y = v.y;
        z = v.z;
        return *this;
    };
    const Vector3D &operator += (const Vector3D &v)
    {
        x += v.x;
        y += v.y;
        z += v.z;
        return *this;
    };
    const Vector3D &operator -= (const Vector3D &v)
    {
        x -= v.x;
        y -= v.y;
        z -= v.z;
        return *this;
    };
    const Vector3D &operator *= (double a)
    {
        x *= a;
        y *= a;
        z *= a;
        return *this;
    };
    const Vector3D &operator /= (double a)
    {
        x /= a;
        y /= a;
        z /= a;
        return *this;
    };
    const Vector3D &operator ^= (const Vector3D &v)
    {
        return *this = *this ^ v;
    };


public:
    friend std::ostream &operator << (std::ostream &stream, const Vector3D &a);
    friend std::istream &operator >> (std::istream &stream, Vector3D &a);

public:
    friend bool operator == (const Vector3D &a, const Vector3D &b)
    {
        return a.x == b.x && a.y == b.y && a.x == b.z;
    };
    friend bool operator != (const Vector3D &a, const Vector3D &b)
    {
        return a.x != b.x || a.y != b.y || a.z != b.z;
    };

public:
    double rho()   const
    {
        return sqrt(x * x + y * y + z * z);
    };
    double theta() const
    {
        return ((1. + rho() > 1.) ? (acos(z / sqrt(x * x + y * y + z * z))) : (0.));
    };
    double phi()   const
    {
        return atan2(y, x);
    };

public:
    double          normalize();
    friend Vector3D normalize(const Vector3D &v);

public:
    friend double norm(const Vector3D &v)
    {
        return v * v;
    };
    friend double abs(const Vector3D &v)
    {
        return sqrt(norm(v));
    };

public:
    Vector3D polar(double rho, double theta, double phi);
};


class Vector2D
{

public:
    Vector2D(double _x = 0, double _y = 0) : x(_x), y(_y) {};
    Vector2D(const Vector2D&);
    ~Vector2D() {};

private:
    double x, y;

    // components
public:
    inline double X()
    {
        return x;
    };
    inline double Y()
    {
        return y;
    };

public:
    friend Vector2D operator + (const Vector2D &a, const Vector2D &b)
    {
        return Vector2D(a.x + b.x, a.y + b.y);
    };
    friend Vector2D operator - (const Vector2D &a, const Vector2D &b)
    {
        return Vector2D(a.x - b.x, a.y - b.y);
    };
    friend Vector2D operator - (const Vector2D &a)
    {
        return Vector2D(-a.x, -a.y);
    };
    friend double   operator * (const Vector2D &a, const Vector2D &b)
    {
        return a.x * b.x + a.y * b.y;
    };
    friend Vector2D operator * (const Vector2D &a, double d)
    {
        return Vector2D(a.x * d, a.y * d);
    };
    friend Vector2D operator * (double d, const Vector2D &a)
    {
        return Vector2D(a.x * d, a.y * d);
    };
    friend Vector2D operator / (const Vector2D &a, double d)
    {
        return Vector2D(a.x / d, a.y / d);
    };

public:
    double rho() const
    {
        return sqrt(x * x + y * y);
    };
    double phi() const
    {
        return atan2(y, x);
    };

public:
    friend double norm(const Vector2D &v)
    {
        return v * v;
    };
    friend double abs(const Vector2D &v)
    {
        return sqrt(norm(v));
    };

public:
    inline Vector2D polar(double rho, double phi)
    {
        return Vector2D(rho * cos(phi), rho * sin(phi));
    };

public:
    operator Vector3D() const
    {
        return Vector3D(x, y, 0);
    };

public:
    const Vector2D &operator = (const Vector2D &v)
    {
        x = v.x;
        y = v.y;
        return *this;
    };
    const Vector2D &operator += (const Vector2D &v)
    {
        x += v.x;
        y += v.y;
        return *this;
    };
    const Vector2D &operator -= (const Vector2D &v)
    {
        x -= v.x;
        y -= v.y;
        return *this;
    };
    const Vector2D &operator *= (double a)
    {
        x *= a;
        y *= a;
        return *this;
    };
    const Vector2D &operator /= (double a)
    {
        x /= a;
        y /= a;
        return *this;
    };

public:
    friend bool operator == (const Vector2D &a, const Vector2D &b)
    {
        return a.x == b.x && a.y == b.y;
    };
    friend bool operator != (const Vector2D &a, const Vector2D &b)
    {
        return a.x != b.x || a.y != b.y;
    };

public:
    friend std::ostream &operator << (std::ostream &stream, const Vector2D &a)
    {
        return stream << "(" << a.x << ", " << a.y << ")";
    };

};

inline Vector3D Vector3D::polar(double rho, double theta, double phi)
{
    double rho_sin_theta = rho * sin(theta);
    return Vector3D(rho_sin_theta * cos(phi), rho_sin_theta * sin(phi), rho * cos(theta));
}


namespace
{
Vector3D operator + (const Vector2D &a, const Vector3D &b)
{
    return Vector3D(a) + b;
}
Vector3D operator + (const Vector3D &a, const Vector2D &b)
{
    return a + Vector3D(b);
}
}

#endif
