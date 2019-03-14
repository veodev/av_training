#ifndef QPLATFORMINTEGRATION_VIRTUALSCREEN_H
#define QPLATFORMINTEGRATION_VIRTUALSCREEN_H

#include <qpa/qplatformintegration.h>
#include <qpa/qplatformscreen.h>

QT_BEGIN_NAMESPACE

class QVirtualScreen : public QPlatformScreen
{
public:
    QVirtualScreen(const QRect &geometry)
        : mGeometry(geometry)
        , mDepth(32)
        , mFormat(QImage::Format_ARGB32_Premultiplied)
        , mPhysicalSize(geometry.size())
    {}

    QRect geometry() const { return mGeometry; }
    int depth() const { return mDepth; }
    QImage::Format format() const { return mFormat; }

public:
    QRect mGeometry;
    int mDepth;
    QImage::Format mFormat;
    QSize mPhysicalSize;
};

QT_END_NAMESPACE

#endif // QPLATFORMINTEGRATION_VIRTUALSCREEN_H
