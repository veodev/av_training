#ifndef ACCESSORIES_H
#define ACCESSORIES_H

#include <QStringList>
#include <QListWidget>
#include <QListWidgetItem>
#include <QTime>
#include <QCoreApplication>

// keys for NAUTIZ X8 ANDROID
#define KEY_F1 16777264
#define KEY_F2 16777265
#define KEY_F3 16777266
#define KEY_CALL 17825796
#define KEY_ENTER 16777220
#define FN_KEY_F1 67
#define FN_KEY_F2 68
#define FN_KEY_F3 69
#define KEY_0 48
#define KEY_1 49
#define KEY_2 50
#define KEY_3 51
#define KEY_4 52
#define KEY_5 53
#define KEY_6 54
#define KEY_7 55
#define KEY_8 56
#define KEY_9 57

#define KEY_HASH 35
#define KEY_STAR 42

#define KEY_FN 0
#define FN_KEY_0 16777313  // set as "backKey" in NAUTIZ /Tools/Program Buttons
#define FN_KEY_1 16777217
#define FN_KEY_2 16777235
#define FN_KEY_3 16777219
#define FN_KEY_4 16777234
#define FN_KEY_6 16777236
#define FN_KEY_8 16777237
// KEY_END_CALL set for start app NoNaviBar in /Tools/Program Buttons

QStringList convertToQStringList(const QListWidget& listWidget);
QStringList convertToQStringList(const QList<QListWidgetItem*>& list);

QString convertBytesToView(double bytes);

void delay(int msToWait);

int createKeyFromSideAndChannel(const int side, const int channelId);
int createKeyFromSchemeAndSideAndChannelAndGateIndex(const int scheme, const int side, const int channelId, const int gateIndex);
int createKeyFromSideAndChannelAndGateIndex(const int side, const int channelId, const int gateIndex);

#endif  // ACCESSORIES_H
