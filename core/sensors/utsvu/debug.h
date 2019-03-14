#ifndef DEBUG_H
#define DEBUG_H

#if defined(QT_NO_DEBUG)
    #define QDEBUG    if(1); else qDebug()
    #define QWARNING  qWarning()
    #define QCRITICAL qCritical()
    #define QFATAL    qFatal()
#else
    #define QDEBUG    qDebug()    << Q_FUNC_INFO << '(' << __LINE__ << "):"
    #define QWARNING  qWarning()  << Q_FUNC_INFO << '(' << __LINE__ << ") warning:"
    #define QCRITICAL qCritical() << Q_FUNC_INFO << '(' << __LINE__ << ") error:"
    #define QFATAL    qFatal()    << Q_FUNC_INFO << '(' << __LINE__ << ") fatal error:"
#endif

#endif // DEBUG_H
