#ifndef SATELLITES_H
#define SATELLITES_H

#include <QWidget>
#include <QGeoSatelliteInfo>

namespace Ui
{
class Satellites;
}

class Satellites : public QWidget
{
    Q_OBJECT

public:
    explicit Satellites(QWidget* parent = 0);
    ~Satellites();

private slots:
    void satellitesInViewUpdated(const QList<QGeoSatelliteInfo>& satellites);

private:
    Ui::Satellites* ui;
};

#endif  // SATELLITES_H
