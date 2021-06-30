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
    }
    int process()
    {
        int result = 0;
        // If it is time to query
        if (dateItemRequestTime == 0)
        {
            // initialize the start time for this query
            dateItemRequestTime = millis();
            // reset flags
            locationDefined = 0;
            dateDefined = 0;
            timeDefined = 0;
            satellitesDefined = 0;
        }
        // On timeout
        else if (dateItemRequestTime + DATE_QUERY_TIMEOUT <= millis())
        {

            // if failed to get the data, set it to -1
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
            dateItemRequestTime = 0;
        }
        // Reading data
        else
        {
            if (SerialGPS.available())
            {
                // read data from gps serial
                if (gps.encode(SerialGPS.read()))
                {
                    if (gps.location.isValid())
                    {
                        // if location is valid set to record
                        record.setLatitude(gps.location.lat());
                        record.setLongitude(gps.location.lng());
                        record.setAltitude(gps.altitude.meters());
                        locationDefined = 1;
                    } // end if
                    if (gps.date.isValid())
                    {
                        // if date is valid set to record
                        String str;
                        str = gps.date.day();
                        str += ("/");
                        str += gps.date.month();
                        str += ("/");
                        str += gps.date.year();
                        record.setDate(str);
                        dateDefined = 1;
                    } // end if
                    if (gps.time.isValid())
                    {
                        // if time is valid set to record
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
                    } // end if
                    if (gps.satellites.isValid())
                    {
                        // if satellites is valid set to record
                        record.setSatellitesNum(gps.satellites.value());
                        satellitesDefined = 1;
                    } // end if
                    result = 1;
                    dateItemRequestTime = 0;
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