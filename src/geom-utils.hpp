#pragma once

#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <math.h>
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "logger.hpp"

namespace geom
{
	// angle between segments ab and bc
	inline int getAngle(const cv::Point2d &a, const cv::Point2d &b, const cv::Point2d &c)
	{
		cv::Point2d ab = b - a;
		cv::Point2d cb = b - c;

		double dotProd = ab.x * cb.x + ab.y * cb.y;
		double abLenSqr = ab.x * ab.x + ab.y * ab.y;
		double cbLenSqr = cb.x * cb.x + cb.y * cb.y;
		double cosSqr = dotProd * dotProd / (abLenSqr * cbLenSqr);

		// cos(2 * alpha) = 2 * (cos(alpha))^2 - 1
		double cos2 = 2 * cosSqr - 1;

		double alpha2 =
				(cos2 <= -1) ? M_PI :
				(cos2 >= 1) ? 0 :
				acos(cos2);

		double angle = (alpha2 / 2) * 180.0f / M_PI;

		if (dotProd < 0)
			angle = 180.0f - angle;

		double det = (ab.x * cb.y - ab.y * cb.x);

		if (det < 0)
			angle = -angle;

		return static_cast<int>(floor(angle + 0.5));
	}

	// returns true if segments AB and CD intersect each other
	inline bool intersect(
			const cv::Point2d &a,
			const cv::Point2d &b,
			const cv::Point2d &c,
			const cv::Point2d &d)
	{
		return (((c.x - a.x) * (b.y - a.y) - (c.y - a.y) * (b.x - a.x))
		        * ((d.x - a.x) * (b.y - a.y) - (d.y - a.y) * (b.x - a.x)) < 0
		        && ((a.x - c.x) * (d.y - c.y) - (a.y - c.y) * (d.x - c.x))
		           * ((b.x - c.x) * (d.y - c.y) - (b.y - c.y) * (d.x - c.x)) < 0);
	}

	// returns true if quadrangle is resembling a rectangle
	inline bool checkQuadrangle(std::vector<cv::Point2d> &quad)
	{
		if (!intersect(quad[0], quad[2], quad[1], quad[3]))
			return false;

		if (intersect(quad[0], quad[1], quad[2], quad[3]) ||
		    intersect(quad[1], quad[2], quad[3], quad[0]))
			return false;

		int minAng = 180, maxAng = 0;

		for (size_t i = 0; i < 4; i++)
		{
			int ang = abs(getAngle(quad[i], quad[(i + 1) % 4], quad[(i + 2) % 4]));
			minAng = std::min(minAng, ang);
			maxAng = std::max(maxAng, ang);
		}

		if (minAng < 20 || maxAng < 70)
			return false;

		double norm0 = norm(quad[0]);

		if (norm0 > norm(quad[1]) &&
		    norm0 > norm(quad[2]) &&
		    norm0 > norm(quad[3]))
			return false;

		double minEdge, maxEdge;
		minEdge = maxEdge = norm(quad[0] - quad[1]);

		for (int i = 1; i < 4; i++)
		{
			double Edge = norm(quad[i] - quad[(i + 1) % 4]);
			minEdge = std::min(minEdge, Edge);
			maxEdge = std::max(maxEdge, Edge);
		}
		return minEdge > 15 && (maxEdge / minEdge < 20);
	}

	// returns centroid of a polygon
	static cv::Point2d centroid(std::vector<cv::Point2d> polygon)
	{
		cv::Point2d c(0, 0);
		double a = 0;

		polygon.push_back(polygon[0]);

		for (size_t i = 0; i < polygon.size() - 1; i++)
		{
			double xyyx = polygon[i].x * polygon[i + 1].y - polygon[i + 1].x * polygon[i].y;
			a += xyyx;
			c.x += (polygon[i].x + polygon[i + 1].x) * xyyx;
			c.y += (polygon[i].y + polygon[i + 1].y) * xyyx;
		}

		c.x /= 3 * a;
		c.y /= 3 * a;

		return c;
	};

	class Quadrangle
	{
		// coefficients of a line equations for each edge
		double a[4] = {0}, b[4] = {0}, c[4] = {0};
		cv::Point2d cent; // a centroid

		// returns distance from point p to i-th edge
		double distToLine(cv::Point2d p, int i)
		{
			return a[i] * p.x + b[i] * p.y + c[i];
		}

	public:
		// calculate coefficients and a centroid
		explicit Quadrangle(std::vector<cv::Point2d> quad)
		{
			expect(quad.size() == 4);
			quad.push_back(quad[0]);

			for (size_t i = 0; i < 4; i++)
			{
				a[i] = -(quad[i + 1].y - quad[i].y);
				b[i] = quad[i + 1].x - quad[i].x;
				c[i] = -(a[i] * quad[i].x + b[i] * quad[i].y);
			}

			cent = centroid(quad);
		}

		// returns true if the point is inside of a quadrangle
		bool surrounds(const cv::Point2d &point)
		{
			// for each edge check whether the point and a centroid are on the same side
			for (int i = 0; i < 4; i++)
				if (copysign(1.0, distToLine(point, i)) !=
				    copysign(1.0, distToLine(cent, i)))
					return false;
			return true;
		}
	};
}


#endif
