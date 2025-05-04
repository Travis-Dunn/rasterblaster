Rasterblaster is a software raster renderer, by Travis Dunn.

## Configuration

This project uses macros to swap functions out at compile time, contained in
config.h. An example is USE_SAFE_PIXEL_FUNCTIONS - define it to use a version
of PutPixel that includes bounds checking, comment it out to use the faster
version.

## Function naming convention

Function names which include an underscore are implementation functions, and
are intended to be called through the control macros set in config.h.
These implementation functions follow the scheme [basename]_[suffix], where
suffix is optional and descriptive. The lack of a suffix indicates that the
function in question is the most basic version.
