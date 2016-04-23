#pragma once

#ifndef GEOMETRY_H
#define GEOMETRY_H

#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"

using namespace cv;

namespace geom
{

	// angle between ab and bc
	inline int getAngle(Point2f a, Point2f b, Point2f c)
	{
		Point2f ab = b - a;
		Point2f cb = b - c;

		float dotProd = ab.x * cb.x + ab.y * cb.y;
		float abLenSqr = ab.x * ab.x + ab.y * ab.y;
		float cbLenSqr = cb.x * cb.x + cb.y * cb.y;
		float cosSqr = dotProd * dotProd / (abLenSqr * cbLenSqr);

		// cos(2 * alpha) = 2 * (cos(alpha))^2 - 1
		float cos2 = 2 * cosSqr - 1;

		const float pi = 3.141592f;

		float alpha2 =
			(cos2 <= -1) ? pi :
			(cos2 >= 1) ? 0 :
			acosf(cos2);

		float angle = (alpha2 / 2) * 180.0f / pi;

		if (dotProd < 0)
			angle = 180.0f - angle;

		float det = (ab.x * cb.y - ab.y * cb.x);

		if (det < 0)
			angle = -angle;

		return (int) (floor(angle + 0.5));
	}

	// check if segments AB and CD intersect each other
	inline bool haveIntersection(Point2f a, Point2f b, Point2f c, Point2f d) {

		return (((c.x - a.x)*(b.y - a.y) - (c.y - a.y)*(b.x - a.x))
			  * ((d.x - a.x)*(b.y - a.y) - (d.y - a.y)*(b.x - a.x)) < 0
			 && ((a.x - c.x)*(d.y - c.y) - (a.y - c.y)*(d.x - c.x))
			  * ((b.x - c.x)*(d.y - c.y) - (b.y - c.y)*(d.x - c.x)) < 0);
	}


	// check if quadrangle is resembling a rectangle
	inline bool checkQuadrangle(vector<Point2f> &quad) {

		if (!haveIntersection(quad[0], quad[2], quad[1], quad[3]))
			return false;

		if (haveIntersection(quad[0], quad[1], quad[2], quad[3])
			|| haveIntersection(quad[1], quad[2], quad[3], quad[0]))
			return false;

			int minAng = 180, maxAng = 0;

		for (size_t i = 0; i < 4; i++) {
			int ang = abs(getAngle(quad[i], quad[(i + 1) % 4], quad[(i + 2) % 4]));
			minAng = min(minAng, ang);
			maxAng = max(maxAng, ang);
		}

		if (minAng < 20 || maxAng < 70)
			return false;

		float minEdge, maxEdge;
		minEdge = maxEdge = norm(quad[0] - quad[1]);

		for (int i = 1; i < 4; i++) {
			float Edge = norm(quad[i] - quad[(i + 1) % 4]);
			minEdge = min(minEdge, Edge);
			maxEdge = max(maxEdge, Edge);
		}

		return minEdge > 15 && (maxEdge / minEdge < 20);
	}
}


#endif
