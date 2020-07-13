#include "Vectors.h"

// copy constructor
Vector3D::Vector3D(const Vector3D& orig)
{
    x = orig.x;
    y = orig.y;
    z = orig.z;
}


std::ostream &operator << (std::ostream &stream, const Vector3D &a)
{
    std::ostringstream str;
    str << '(' << a.x << ',' << a.y << ',' << a.z << ')';
    return stream << str.str();
}

std::istream &operator >> (std::istream &stream, Vector3D &a)
{
   // bool success = false;
    char c = 0;

    stream >> c;
    if(c == '(')
    {
        stream >> a.x >> c;

        if(c == ',')
        {
            stream >> a.y >> c;

            if(c == ',')
            {
                stream >> a.z >> c;

                // if(c == ')') success = true;

            }
            else if(c == ')')
            {
                a.z = 0;
                // success = true;
            }
        }
    }
    return stream;
}

double Vector3D::normalize()
{
    double _x = (x > 0.0) ? x : -x;
    double _y = (y > 0.0) ? y : -y;
    double _z = (z > 0.0) ? z : -z;
    double denom = 1.;

    if(_x > _y)
    {
        if(_x > _z)
        {
            if(1.0 + _x > 1.0)
            {
                _y /= _x;
                _z /= _x;
                denom = 1.0 / (_x * sqrt(1.0 + _y * _y + _z * _z));
            }
        }
        else
        {
            if(1.0 + _z > 1.0)
            {
                _y /= _z;
                _x /= _z;
                denom = 1.0 / (_z * sqrt(1.0 + _y * _y + _x * _x));
            }
        }
    }
    else
    {
        if(_y > _z)
        {
            if(1.0 + _y > 1.0)
            {
                _z /= _y;
                _x /= _y;
                denom = 1.0 / (_y * sqrt(1.0 + _z * _z + _x * _x));
            }
        }
        else
        {
            if(1.0 + _z > 1.0)
            {
                _y /= _z;
                _x /= _z;
                denom = 1.0 / (_z * sqrt(1.0 + _y * _y + _x * _x));
            }
        }
    }

    if(1.0 + _x + _y + _z > 1.0)
    {
        x *= denom;
        y *= denom;
        z *= denom;
        return 1.0 / denom;
    }
    else
    {
        return 0.0;
    }
}

Vector3D normalize(const Vector3D &v)
{
    double x = (v.x > 0.0) ? v.x : -v.x;
    double y = (v.y > 0.0) ? v.y : -v.y;
    double z = (v.z > 0.0) ? v.z : -v.z;
    double denom = 1.;

    if(x > y)
    {
        if(x > z)
        {
            if(1.0 + x > 1.0)
            {
                y = y / x;
                z = z / x;
                denom = 1.0 / (x * sqrt(1.0 + y * y + z * z));
            }
        }
        else
        {
            if(1.0 + z > 1.0)
            {
                y = y / z;
                x = x / z;
                denom = 1.0 / (z * sqrt(1.0 + y * y + x * x));
            }
        }
    }
    else
    {
        if(y > z)
        {
            if(1.0 + y > 1.0)
            {
                z = z / y;
                x = x / y;
                denom = 1.0 / (y * sqrt(1.0 + z * z + x * x));
            }
        }
        else
        {
            if(1.0 + z > 1.0)
            {
                y = y / z;
                x = x / z;
                denom = 1.0 / (z * sqrt(1.0 + y * y + x * x));
            }
        }
    }

    if(1.0 + x + y + z > 1.0)
        return denom * v;
    else
        return v;

}

// copy constructor
Vector2D::Vector2D(const Vector2D& orig)
{
    x = orig.x;
    y = orig.y;
}
