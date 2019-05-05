#include "CatmullRomCurveEvaluator.h"
#include <assert.h>
#include "mat.h"
#include "vec.h"

void CatmullRomCurveEvaluator::evaluateCurve(const std::vector<Point>& ptvCtrlPts,
	std::vector<Point>& ptvEvaluatedCurvePts,
	const float& fAniLength,
	const bool& bWrap) const
{
	ptvEvaluatedCurvePts.clear();
	const Mat4d CatM = Mat4d(
		-1, 3, -3, 1,
		2, -5, 4, -1,
		-1, 0, 1, 0,
		0, 2, 0, 0) / 2.0;
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
		Vec4d ctrl_x = Vec4d(finalPts[i].x, finalPts[i + 1].x,
			finalPts[i + 2].x, finalPts[i + 3].x);
		Vec4d ctrl_y = Vec4d(finalPts[i].y, finalPts[i + 1].y,
			finalPts[i + 2].y, finalPts[i + 3].y);
		std::vector<Point> ptvEvalPts;
		for (int t_prime = 0; t_prime < 30; t_prime++)
		{
			double t = t_prime / 30.0;
			Vec4d para = Vec4d(t*t*t, t*t, t, 1);
			Point eval_pt = Point((float)(para*CatM*ctrl_x), (float)(para*CatM*ctrl_y));
			if (eval_pt.x > finalPts[i + 1].x && eval_pt.x < finalPts[i + 2].x && (ptvEvalPts.empty() || eval_pt.x > ptvEvalPts.back().x))
			{
				ptvEvalPts.push_back(eval_pt);
			}
		}
		ptvEvaluatedCurvePts.insert(ptvEvaluatedCurvePts.begin(),finalPts[i + 1]);
		ptvEvaluatedCurvePts.insert(ptvEvaluatedCurvePts.end(), ptvEvalPts.begin(), ptvEvalPts.end());
		ptvEvaluatedCurvePts.push_back(finalPts[i + 2]);
	}

	if (!bWrap)
	{
		ptvEvaluatedCurvePts.insert(ptvEvaluatedCurvePts.begin(),Point(0, ptvCtrlPts.front().y));
		ptvEvaluatedCurvePts.push_back(Point(fAniLength, ptvCtrlPts.back().y));
	}
}