#ifndef OGLFTX_FACE_H
#define OGLFTX_FACE_H

#include "OGLFTX.h"

/**
 * \addtogroup OGLFTX
 * @{
 */

/**
 * \brief This virtual base class describes an interface used to render a font.
 *
 *      Contains most of the functions that you are likely to call to
 * manipulate the rendering of the text.
 *
 * \author Mike Hewson\n
 */

namespace OGLFTX {
    class Face {
        public:
            /// Horizontal text justification constants.
            enum HorizontalJustification{LEFT,
                                         ORIGIN, /// Natural origin (default)
                                         CENTER,
                                         RIGHT};

            /// Vertical text justification constants.
            enum VerticalJustification{BOTTOM,
                                       BASELINE, /// Baseline alignment (default)
                                       MIDDLE,
                                       TOP};

            /// Control how OpenGL display lists are created for individual glyphs.
            /// The default mode is to create display lists for each glyph as it
            /// is requested. Therefore, the Face drawing routines cannot themselves
            /// be called from within an open display list. In IMMEDIATE mode,
            /// cached glyphs will be drawn if available, otherwise the FreeType
            /// data for a glyph is re-rendered each time.
            enum GlyphCompileMode{COMPILE,
                                  IMMEDIATE};

        protected:
            /// The FreeType face - experimentally, this is now an array of
            /// faces so that we can handle a wider range of UNICODE points
            /// in case a face doesn't cover the points of interest.
            std::vector<FaceData> faces_;

            /// Did a font load OK?
            bool valid_;

            /// Glyph display list creation mode.
            enum GlyphCompileMode compile_mode_;

            /// Nominal point size.
            float point_size_;

            /// Display resolution in pixels per inch.
            FT_UInt resolution_;

            /// Does rendering text affect the MODELVIEW matrix?
            bool advance_;

            /// Foreground color (I really wanted to avoid this, but not really
            /// possible without state queries, which you can't put into
            /// display lists. Anyway, you'll be able to get even more fancy
            /// by passing in a function to map the color with, so why balk at
            /// this?)
            GLfloat foreground_color_[4];

            /// Background color (what modes would use this?)
            GLfloat background_color_[4];

            /// PHIGS-like horizontal positioning of text.
            enum HorizontalJustification horizontal_justification_;

            /// PHIGS-like vertical positioning of text.
            enum VerticalJustification vertical_justification_;

            /// Rotate an entire string in the Z plane
            GLfloat string_rotation_;

            /// Let the user decide which character to use as the rotation reference.
            /// Use "o" by default, I suppose.
            FT_UInt rotation_reference_glyph_;

            /// The rotation reference character could be in any face.
            FT_Face rotation_reference_face_;

            /// These are the translation offsets provided by the rotation reference
            /// character; for whom, we've discovered, only the Y position is relevant.
            GLfloat rotation_offset_y_;

            /// Type of the cache of defined glyph to display list mapping.
            typedef std::map< FT_UInt, GLuint > GlyphDLists;

            /// A convenience definition of the iterator over the glyph to display
            /// list map.
            typedef GlyphDLists::const_iterator GDLCI;

            /// A convenience definition of the iterator over the glyph to display
            /// list map.
            typedef GlyphDLists::iterator GDLI;

            /// Cache of defined glyph display lists
            GlyphDLists glyph_dlists_;

            /// The user can supply an array of display list which are invoked
            /// before each glyph is rendered.
            DisplayLists character_display_lists_;
  private:
    /// We allow a Face to be constructed either from a file name
    /// or passed in as an already opened FreeType FT_Face. In the case
    /// of the later (already opened), we don't close the FT_Face on
    /// destruction. This way you can share FT_Faces between related
    /// OGLFTX faces.
    struct FaceData {
      FT_Face face_;
      bool free_on_exit_;
      FaceData ( FT_Face face, bool free_on_exit = true )
	: face_( face ), free_on_exit_( free_on_exit )
      {}
    };


  public:
    /*!
     * Construct a Face by loading a font from the given file.
     * \param filename the filename which contains the font face.
     * \param point_size the initial point size of the font to generate. A point
     * is essentially 1/72th of an inch. Defaults to 12.
     * \param resolution the pixel density of the display in dots per inch (DPI).
     * Defaults to 100 DPI.
     */
    Face ( const char* filename, float point_size = 12, FT_UInt resolution = 100 );

    /*!
     * Alternatively, the user may have already opened a face and just
     * wants to draw with it. This is useful for Multiple Master fonts or
     * combining multiple files to increase UNICODE point coverage.
     * \param face open Freetype FT_Face.
     * \param point_size the initial point size of the font to generate. A point
     * is essentially 1/72th of an inch. Defaults to 12.
     * \param resolution the pixel density of the display in dots per inch (DPI).
     * Defaults to 100 DPI.
     */
    Face ( FT_Face face, float point_size = 12, FT_UInt resolution = 100 );

    /*!
     * Deleting a Face frees its FreeType face (and anything else it's
     * styles have allocated).
     */
    virtual ~Face ( void );

    /*!
     * Let the user test to see if the font was loaded OK.
     * \return true if the FT_Face was successfully created.
     */
    bool isValid ( void ) const { return valid_; }

    /*!
     * Add another FT_Face to the OGLFTX Face. Generally used to add more
     * coverage of UNICODE points (at least that's the plan). This
     * routine takes a filename and takes ownership of the FT_Face.
     * \param filename name of file containing font face data.
     * \return true if face was successfully added.
     */
    bool addAuxiliaryFace ( const char* filename );

    /*!
     * Add another FT_Face to the OGLFTX Face. Generally used to add more
     * coverage of UNICODE points (at least that's the plan). This
     * routine takes an already open FT_Face. The user is responsible
     * for clean up.
     * \param face open FreeType FT_Face
     * \return true if face was successfully added.
     */
    bool addAuxiliaryFace ( FT_Face face );

    /*!
     * By default, each time a new character is seen, its glyph is rendered
     * into a display list. This means that a display list cannot already
     * be open (since OpenGL doesn't allow nested display list creation).
     * Rendering can be set into immediate mode in which case glyphs are
     * rendered from display lists if available, but are otherwise generated
     * anew each time.
     * \param compile_mode the new compile mode.
     */
    void setCompileMode ( enum GlyphCompileMode compile_mode )
    {
      compile_mode_ = compile_mode;
    }

    /*!
     * \return the current glyph compile mode.
     */
    enum GlyphCompileMode compileMode ( void ) const { return compile_mode_; }

    /*!
     * For the rasterized styles (Monochrome, Grayscale, Translucent, Texture),
     * glyphs are rendered at the pixel size given by:
     *
     * point_size [pts] * / 72 [pts/in] * resolution [dots/in] = [dots].
     *
     * For the polygon styles (Outline, Filled, Solid), the "nominal" size of
     * the glyphs is:
     *
     * point_size[pts] / 72 [pts/in] * resolution [dots/in]
     * / units_per_EM [font unit/EM] = [dots * EM].
     *
     * If the MODELVIEW and PROJECTION matrices are such that one screen pixel
     * corresponds to one modeling unit, then polygonal Faces will
     * be the same size as raster Faces.
     *
     * Note that changing the point size after Face creation will invalidate
     * the cache of OpenGL display lists and any other information which
     * the individual styles have cached.
     * \param point_size the new point size in points (1/72-th inch).
     */
    void setPointSize ( float point_size );

    /*!
     * \return the current point size.
     */
    float pointSize ( void ) { return point_size_; }

    /*!
     * For the rasterized styles (Monochrome, Grayscale,
     * Translucent, Texture), the exact rendered size of the glyphs depends on
     * the resolution of the display (as opposed to the polygon styles
     * whose size is controlled by the viewing matrices). The Texture
     * style is slightly different because the glyphs are texture-mapped
     * onto an arbitrary rectangle; here, the resolution only controls
     * how accurately the glyph is rendered.
     * \param resolution the resolution in DPI (dots per inch).
     */
    void setResolution ( FT_UInt resolution );

    /*!
     * \return the current raster resolution.
     */
    FT_UInt resolution ( void ) { return resolution_; }

    /*!
     * If advance is true, then the changes made to the MODELVIEW matrix
     * to render a string are allowed to remain. Otherwise, the library
     * pushes the current MODELVIEW matrix onto the matrix stack, renders
     * the string and then pops it off again. Rendering a character always
     * modifies the MODELVIEW matrix.
     * \param advance whether or not the MODELVIEW matrix should be left
     * translated by the advancement of a rendered string.
     */
    void setAdvance ( bool advance ) { advance_ = advance; }

    /*!
     * \return the advance value.
     */
    bool advance ( void ) const { return advance_; }

    /*!
     * This is the nominal color of the glyphs. A lot of other things
     * can alter what you actually see! Note that changing the foreground
     * color invalidates the glyph cache.
     * \param red the red component of the foreground color.
     * \param green the green component of the foreground color.
     * \param blue the blue component of the foreground color.
     * \param alpha the alpha component of the foreground color.
     */
    void setForegroundColor ( GLfloat red = 0.0,
			      GLfloat green = 0.0,
			      GLfloat blue = 0.0,
			      GLfloat alpha = 1.0 );

    /*!
     * This is the nominal color of the glyphs. A lot of other things
     * can alter what you actually see! Note that changing the foreground
     * color invalidates the glyph cache.
     * \param foreground_color an array of 4 values corresponding to the
     * red, green, blue and alpha components of the foreground color.
     */
    void setForegroundColor ( const GLfloat foreground_color[4] );

    /*!
     * \return the red component of the foreground color
     */
    GLfloat foregroundRed ( void ) const { return foreground_color_[R]; }
    /*!
     * \return the green component of the foreground color
     */
    GLfloat foregroundGreen ( void ) const { return foreground_color_[G]; }
    /*!
     * \return the blue component of the foreground color
     */
    GLfloat foregroundBlue ( void ) const { return foreground_color_[B]; }
    /*!
     * \return the alpha component of the foreground color
     */
    GLfloat foregroundAlpha ( void ) const { return foreground_color_[A]; }

    /*!
     * This is the nominal background color of the glyphs. A lot of other things
     * can alter what you actually see! Note that changing the background
     * color invalidates the glyph cache.
     * \param red the red component of the background color.
     * \param green the green component of the background color.
     * \param blue the blue component of the background color.
     * \param alpha the alpha component of the background color.
     */
    void setBackgroundColor ( GLfloat red = 1.0,
			      GLfloat green = 1.0,
			      GLfloat blue = 1.0,
			      GLfloat alpha = 0.0 );

    /*!
     * This is the nominal background color of the glyphs. A lot of other things
     * can alter what you actually see! Note that changing the background
     * color invalidates the glyph cache.
     * \param background_color an array of 4 values corresponding to the
     * red, green, blue and alpha components of the background color.
     */
    void setBackgroundColor ( const GLfloat background_color[4] );

    /*!
     * \return the red component of the background color
     */
    GLfloat backgroundRed ( void ) const { return background_color_[R]; }
    /*!
     * \return the green component of the background color
     */
    GLfloat backgroundGreen ( void ) const { return background_color_[G]; }
    /*!
     * \return the blue component of the background color
     */
    GLfloat backgroundBlue ( void ) const { return background_color_[B]; }
    /*!
     * \return the alpha component of the background color
     */
    GLfloat backgroundAlpha ( void ) const { return background_color_[A]; }

    /*!
     * Set the individual character rotation in the Z direction.
     * \param character_rotation_z angle in degrees of z rotation.
     */
    virtual void setCharacterRotationZ ( GLfloat character_rotation_z ) = 0;

    /*!
     * \return the character rotation in the Z direction.
     */
    virtual GLfloat characterRotationZ ( void ) const = 0;

    /*!
     * The z rotation angle needs a center. Nominate a character whose
     * center is to be the center of rotation. By default, use "o".
     * \param c rotation reference character.
     */
    void setCharacterRotationReference ( unsigned char c );

    /*!
     * Rotate an entire string through the given angle (in the Z plane only).
     * (Somewhat pointless for the vector styles since you can do mostly
     * the same thing with the MODELVIEW transform, however, for what its
     * worth, this routine uses the FreeType rotation function to compute
     * the "proper" metrics for glyph advance.)
     * \param string_rotation angle in degrees of z rotation.
     */
    void setStringRotation ( GLfloat string_rotation );

    /*!
     * \return the (Z plane) string rotation angle.
     */
    GLfloat stringRotation ( void ) const { return string_rotation_; }

    /*!
     * Set the horizontal justification.
     * \param horizontal_justification the new horizontal justification.
     */
    void setHorizontalJustification ( enum HorizontalJustification
				      horizontal_justification )
    {
      horizontal_justification_ = horizontal_justification;
    }

    /*!
     * \return the horizontal justification.
     */
    enum HorizontalJustification horizontalJustification ( void ) const
    { return horizontal_justification_; }

    /*!
     * Set the vertical justification.
     * \param vertical_justification the new vertical justification
     */
    void setVerticalJustification ( enum VerticalJustification
				    vertical_justification )
    {
      vertical_justification_ = vertical_justification;
    }

    /*!
     * \return the vertical justification.
     */
    enum VerticalJustification verticaljustification ( void )
      const { return vertical_justification_; }

    /*!
     * Specify an OpenGL display list to be invoked before
     * each character in a string. Face makes a copy of the argument. Pass
     * an empty DisplayLists to disable this feature.
     * \param character_display_lists STL vector<GLuint> containing a display
     * list to invoke before each glyph in a string is drawn.
     */
    void setCharacterDisplayLists ( const DisplayLists& character_display_lists )
    {
      character_display_lists_ = character_display_lists;
    }

    /*!
     * \return a reference to the array of character display lists. This is
     * the live list as stored in the Face.
     */
    DisplayLists& characterDisplayLists ( void )
    { return character_display_lists_; }

    /*!
     * \return the height (i.e., line spacing) at the current character size.
     */
    virtual double height ( void ) const = 0;

    /*!
     * Compute the bounding box info for a character.
     * \param c the (latin1) character to measure.
     * \return the bounding box of c.
     */
    virtual BBox measure ( unsigned char c ) = 0;

    /*!
     * Compute the bounding box info for a string.
     * \param s the (latin1) string to measure.
     * \return the bounding box of s.
     */
    virtual BBox measure ( const char* s );
    /*!
     * Compute the bounding box info for a string without conversion
     * to modeling coordinates.
     * \param s the (latin1) string to measure.
     * \return the bounding box of s.
     */
    virtual BBox measureRaw ( const char* s );

    /*!
     * Compile a string into an OpenGL display list for later
     * rendering.  Essentially, the string is rendered at the origin
     * of the current MODELVIEW. Note: no other display lists should
     * be open when this routine is called. Also, the Face does not
     * keep track of these lists, so you must delete them in order
     * to recover the memory.
     * \param s the (latin1) string to compile.
     * \return the display list name for the string.
     */
    GLuint compile ( const char* s );

    /*!
     * Compile a single character (glyph) into an OpenGL display list
     * for later rendering. The Face \em does keep track of these
     * display lists, so do not delete them.
     * \param c the (latin1) character to compile.
     * \return the display list name for the character.
     */
    GLuint compile ( unsigned char c );

    /*!
     * Draw a (latin1) string using the current MODELVIEW matrix. If
     * advance is true, then the final glyph advance changes to the
     * MODELVIEW matrix are left in place.
     * \param s the (latin1) string to draw.
     */
    void draw ( const char* s );

    /*!
     * Draw the character using the current MODELVIEW matrix. Note that
     * the MODELVIEW matrix is modified by the glyph advance. Draw a
     * string if you don't want the MODELVIEW matrix changed.
     * \param c the (latin1) character to draw.
     */
    void draw ( unsigned char c );

    /*!
     * Draw the (latin1) character at the given 2D point. Note that
     * the MODELVIEW matrix is modified by the glyph advance. Draw
     * a string if you don't want the MODELVIEW matrix changed.
     * \param x the X position.
     * \param y the Y position.
     * \param c the (latin1) character to draw.
     */
    void draw ( GLfloat x, GLfloat y, unsigned char c );
    /*!
     * Draw the (latin1) character at the given 3D point. Note that
     * the MODELVIEW matrix is modified by the glyph advance. Draw
     * a string if you don't want the MODELVIEW matrix changed.
     * \param x the X position.
     * \param y the Y position.
     * \param z the Z position.
     * \param c the (latin1) character to draw.
     */
    void draw ( GLfloat x, GLfloat y, GLfloat z, unsigned char c );

    /*!
     * Draw a string at the given 2D point.
     * \param x the X position.
     * \param y the Y position.
     * \param s the (latin1) string to draw.
     */
    void draw ( GLfloat x, GLfloat y, const char* s );
    /*!
     * Draw a string at the given 3D point.
     * \param x the X position.
     * \param y the Y position.
     * \param z the Z position.
     * \param s the (latin1) string to draw.
     */
    void draw ( GLfloat x, GLfloat y, GLfloat z, const char* s );

    /*!
     * \return the nominal ascender from the face. This is in "notional"
     * units.
     */
    int ascender ( void ) { return faces_.front().face_->ascender; }

    /*!
     * \return the nominal descender from the face. This is in "notional"
     * units.
     */
    int descender ( void ) { return faces_.front().face_->descender; }

  protected:
    // The various styles override these routines

    /// Some styles, in particular the Texture, need specialized steps
    /// to compile a glyph into an OpenGL display list.
    /// \param face the FT_Face containing the glyph.
    /// \param glyph_index the index of the glyph in face.
    /// \return the display list of the compiled glyph.
    virtual GLuint compileGlyph ( FT_Face face, FT_UInt glyph_index ) = 0;

    /// Each style implements its own glyph rendering routine.
    /// \param face the FT_Face containing the glyph.
    /// \param glyph_index the index of the glyph in face.
    virtual void renderGlyph ( FT_Face face, FT_UInt glyph_index ) = 0;

    /// There is a slight different between the way in which the polygonal
    /// and raster styles select the character size for FreeType to generate.
    virtual void setCharSize ( void ) = 0;

    /// The different styles have different caching needs (well, really only
    /// the texture style currently has more than the display list cache).
    virtual void clearCaches ( void ) = 0;

    /// The polygonal and raster styles compute different values for the
    /// Z rotation offset. (It's in integer pixels for the raster styles and
    /// in floating point pixels for the polygonal styles.)
    virtual void setRotationOffset ( void ) = 0;

  private:
    void init ( void );
    BBox measure_nominal ( const char* s );
  };

    }

/**
 * @}
 */

#endif // OGLFTX_ADVANCE_H
