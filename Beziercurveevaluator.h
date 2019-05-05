#ifndef INCLUDED_BEXIER_CURVE_EVALUATOR_H
#define INCLUDED_BEXIER_CURVE_EVALUATOR_H
#pragma warning(disable : 4786)

#include "CurveEvaluator.h"

class BezierCurveEvaluator : public CurveEvaluator
{
public:
	void evaluateCurve(const std::vector<Point>& ptvCtrlPts,
		std::vector<Point>& ptvEcvalluatedCurvePts,
		const float& fAniLength,
		const bool& bWrap) const;
};

#endif /*INCLUDED_BEXIER_CURVE_EVALUATOR_H*/