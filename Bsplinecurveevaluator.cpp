#include <assert.h>
#include "BsplineCurveEvaluator.h"
#include "mat.h"
#include "vec.h"

void  BsplineCurveEvaluator::evaluateCurve(const std::vector<Point>& ptvCtrlPts,
	std::vector<Point>& ptvEvaluatedCurvePts,
	const float& fAniLength,
	const bool& bWrap) const
{
	ptvEvaluatedCurvePts.clear();
	const Mat4d BsplineM = Mat4d(
		-1, 3, -3, 1,
		3, -6, 3, 0,
		-3, 0, 3, 0,
		1, 4, 1, 0) / 6.0;
	std::vector<Point> finalPts(ptvCtrlPts);
	if (bWrap)
	{
		finalPts.insert(finalPts.begin(),Point((ptvCtrlPts.end() - 1)->x - fAniLength, (ptvCtrlPts.end() - 1)->y));
		finalPts.insert(finalPts.begin(), Point((ptvCtrlPts.end() - 2)->x - fAniLength, (ptvCtrlPts.end() - 2)->y));
		finalPts.push_back(Point((ptvCtrlPts.begin())->x + fAniLength, (ptvCtrlPts.begin())->y));
		finalPts.push_back(Point((ptvCtrlPts.begin() + 1)->x + fAniLength, (ptvCtrlPts.begin() + 1)->y));
	}
	else
	{
		finalPts.insert(finalPts.begin(), Point(ptvCtrlPts.back().x - fAniLength, ptvCtrlPts.back().y));
		finalPts.push_back(Point(ptvCtrlPts.front().x + fAniLength, ptvCtrlPts.front().y));
	}
	int iCtrlPtCount = finalPts.size();
	for (int i = 0; i < iCtrlPtCount - 3; i++)
	{
		Vec4d ctrl_x(finalPts[i].x, finalPts[i + 1].x,
			finalPts[i + 2].x, finalPts[i + 3].x);
		Vec4d ctrl_y(finalPts[i].y, finalPts[i + 1].y,
			finalPts[i + 2].y, finalPts[i + 3].y);
		for (int t_prime = 0; t_prime < 20; t_prime++)
		{
			double t = t_prime / 20.0;
			Vec4d para(t*t*t, t*t, t, 1);
			Point eval_pt((float)(para*BsplineM*ctrl_x), (float)(para*BsplineM*ctrl_y));
			ptvEvaluatedCurvePts.push_back(eval_pt);
			std::cout << ptvEvaluatedCurvePts.size() << std::endl;
		}
	}
	if (!bWrap)
	{
		ptvEvaluatedCurvePts.insert(ptvEvaluatedCurvePts.begin(),Point(0, ptvCtrlPts.front().y));
		ptvEvaluatedCurvePts.push_back(Point(fAniLength, ptvCtrlPts.back().y));
	}
}