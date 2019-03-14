#ifndef COLORSCHEMEMANAGER_HPP
#define COLORSCHEMEMANAGER_HPP

#include <QObject>
#include "colorscheme.h"
#include <vector>


class ColorSchemeManager : public QObject
{
    Q_OBJECT
private:
    const static unsigned short COLOR_1 = 0xF800;  // RED
    const static unsigned short COLOR_2 = 0x001F;  // BLUE
    const static unsigned short COLOR_3 = 0x4EFB;  // CYAN
    const static unsigned short COLOR_4 = 0x3E49;  // MAGENTA

    const static unsigned short COLOR_R = 0x9800;  // RED
    const static unsigned short COLOR_G = 0x07E0;  // GREEN
    const static unsigned short COLOR_Y = 0xFFE0;  // YELLOW
    const static unsigned short COLOR_B = 0x001F;  // BLUE
    int _currentShemeIndex;
    std::vector<ColorScheme> _schemes;
    void loadSchemes();

    void setIndividualColors(ColorScheme& colorScheme);

public:
    ColorScheme getDefaultLightScheme();
    ColorScheme getDefaultDarkScheme();
    ColorScheme getCurrentScheme();

    ColorSchemeManager(QObject* parent = nullptr);
    std::vector<ColorScheme> getSchemes() const;
    void switchCurrentScheme(int index);
};

#endif  // COLORSCHEMEMANAGER_HPP
