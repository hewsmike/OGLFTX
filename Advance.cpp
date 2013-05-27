#include "Advance.h"

Advance::Advance(Advance(float dx = 0, float dy = 0) :
                    dx_(dx), dy_(dy) {
    }

Advance::Advance(FT_Vector vec) {
    // Note scaling to grid coordinates from FreeType scaled pixel positions.
    dx_ = vec.x / OGLFTX::F_EXP_7;
    dy_ = vec.y / OGLFTX::F_EXP_7;
    }

Advance::~Advance() {
    }

float Advance::dx(void) const {
    return dx_;
    }

float Advance::dy(void) const {
    return dy_;
    }

Advance& Advance::operator+=(const FT_Vector vec) {
    // Note scaling to grid coordinates from FreeType scaled pixel positions.
    dx_ += vec.x / OGLFTX::F_EXP_7;
    dy_ += vec.y / OGLFTX::F_EXP_7;
    return *this;
    }
