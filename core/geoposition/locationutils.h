#ifndef LOCATIONUTILS_H
#define LOCATIONUTILS_H

#include <QtCore/QtGlobal>

class QTime;
class QByteArray;
class QGeoPositionInfo;
class QGeoSatelliteInfo;

class LocationUtils
{
public:
    inline static bool isValidLat(double lat)
    {
        return ((lat >= -90) && (lat <= 90));
    }
    inline static bool isValidLong(double lng)
    {
        return ((lng >= -180) && (lng <= 180));
    }

    inline static double clipLat(double lat)
    {
        if (lat > 90) {
            lat = 90;
        }
        else if (lat < -90) {
            lat = -90;
        }
        return lat;
    }

    inline static double wrapLong(double lng)
    {
        if (lng > 180) {
            lng -= 360;
        }
        else if (lng < -180) {
            lng += 360;
        }
        return lng;
    }

    /*
        Creates a QGeoPositionInfo from a GGA, GLL, RMC, VTG or ZDA sentence.

        Note:
        - GGA and GLL sentences have time but not date so the update's
          QDateTime object will have an invalid date.
        - RMC reports date with a two-digit year so in this case the year
          is assumed to be after the year 2000.
    */
    static bool getPosInfoFromNmea(const char* data, int size, QGeoPositionInfo* info, bool* hasFix = 0);

    /*
        Creates QGeoSatelliteInfo list from a GSV sentence.
     */
    static bool getSatInfoFromNmea(const char* data, int size, QList<QGeoSatelliteInfo>& satellites, bool& isCompleted, bool& isInUse);

    /*
        Returns true if the given NMEA sentence has a valid checksum.
    */
    static bool hasValidNmeaChecksum(const char* data, int size);

    /*
        Returns time from a string in hhmmss or hhmmss.z+ format.
    */
    static bool getNmeaTime(const QByteArray& bytes, QTime* time);

    /*
        Accepts for example ("2734.7964", 'S', "15306.0124", 'E') and returns the
        lat-long values. Fails if lat or long fail isValidLat() or isValidLong().
    */
    static bool getNmeaLatLong(const QByteArray& latString, char latDirection, const QByteArray& lngString, char lngDirection, double* lat, double* lon);
};

#endif  // LOCATIONUTILS_H
