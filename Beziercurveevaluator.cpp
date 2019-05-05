#include <assert.h>
#include "Beziercurveevaluator.h"
#include "mat.h"
#include "vec.h"

void BezierCurveEvaluator::evaluateCurve(const std::vector<Point>& ptvCtrlPts,
	std::vector<Point>& ptvEvaluatedCurvePts,
	const float& fAniLength,
	const bool& bwrap) const
{
	ptvEvaluatedCurvePts.clear();
	const Mat4d BezierM(
		-1, 3, -3, 1,
		3, -6, 3, 0,
		-3, 3, 0, 0,
		1, 0, 0, 0);
	std::vector<Point> finalPts(ptvCtrlPts);
	if (bwrap)
	{
		finalPts.push_back(Point(ptvCtrlPts.front().x + fAniLength, ptvCtrlPts.front().y));
		finalPts.insert(finalPts.begin(), Point(ptvCtrlPts.back().x - fAniLength, ptvCtrlPts.back().y));
	}

	int iCtrlPtCount = finalPts.size();
	if (finalPts.front().x > 0)
	{
		ptvEvaluatedCurvePts.push_back(finalPts.front());
	}
	int i;
	for (i = 0; i < iCtrlPtCount - 3; i += 3)
	{
		Vec4d ctrl_x(finalPts[i].x, finalPts[i + 1].x,
			finalPts[i + 2].x, finalPts[i + 3].x);
		Vec4d ctrl_y(finalPts[i].y, finalPts[i + 1].y,
			finalPts[i + 2].y, finalPts[i + 3].y);
		for (int t_prime = 0; t_prime < 20; t_prime++)
		{
			double t = t_prime / 20.0;
			Vec4d func(t*t*t, t*t, t, 1);
			Point eval_pt((float)(func*BezierM*ctrl_x), (float)(func*BezierM*ctrl_y));
			if (eval_pt.x < 0)
			{
				continue;
			}
			else if (eval_pt.x > fAniLength)
			{
				Point previous_pt(ptvEvaluatedCurvePts.back());
				float mid_pt_y = (eval_pt.y*(fAniLength - previous_pt.x) + previous_pt.y*(eval_pt.x - fAniLength)) / (eval_pt.x - previous_pt.x);
				ptvEvaluatedCurvePts.push_back(Point(fAniLength, mid_pt_y));
				ptvEvaluatedCurvePts.insert(ptvEvaluatedCurvePts.begin(), Point(0, mid_pt_y));
				break;
			}
			ptvEvaluatedCurvePts.push_back(eval_pt);
		}
		if (finalPts[i + 3].x <= fAniLength) {
			ptvEvaluatedCurvePts.push_back(finalPts[i + 3]);
		}
	}
	for (; i < iCtrlPtCount; i++) {
		ptvEvaluatedCurvePts.push_back(ptvCtrlPts[i]);
	}
	if (!bwrap)
	{
		ptvEvaluatedCurvePts.push_back(Point(0, ptvCtrlPts.front().y));
		ptvEvaluatedCurvePts.push_back(Point(fAniLength, ptvCtrlPts.back().y));
	}
	else
	{
		float mid_pt_y = (ptvCtrlPts.back().y*ptvCtrlPts.front().x
			+ ptvCtrlPts.front().y*(fAniLength - ptvCtrlPts.back().x))
			/ (ptvCtrlPts.front().x + fAniLength - ptvCtrlPts.back().x);
		ptvEvaluatedCurvePts.push_back(Point(fAniLength, mid_pt_y));
		ptvEvaluatedCurvePts.insert(ptvEvaluatedCurvePts.begin(), Point(0, mid_pt_y));
	}
}