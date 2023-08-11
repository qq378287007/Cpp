

#include <string.h>
#include <float.h>

#include <cmath>
#include <vector>
#include <algorithm>
using namespace std;

typedef unsigned char byte;
struct Color
{
	byte m_r;
	byte m_g;
	byte m_b;
	byte m_a;

	Color(byte r = 255, byte g = 255, byte b = 255, byte a = 255)
	{
		m_r = r;
		m_g = g;
		m_b = b;
		m_a = a;
	}
};

struct Point
{
	double m_x;
	double m_y;
	Color m_color;

	Point(double x = 0.0, double y = 0.0, Color color = Color())
	{
		m_x = x;
		m_y = y;
		m_color = color;
	}
};

class Canvas
{
public:
	int m_width;
	int m_height;
	Color *m_buffer;

public:
	Canvas(int width, int height)
	{
		m_width = width;
		m_height = height;
		m_buffer = (Color *)malloc(m_width * m_height * sizeof(Color));
		memset(m_buffer, 0, m_width * m_height * sizeof(Color));
	}
	~Canvas()
	{
		if (m_buffer != NULL)
			free(m_buffer);
	}

	// 排序
	static inline bool MySort(const Point &p1, const Point &p2)
	{
		return p1.m_y > p2.m_y;
	}

	// 颜色插值
	static inline Color ColorLerp(Color color1, Color color2, double scale)
	{
		byte m_r = color1.m_r + (color2.m_r - color1.m_r) * scale;
		byte m_g = color1.m_g + (color2.m_g - color1.m_g) * scale;
		byte m_b = color1.m_b + (color2.m_b - color1.m_b) * scale;
		byte m_a = color1.m_a + (color2.m_a - color1.m_a) * scale;
		return Color(m_r, m_g, m_b, m_a);
	}

	// 清屏
	void ClearColorBuffer()
	{
		if (m_buffer != NULL)
			memset(m_buffer, 0, m_width * m_height * sizeof(Color));
	}

	// 画点
	void DrawPoint(Point p)
	{
		if (p.m_x < 0 || p.m_x > m_width - 1 || p.m_y < 0 || p.m_y > m_height - 1)
			return;
		m_buffer[int(p.m_y * m_width + p.m_x)] = p.m_color;
	}

	// 画线
	void DrawLine(Point p1, Point p2)
	{
		int deltaX = abs(p2.m_x - p1.m_x);
		int deltaY = abs(p2.m_y - p1.m_y);

		int xNow = p1.m_x;
		int yNow = p1.m_y;
		Point pNow(xNow, yNow, p1.m_color);

		int stepX = 0;
		int stepY = 0;

		if (p2.m_x > p1.m_x)
			stepX = 1;
		else
			stepX = -1;

		if (p2.m_y > p1.m_y)
			stepY = 1;
		else
			stepY = -1;

		int pk = 0; // 决策参数
		bool useX = true;
		int sumStep = 0; // 需要步进的次数
		// 斜率大于1
		if (deltaY > deltaX)
		{
			useX = false;
			sumStep = deltaY;
			int t = deltaX;
			deltaX = deltaY;
			deltaY = t;
		}
		else
		{
			useX = true;
			sumStep = deltaX;
		}
		pk = 2 * deltaY - deltaX;

		for (int i = 0; i < sumStep; i++)
		{
			DrawPoint(pNow);

			// 颜色插值
			double scale = 0.0;
			Color color;
			if (useX)
			{
				if (p2.m_x == p1.m_x)
				{ // 此时两个点重合
					scale = 0.0;
				}
				else
				{
					scale = (xNow - p1.m_x) / (p2.m_x - p1.m_x);
				}
			}
			else
			{
				if (p2.m_y == p1.m_y)
				{ // 此时两个点重合
					scale = 0.0;
				}
				else
				{
					scale = (yNow - p1.m_y) / (p2.m_y - p1.m_y);
				}
			}
			color = ColorLerp(p1.m_color, p2.m_color, scale);
			pNow.m_color = color;

			if (pk > 0)
			{
				if (useX)
				{
					yNow += stepY;
				}
				else
				{
					xNow += stepX;
				}

				// 只有pk>0时才有这一项
				pk = pk - 2 * deltaX;
			}

			// 主坐标不管pk是何值都要步进
			if (useX)
			{
				xNow += stepX;
			}
			else
			{
				yNow += stepY;
			}
			pNow.m_x = xNow;
			pNow.m_y = yNow;

			// 无论pk为何值都有这一项
			pk = pk + 2 * deltaY;
		}
	}

	// 画三角形
	void DrawTriangle1(Point p1, Point p2, Point p3) // 包围盒
	{
		// 计算包围盒的范围
		int left = (int)p1.m_x;
		int right = (int)p2.m_x;
		int bottom = (int)p1.m_y;
		int top = (int)p1.m_y;

		// 初始化三角形三条边的k值和b值
		// 将k值初始化为单精度浮点数的最大值，代表直线垂直于x轴的情况
		double k1 = FLT_MAX;
		double k2 = FLT_MAX;
		double k3 = FLT_MAX;
		double b1 = 0.0;
		double b2 = 0.0;
		double b3 = 0.0;

		if (p3.m_x != p2.m_x)
		{
			k1 = (p3.m_y - p2.m_y) / (p3.m_x - p2.m_x);
			b1 = p3.m_y - k1 * p3.m_x;
		}
		if (p3.m_x != p1.m_x)
		{
			k2 = (p3.m_y - p1.m_y) / (p3.m_x - p1.m_x);
			b2 = p1.m_y - k2 * p1.m_x;
		}
		if (p1.m_x != p2.m_x)
		{
			k3 = (p1.m_y - p2.m_y) / (p1.m_x - p2.m_x);
			b3 = p2.m_y - k3 * p2.m_x;
		}

		double judge1 = 0.0;
		double judge2 = 0.0;
		double judge3 = 0.0;

		for (int x = left; x < right; x++)
		{
			for (int y = bottom; y < top; y++)
			{
				if (k1 != FLT_MAX)
				{
					judge1 = (y - k1 * x - b1) * (p1.m_y - k1 * p1.m_x - b1);
				}
				else
				{
					judge1 = (x - p2.m_x) * (p1.m_x - p2.m_x);
				}
				if (k2 != FLT_MAX)
				{
					judge2 = (y - k2 * x - b2) * (p2.m_y - k2 * p2.m_x - b2);
				}
				else
				{
					judge2 = (x - p3.m_x) * (p2.m_x - p3.m_x);
				}
				if (k3 != FLT_MAX)
				{
					judge3 = (y - k3 * x - b3) * (p3.m_y - k3 * p3.m_x - b3);
				}
				else
				{
					judge3 = (x - p1.m_x) * (p3.m_x - p1.m_x);
				}

				if (judge1 >= 0 && judge2 >= 0 && judge3 >= 0)
				{
					Point p(x, y, p1.m_color);
					DrawPoint(p);
				}
			}
		}
	}

	void DrawTriangleFlat(Point p1, Point p2, Point p0) // 扫描线法画平顶平底三角形，p0为y值与另外两个点不同的点
	{
		float k1 = FLT_MAX;
		float k2 = FLT_MAX;
		float b1 = 0.0f;
		float b2 = 0.0f;

		Color ColorBegin1;
		Color ColorBegin2;
		Color ColorEnd1;
		Color ColorEnd2;

		if (p1.m_x != p0.m_x)
		{
			k1 = (p1.m_y - p0.m_y) / (p1.m_x - p0.m_x);
			b1 = p0.m_y - k1 * p0.m_x;
		}
		if (p2.m_x != p0.m_x)
		{
			k2 = (p2.m_y - p0.m_y) / (p2.m_x - p0.m_x);
			b2 = p0.m_y - k2 * p0.m_x;
		}

		// 计算y值的取值范围，由于p1与p2的y值相同，所以这里我就只用p1的y值了
		// float yStart = GetValue(p1.m_y, p0.m_y, <);
		// float yEnd = GetValue(p1.m_y, p0.m_y, >);
		float yStart = p1.m_y;
		float yEnd = p0.m_y;

		// 计算颜色插值，y值从小到大
		if (p0.m_y < p1.m_y)
		{
			ColorBegin1 = p0.m_color;
			ColorBegin2 = p0.m_color;
			ColorEnd1 = p1.m_color;
			ColorEnd2 = p2.m_color;
		}
		else
		{
			ColorBegin1 = p1.m_color;
			ColorBegin2 = p2.m_color;
			ColorEnd1 = p0.m_color;
			ColorEnd2 = p0.m_color;
		}

		// 记录颜色比例和步长
		float ColorStep = 1.0f / (yEnd - yStart);
		float ColorStart = yStart;

		for (float y = yStart; y <= yEnd; y++)
		{
			float x1 = 0.0f;
			float x2 = 0.0f;
			if (k1 == FLT_MAX)
			{
				x1 = p0.m_x; // 说明p2与p0x值相同
			}
			else
			{
				x1 = (y - b1) / k1;
			}

			if (k2 == FLT_MAX)
			{
				x2 = p0.m_x; // 说明p1与p0x值相同
			}
			else
			{
				x2 = (y - b2) / k2;
			}

			float scale = (y - ColorStart) * ColorStep;
			Color color1 = ColorLerp(ColorBegin1, ColorEnd1, scale);
			Color color2 = ColorLerp(ColorBegin2, ColorEnd2, scale);

			Point pt1(x1, y, color1);
			Point pt2(x2, y, color2);

			DrawLine(pt1, pt2);
		}
	}

	void DrawTriangle2(Point p1, Point p2, Point p3) // 扫描线法绘制任意三角形
	{
		std::vector<Point> pv;
		pv.push_back(p1);
		pv.push_back(p2);
		pv.push_back(p3);
		std::sort(pv.begin(), pv.end(), MySort);

		Point pMax = pv[0];
		Point pMid = pv[1];
		Point pMin = pv[2];

		if (pMax.m_y == pMid.m_y)
		{
			DrawTriangleFlat(pMax, pMid, pMin);
			return;
		}

		if (pMin.m_y == pMid.m_y)
		{
			DrawTriangleFlat(pMin, pMid, pMax);
			return;
		}

		// 计算直线pMaxpMin的k值和b值
		float k = FLT_MAX;
		float b = 0.0f;

		if (pMax.m_x != pMin.m_x)
		{
			k = (pMax.m_y - pMin.m_y) / (pMax.m_x - pMin.m_x);
			b = pMax.m_y - k * pMax.m_x;
		}

		Point p;
		p.m_y = pMid.m_y;
		if (k != FLT_MAX)
		{
			p.m_x = (p.m_y - b) / k;
		}
		else
		{
			p.m_x = pMin.m_x;
		}
		float scale = (p.m_y - pMin.m_y) / (pMax.m_y - pMin.m_y);
		p.m_color = ColorLerp(pMin.m_color, pMax.m_color, scale);

		DrawTriangleFlat(p, pMid, pMax);
		DrawTriangleFlat(p, pMid, pMin);
	}
};

int main()
{
	Canvas canvas(800, 600);

	Color color1(255, 0, 0, 0);
	Color color2(0, 255, 0, 0);
	Color color3(0, 0, 255, 0);
	Point p1(100, 100, color1);
	Point p2(200, 300, color2);
	Point p3(400, 400, color3);
	//canvas.DrawTriangle2(p1, p2, p3);

	return 0;
}
