// Font.cpp

#include "Font.h"

#include "Colour.h"
#include "Files.h"
#include "ImageBuffer.h"
#include "Screen.h"

#include "gl_header.h"

#include <algorithm>
#include <cmath>
#include <string>
#include <vector>

using namespace std;

namespace {
  bool showUnderlines = false;
  const int TOTAL_TAB_STOPS = 8;
  const Font::Layout defaultParams;
  
  // 95 and x5f means "_".
  const vector<string> acceptableCharacterReferences{ "gt;", "lt;", "amp;", "#95;", "#x5f;", "#x5F;" };

  // Convert from PANGO to pixel scale.
  int PixelFromPangoCeil(int pangoScale)
  {
    return (pangoScale + PANGO_SCALE - 1) / PANGO_SCALE;
  }
}



Font::Font()
  : shader(), vao(0), vbo(0), scaleI(0), centreI(0), sizeI(0), colourI(0),
  screenWidth(1), screenHeight(1), viewWidth(1), viewHeight(1), cr(nullptr),
  fontDescName(), refDescName(), context(nullptr), layout(nullptr), lang(nullptr),
  pixelSize(0), fontViewHeight(0), surfaceWidth(256), surfaceHeight(64), cache()
{
  lang = pango_language_from_string("en");
  SetUpShader();
  UpdateSurfaceSize();

  cache.SetUpdateInterval(3600);
}



Font::~Font()
{
  if(cr)
    cairo_destroy(cr);
  if(layout)
    g_object_unref(layout);
}



void Font::SetFontDescription(const std::string &desc)
{
  fontDescName = desc;
  UpdateFontDesc();
}



 void Font::SetLayoutReference(const std::string &desc)
{
  refDescName = desc;
  UpdateFontDesc();
}



void Font::SetPixelSize(int size)
{
  pixelSize = size;
  UpdateFontDesc();
}



void Font::SetLanguage(const std::string &langCode)
{
  lang = pango_language_from_string(langCode.c_str());
  UpdateFontDesc();
}



void Font::Draw(const std::string &str, const Point &point, const Colour &colour, const Layout *params) const
{
  DrawCommon(str, point.X(), point.Y(), colour, params, true);
}



void Font::DrawAliased(const std::string &str, double x, double y, const Colour &colour, const Layout *params) const
{
  DrawCommon(str, x, y, colour, params, false);
}



int Font::Height(const std::string &str, const Layout *params) const
{
  if(str.empty())
    return 0;

  const RenderedText &text = Render(str, params);
  if(!text.texture)
    return 0;
  return TextFromViewCeilY(text.height);
}



int Font::Width(const std::string &str, const Layout *params) const
{
    return TextFromViewCeilX(ViewWidth(str, params));
}



int Font::Height() const
{
  return TextFromViewCeilY(fontViewHeight);
}



void Font::ShowUnderlines(bool show)
{
  showUnderlines = show;
}



string Font::EscapeMarkupHasError(const string &str)
{
  const string text = ReplaceCharacters(str);
  if(pango_parse_markup(text.c_str(), -1, '_', nullptr, nullptr, nullptr, nullptr))
    return str;
  else
  {
    string result;
    for(const auto &c : text)
    {
      if(c == '<')
        result += "&lt;";
      else if(c == '>')
        result += "&gt;";
      else
        result += c;
    }

     return result;
  }
}



void Font::UpdateSurfaceSize() const
{
  if(cr)
    cairo_destroy(cr);

  cairo_surface_t *sf = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, surfaceWidth, surfaceHeight);
  cr = cairo_create(sf);
  cairo_surface_destroy(sf);

  if(layout)
    g_object_unref(layout);
  layout = pango_cairo_create_layout(cr);
  context = pango_layout_get_context(layout);

  pango_layout_set_wrap(layout, PANGO_WRAP_WORD);

  UpdateFontDesc();
}



void Font::UpdateFontDesc() const
{
  if(pixelSize <= 0)
    return;

  // Get font descriptions.
  PangoFontDescription *fontDesc = pango_font_description_from_string(fontDescName.c_str());
  PangoFontDescription *refDesc = pango_font_description_from_string(refDescName.c_str());

  // Set the pixel size.
  const int fontSize = ViewFromTextFloorY(pixelSize) * PANGO_SCALE;
  pango_font_description_set_absolute_size(fontDesc, fontSize);
  pango_font_description_set_absolute_size(refDesc, fontSize);

  // Update the context.
  pango_context_set_language(context, lang);

  // Update the layout.
  pango_layout_set_font_description(layout, fontDesc);

  // Update layout parameters.
  PangoFontMetrics *metrics = pango_context_get_metrics(context, refDesc, lang);
  const int ascent = pango_font_metrics_get_ascent(metrics);
  const int descent = pango_font_metrics_get_descent(metrics);
  fontViewHeight = PixelFromPangoCeil(ascent + descent);

  // Clean up.
  pango_font_metrics_unref(metrics);
  pango_font_description_free(fontDesc);
  pango_font_description_free(refDesc);
  cache.Clear();

  // Tab Stop
  space = ViewWidth(" ");
  const int tabSize = 4 * space * PANGO_SCALE;
  PangoTabArray *tb = pango_tab_array_new(TOTAL_TAB_STOPS, FALSE);
  for(int i = 0; i < TOTAL_TAB_STOPS; ++i)
    pango_tab_array_set_tab(tb, i, PANGO_TAB_LEFT, i * tabSize);
  pango_layout_set_tabs(layout, tb);
  pango_tab_array_free(tb);
}



// Replaces straight quotation marks with curly ones,
// and escapes "&" except for minimum necessaries because a pilot name may
// contain "&" and that representation should be the same as the old version.
string Font::ReplaceCharacters(const string &str)
{
  string buf;
  buf.reserve(str.length());
  bool isAfterWhitespace = true;
  bool isAfterAccel = false;
  bool isTag = false;
  const size_t len = str.length();
  for(size_t pos = 0; pos < len; ++pos)
  {
    if(isTag)
    {
      if(str[pos] == '>')
        isTag = false;
      buf.append(1, str[pos]);
    }
    else
    {
      // U+2018 LEFT_SINGLE_QUOTATION_MARK
      // U+2019 RIGHT_SINGLE_QUOTATION_MARK
      // U+201C LEFT_DOUBLE_QUOTATION_MARK
      // U+201D RIGHT_DOUBLE_QUOTATION_MARK
      if(str[pos] == '\'')
        buf.append(isAfterWhitespace ? "\xE2\x80\x98" : "\xE2\x80\x99");
      else if(str[pos] == '"')
        buf.append(isAfterWhitespace ? "\xE2\x80\x9C" : "\xE2\x80\x9D");
      else if(isAfterAccel && str[pos] == '_')
        // Remove an extra underbar.
        ;
      else if(str[pos] == '&')
      {
        buf.append(1, '&');
        bool hit = false;
        for(const auto &s : acceptableCharacterReferences)
        {
          const size_t slen = s.length();
          if(len - pos > slen && !str.compare(pos + 1, slen, s))
          {
            hit = true;
            break;
          }
        }
        if(!hit)
          buf.append("amp;");
      }
      else
        buf.append(1, str[pos]);
      isAfterWhitespace = (str[pos] == ' ');
      isAfterAccel = (str[pos] == '_');
      isTag = (str[pos] == '<');
    }
  }
  return buf;
}



string Font::RemoveAccelerator(const string &str)
{
  string dest;
  bool isTag = false;
  for(char c : str)
  {
    if(isTag)
    {
      dest += c;
      isTag = (c != '>');
    }
    else if(c == '<')
    {
      dest += c;
      isTag = true;
    }
    else if(c != '_')
      dest += c;
  }
  return dest;
}



void Font::DrawCommon(const std::string &str, double x, double y, const Colour &colour, const Layout *params, bool alignToDot) const
{
  if(str.empty())
    return;

  const bool screenChanged = Screen::Width() != screenWidth || Screen::Height() != screenHeight;
  if(screenChanged)
  {
    screenWidth = Screen::Width();
    screenHeight = Screen::Height();
    GLint xyhw[4];
    glGetIntegerv(GL_VIEWPORT, xyhw);
    viewWidth = xyhw[2];
    viewHeight = xyhw[3];

    UpdateFontDesc();
  }

  const RenderedText &text = Render(str, params);
  if(!text.texture)
    return;

  glUseProgram(shader.Object());
  glBindVertexArray(vao);

  // Update the texture.
  glBindTexture(GL_TEXTURE_2D, text.texture);

  // Update the scale, only if the screen size has changed.
  if(screenChanged)
  {
    GLfloat scale[2] = {2.f / viewWidth, -2.f / viewHeight};
    glUniform2fv(scaleI, 1, scale);

  }

  // Update the centre.
  Point centre = Point(ViewFromTextX(x), ViewFromTextY(y));
  if(alignToDot)
    centre = Point(round(centre.X()), round(centre.Y()));
  centre += text.centre;
  glUniform2f(centreI, centre.X(), centre.Y());

  // Update the size.
  glUniform2f(sizeI, text.width, text.height);

  // Update the colour.
  glUniform4fv(colourI, 1, colour.Get());

  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

  glBindTexture(GL_TEXTURE_2D, 0);
  glBindVertexArray(0);
  glUseProgram(0);
}



// Render the text.
const Font::RenderedText &Font::Render(const string &str, const Layout *params) const
{
    if(!params)
    params = &defaultParams;

  // Return if already cached.
  const CacheKey key(str, *params, showUnderlines);
  auto cached = cache.Use(key);
  if(cached.second)
    return *cached.first;

  // Convert to viewport coodinates.
  Layout viewParams(*params);
  if(params->width > 0)
    viewParams.width = ViewFromTextX(params->width);
  if(params->lineHeight != DEFAULT_LINE_HEIGHT)
    viewParams.lineHeight = ViewFromTextFloorY(params->lineHeight);
  if(params->paragraphBreak != 0)
    viewParams.paragraphBreak = ViewFromTextFloorY(params->paragraphBreak);

  // Truncate
  const int layoutWidth = viewParams.width < 0 ? -1 : viewParams.width * PANGO_SCALE;
  pango_layout_set_width(layout, layoutWidth);
  PangoEllipsizeMode ellipsize;
  switch(viewParams.truncate)
  {
  case TRUNC_NONE:
    ellipsize = PANGO_ELLIPSIZE_NONE;
    break;
  case TRUNC_FRONT:
    ellipsize = PANGO_ELLIPSIZE_START;
    break;
  case TRUNC_MIDDLE:
    ellipsize = PANGO_ELLIPSIZE_MIDDLE;
    break;
  case TRUNC_BACK:
    ellipsize = PANGO_ELLIPSIZE_END;
    break;
  default:
    ellipsize = PANGO_ELLIPSIZE_NONE;
  }
  pango_layout_set_ellipsize(layout, ellipsize);

  // Align and justification
  PangoAlignment align;
  gboolean justify;
  switch(viewParams.align)
  {
  case LEFT:
    align = PANGO_ALIGN_LEFT;
    justify = FALSE;
    break;
  case CENTRE:
    align = PANGO_ALIGN_CENTER;
    justify = FALSE;
    break;
  case RIGHT:
    align = PANGO_ALIGN_RIGHT;
    justify = FALSE;
    break;
  case JUSTIFIED:
    align = PANGO_ALIGN_LEFT;
    justify = TRUE;
    break;
  default:
    align = PANGO_ALIGN_LEFT;
    justify = FALSE;
  }
  pango_layout_set_justify(layout, justify);
  pango_layout_set_alignment(layout, align);

  // Replaces straight quotation marks with curly ones.
  const string text = ReplaceCharacters(str);
  
  // Keyboard accelerator
  char *removeMarkupText;
  const char *rawText;
  PangoAttrList *al = nullptr;
  GError *error = nullptr;
  const char accel = showUnderlines ? '_' : '\0';
  const string &nonAccelText = RemoveAccelerator(text);
  const string &parseText = showUnderlines ? text : nonAccelText;
  if(pango_parse_markup(parseText.c_str(), -1, accel, &al, &removeMarkupText, 0, &error))
    rawText = removeMarkupText;
  else
  {
    if(error->message)
      Files::LogError(error->message);
    rawText = nonAccelText.c_str();
  }

  // Set the text and attributes to layout.
  pango_layout_set_text(layout, rawText, -1);
  pango_layout_set_attributes(layout, al);
  pango_attr_list_unref(al);

  // Check the image buffer size.
  int textWidth;
  int textHeight;
  pango_layout_get_pixel_size(layout, &textWidth, &textHeight);
  // Pango draws a PANGO_UNDERLINE_LOW under the logical rectangle,
  // and an underline may be longer than a text width.
  PangoRectangle ink_rect;
  pango_layout_get_pixel_extents(layout, &ink_rect, nullptr);
  textHeight = max(textHeight, ink_rect.y + ink_rect.height);
  textWidth = max(textWidth, ink_rect.x + ink_rect.width);
  // Check this surface has enough width.
  if(surfaceWidth < textWidth)
  {
    surfaceWidth *= (textWidth / surfaceWidth) + 1;
    UpdateSurfaceSize();
    return Render(str, params);
  }

  // Render
  const bool isDefaultLineHeight = viewParams.lineHeight == DEFAULT_LINE_HEIGHT;
  const bool isDefaultSkip = isDefaultLineHeight && viewParams.paragraphBreak == 0;
  cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
  if(isDefaultSkip)
  {
    cairo_move_to(cr, 0, 0);
    pango_cairo_show_layout(cr, layout);
  }
  else
  {
    // Control line skips and paragraph breaks manually.
    const char *layoutText = pango_layout_get_text(layout);
    PangoLayoutIter *it = pango_layout_get_iter(layout);
    int y0 = pango_layout_iter_get_baseline(it);
    int baselineY = PixelFromPangoCeil(y0);
    int sumExtraY = 0;
    PangoRectangle logical_rect;
    pango_layout_iter_get_line_extents(it, nullptr, &logical_rect);
    cairo_move_to(cr, PixelFromPangoCeil(logical_rect.x), baselineY);
    pango_cairo_update_layout(cr, layout);
    PangoLayoutLine *line = pango_layout_iter_get_line_readonly(it);
    pango_cairo_show_layout_line(cr, line);
    while(pango_layout_iter_next_line(it))
    {
      const int y1 = pango_layout_iter_get_baseline(it);
      const int index = pango_layout_iter_get_index(it);
      const int diffY = PixelFromPangoCeil(y1 - y0);
      if(layoutText[index] == '\0')
      {
        sumExtraY -= diffY;
        break;
      }
      int add = isDefaultLineHeight ? diffY : viewParams.lineHeight;
      if(layoutText[index-1] == '\n')
        add += viewParams.paragraphBreak;
      baselineY += add;
      sumExtraY += add - diffY;
      pango_layout_iter_get_line_extents(it, nullptr, &logical_rect);
      cairo_move_to(cr, PixelFromPangoCeil(logical_rect.x), baselineY);
      pango_cairo_update_layout(cr, layout);
      line = pango_layout_iter_get_line_readonly(it);
      pango_cairo_show_layout_line(cr, line);
      y0 = y1;
    }
    textHeight += sumExtraY + viewParams.paragraphBreak;
    pango_layout_iter_free(it);
  }

  // Check this surface has enough height.
  if(surfaceHeight < textHeight)
  {
    surfaceHeight *= (textHeight / surfaceHeight) + 1;
    UpdateSurfaceSize();
    return Render(str, params);
  }

  // Copy to image buffer and clear the surface.
  cairo_surface_t *sf = cairo_get_target(cr);
  cairo_surface_flush(sf);
  ImageBuffer image;
  image.Allocate(textWidth, textHeight);
  uint32_t *src = reinterpret_cast<uint32_t*>(cairo_image_surface_get_data(sf));
  uint32_t *dest = image.Pixels();
  const int stride = surfaceWidth - textWidth;
  for(int y = 0; y < textHeight; ++y)
  {
    for(int x = 0; x < textWidth; ++x)
    {
      *dest = *src;
      *src = 0;
      ++dest;
      ++src;
    }
    src += stride;
  }
  cairo_surface_mark_dirty(sf);

  // Try to reuse an old texture.
  GLuint texture = 0;
  auto recycled = cache.Recycle();
  if(recycled.second)
    texture = recycled.first.texture;

  // Record rendered text.
  RenderedText &renderedText = recycled.first;
  renderedText.texture = texture;
  renderedText.width = textWidth;
  renderedText.height = textHeight;
  renderedText.centre.X() = .5 * textWidth;
  renderedText.centre.Y() = .5 * textHeight;

  // Upload the image as a texture.
  if(!renderedText.texture)
    glGenTextures(1, &renderedText.texture);
  glBindTexture(GL_TEXTURE_2D, renderedText.texture);
  const auto &cachedText = cache.New(key, move(renderedText));
  
  // Use linear interpolation and no wrapping.
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  // Upload the image data (target, mipmap level, internal format, width, height, border, input format, data type, data).
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, textWidth, textHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, image.Pixels());

  // Unbind the texture.
  glBindTexture(GL_TEXTURE_2D, 0);

  return cachedText;
}



void Font::SetUpShader()
{
  static const char *vertexCode =
    // Parameter: Convert pixel coordinates to GL coordinates (-1 to 1).
    "uniform vec2 scale;\n"
    // Parameter: Position of the top left corner of the texture in pixels.
    "uniform vec2 centre;\n"
    // Parameter: Size of the texture in pixels.
    "uniform vec2 size;\n"

    // Input: Coordinate from the VBO.
    "in vec2 vert;\n"

    // Output: Texture coordinate for the fragment shader.
    "out vec2 texCoord;\n"

    "void main() {\n"
    "  gl_Position = vec4((centre + vert * size) * scale, 0, 1);\n"
    "  texCoord = vert + vec2(.5, .5);\n"
    "}\n";

  static const char *fragmentCode =
    // Parameter: Texture with the text.
    "uniform sampler2D tex;\n"
    // Parameter: Colour to apply to the text.
    "uniform vec4 colour = vec4(1, 1, 1, 1);\n"

    // Input: Texture coordinate from the vertex shader.
    "in vec2 texCoord;\n"

    // Output: Colour for the screen.
    "out vec4 finalColour;\n"

    "void main() {\n"
    "  finalColour = colour * texture(tex, texCoord);\n"
    "}\n";
  
  shader = Shader(vertexCode, fragmentCode);
  scaleI = shader.Uniform("scale");
  centreI = shader.Uniform("centre");
  sizeI = shader.Uniform("size");
  colourI = shader.Uniform("colour");

  // The texture always comes from texture unit 0.
  glUseProgram(shader.Object());
  glUniform1i(shader.Uniform("tex"), 0);
  glUseProgram(0);
  
  // Create the VAO and VBO.
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);
  
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  
  // Triangle strip.
  GLfloat vertexData[] = {
    -.5f, -.5f,
    -.5f,  .5f,
     .5f, -.5f,
     .5f,  .5f
  };
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);
  
  glEnableVertexAttribArray(shader.Attrib("vert"));
  glVertexAttribPointer(shader.Attrib("vert"), 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), nullptr);
  
  // Unbind the VBO and VAO.
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
  
  // We must update the screen size next time we draw.
  screenWidth = 1;
  screenHeight = 1;
  viewWidth = 1;
  viewHeight = 1;
}



int Font::ViewWidth(const std::string &str, const Layout *params) const
{
  if(str.empty())
    return 0;
const RenderedText &text = Render(str, params);
  if(!text.texture)
    return 0;
  return text.width;
}



double Font::ViewFromTextX(double x) const
{
  return x * viewWidth / screenWidth;
}



double Font::ViewFromTextY(double y) const
{
  return y * viewHeight / screenHeight;
}



int Font::ViewFromTextX(int x) const
{
  return (x * viewWidth + screenWidth/2) / screenWidth;
}



int Font::ViewFromTextY(int y) const
{
  return (y * viewHeight + screenHeight/2) / screenHeight;
}



int Font::ViewFromTextCeilX(int x) const
{
  return (x * viewWidth + screenWidth - 1) / screenWidth;
}



int Font::ViewFromTextCeilY(int y) const
{
  return (y * viewHeight + screenHeight - 1) / screenHeight;
}



int Font::ViewFromTextFloorX(int x) const
{
  return (x * viewWidth) / screenWidth;
}



int Font::ViewFromTextFloorY(int y) const
{
  return (y * viewHeight) / screenHeight;
}



double Font::TextFromViewX(double x) const
{
  return x * screenWidth / viewWidth;
}



double Font::TextFromViewY(double y) const
{
  return y * screenHeight / viewHeight;
}



int Font::TextFromViewX(int x) const
{
  return (x * screenWidth + viewWidth/2) / viewWidth;
}



int Font::TextFromViewY(int y) const
{
  return (y * screenHeight + viewHeight/2) / viewHeight;
}



int Font::TextFromViewCeilX(int x) const
{
  return (x * screenWidth + viewWidth - 1) / viewWidth;
}



int Font::TextFromViewCeilY(int y) const
{
  return (y * screenHeight + viewHeight - 1) / viewHeight;
}



int Font::TextFromViewFloorX(int x) const
{
  return (x * screenWidth) / viewWidth;
}



int Font::TextFromViewFloorY(int y) const
{
  return (y * screenHeight) / viewHeight;
}

