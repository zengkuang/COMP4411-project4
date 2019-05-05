#ifndef INCLUDED_C2INTERPOLATING_CURVE_EVALUATOR_H
#define INCLUDED_C2INTERPOLATING_CURVE_EVALUATOR_H
#pragma warning(disable : 4786)  
#include "CurveEvaluator.h"

class C2InterpolationCurveEvaluator : public CurveEvaluator
{
public:
	void evaluateCurve(const std::vector<Point>& ptvCtrlPts,
		std::vector<Point>& ptvEvaluatedCurvePts,
		const float& fAniLength,
		const bool& bWrap) const override;
private:
	void evaluateDerivative(std::vector<Point>& ptvCtrlPtsDrv,
		std::vector<Point> ptvCtrlPts) const;
};

#endif /*INCLUDED_C2INTERPOLATING_CURVE_EVALUATOR_H*/