#include <Adafruit_GFX.h>    // Core graphics library

bool CompareFontNames(const char *name, char *compare);
GFXfont *GetFont(char *name);
char * getFontNamesAsJson();