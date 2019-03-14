#include "batteryplot.h"
#include "ui_batteryplot.h"

#include <QPainter>
#include <QFile>
#include "battery.h"
#include "debug.h"
#include "appdatapath.h"

bool BatteryPlot::loggingEnabled() const
{
    return _loggingEnabled;
}

void BatteryPlot::setLoggingEnabled(bool loggingEnabled)
{
    _loggingEnabled = loggingEnabled;
}

BatteryPlot::BatteryPlot(QWidget* parent)
    : QWidget(parent)
    , _loggingEnabled(false)
    , ui(new Ui::BatteryPlot)
{
    ui->setupUi(this);
    // Enable for data collection
    //ASSERT(connect(&Battery::instance(), &Battery::voltage, this, &BatteryPlot::onVoltage));
    //_timer.start();
}

BatteryPlot::~BatteryPlot()
{
    delete ui;
}

void BatteryPlot::paintEvent(QPaintEvent*)
{
    QPainter painter(this);

    int w = width() - 50;
    int h = height() - 20;

    painter.setPen(QPen(Qt::black, 2, Qt::SolidLine));
    painter.drawRect(0, 0, w, h);

    painter.setPen(QPen(Qt::darkGray, 1, Qt::DotLine));
    for (int i = 1; i < 6; ++i) {
        painter.drawLine(0, (h / 6.0) * i, w, (h / 6.0) * i);
    }

    painter.setPen(QPen(Qt::lightGray, 1, Qt::DotLine));
    for (int j = 1; j < 20; ++j) {
        painter.drawLine((w / 20.0) * j, 0, (w / 20.0) * j, h);
    }

    painter.setPen(QPen(Qt::red, 1, Qt::SolidLine));
    painter.drawText(0, h + 12, "t = 0.0s");

    int size = _voltageValues.size();
    float voltageStart = 9.0;
    float voltageEnd = 15.0;

    painter.drawText(w + 2, 10, QString::number(voltageEnd, 'f', 2) + "V");
    painter.drawText(w + 2, h, QString::number(voltageStart, 'f', 2) + "V");
    float voltagespan = voltageEnd - voltageStart;

    if (size != 0) {
        const VoltagePoint& first = _voltageValues.front();
        const VoltagePoint& last = _voltageValues.back();
        painter.drawText(w + 2, h - ((last.voltage - voltageStart) / voltagespan) * h, QString::number(last.voltage, 'f', 2) + "V");

        unsigned int timespan = last.time - first.time;
        painter.drawText(w - 40, h + 12, "t = " + QString::number(last.time) + "s");


        QPoint prevPoint(0, h - ((first.voltage - voltageStart) / voltagespan) * h);

        for (std::vector<VoltagePoint>::const_iterator it = _voltageValues.begin(); it != _voltageValues.end(); ++it) {
            const VoltagePoint& point = it.operator*();
            float step = ((point.time - first.time) / static_cast<float>(timespan)) * w;
            float voltageHeight = h - ((point.voltage - voltageStart) / voltagespan) * h;
            QPoint curPoint = QPoint(step, voltageHeight);
            painter.drawLine(prevPoint, curPoint);
            prevPoint = curPoint;
        }

        painter.setPen(QPen(Qt::blue, 1, Qt::SolidLine));
        prevPoint = QPoint(0, h - ((first.voltage - voltageStart) / voltagespan) * h);
        if (_voltageValues.size() > 10) {
            for (size_t i = 10; i < _voltageValues.size(); ++i) {
                const VoltagePoint& point = _voltageValues[i];
                float sum = _voltageValues[i - 9].voltage + _voltageValues[i - 8].voltage + _voltageValues[i - 7].voltage + _voltageValues[i - 6].voltage + _voltageValues[i - 5].voltage + _voltageValues[i - 4].voltage + _voltageValues[i - 3].voltage + _voltageValues[i - 2].voltage
                            + _voltageValues[i - 1].voltage + _voltageValues[i].voltage;
                float average = sum / 10.0;

                float step = ((point.time - first.time) / static_cast<float>(timespan)) * w;
                float voltageHeight = h - ((average - voltageStart) / voltagespan) * h;
                QPoint curPoint = QPoint(step, voltageHeight);
                painter.drawLine(prevPoint, curPoint);
                prevPoint = curPoint;
            }
            int i = _voltageValues.size() - 1;
            float sum = _voltageValues[i - 9].voltage + _voltageValues[i - 8].voltage + _voltageValues[i - 7].voltage + _voltageValues[i - 6].voltage + _voltageValues[i - 5].voltage + _voltageValues[i - 4].voltage + _voltageValues[i - 3].voltage + _voltageValues[i - 2].voltage
                        + _voltageValues[i - 1].voltage + _voltageValues[i].voltage;
            float average = sum / 10.0;
            painter.drawText(w + 2, h - ((average - voltageStart) / voltagespan) * h + 30, QString::number(average, 'f', 2) + "V");
        }
    }
    update();
}

void BatteryPlot::onVoltage(double voltage)
{
    VoltagePoint point;
    point.voltage = static_cast<float>(voltage);
    point.time = qRound(_timer.elapsed() / 1000.0);
    _voltageValues.push_back(point);
    if (_loggingEnabled) {
        if ((_voltageValues.size() % 60) == 0) {
            QFile output(logsPath() + "batterystats.txt");
            output.open(QIODevice::Truncate | QIODevice::Text | QIODevice::ReadWrite);

            QTextStream out(&output);
            for (std::vector<VoltagePoint>::const_iterator it = _voltageValues.begin(); it != _voltageValues.end(); ++it) {
                const VoltagePoint& point = it.operator*();
                out << point.time << "," << QString::number(point.voltage, 'f', 2) << endl;
            }

            output.close();
        }
    }
}
