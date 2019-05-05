#include <assert.h>
#include <algorithm>
#include "C2InterpolationCurveEvaluator.h"
#include "LinearCurveEvaluator.h"
#include "mat.h"
#include "vec.h"

void C2InterpolationCurveEvaluator::evaluateCurve(const std::vector<Point>& ptvCtrlPts,
	std::vector<Point>& ptvEvaluatedCurvePts,
	const float& fAniLength,
	const bool& bWrap) const
{
	ptvEvaluatedCurvePts.clear();
	if ((bWrap && ptvCtrlPts.size() < 2) || (!bWrap && ptvCtrlPts.size() < 4))
	{
		LinearCurveEvaluator lineCrvEvl = LinearCurveEvaluator();
		lineCrvEvl.evaluateCurve(ptvCtrlPts, ptvEvaluatedCurvePts, fAniLength, bWrap);
		return;
	}
	const Mat4d BezierM(
		-1, 3, -3, 1,
		3, -6, 3, 0,
		-3, 3, 0, 0,
		1, 0, 0, 0);
	std::vector<Point> finalPts(ptvCtrlPts);
	if (bWrap)
	{
		finalPts.insert(finalPts.begin(), Point(ptvCtrlPts.back().x - fAniLength, ptvCtrlPts.back().y));
		finalPts.push_back(Point(ptvCtrlPts.front().x + fAniLength, ptvCtrlPts.front().y));
	}

	int iCtrlPtCount = finalPts.size();

	std::vector<Point> ptvCtrlPtsDivr;
	evaluateDerivative(ptvCtrlPtsDivr, finalPts);
	std::vector<Point> BezierPts;
	for (int i = 0; i < iCtrlPtCount - 1; i++)
	{
		BezierPts.push_back(finalPts[i]);
		BezierPts.push_back(Point((float)(finalPts[i].x + ptvCtrlPtsDivr[i].x / 3.0),(float)(finalPts[i].y + ptvCtrlPtsDivr[i].y / 3.0)));
		BezierPts.push_back(Point((float)(finalPts[i + 1].x - ptvCtrlPtsDivr[i + 1].x / 3.0),(float)(finalPts[i + 1].y - ptvCtrlPtsDivr[i + 1].y / 3.0)));
	}
	BezierPts.push_back(finalPts.back());

	int BezierPtCount = BezierPts.size();

	for (int i = 0; i < BezierPtCount - 3; i += 3)
	{

		Vec4d ctrl_x(BezierPts[i].x, BezierPts[i + 1].x,
			BezierPts[i + 2].x, BezierPts[i + 3].x);
		Vec4d ctrl_y(BezierPts[i].y, BezierPts[i + 1].y,
			BezierPts[i + 2].y, BezierPts[i + 3].y);
		ptvEvaluatedCurvePts.push_back(BezierPts[i]);
		for (int t_prime = 0; t_prime < 20; t_prime++)
		{
			double t = t_prime / 20.0;
			Vec4d para(t*t*t, t*t, t, 1);
			Point eval_pt((float)(para*BezierM*ctrl_x), (float)(para*BezierM*ctrl_y));
			if (eval_pt.x > BezierPts[i].x && eval_pt.x < BezierPts[i + 3].x
				&& (ptvEvaluatedCurvePts.empty() || eval_pt.x > ptvEvaluatedCurvePts.back().x))
			{
				ptvEvaluatedCurvePts.push_back(eval_pt);
			}
		}
		ptvEvaluatedCurvePts.push_back(BezierPts[i + 3]);
	}

	if (!bWrap)
	{
		ptvEvaluatedCurvePts.insert(ptvEvaluatedCurvePts.begin(),Point(0, BezierPts.front().y));
		ptvEvaluatedCurvePts.push_back(Point(fAniLength, BezierPts.back().y));
	}
}



void C2InterpolationCurveEvaluator::evaluateDerivative(std::vector<Point>& ptvCtrlPtsDivr,
	std::vector<Point> ptvCtrlPtsCpy) const
{
	int num = ptvCtrlPtsCpy.size() - 1;
	std::vector<double> coefficient(num + 1, 0.0);
	std::vector<double> div_x(num + 1, 0.0);
	std::vector<double> div_y(num + 1, 0.0);

	coefficient[0] = 0.5;
	for (int i = 1; i < num; i++)
	{ 
		coefficient[i] = 1.0 / (4.0 - coefficient[i - 1]); 
	}
	coefficient[num] = 1.0 / (2.0 - coefficient[num - 1]);

	div_y[0] = 1.5 * (ptvCtrlPtsCpy[1].y - ptvCtrlPtsCpy[0].y);
	for (int i = 1; i < num; i++)
	{
		div_y[i] = coefficient[i] * (3 * (ptvCtrlPtsCpy[i + 1].y - ptvCtrlPtsCpy[i - 1].y) - div_y[i - 1]);
	}
	div_y[num] = coefficient[num] * (3 * (ptvCtrlPtsCpy[num].y - ptvCtrlPtsCpy[num - 1].y) - div_y[num]);

	div_x[0] = 1.5 * (ptvCtrlPtsCpy[1].x - ptvCtrlPtsCpy[0].x);
	for (int i = 1; i < num; i++)
	{
		div_x[i] = coefficient[i] * (3 * (ptvCtrlPtsCpy[i + 1].x - ptvCtrlPtsCpy[i - 1].x) - div_x[i - 1]);
	}
	div_x[num] = coefficient[num] * (3 * (ptvCtrlPtsCpy[num].x - ptvCtrlPtsCpy[num - 1].x) - div_x[num]);

	ptvCtrlPtsDivr.push_back(Point((float)(div_x[num]), (float)(div_y[num])));
	for (int i = num - 1; i >= 0; i--)
	{
		div_x[i] = div_x[i] - coefficient[i] * div_x[i + 1];
		div_y[i] = div_y[i] - coefficient[i] * div_y[i + 1];
		ptvCtrlPtsDivr.push_back(Point((float)(div_x[i]), (float)(div_y[i])));
	}
	std::reverse(ptvCtrlPtsDivr.begin(), ptvCtrlPtsDivr.end());
}