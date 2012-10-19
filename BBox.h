#ifndef OGLFTX_BBOX_H
#define OGLFTX_BBOX_H

#include "OGLFTX.h"

#include "Advance.h"

/**
 * \addtogroup OGLFTX
 * @{
 */

/**
 * \brief This class describes the 'bounding box' metric of a glyph.
 *
 *      A bounding box defines the extents of actual Represents the distance in model space that the origin of the
 * next glyph be located. Assists in the computation of string metrics.
 *
 * \author Mike Hewson\n
 */

namespace OGLFTX {
    class BBox {
        public:
            /**
             * \brief Constructor
             */
            BBox(void);

            /**
            * \brief Constructor initialized with a FreeType bounding box
            *
            * \param ft_bbox - a FreeType bounding box
            */
            BBox(FT_BBox ft_bbox);

            /**
             * \brief Destructor
             */
            virtual ~BBox();

            /// The left-most position at which "ink" appears
            /**
            * \brief Constructor initialized with a FreeType bounding box
            *
            * \param ft_bbox - a FreeType bounding box
            */
            float BBox()::x_min() const;

            /// the bottom-most position at which "ink" appears
            float BBox()::y_min() const;
            /// The right-most position at which "ink" appears

            float BBox()::x_max() const;
            /// The top-most position at which "ink" appears

            float BBox()::y_max() const;
            /// The (total) advancement

            Advance BBox()::advance();

            /**
             * \brief Scale a bounding box by a constant
             *
             * \param k - the scaling factor
             *
             * \return a self reference
             */
            BBox& operator*=(double k);

            /**
             * \brief Merge a bounding box into the current one
             *
             *      Each time a BBox is "added", the current BBox is expanded
             * to include the metrics of the new BBox. May only work for
             * horizontal fonts, though.
             *
             * \param b - the bounding box to merge
             *
             * \return a self reference
             */
            BBox& operator+=(const BBox& b);

        private:
            /// The left-most position at which "ink" appears
            float x_min_;
            /// the bottom-most position at which "ink" appears
            float y_min_;
            /// The right-most position at which "ink" appears
            float x_max_;
            /// The top-most position at which "ink" appears
            float y_max_;
            /// The (total) advancement
            Advance advance_;
        };
    }

/**
 * @}
 */

#endif // OGLFTX_ADVANCE_H

