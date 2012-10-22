/*
 * OGLFT: A library for drawing text with OpenGL using the FreeType library
 * Copyright (C) 2002 lignum Computing, Inc. <OGLFT@lignumcomputing.com>
 * $Id: OGLFT.h,v 1.15 2003/10/01 14:41:09 allen Exp $
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * OGLFTX is based upon OGLFT, as noted above, and was developed in 2012 to
 * provide OpenGL v3.2+ forward compatibility. Main changes :
 *          - replacing display lists with buffer objects.
 *          - all references to Qt removed ( I have no idea how to integrate
 *            that correctly )
 *          - static build of OGLFTX.h and OGLFTX.cpp into application, no
 *            separate library build provided.
 *
 * Mike Hewson <hewsmike[AT]iinet.net.au>.
 *
 */

// Load a new face

  Face::Face ( const char* filename, float point_size, FT_UInt resolution )
    : point_size_( point_size ), resolution_( resolution )
  {
    valid_ = true; // Assume the best :-)

    FT_Face ft_face;

    FT_Error error = FT_New_Face( Library::instance(), filename, 0, &ft_face );

    if ( error != 0 ) {
      valid_ = false;
      return;
    }

    // As of FreeType 2.1: only a UNICODE charmap is automatically activated.
    // If no charmap is activated automatically, just use the first one.
    if ( ft_face->charmap == 0 && ft_face->num_charmaps > 0 )
      FT_Select_Charmap( ft_face, ft_face->charmaps[0]->encoding );

    faces_.push_back( FaceData( ft_face ) );

    init();
  }

  // Go with a face that the user has already opened.

  Face::Face ( FT_Face face, float point_size, FT_UInt resolution )
    : point_size_( point_size ), resolution_( resolution )
  {
    valid_ = true;

    // As of FreeType 2.1: only a UNICODE charmap is automatically activated.
    // If no charmap is activated automatically, just use the first one.
    if ( face->charmap == 0 && face->num_charmaps > 0 )
      FT_Select_Charmap( face, face->charmaps[0]->encoding );

    faces_.push_back( FaceData( face, false ) );

    init();
  }

  // Standard initialization behavior once the font file is opened.

  void Face::init ( void )
  {
    // By default, each glyph is compiled into a display list the first
    // time it is encountered

    compile_mode_ = COMPILE;

    // By default, all drawing is wrapped with push/pop matrix so that the
    // MODELVIEW matrix is not modified. If advance_ is set, then subsequent
    // drawings follow from the advance of the last glyph rendered.

    advance_ = false;

    // Initialize the default colors

    foreground_color_[R] = 0.;
    foreground_color_[G] = 0.;
    foreground_color_[B] = 0.;
    foreground_color_[A] = 1.;

    background_color_[R] = 1.;
    background_color_[G] = 1.;
    background_color_[B] = 1.;
    background_color_[A] = 0.;

    // The default positioning of the text is at the origin of the first glyph
    horizontal_justification_ = ORIGIN;
    vertical_justification_ = BASELINE;

    // By default, strings are rendered in their nominal direction
    string_rotation_ = 0;

    // setCharacterRotationReference calls the virtual function clearCaches()
    // so it is up to a subclass to set the real default
    rotation_reference_glyph_ = 0;
    rotation_reference_face_ = 0;
    rotation_offset_y_ = 0.;
  }

  Face::~Face ( void )
  {
    for ( unsigned int i = 0; i < faces_.size(); i++ )
      if ( faces_[i].free_on_exit_ )
	FT_Done_Face( faces_[i].face_ );
  }

  // Add another Face to select characters from

  bool Face::addAuxiliaryFace ( const char* filename )
  {
    FT_Face ft_face;

    FT_Error error = FT_New_Face( Library::instance(), filename, 0, &ft_face );

    if ( error != 0 )
      return false;

    faces_.push_back( FaceData( ft_face ) );

    setCharSize();

    return true;
  }

  // Add another Face to select characters from

  bool Face::addAuxiliaryFace ( FT_Face face )
  {
    faces_.push_back( FaceData( face, false ) );

    setCharSize();

    return true;
  }

  // Note: Changing the point size also clears the display list cache

  void Face::setPointSize ( float point_size )
  {
    if ( point_size != point_size_ ) {

      point_size_ = point_size;

      clearCaches();

      setCharSize();
    }
  }

  // Note: Changing the resolution also clears the display list cache

  void Face::setResolution ( FT_UInt resolution )
  {
    if ( resolution != resolution_ ) {

      resolution_ = resolution;

      clearCaches();

      setCharSize();
    }
  }

  // Note: Changing the background color also clears the display list cache.

  void Face::setBackgroundColor ( GLfloat red, GLfloat green, GLfloat blue,
				  GLfloat alpha )
  {
    if ( background_color_[R] != red ||
	 background_color_[G] != green ||
	 background_color_[B] != blue ||
	 background_color_[A] != alpha ) {

      background_color_[R] = red;
      background_color_[G] = green;
      background_color_[B] = blue;
      background_color_[A] = alpha;

      clearCaches();
    }
  }

  // Note: Changing the foreground color also clears the display list cache.

  void Face::setForegroundColor ( GLfloat red, GLfloat green, GLfloat blue,
				  GLfloat alpha )
  {
    if ( foreground_color_[R] != red ||
	 foreground_color_[G] != green ||
	 foreground_color_[B] != blue ||
	 foreground_color_[A] != alpha ) {

      foreground_color_[R] = red;
      foreground_color_[G] = green;
      foreground_color_[B] = blue;
      foreground_color_[A] = alpha;

      clearCaches();
    }
  }

  // Note: Changing the foreground color also clears the display list cache.

  void Face::setForegroundColor ( const GLfloat foreground_color[4] )
  {
    if ( foreground_color_[R] != foreground_color[R] ||
	 foreground_color_[G] != foreground_color[G] ||
	 foreground_color_[B] != foreground_color[B] ||
	 foreground_color_[A] != foreground_color[A] ) {

      foreground_color_[R] = foreground_color[R];
      foreground_color_[G] = foreground_color[G];
      foreground_color_[B] = foreground_color[B];
      foreground_color_[A] = foreground_color[A];

      clearCaches();
    }
  }

  // Note: Changing the background color also clears the display list cache.

  void Face::setBackgroundColor ( const GLfloat background_color[4] )
  {
    if ( background_color_[R] != background_color[R] ||
	 background_color_[G] != background_color[G] ||
	 background_color_[B] != background_color[B] ||
	 background_color_[A] != background_color[A] ) {

      background_color_[R] = background_color[R];
      background_color_[G] = background_color[G];
      background_color_[B] = background_color[B];
      background_color_[A] = background_color[A];

      clearCaches();
    }
  }

  // Note: Changing the string rotation angle clears the display list cache

  void Face::setStringRotation ( GLfloat string_rotation )
  {
    if ( string_rotation != string_rotation_ ) {
      string_rotation_ = string_rotation;

      clearCaches();

      // Note that this affects ALL glyphs accessed through
      // the Face, both the vector and the raster glyphs. Very nice!

      if ( string_rotation_ != 0. ) {
	float angle;
	if ( string_rotation_ < 0. ) {
	  angle = 360. - fmod( fabs( string_rotation_ ), 360.f );
	}
	else {
	  angle = fmod( string_rotation_, 360.f );
	}

	FT_Matrix rotation_matrix;
	FT_Vector sinus;

	FT_Vector_Unit( &sinus, (FT_Angle)(angle * 0x10000L) );

	rotation_matrix.xx = sinus.x;
	rotation_matrix.xy = -sinus.y;
	rotation_matrix.yx = sinus.y;
	rotation_matrix.yy = sinus.x;

	for ( unsigned int i = 0; i < faces_.size(); i++ )
	  FT_Set_Transform( faces_[i].face_, &rotation_matrix, 0 );
      }
      else
	for ( unsigned int i = 0; i < faces_.size(); i++ )
	  FT_Set_Transform( faces_[i].face_, 0, 0 );
    }
  }

  // Note: Changing the rotation reference character clears the display list cache.

  void Face::setCharacterRotationReference ( unsigned char c )
  {
    unsigned int f;
    FT_UInt glyph_index = 0;

    for ( f = 0; f < faces_.size(); f++ ) {
      glyph_index = FT_Get_Char_Index( faces_[f].face_, c );
      if ( glyph_index != 0 ) break;
    }

    if ( f < faces_.size() && glyph_index != rotation_reference_glyph_ ) {

      FT_Error error = FT_Load_Glyph( faces_[f].face_, glyph_index,
				      FT_LOAD_DEFAULT );

      if ( error != 0 ) return;

      rotation_reference_glyph_ = glyph_index;

      rotation_reference_face_ = faces_[f].face_;

      setRotationOffset();

      clearCaches();
    }
  }

  BBox Face::measure ( const char* s )
  {
    BBox bbox;
    char c;

    if ( ( c = *s++ ) != 0 ) {

      bbox = measure( c );

      for ( c = *s; c != 0; c = *++s ) {

	BBox char_bbox = measure( c );

	bbox += char_bbox;
      }
    }

    return bbox;
  }

  BBox Face::measureRaw ( const char* s )
  {
    BBox bbox;

    for ( char c = *s; c != 0; c = *++s ) {
      BBox char_bbox;

      unsigned int f;
      FT_UInt glyph_index = 0;

      for ( f = 0; f < faces_.size(); f++ ) {
	glyph_index = FT_Get_Char_Index( faces_[f].face_, c );
	if ( glyph_index != 0 ) break;
      }

      if ( glyph_index == 0 ) continue;

      FT_Error error = FT_Load_Glyph( faces_[f].face_, glyph_index,
				      FT_LOAD_DEFAULT );
      if ( error != 0 ) continue;

      FT_Glyph glyph;
      error = FT_Get_Glyph( faces_[f].face_->glyph, &glyph );
      if ( error != 0 ) continue;

      FT_BBox ft_bbox;
      FT_Glyph_Get_CBox( glyph, ft_glyph_bbox_unscaled, &ft_bbox );

      FT_Done_Glyph( glyph );

      char_bbox = ft_bbox;
      char_bbox.advance_ = faces_[f].face_->glyph->advance;

      bbox += char_bbox;
    }

    return bbox;
  }


  // Measure the bounding box as if the (latin1) string were not rotated

  BBox Face::measure_nominal ( const char* s )
  {
    if ( string_rotation_ == 0. )
      return measure( s );

    for ( unsigned int f = 0; f < faces_.size(); f++ )
      FT_Set_Transform( faces_[f].face_, 0, 0 );

    BBox bbox = measure( s );

    float angle;
    if ( string_rotation_ < 0. ) {
      angle = 360. - fmod( fabs( string_rotation_ ), 360.f );
    }
    else {
      angle = fmod( string_rotation_, 360.f );
    }

    FT_Matrix rotation_matrix;
    FT_Vector sinus;

    FT_Vector_Unit( &sinus, (FT_Angle)(angle * 0x10000L) );

    rotation_matrix.xx = sinus.x;
    rotation_matrix.xy = -sinus.y;
    rotation_matrix.yx = sinus.y;
    rotation_matrix.yy = sinus.x;

    for ( unsigned int f = 0; f < faces_.size(); f++ )
      FT_Set_Transform( faces_[f].face_, &rotation_matrix, 0 );

    return bbox;
  }

  // Compile a (latin1) string into a display list

  GLuint Face::compile ( const char* s )
  {
    // First, make sure all the characters in the string are themselves
    // in display lists
    const char* s_tmp = s;

    for ( char c = *s_tmp; c != 0; c = *++s_tmp ) {
      compile( c );
    }

    GLuint dlist = glGenLists( 1 );
    glNewList( dlist, GL_COMPILE );

    glColor4f( foreground_color_[R], foreground_color_[G], foreground_color_[B],
	       foreground_color_[A] );
    if ( !advance_ )
      glPushMatrix();

    draw( s );

    if ( !advance_ )
      glPopMatrix();

    glEndList();

    return dlist;
  }

  // Compile a (latin1) character glyph into a display list and cache
  // it for later

  GLuint Face::compile ( unsigned char c )
  {
    // See if we've done it already

    GDLCI fgi = glyph_dlists_.find( c );

    if ( fgi != glyph_dlists_.end() )
      return fgi->second;

    unsigned int f;
    FT_UInt glyph_index = 0;

    for ( f = 0; f < faces_.size(); f++ ) {
      glyph_index = FT_Get_Char_Index( faces_[f].face_, c );
      if ( glyph_index != 0 ) break;
    }

    if ( glyph_index == 0 )
      return 0;

    GLuint dlist = compileGlyph( faces_[f].face_, glyph_index );

    glyph_dlists_[ c ] = dlist;

    return dlist;
  }

  // Assume the MODELVIEW matrix is already set and draw the (latin1)
  // string.  Note: this routine now ignores almost all settings:
  // including the position (both modelview and raster), color,
  // justification and advance settings. Consider this to be the raw
  // drawing routine for which you are responsible for most of the
  // setup.

  void Face::draw ( const char* s )
  {
    DLCI character_display_list = character_display_lists_.begin();

    for ( char c = *s; c != 0; c = *++s ) {

      if ( character_display_list != character_display_lists_.end() ) {
	glCallList( *character_display_list );
	character_display_list++;
      }

      draw( c );
    }
  }

  // Assume the MODELVIEW matrix is already setup and draw the
  // (latin1) character.

  void Face::draw ( unsigned char c )
  {
    // See if we've done it already

    GDLCI fgi = glyph_dlists_.find( c );

    if ( fgi != glyph_dlists_.end( ) ) {
      glCallList( fgi->second );
      return;
    }

    unsigned int f;
    FT_UInt glyph_index = 0;

    for ( f = 0; f < faces_.size(); f++ ) {
      glyph_index = FT_Get_Char_Index( faces_[f].face_, c );
      if ( glyph_index != 0 ) break;
    }

    if ( glyph_index == 0 )
      return;

    // Otherwise, either compile it (and call it) or ...

    else if ( compile_mode_ == COMPILE ) {
      GLuint dlist = compile( c );
      glCallList( dlist );
    }

    // ... render it immediately

    else {
      renderGlyph( faces_[f].face_, glyph_index );
    }
  }

  // Draw the (latin1) character at the given position. The MODELVIEW
  // matrix is modified by the glyph advance.

  void Face::draw ( GLfloat x, GLfloat y, unsigned char c )
  {
    glTranslatef( x, y, 0. );

    glColor4f( foreground_color_[R], foreground_color_[G], foreground_color_[B],
	       foreground_color_[A] );

    glRasterPos2i( 0, 0 );

    draw( c );
  }

  // Draw the (latin1) character at the given position. The MODELVIEW
  // matrix is modified by the glyph advance.

  void Face::draw ( GLfloat x, GLfloat y, GLfloat z, unsigned char c )
  {
    glTranslatef( x, y, z );

    glColor4f( foreground_color_[R], foreground_color_[G], foreground_color_[B],
	       foreground_color_[A] );

    glRasterPos2i( 0, 0 );

    draw( c );
  }

  // Draw the (latin1) string at the given position.

  void Face::draw ( GLfloat x, GLfloat y, const char* s )
  {
    if ( !advance_ )
      glPushMatrix();

    if ( horizontal_justification_ != ORIGIN ||
	 vertical_justification_ != BASELINE ) {
      glPushMatrix();

      BBox bbox = measure_nominal( s );

      GLfloat dx = 0, dy = 0;

      switch ( horizontal_justification_ ) {
      case LEFT:
	dx = -bbox.x_min_; break;
      case CENTER:
	dx = -( bbox.x_min_ + bbox.x_max_ ) / 2.; break;
      case RIGHT:
	dx = -bbox.x_max_; break;
      default:
	break;
      }
      switch ( vertical_justification_ ) {
      case BOTTOM:
	dy = -bbox.y_min_; break;
      case MIDDLE:
	dy = -( bbox.y_min_ + bbox.y_max_ ) / 2.; break;
      case TOP:
	dy = -bbox.y_max_; break;
      default:
	break;
      }

      // There is probably a less expensive way to compute this

      glRotatef( string_rotation_, 0., 0., 1. );
      glTranslatef( dx, dy, 0 );
      glRotatef( -string_rotation_, 0., 0., 1. );
    }

    glTranslatef( x, y, 0. );

    glColor4f( foreground_color_[R], foreground_color_[G], foreground_color_[B],
	       foreground_color_[A] );

    glRasterPos2i( 0, 0 );

    draw( s );

    if ( horizontal_justification_ != ORIGIN ||
	 vertical_justification_ != BASELINE )
      glPopMatrix();

    if ( !advance_ )
      glPopMatrix();
  }

  // Draw the (latin1) string at the given position.

  void Face::draw ( GLfloat x, GLfloat y, GLfloat z, const char* s )
  {
    if ( !advance_ )
      glPushMatrix();

    if ( horizontal_justification_ != ORIGIN ||
	 vertical_justification_ != BASELINE ) {
      glPushMatrix();

      BBox bbox = measure_nominal( s );

      GLfloat dx = 0, dy = 0;

      switch ( horizontal_justification_ ) {
      case LEFT:
	dx = -bbox.x_min_; break;
      case CENTER:
	dx = -( bbox.x_min_ + bbox.x_max_ ) / 2.; break;
      case RIGHT:
	dx = -bbox.x_max_; break;
      default:
	break;
      }
      switch ( vertical_justification_ ) {
      case BOTTOM:
	dy = -bbox.y_min_; break;
      case MIDDLE:
	dy = -( bbox.y_min_ + bbox.y_max_ ) / 2.; break;
      case TOP:
	dy = -bbox.y_max_; break;
      default:
	break;
      }

      // There is probably a less expensive way to compute this

      glRotatef( string_rotation_, 0., 0., 1. );
      glTranslatef( dx, dy, 0 );
      glRotatef( -string_rotation_, 0., 0., 1. );
    }

    glTranslatef( x, y, z );

    glColor4f( foreground_color_[R], foreground_color_[G], foreground_color_[B],
	       foreground_color_[A] );

    glRasterPos2i( 0, 0 );

    draw( s );

    if ( horizontal_justification_ != ORIGIN ||
	 vertical_justification_ != BASELINE )
      glPopMatrix();

    if ( !advance_ )
      glPopMatrix();
  }
