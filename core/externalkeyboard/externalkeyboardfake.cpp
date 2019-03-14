#include <QFile>
#include <QDebug>

#include "externalkeyboard.h"

ExternalKeyboard::ExternalKeyboard(QObject* parent)
    : QObject(parent)
{
    qDebug() << "Fake keyboard created!";
}

ExternalKeyboard::~ExternalKeyboard()
{
    qDebug() << "Fake keyboard deleted!";
}

void ExternalKeyboard::tick() {}
