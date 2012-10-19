#include "BBox.h"

//! Default constructor is all zeros.
        BBox() : x_min_(0), y_min_(0), x_max_(0), y_max_(0) {
            }

        /*!
         *(Partially) initialize a BBox from a FreeType bounding box member.
         *(The advancement is initialized to zero by its default constructor).
         * \param ft_bbox a FreeType bounding box as retrieved from
         * \c FT_Glyph_Get_CBox.
         */
        BBox(FT_BBox ft_bbox) {
            x_min_ = ft_bbox.xMin / 64.;
            y_min_ = ft_bbox.yMin / 64.;
            x_max_ = ft_bbox.xMax / 64.;
            y_max_ = ft_bbox.yMax / 64.;
            }

        //! Scale the bounding box by a constant.
        //! \param k a constant to scale the bounding box by.
        //! \return a reference to oneself.
        BBox& operator*=(double k) {
            x_min_ *= k;
            y_min_ *= k;
            x_max_ *= k;
            y_max_ *= k;
            advance_.dx_ *= k;
            advance_.dy_ *= k;

            return *this;
            }

    /*!
     * Merge a bounding box into the current one (not really addition).
     * Each time a BBox is "added", the current BBox is expanded to include
     * the metrics of the new BBox. May only work for horizontal fonts, though.
     * \param b the bounding box to merge.
     * \return a reference to oneself.
     */
    BBox& operator+= ( const BBox& b )
    {
      float new_value;

      new_value = b.x_min_ + advance_.dx_;
      if ( new_value < x_min_ ) x_min_ = new_value;

      new_value = b.y_min_ + advance_.dy_;
      if ( new_value < y_min_ ) y_min_ = new_value;

      new_value = b.x_max_ + advance_.dx_;
      if ( new_value > x_max_ ) x_max_ = new_value;

      new_value = b.y_max_ + advance_.dy_;
      if ( new_value > y_max_ ) y_max_ = new_value;

      advance_.dx_ += b.advance_.dx_;
      advance_.dy_ += b.advance_.dy_;

      return *this;
    }
