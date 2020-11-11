// gl_header.h

// Include whichever header is used for OpenGL on this operating system.
#ifdef __APPLE__
#include <OpenGL/GL3.h>
#else
#include <GL/glew.h>
#endif

