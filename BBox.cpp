#include "BBox.h"

/// Default constructor is all zeros.
BBox::BBox(void) : x_min_(0), y_min_(0), x_max_(0), y_max_(0) {
    }

BBox::BBox(FT_BBox ft_bbox) {
    x_min_ = ft_bbox.xMin / OGLFTX::FONT_FUDGE_FACTOR;
    y_min_ = ft_bbox.yMin / OGLFTX::FONT_FUDGE_FACTOR;
    x_max_ = ft_bbox.xMax / OGLFTX::FONT_FUDGE_FACTOR;
    y_max_ = ft_bbox.yMax / OGLFTX::FONT_FUDGE_FACTOR;
    }

BBox& BBox::operator*=(double scale) {
    x_min_ *= scale;
    y_min_ *= scale;
    x_max_ *= scale;
    y_max_ *= scale;
    advance_.dx_ *= scale;
    advance_.dy_ *= scale;
    return *this;
    }

BBox& BBox::operator+=(const BBox& box) {
    float new_value;

    new_value = box.x_min_ + advance_.dx_;
    if(new_value < x_min_) {
        x_min_ = new_value;
        }

    new_value = box.y_min_ + advance_.dy_;
    if(new_value < y_min_) {
        y_min_ = new_value;
        }

    new_value = box.x_max_ + advance_.dx_;
    if(new_value > x_max_) {
        x_max_ = new_value;
        }

    new_value = box.y_max_ + advance_.dy_;
    if(new_value > y_max_) {
        y_max_ = new_value;
        }

    /// TODO Shouldn't the greater of the advance values be taken
    /// as the new value ??
    advance_.dx_ += box.advance_.dx_;
    advance_.dy_ += box.advance_.dy_;

    return *this;
    }
