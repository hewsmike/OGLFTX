#ifndef OGLFTX_COLOR_TESS_H
#define OGLFTX_COLOR_TESS_H

#include "OGLFTX.h"

/**
 * \addtogroup OGLFTX
 * @{
 */

/**
 * \brief This virtual base class describes an interface for computing the
 * color of a point in a glyph's local co-ordinate system.
 *
 *      During tesselation of a polygonal Face (outline, filled or solid),
 * an object which implements this interface can be used to compute a
 * different color for each vertex.
 *
 * \author Mike Hewson\n
 */

namespace OGLFTX {
     class ColorTess {
        public:
            /**
             * \brief Compute a color for a glyph point.
             *
             *      Note that the position is in the glyph's local coordinate
             * system.
             *
             * \param p - vertex position in glyph's local coordinate system.
             *            Argument is a GLdouble[3].
             *
             * \return a GLfloat[4] (RGBA) color specification.
             */
            virtual GLfloat* color(GLdouble* p) = 0;
        };
    }

/**
 * @}
 */

#endif // OGLFTX_COLOR_TESS_H
