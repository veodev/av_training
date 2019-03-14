#include "colorschememanager.h"
#include "accessories.h"
#include <QDebug>
#include "ChannelsIds.h"
#include "colors.h"

std::vector<ColorScheme> ColorSchemeManager::getSchemes() const
{
    return _schemes;
}

void ColorSchemeManager::switchCurrentScheme(int index)
{
    qDebug() << "Current scheme:" << index;
    _currentShemeIndex = index;
}

void ColorSchemeManager::loadSchemes()
{
    _schemes.push_back(getDefaultLightScheme());
    _schemes.push_back(getDefaultDarkScheme());
    _currentShemeIndex = 0;
}

void ColorSchemeManager::setIndividualColors(ColorScheme& colorScheme)
{
    colorScheme.setIndividualColorForKey(F70E, getRGB565FromQColor(QColor(200, 200, 200)));
    colorScheme.setIndividualColorForKey(B70E, getRGB565FromQColor(QColor(255, 201, 14)));
    colorScheme.setIndividualColorForKey(F58ELW, getRGB565FromQColor(Qt::red));
    colorScheme.setIndividualColorForKey(B58ELW, getRGB565FromQColor(Qt::blue));
    colorScheme.setIndividualColorForKey(F58ELU, getRGB565FromQColor(QColor(255, 174, 201)));
    colorScheme.setIndividualColorForKey(B58ELU, getRGB565FromQColor(QColor(153, 217, 234)));
    colorScheme.setIndividualColorForKey(F58MLW, getRGB565FromQColor(Qt::red));
    colorScheme.setIndividualColorForKey(B58MLW, getRGB565FromQColor(Qt::blue));
    colorScheme.setIndividualColorForKey(F58MLU, getRGB565FromQColor(QColor(255, 174, 201)));
    colorScheme.setIndividualColorForKey(B58MLU, getRGB565FromQColor(QColor(153, 217, 234)));
    colorScheme.setIndividualColorForKey(F42E, getRGB565FromQColor(QColor(136, 0, 21)));
    colorScheme.setIndividualColorForKey(B42E, getRGB565FromQColor(QColor(63, 72, 204)));
    colorScheme.setIndividualColorForKey(N0EMS, getRGB565FromQColor(QColor(185, 122, 87)));
}

ColorScheme ColorSchemeManager::getDefaultLightScheme()
{
    ColorScheme colorScheme;

    colorScheme.setName(tr("Light"));
    colorScheme.setForwardColor(0xFFFF);
    colorScheme.setBackwardColor(0xFFE0);

    colorScheme.setColorForKey(cdZoomIn1, COLOR_1);
    colorScheme.setColorForKey(cdZoomIn2, COLOR_3);
    colorScheme.setColorForKey(cdZoomOut1, COLOR_2);
    colorScheme.setColorForKey(cdZoomOut2, COLOR_4);
    colorScheme.setColorForKey(cdDirect1, COLOR_1);
    colorScheme.setColorForKey(cdDirect2, COLOR_2);
    colorScheme.setColorForKey(cdTMDirect1, COLOR_1);
    colorScheme.setColorForKey(cdTMDirect2, COLOR_2);
    colorScheme.setColorForKey(cdNotSet, 0x0000);

    setIndividualColors(colorScheme);

    return colorScheme;
}

ColorScheme ColorSchemeManager::getDefaultDarkScheme()
{
    ColorScheme colorScheme;

    colorScheme.setName(tr("Dark"));
    colorScheme.setForwardColor(0x0000);
    colorScheme.setBackwardColor(0x8888);

    colorScheme.setColorForKey(cdZoomIn1, 0x07E0);
    colorScheme.setColorForKey(cdZoomIn2, 0x07E0);
    colorScheme.setColorForKey(cdZoomOut1, 0xFFE0);
    colorScheme.setColorForKey(cdZoomOut2, 0xFFE0);
    colorScheme.setColorForKey(cdDirect1, 0xFFE0);
    colorScheme.setColorForKey(cdDirect2, 0x07E0);
    colorScheme.setColorForKey(cdTMDirect1, 0xFFE0);
    colorScheme.setColorForKey(cdTMDirect2, 0x07E0);
    colorScheme.setColorForKey(cdNotSet, 0x0000);

    setIndividualColors(colorScheme);

    return colorScheme;
}

ColorScheme ColorSchemeManager::getCurrentScheme()
{
    return _schemes.at(_currentShemeIndex);
}

ColorSchemeManager::ColorSchemeManager(QObject* parent)
    : QObject(parent)
    , _currentShemeIndex(-1)
{
    loadSchemes();
}
