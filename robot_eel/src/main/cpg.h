#pragma once
#include <math.h>
#include "config.h"
#include "utils.h"

inline float wrap_pi(float x) {
    while (x >  M_PI) x -= 2*M_PI;
    while (x < -M_PI) x += 2*M_PI;
    return x;
}

inline void initCPG() {
    for (int j = 0; j < bodyNum; j++) {
        cpg[j].r      = 0.25f;
        cpg[j].theta  = j / (lambda * L);
        cpg[j].alpha  = 12.0f;
        cpg[j].mu     = 1.0f;
    }
}

inline float getCPGOutput(int j) {
    return Ajoint * cpg[j].r * cosf(cpg[j].theta);
}

inline float getLambdaInput() { return lambda * L; }
inline float getTargetDelta() { return 1.0f / getLambdaInput(); }

inline void updateCPG(float t, float dt, int j, float fb_phase, float fb_amp)
{
    HopfOscillator &o = cpg[j];

    float omega = 2.0f * M_PI * frequency;
    float target_delta = getTargetDelta();

    float dr = o.alpha * (o.mu - o.r * o.r) * o.r;
    float dtheta = omega;

    const float K_couple = 1.0f;
    const float K_anchor = 0.3f;
    const float k_fb_phase = 0.8f;
    const float k_fb_amp   = 0.25f;

    if (j > 0) {
        float errL = wrap_pi((cpg[j-1].theta - o.theta) + target_delta);
        dtheta += K_couple * sinf(errL);
    }

    if (j + 1 < bodyNum) {
        float errR = wrap_pi((cpg[j+1].theta - o.theta) - target_delta);
        dtheta += K_couple * sinf(errR);
    }

    float theta_ref = omega * t + j * target_delta;
    float e_ref = wrap_pi(theta_ref - o.theta);
    dtheta += K_anchor * sinf(e_ref);

    dtheta += k_fb_phase * fb_phase;
    dr     += k_fb_amp   * fb_amp;

    o.r += dr * dt;
    o.r = fmaxf(0.0f, fminf(o.r, 1.2f));  // clamp r

    o.theta = wrap_pi(o.theta + dtheta * dt);
}

inline void updateAllCPG(float t, float dt) {
    for (int j = 0; j < bodyNum; j++) {
        updateCPG(t, dt, j, 0.0f, 0.0f);
    }
}
