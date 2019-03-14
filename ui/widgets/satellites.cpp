#include "satellites.h"
#include "ui_satellites.h"
#include "debug.h"
#include "geoposition.h"

Satellites::Satellites(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::Satellites)
{
    ui->setupUi(this);
#if defined TARGET_AVICON31
    GeoPosition* geoPosition = GeoPosition::instance("/dev/ttymxc1");
#elif defined TARGET_AVICON15
    GeoPosition* geoPosition = GeoPosition::instance();
#endif
#if defined TARGET_AVICON15 || defined TARGET_AVICON31
    ASSERT(connect(geoPosition, &GeoPosition::satellitesInViewUpdated, this, &Satellites::satellitesInViewUpdated));
#endif
}

Satellites::~Satellites()
{
    delete ui;
}

void Satellites::satellitesInViewUpdated(const QList<QGeoSatelliteInfo>& satellites)
{
    QTableWidget* tableWidget = NULL;

    if (satellites.count()) {
        if (satellites.at(0).satelliteSystem() == QGeoSatelliteInfo::GPS) {
            tableWidget = ui->gpsTableWidget;
        }
        else if (satellites.at(0).satelliteSystem() == QGeoSatelliteInfo::GLONASS) {
            tableWidget = ui->glonasTableWidget;
        }
    }

    if (tableWidget != NULL) {
        tableWidget->clear();
        tableWidget->setRowCount(satellites.count());
        tableWidget->setColumnCount(2);
        tableWidget->setHorizontalHeaderItem(0, new QTableWidgetItem(QString(tr("Satellite Id"))));
        tableWidget->setHorizontalHeaderItem(1, new QTableWidgetItem(QString(tr("Strength"))));
        for (int i = 0; i < satellites.count(); ++i) {
            tableWidget->setItem(i, 0, new QTableWidgetItem(QString::number(satellites.at(i).satelliteIdentifier())));
            tableWidget->setItem(i, 1, new QTableWidgetItem(QString::number(satellites.at(i).signalStrength())));
        }
    }
    else {
        qDebug() << "Error: tableWidget == NULL!";
    }
}
