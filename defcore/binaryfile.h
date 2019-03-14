#ifndef BINARYFILE_H
#define BINARYFILE_H

#include <QString>
#include <QFile>
#include <QBuffer>
#include <QMessageBox>
#include <QTextStream>
#include <QTextCodec>
#include <QFileInfo>
#include <QDateTime>


class cBinaryFile
{
public:
    QFile *DataFile;
    cBinaryFile(QString FN);
    void S(uchar d0,
           uchar d1,
           uchar d2,
           uchar d3,
           uchar d4,
           uchar d5,
           uchar d6,
           uchar d7,
           uchar d8,
           uchar d9,
           uchar d10,
           uchar d11,
           uchar d12,
           uchar d13,
           uchar d14,
           uchar d15,
           uchar d16,
           uchar d17,
           uchar d18,
           uchar d19, int Len);
};

#endif // BINARYFILE_H
