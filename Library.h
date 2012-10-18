#ifndef OGLFTX_LIBRARY_H
#define OGLFTX_LIBRARY_H

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H
#include FT_OUTLINE_H
#include FT_TRIGONOMETRY_H

/**
 * \addtogroup OGLFTX
 * @{
 */

/**
 * \brief This class enacts a Freetype library singleton handle.
 *
 * \author Mike Hewson\n
 */

namespace OGLFTX {
    class Library {
        public:
            /**
             * \brief Obtain the singleton's handle.
             *
             * \return the global OGLFTX FreeType library handle.
             */
            static FT_Library& instance(void);

        protected:
            /**
             * \brief Constructor protected to ensure singleton.
             */
            Library(void);

            /**
             * \brief Destructor.
             */
            ~Library(void);

        private:
            /// The single instance of the Freetype library, plus
            /// a handle to it.
            static Library library;
            static FT_Library library_;
        };
    }

/**
 * @}
 */

#endif // OGLFTX_LIBRARY_H
