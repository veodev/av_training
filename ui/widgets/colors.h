#ifndef COLORS_H
#define COLORS_H

#include <qcolor.h>

QColor HLStoRGB(int H, int L, int S);
ushort HLStoRGB16(int H);
QColor getColorFromRGB565(unsigned short color);
unsigned short getRGB565FromQColor(const QColor& color);
unsigned short getGradientColor(float gradPosition, unsigned short begin, unsigned short end);

#endif  // COLORS_H
