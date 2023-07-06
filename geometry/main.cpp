#include <cmath>
#include <utility>
#include <algorithm>
using namespace std;

const double eps = 1.0e-8;
const double PI = acos(-1.0);

int sgn(double x)
{
    if (fabs(x) < eps)
        return 0;

    if (x < 0)
        return -1;
    else
        return 1;
}

struct Point
{
    double x, y;
    Point(double _x = 0.0, double _y = 0.0) : x(_x), y(_y) {}
    Point operator-(const Point &b) const { return Point(x - b.x, y - b.y); }
    Point operator+(const Point &b) const { return Point(x + b.x, y + b.y); }

    double operator^(const Point &b) const { return x * b.y - y * b.x; } // 叉积
    double operator*(const Point &b) const { return x * b.x + y * b.y; } // 点积

    bool operator<(const Point &b) const { return x < b.x || (x == b.x && y < b.y); }
    bool operator==(const Point &b) const { return sgn(x - b.x) == 0 && sgn(y - b.y) == 0; }

    Point Rotate(double B, Point P) // 绕着点P，逆时针旋转角度B(弧度)
    {
        Point tmp;
        tmp.x = (x - P.x) * cos(B) - (y - P.y) * sin(B) + P.x;
        tmp.y = (x - P.x) * sin(B) + (y - P.y) * cos(B) + P.y;
        return tmp;
    }
};

double dist(Point a, Point b) { return sqrt((a - b) * (a - b)); } // 两点间距离
double len(Point a) { return sqrt(a.x * a.x + a.y * a.y); }       // 向量的长度

struct Line
{
    Point s, e;
    Line() {}
    Line(Point _s, Point _e) : s(_s), e(_e) {}

    // 两直线相交求交点
    // 第一个值为0表示直线重合，为1表示平行,为2是相交
    // 只有第一个值为2时，交点才有意义

    pair<int, Point> operator&(const Line &b) const
    {
        Point res = s;
        if (sgn((s - e) ^ (b.s - b.e)) == 0)
        {
            if (sgn((s - b.e) ^ (b.s - b.e)) == 0)
                return make_pair(0, res); // 重合
            else
                return make_pair(1, res); // 平行
        }
        double t = ((s - b.s) ^ (b.s - b.e)) / ((s - e) ^ (b.s - b.e));
        res.x += (e.x - s.x) * t;
        res.y += (e.y - s.y) * t;
        return make_pair(2, res);
    }
};

// 判断线段是否相交
// 返回1表示相交, 0表示不相交
bool inter(Line l1, Line l2)
{
    return max(l1.s.x, l1.e.x) >= min(l2.s.x, l2.e.x) &&
           max(l2.s.x, l2.e.x) >= min(l1.s.x, l1.e.x) &&
           max(l1.s.y, l1.e.y) >= min(l2.s.y, l2.e.y) &&
           max(l2.s.y, l2.e.y) >= min(l1.s.y, l1.e.y) &&
           sgn((l2.s - l1.e) ^ (l1.s - l1.e)) * sgn((l2.e - l1.e) ^ (l1.s - l1.e)) <= 0 &&
           sgn((l1.s - l2.e) ^ (l2.s - l2.e)) * sgn((l1.e - l2.e) ^ (l2.s - l2.e)) <= 0;
}

// 判断直线l1和线段l2是否相交
// 返回1表示相交,0表示不相交
bool Seg_inter_line(Line l1, Line l2)
{
    return sgn((l2.s - l1.e) ^ (l1.s - l1.e)) * sgn((l2.e - l1.e) ^ (l1.s - l1.e)) <= 0;
}

// 点到直线的距离
Point PointToLine(Point P, Line L)
{
    Point result;
    double t = ((P - L.s) * (L.e - L.s)) / ((L.e - L.s) * (L.e - L.s));
    result.x = L.s.x + (L.e.x - L.s.x) * t;
    result.y = L.s.y + (L.e.y - L.s.y) * t;
    return result;
}

// 点到线段的距离
// 点到线段上最近的点
Point NearestPointToLineSeg(Point P, Line L)
{
    Point result;
    double t = ((P - L.s) * (L.e - L.s)) / ((L.e - L.s) * (L.e - L.s));
    if (t >= 0 && t <= 1)
    {
        result.x = L.s.x + (L.e.x - L.s.x) * t;
        result.y = L.s.y + (L.e.y - L.s.y) * t;
    }
    else
    {
        if (dist(P, L.s) < dist(P, L.e))
            result = L.s;
        else
            result = L.e;
    }
    return result;
}

// 计算多边形面积,点的编号从0~n-1
double CalcArea(Point p[], int n)
{
    double res = 0;
    for (int i = 0; i < n; i++)
        res += (p[i] ^ p[(i + 1) % n]) / 2;
    return fabs(res);
}

// 判断点在线段上
bool OnSeg(Point P, Line L)
{
    return sgn((L.s - P) ^ (L.e - P)) == 0 &&
           sgn((P.x - L.s.x) * (P.x - L.e.x)) <= 0 &&
           sgn((P.y - L.s.y) * (P.y - L.e.y)) <= 0;
}

// 凸包Andrew算法
int ConvexHull(Point *p, int n, Point *ch) // 求凸包
{
    sort(p, p + n);
    n = unique(p, p + n) - p; // 去重
    int m = 0;
    for (int i = 0; i < n; ++i)
    {
        while (m > 1 && sgn((ch[m - 1] - ch[m - 2]) ^ (p[i] - ch[m - 1])) <= 0)
            --m;
        ch[m++] = p[i];
    }
    int k = m;
    for (int i = n - 2; i >= 0; i--)
    {
        while (m > k && sgn((ch[m - 1] - ch[m - 2]) ^ (p[i] - ch[m - 1])) <= 0)
            --m;
        ch[m++] = p[i];
    }
    if (n > 1)
        m--;
    return m;
}

// 极角排序
/*
叉积：对于 tmp = a x b
如果b在a的逆时针(左边):tmp > 0
顺时针(右边): tmp < 0
同向: tmp = 0
相对于原点的极角排序
如果是相对于某一点x,只需要把x当作原点即可
*/
bool mycmp(Point a, Point b)
{
    if (atan2(a.y, a.x) != atan2(b.y, b.x))
        return atan2(a.y, a.x) < atan2(b.y, b.x);
    else
        return a.x < b.x;
}

// 点形成一个逆时针凸包
/*
-1:点在凸多边形外
0:点在凸多边形边界上
1:点在凸多边形内
*/
int inConvexPoly(Point a, Point p[], int n)
{
    for (int i = 0; i < n; i++)
    {
        if (sgn((p[i] - a) ^ (p[(i + 1) % n] - a)) < 0)
            return -1;
        else if (OnSeg(a, Line(p[i], p[(i + 1) % n])))
            return 0;
    }
    return 1;
}

bool inConvex(Point A, Point *p, int tot)
{
    int l = 1, r = tot - 2, mid;
    while (l <= r)
    {
        mid = (l + r) >> 1;
        double a1 = (p[mid] - p[0]) ^ (A - p[0]);
        double a2 = (p[mid + 1] - p[0]) ^ (A - p[0]);
        if (a1 >= 0 && a2 <= 0)
        {
            if (((p[mid + 1] - p[mid]) ^ (A - p[mid])) >= 0)
                return true;
            return false;
        }
        else if (a1 < 0)
            r = mid - 1;
        else
            l = mid + 1;
    }
    return false;
}

// 判断点在任意多边形内，射线法
int inPoly(Point p, Point poly[], int n)
{
    int cnt;
    Line ray, side;
    cnt = 0;
    ray.s = p;
    ray.e.y = p.y;
    ray.e.x = -100000000000.0; //-INF,注意取值防止越界

    for (int i = 0; i < n; i++)
    {
        side.s = poly[i];
        side.e = poly[(i + 1) % n];

        if (OnSeg(p, side))
            return 0;

        // 如果平行轴则不考虑
        if (sgn(side.s.y - side.e.y) == 0)
            continue;

        if (OnSeg(side.s, ray))
        {
            if (sgn(side.s.y - side.e.y) > 0)
                cnt++;
        }
        else if (OnSeg(side.e, ray))
        {
            if (sgn(side.e.y - side.s.y) > 0)
                cnt++;
        }
        else if (inter(ray, side))
            cnt++;
    }
    if (cnt % 2 == 1)
        return 1;
    else
        return -1;
}

// 判断凸多边形
// 点是顺时针或者逆时针
bool isconvex(Point poly[], int n)
{
    bool s[3] = {false, false, false};
    for (int i = 0; i < n; i++)
    {
        s[sgn((poly[(i + 1) % n] - poly[i]) ^ (poly[(i + 2) % n] - poly[i])) + 1] = true;
        if (s[0] && s[2])
            return false;
    }
    return true;
}

// 判断凸包是否相离
bool isConvexHullSeparate(int n, int m, Point a[], Point b[])
{
    for (int i = 0; i < n; i++)
        if (inPoly(a[i], b, m) != -1)
            return false;

    for (int i = 0; i < m; i++)
        if (inPoly(b[i], a, n) != -1)
            return false;

    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < m; j++)
        {
            Line l1 = Line(a[i], a[(i + 1) % n]);
            Line l2 = Line(b[j], b[(j + 1) % m]);
            if (inter(l1, l2))
                return false;
        }
    }
    return true;
}

//返回凸包A和B的闵可夫斯基和的凸包M上的点的数量
//由于可能三点共线，需要对M再求一次凸包(主函数内进行)
const int MAX = 2e5 + 5;
Point s1[MAX], s2[MAX];

int Minkowski(Point A[], int n, Point B[], int m, Point M[])
{
    int tot = 0;
    for (int i = 0; i < n; i++)
        s1[i] = A[(i + 1) % n] - A[i];

    for (int i = 0; i < m; i++)
        s2[i] = B[(i + 1) % m] - B[i];

    M[tot] = A[0] + B[0];
    int p1 = 0, p2 = 0;

    while (p1 < n && p2 < m)
        ++tot, M[tot] = M[tot - 1] + ((s1[p1] ^ s2[p2]) >= 0 ? s1[p1++] : s2[p2++]);
    while (p1 < n)
        ++tot, M[tot] = M[tot - 1] + s1[p1++];
    while (p2 < m)
        ++tot, M[tot] = M[tot - 1] + s2[p2++];
    return tot + 1;
}



int main()
{
    return 0;
}