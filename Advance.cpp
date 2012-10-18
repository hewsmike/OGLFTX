#include "Advance.h"

Advance::Advance(Advance(float dx = 0, float dy = 0) : dx_(dx), dy_(dy) {
    }

Advance::Advance(FT_Vector v) {
    dx_ = v.x / OGLFTX::FONT_FUDGE_FACTOR;
    dy_ = v.y / OGLFTX::FONT_FUDGE_FACTOR;
    }

Advance::~Advance() {
    }

float Advance::dx(void) const {
    return dx_;
    }

float Advance::dy(void) const {
    return dy_;
    }

Advance& Advance::operator+=(const FT_Vector v) {
    dx_ += v.x / OGLFTX::FONT_FUDGE_FACTOR;
    dy_ += v.y / OGLFTX::FONT_FUDGE_FACTOR;
    return *this;
    }
