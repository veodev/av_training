#ifndef COLORSCHEME_H
#define COLORSCHEME_H

#include <QString>
#include <QFile>
#include <map>

#include "Definitions.h"

class ColorScheme
{
private:
    using ColorKey = eBScanColorDescr;
    using ColorType = unsigned short;
    QString _name;

    ColorType _forwardColor;
    ColorType _backwardColor;
    std::map<ColorKey, ColorType> _colors;
    std::map<CID, ColorType> _individualColors;

public:
    ColorScheme();
    bool loadFromFile(QFile& file);
    bool saveToFile(QFile& file);
    ColorType getColorForKey(ColorKey key);
    void setColorForKey(ColorKey key, ColorType color);
    ColorType getIndividualColorForKey(CID cid);
    void setIndividualColorForKey(CID cid, ColorType color);
    QString getName() const;
    void setName(const QString& name);
    ColorType getForwardColor() const;
    void setForwardColor(const ColorType& forwardColor);
    ColorType getBackwardColor() const;
    void setBackwardColor(const ColorType& backwardColor);
};

#endif  // COLORSCHEME_H
