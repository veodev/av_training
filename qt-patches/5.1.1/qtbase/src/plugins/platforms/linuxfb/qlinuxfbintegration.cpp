/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the plugins of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qlinuxfbintegration.h"
#include "qlinuxfbscreen.h"

#include "qvirtualscreen.h"

#include <QtPlatformSupport/private/qgenericunixfontdatabase_p.h>
#include <QtPlatformSupport/private/qgenericunixeventdispatcher_p.h>
#include <QtPlatformSupport/private/qfbbackingstore_p.h>
#include <QtPlatformSupport/private/qfbwindow_p.h>
#include <QtPlatformSupport/private/qfbcursor_p.h>

#include <QtGui/private/qguiapplication_p.h>
#include <QtGui/private/qpixmap_raster_p.h>

QT_BEGIN_NAMESPACE

QLinuxFbIntegration::QLinuxFbIntegration(const QStringList &paramList)
    : m_fontDb(new QGenericUnixFontDatabase()),
      m_eventDispatcher(createUnixEventDispatcher())
{
    QGuiApplicationPrivate::instance()->setEventDispatcher(m_eventDispatcher);

    m_primaryScreen = new QLinuxFbScreen;
    if (m_primaryScreen->initialize(paramList))
        screenAdded(m_primaryScreen);

    initializeVirtualScreens(paramList);
}

QLinuxFbIntegration::~QLinuxFbIntegration()
{
    qDeleteAll(m_virtualScreens.begin(), m_virtualScreens.end());
    delete m_primaryScreen;
}

bool QLinuxFbIntegration::hasCapability(QPlatformIntegration::Capability cap) const
{
    switch (cap) {
    case ThreadedPixmaps: return true;
    default: return QPlatformIntegration::hasCapability(cap);
    }
}

QPlatformPixmap *QLinuxFbIntegration::createPlatformPixmap(QPlatformPixmap::PixelType type) const
{
    return new QRasterPlatformPixmap(type);
}

QPlatformBackingStore *QLinuxFbIntegration::createPlatformBackingStore(QWindow *window) const
{
    return new QFbBackingStore(window);
}

QPlatformWindow *QLinuxFbIntegration::createPlatformWindow(QWindow *window) const
{
    return new QFbWindow(window);
}

QAbstractEventDispatcher *QLinuxFbIntegration::guiThreadEventDispatcher() const
{
    return m_eventDispatcher;
}

QList<QPlatformScreen *> QLinuxFbIntegration::screens() const
{
    QList<QPlatformScreen *> list;
    list.append(m_primaryScreen);
    list.append(m_virtualScreens);
    return list;
}

QPlatformFontDatabase *QLinuxFbIntegration::fontDatabase() const
{
    return m_fontDb;
}

void QLinuxFbIntegration::initializeVirtualScreens(const QStringList &args)
{
    QRegExp virtualScreenRx(QLatin1String("virtual=(-?\\d+)x(-?\\d+)x(\\d+)x(\\d+)"));
    foreach (const QString &arg, args) {
        if (virtualScreenRx.indexIn(arg) != -1) {
            QVirtualScreen *virtualScreen = new QVirtualScreen(
                QRect(
                    virtualScreenRx.cap(1).toInt(),
                    virtualScreenRx.cap(2).toInt(),
                    virtualScreenRx.cap(3).toInt(),
                    virtualScreenRx.cap(4).toInt())
            );
            screenAdded(virtualScreen);
            m_virtualScreens.append(virtualScreen);
        }
    }

}

QT_END_NAMESPACE
