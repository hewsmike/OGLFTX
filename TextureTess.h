#ifndef OGLFTX_TEXTURE_TESS_H
#define OGLFTX_TEXTURE_TESS_H

#include "OGLFTX.h"

/**
 * \addtogroup OGLFTX
 * @{
 */

/**
 * \brief This virtual base class describes an interface for computing the
 * texture coordinates of a point in a glyph's local co-ordinate system.
 *
 *      During tesselation of a polygonal Face (outline, filled or solid),
 * an object which implements this interface can be used to compute a
 * different texture coordinate for each vertex.
 *
 * \author Mike Hewson\n
 */

namespace OGLFTX {
    class TextureTess {
        public:
            /**
             * \brief Compute texture coordinates for this position.
             *
             * Note that position is in the glyph's local coordinate system.
             *
             * \param p - vertex position in glyph's local coordinate system.
             *            Argument is a GLdouble[3].
             *
             * \return GLfloat[2] (s,t) texture coordinates.
             */
            virtual GLfloat* texCoord(GLdouble* p) = 0;
        };
    }

/**
 * @}
 */

#endif // OGLFTX_TEXTURE_TESS_H
