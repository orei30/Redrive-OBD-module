#ifndef Gps_Reader_h
#define Gps_Reader_h

#include "global.h"
#include <TinyGPS++.h>

class GpsReader
{
public:
    GpsReader() : locationDefined(0),
                  dateDefined(0),
                  timeDefined(0),
                  satellitesDefined(0),
                  dateItemRequestTime(0)
    {
    }
    void init()
    {
        SerialGPS.begin(9600);
        SerialMon.println("gps initialized");
    }
    int process()
    {
        int result = 0;
        // If it is time to query
        if (dateItemRequestTime == 0)
        {
            SerialMon.println("start processing gps data.");
            locationDefined = 0;
            dateDefined = 0;
            timeDefined = 0;
            satellitesDefined = 0;
            dateItemRequestTime = millis();
        }
        // On timeout
        else if (dateItemRequestTime + DATE_QUERY_TIMEOUT <= millis())
        {
            SerialMon.println("gps processing timedout.");
            dateItemRequestTime = 0;
            if (!locationDefined)
            {
                record.setLatitude(-1);
                record.setLongitude(-1);
                record.setAltitude(-1);
            }
            if (!dateDefined)
            {
                record.setDate("-1");
            }
            if (!timeDefined)
            {
                record.setTime("-1");
            }
            if (!satellitesDefined)
            {
                record.setSatellitesNum(-1);
            }
            result = 1;
        }
        // Reading data
        else
        {
            if (SerialGPS.available())
            {
                SerialMon.println("gps serial available.");
                // check procces time if it if and if it while.
                if (gps.encode(SerialGPS.read()))
                {
                    SerialMon.println("gps encoding succeed.");
                    if (gps.location.isValid())
                    {
                        SerialMon.println("location is valid.");
                        record.setLatitude(gps.location.lat());
                        record.setLongitude(gps.location.lng());
                        record.setAltitude(gps.altitude.meters());
                        locationDefined = 1;
                        SerialMon.print("lat: ");
                        SerialMon.print(gps.location.lat());
                        SerialMon.print("lng: ");
                        SerialMon.print(gps.location.lng());
                        SerialMon.print("meters: ");
                        SerialMon.print(gps.altitude.meters());
                        SerialMon.println();
                    } // end if
                    if (gps.date.isValid())
                    {
                        SerialMon.println("date is valid.");
                        String str;
                        str = gps.date.day();
                        str += ("/");
                        str += gps.date.month();
                        str += ("/");
                        str += gps.date.year();
                        record.setDate(str);
                        dateDefined = 1;
                        SerialMon.print("date: ");
                        SerialMon.print(str);
                        SerialMon.println();
                    } // end if
                    if (gps.time.isValid())
                    {
                        SerialMon.println("time is valid.");
                        String str;
                        str = gps.time.hour();
                        str += (":");
                        str += gps.time.minute();
                        str += (":");
                        str += gps.time.second();
                        str += (".");
                        str += gps.time.centisecond();
                        record.setTime(str);
                        timeDefined = 1;
                        SerialMon.print("time: ");
                        SerialMon.print(str);
                        SerialMon.println();
                    } // end if
                    if (gps.satellites.isValid())
                    {
                        SerialMon.println("satellites is valid.");
                        record.setSatellitesNum(gps.satellites.value());
                        satellitesDefined = 1;
                        SerialMon.print("satellites: ");
                        SerialMon.print(gps.satellites.value());
                        SerialMon.println();
                    } // end if
                    result = 1;
                } //end if
            }     // end if
            else {
                SerialMon.println("gps serial unavailable.");
            }
            if (locationDefined && dateDefined && timeDefined && satellitesDefined)
                result = 1;
        }
        return result;
    }

private:
    TinyGPSPlus gps;
    int locationDefined;
    int dateDefined;
    int timeDefined;
    int satellitesDefined;
    unsigned long dateItemRequestTime;
};
#endif