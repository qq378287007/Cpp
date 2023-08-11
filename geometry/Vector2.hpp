#pragma once

#include <cmath>

class Vector2
{
public:
    explicit Vector2(double xpos = 0.0, double ypos = 0.0) : v{xpos, ypos} {}

    bool isNull() const { return v[0] == 0.0 && v[1] == 0.0; }
    double x() const { return v[0]; }
    double y() const { return v[1]; }
    void setX(double aX) { v[0] = aX; }
    void setY(double aY) { v[1] = aY; }

    double &operator[](int i) { return v[i]; }
    double operator[](int i) const { return v[i]; }

    double length() const { return v[0] * v[0] + v[1] * v[1]; }
    double lengthSquared() const { return v[0] * v[0] + v[1] * v[1]; }

    Vector2 normalized() const
    {
        double len = v[0] * v[0] + v[1] * v[1];
        if (len == 1.0)
            return *this;

        if (len == 0.0)
            return Vector2();

        double sqrtLen = std::sqrt(len);
        return Vector2(v[0] / sqrtLen, v[1] / sqrtLen);
    }

    void normalize()
    {
        double len = v[0] * v[0] + v[1] * v[1];
        if (len == 1.0 || len == 0.0)
            return;

        len = std::sqrt(len);
        v[0] /= len;
        v[1] /= len;
    }
    double distanceToPoint(const Vector2 &point) const { return (*this - point).length(); }
    double distanceToLine(const Vector2 &point, const Vector2 &direction) const
    {
        if (direction.isNull())
            return (*this - point).length();
        Vector2 p = point + dotProduct(*this - point, direction) * direction;
        return (*this - p).length();
    }

    Vector2 &operator+=(const Vector2 &vector)
    {
        v[0] += vector.v[0];
        v[1] += vector.v[1];
        return *this;
    }
    Vector2 &operator-=(const Vector2 &vector)
    {
        v[0] -= vector.v[0];
        v[1] -= vector.v[1];
        return *this;
    }
    Vector2 &operator*=(double factor)
    {
        v[0] *= factor;
        v[1] *= factor;
        return *this;
    }
    Vector2 &operator*=(const Vector2 &vector)
    {
        v[0] *= vector.v[0];
        v[1] *= vector.v[1];
        return *this;
    }
    Vector2 &operator/=(double divisor)
    {
        v[0] /= divisor;
        v[1] /= divisor;
        return *this;
    }
    Vector2 &operator/=(const Vector2 &vector)
    {
        v[0] /= vector.v[0];
        v[1] /= vector.v[1];
        return *this;
    }

    friend bool operator==(const Vector2 &v1, const Vector2 &v2);
    friend bool operator!=(const Vector2 &v1, const Vector2 &v2);
    friend const Vector2 operator+(const Vector2 &v1, const Vector2 &v2);
    friend const Vector2 operator-(const Vector2 &v1, const Vector2 &v2);
    friend const Vector2 operator*(double factor, const Vector2 &vector);
    friend const Vector2 operator*(const Vector2 &vector, double factor);
    friend const Vector2 operator*(const Vector2 &v1, const Vector2 &v2);
    friend const Vector2 operator-(const Vector2 &vector);
    friend const Vector2 operator/(const Vector2 &vector, double divisor);
    friend const Vector2 operator/(const Vector2 &vector, const Vector2 &divisor);
    friend bool qFuzzyCompare(const Vector2 &v1, const Vector2 &v2);

    static double dotProduct(const Vector2 &v1, const Vector2 &v2);

private:
    double v[2];
};

inline bool operator==(const Vector2 &v1, const Vector2 &v2)
{
    return v1.v[0] == v2.v[0] && v1.v[1] == v2.v[1];
}
inline bool operator!=(const Vector2 &v1, const Vector2 &v2)
{
    return v1.v[0] != v2.v[0] || v1.v[1] != v2.v[1];
}
inline const Vector2 operator+(const Vector2 &v1, const Vector2 &v2)
{
    return Vector2(v1.v[0] + v2.v[0], v1.v[1] + v2.v[1]);
}
inline const Vector2 operator-(const Vector2 &v1, const Vector2 &v2)
{
    return Vector2(v1.v[0] - v2.v[0], v1.v[1] - v2.v[1]);
}
inline const Vector2 operator*(double factor, const Vector2 &vector)
{
    return Vector2(vector.v[0] * factor, vector.v[1] * factor);
}
inline const Vector2 operator*(const Vector2 &vector, double factor)
{
    return Vector2(vector.v[0] * factor, vector.v[1] * factor);
}
inline const Vector2 operator*(const Vector2 &v1, const Vector2 &v2)
{
    return Vector2(v1.v[0] * v2.v[0], v1.v[1] * v2.v[1]);
}
inline const Vector2 operator-(const Vector2 &vector)
{
    return Vector2(-vector.v[0], -vector.v[1]);
}
inline const Vector2 operator/(const Vector2 &vector, double divisor)
{
    return Vector2(vector.v[0] / divisor, vector.v[1] / divisor);
}
inline const Vector2 operator/(const Vector2 &vector, const Vector2 &divisor)
{
    return Vector2(vector.v[0] / divisor.v[0], vector.v[1] / divisor.v[1]);
}
inline bool qFuzzyCompare(const Vector2 &v1, const Vector2 &v2)
{
    return v1.v[0] == v2.v[0] && v1.v[1] == v2.v[1];
}

double Vector2::dotProduct(const Vector2 &v1, const Vector2 &v2)
{
    return v1.v[0] * v2.v[0] + v1.v[1] * v2.v[1];
}
