#ifndef DEBUG_H
#define DEBUG_H

#include <QDebug>

#if defined(QT_NO_DEBUG)
#   define ASSERT(cond) cond
#   define QDEBUG    if(1); else qDebug()
#   define QWARNING  if(1); else qWarning()
#   define QCRITICAL if(1); else qCritical()
#   define QFATAL    if(1): else qFatal()
#   define TRACE_VALUE(value) do {} while(0)
#else
#   define ASSERT(cond) Q_ASSERT(cond)
#   define QDEBUG    qDebug()
#   define QWARNING  qWarning()
#   define QCRITICAL qCritical()
#   define QFATAL    qFatal()
#   define TRACE_VALUE(value) qDebug() << #value << ":'" << value << '\''
#   ifndef DEBUG
#       define DEBUG
#   endif
#endif

#endif // DEBUG_H
