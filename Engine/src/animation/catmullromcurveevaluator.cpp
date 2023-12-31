/****************************************************************************
 * Copyright ©2017 Brian Curless.  All rights reserved.  Permission is hereby
 * granted to students registered for University of Washington CSE 457 or CSE
 * 557 for use solely during Autumn Quarter 2017 for purposes of the course.
 * No other use, copying, distribution, or modification is permitted without
 * prior written consent. Copyrights for third-party components of this work
 * must be honored.  Instructors interested in reusing these course materials
 * should contact the author.
 ****************************************************************************/
#include "catmullromcurveevaluator.h"

std::vector<glm::vec2> CatmullRomCurveEvaluator::EvaluateCurve(const std::vector<glm::vec2> &ctrl_pts, int density) const {
    std::vector<glm::vec2> evaluated_pts;

    // ANIMATOR:
    // Currently this function returns points for a Linear Evaluation.
    // Replace this code with code that returns evaluated points for a Catmull-Rom
    // curve. Be sure to respect the extend_x_ and wrap_ flags; in particular,
    // the wrapped function should be C1 continuous like the rest of the curve.

    if (density == 0) density = 100;
    for (size_t i = 0; i < ctrl_pts.size()-1; i++) {
        for (int j = 0; j < density; j++) {
            float t = j/(float) density;
            glm::vec2 p = t*ctrl_pts[i+1] + (1-t)*ctrl_pts[i];
            evaluated_pts.push_back(p);
        }
    }
    evaluated_pts.push_back(ctrl_pts.back());
    if (extend_x_) ExtendX(evaluated_pts, ctrl_pts);
    return evaluated_pts;
}
