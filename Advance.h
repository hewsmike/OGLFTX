#ifndef OGLFTX_ADVANCE_H
#define OGLFTX_ADVANCE_H

#include "OGLFTX.h"

/**
 * \addtogroup OGLFTX
 * @{
 */

/**
 * \brief This class describes the 'advance' metric of a glyph.
 *
 *      Represents the distance in model space that the origin of the
 * next glyph be located. Assists in the computation of string metrics.
 *
 * \author Mike Hewson\n
 */

namespace OGLFTX {
    class Advance {
        public:
            /**
             * \brief Constructor
             *
             * \param dx - the increment in the x-direction
             * \param dy - the increment in the y-direction
             */
            Advance(float dx, float dy);

            /**
             * \brief Constructor initialized with a FreeType advance.
             *
             * \param v - the Freetype vector
             */
            Advance(FT_Vector v);

            /**
             * \brief Destructor
             */
            virtual ~Advance();

            /**
             * \brief Obtain the increment along the x-direction.
             *
             * \return the x-direction increment.
             */
            float dx(void) const;

            /**
             * \brief Obtain the increment along the y-direction.
             *
             * \return the y-direction increment.
             */
            float dy(void) const;

            /**
             * \brief Add a FreeType advance member
             *
             * \return a self reference
             */
            Advance& operator+=(const FT_Vector v);

        private:
            /// The increment in the x-direction
            float dx_;
            /// The increment in the y-direction
            float dy_;
        };
    }

/**
 * @}
 */

#endif // OGLFTX_ADVANCE_H
