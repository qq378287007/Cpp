#pragma once

#include <cmath>

class Vector3
{
public:
    explicit Vector3(double xpos = 0.0, double ypos = 0.0, double zpos = 0.0) : v{xpos, ypos, zpos} {}

    bool isNull() const { return v[0] == 0.0 && v[1] == 0.0 && v[2] == 0.0; }
    double x() const { return v[0]; }
    double y() const { return v[1]; }
    double z() const { return v[2]; }
    void setX(double x) { v[0] = x; }
    void setY(double y) { v[1] = y; }
    void setZ(double z) { v[2] = z; }

    double &operator[](int i) { return v[i]; }
    double operator[](int i) const { return v[i]; }

    double length() const
    {
        double len = v[0] * v[0] + v[1] * v[1] + v[2] * v[2];
        return std::sqrt(len);
    }
    double lengthSquared() const { return v[0] * v[0] + v[1] * v[1] + v[2] * v[2]; }
    Vector3 normalized() const
    {
        double len = v[0] * v[0] + v[1] * v[1] + v[2] * v[2];
        if (len == 1.0)
            return *this;
        if (len == 0.0)
            return Vector3();

        double sqrtLen = std::sqrt(len);
        return Vector3(v[0] / sqrtLen, v[1] / sqrtLen, v[2] / sqrtLen);
    }
    void normalize()
    {
        double len = v[0] * v[0] + v[1] * v[1] + v[2] * v[2];
        if (len == 1.0 || len == 0.0)
            return;

        len = std::sqrt(len);
        v[0] /= len;
        v[1] /= len;
        v[2] /= len;
    }

    Vector3 &operator+=(const Vector3 &vector)
    {
        v[0] += vector.v[0];
        v[1] += vector.v[1];
        v[2] += vector.v[2];
        return *this;
    }
    Vector3 &operator-=(const Vector3 &vector)
    {
        v[0] -= vector.v[0];
        v[1] -= vector.v[1];
        v[2] -= vector.v[2];
        return *this;
    }
    Vector3 &operator*=(double factor)
    {
        v[0] *= factor;
        v[1] *= factor;
        v[2] *= factor;
        return *this;
    }
    Vector3 &operator*=(const Vector3 &vector)
    {
        v[0] *= vector.v[0];
        v[1] *= vector.v[1];
        v[2] *= vector.v[2];
        return *this;
    }
    Vector3 &operator/=(double divisor)
    {
        v[0] /= divisor;
        v[1] /= divisor;
        v[2] /= divisor;
        return *this;
    }
    inline Vector3 &operator/=(const Vector3 &vector)
    {
        v[0] /= vector.v[0];
        v[1] /= vector.v[1];
        v[2] /= vector.v[2];
        return *this;
    }

    static double dotProduct(const Vector3 &v1, const Vector3 &v2);
    static Vector3 crossProduct(const Vector3 &v1, const Vector3 &v2);
    static Vector3 normal(const Vector3 &v1, const Vector3 &v2);
    static Vector3 normal(const Vector3 &v1, const Vector3 &v2, const Vector3 &v3);
    /*
            Vector3 project(const QMatrix4x4 &modelView, const QMatrix4x4 &projection, const QRect &viewport) const;
            Vector3 unproject(const QMatrix4x4 &modelView, const QMatrix4x4 &projection, const QRect &viewport) const;
*/
    double distanceToPoint(const Vector3 &point) const
    {
        return (*this - point).length();
    }
    double distanceToPlane(const Vector3 &plane, const Vector3 &normal) const
    {
        return dotProduct(*this - plane, normal);
    }
    double distanceToPlane(const Vector3 &plane1, const Vector3 &plane2, const Vector3 &plane3) const
    {
        Vector3 n = normal(plane2 - plane1, plane3 - plane1);
        return dotProduct(*this - plane1, n);
    }
    double distanceToLine(const Vector3 &point, const Vector3 &direction) const
    {
        if (direction.isNull())
            return (*this - point).length();
        Vector3 p = point + dotProduct(*this - point, direction) * direction;
        return (*this - p).length();
    }

    friend inline bool operator==(const Vector3 &v1, const Vector3 &v2);
    friend inline bool operator!=(const Vector3 &v1, const Vector3 &v2);
    friend inline const Vector3 operator+(const Vector3 &v1, const Vector3 &v2);
    friend inline const Vector3 operator-(const Vector3 &v1, const Vector3 &v2);
    friend inline const Vector3 operator*(double factor, const Vector3 &vector);
    friend inline const Vector3 operator*(const Vector3 &vector, double factor);
    friend const Vector3 operator*(const Vector3 &v1, const Vector3 &v2);
    friend inline const Vector3 operator-(const Vector3 &vector);
    friend inline const Vector3 operator/(const Vector3 &vector, double divisor);
    friend inline const Vector3 operator/(const Vector3 &vector, const Vector3 &divisor);
    friend inline bool qFuzzyCompare(const Vector3 &v1, const Vector3 &v2);

private:
    double v[3];
};

double Vector3::dotProduct(const Vector3 &v1, const Vector3 &v2)
{
    return v1.v[0] * v2.v[0] + v1.v[1] * v2.v[1] + v1.v[2] * v2.v[2];
}
Vector3 Vector3::crossProduct(const Vector3 &v1, const Vector3 &v2)
{
    return Vector3(v1.v[1] * v2.v[2] - v1.v[2] * v2.v[1], v1.v[2] * v2.v[0] - v1.v[0] * v2.v[2], v1.v[0] * v2.v[1] - v1.v[1] * v2.v[0]);
}
Vector3 Vector3::normal(const Vector3 &v1, const Vector3 &v2)
{
    return crossProduct(v1, v2).normalized();
}
Vector3 Vector3::normal(const Vector3 &v1, const Vector3 &v2, const Vector3 &v3)
{
    return crossProduct((v2 - v1), (v3 - v1)).normalized();
}

inline bool operator==(const Vector3 &v1, const Vector3 &v2)
{
    return v1.v[0] == v2.v[0] && v1.v[1] == v2.v[1] && v1.v[2] == v2.v[2];
}
inline bool operator!=(const Vector3 &v1, const Vector3 &v2)
{
    return v1.v[0] != v2.v[0] || v1.v[1] != v2.v[1] || v1.v[2] != v2.v[2];
}
inline const Vector3 operator+(const Vector3 &v1, const Vector3 &v2)
{
    return Vector3(v1.v[0] + v2.v[0], v1.v[1] + v2.v[1], v1.v[2] + v2.v[2]);
}
inline const Vector3 operator-(const Vector3 &v1, const Vector3 &v2)
{
    return Vector3(v1.v[0] - v2.v[0], v1.v[1] - v2.v[1], v1.v[2] - v2.v[2]);
}
inline const Vector3 operator*(double factor, const Vector3 &vector)
{
    return Vector3(vector.v[0] * factor, vector.v[1] * factor, vector.v[2] * factor);
}
inline const Vector3 operator*(const Vector3 &vector, double factor)
{
    return Vector3(vector.v[0] * factor, vector.v[1] * factor, vector.v[2] * factor);
}
inline const Vector3 operator*(const Vector3 &v1, const Vector3 &v2)
{
    return Vector3(v1.v[0] * v2.v[0], v1.v[1] * v2.v[1], v1.v[2] * v2.v[2]);
}
inline const Vector3 operator-(const Vector3 &vector)
{
    return Vector3(-vector.v[0], -vector.v[1], -vector.v[2]);
}
inline const Vector3 operator/(const Vector3 &vector, double divisor)
{
    return Vector3(vector.v[0] / divisor, vector.v[1] / divisor, vector.v[2] / divisor);
}
inline const Vector3 operator/(const Vector3 &vector, const Vector3 &divisor)
{
    return Vector3(vector.v[0] / divisor.v[0], vector.v[1] / divisor.v[1], vector.v[2] / divisor.v[2]);
}

inline bool qFuzzyCompare(const Vector3 &v1, const Vector3 &v2)
{
    return v1.v[0] == v2.v[0] && v1.v[1] == v2.v[1] && v1.v[2] == v2.v[2];
}
