#ifndef Record_h
#define Record_h

class Record
{
public:
    Record() :
        time(-1), date(-1), longitude(-1), latitude(-1), altitude(-1), satellitesNum(-1), // GPS data
        speed(-1), rpm(-1), throttlePosition(-1), engineLoad(-1), maf(-1), // OBD data
        acceX(0), acceY(0), acceZ(0), gyroX(0),gyroY(0), gyroZ(0) // Gyro data
    {
    }

    void setTime(String currentTime) {
        time = currentTime;
    }

    void setDate(String currentDate) {
        date = currentDate;
    }

    void setLatitude(double currentLatitude) {
        latitude = currentLatitude;
    }

    void setLongitude(double currentLongitude) {
        longitude = currentLongitude;
    }

    void setAltitude(double currentAltitude) {
        altitude = currentAltitude;
    }

    void setSatellitesNum(int currentSatellitesNum) {
        satellitesNum = currentSatellitesNum;
    }

    void setSpeed(int currentSpeed) {
        speed = currentSpeed;
    }

    void setRPM(int currentRPM) {
        rpm = currentRPM;
    }

    void setThrottlePosition(int currentThrottlePosition) {
        throttlePosition = currentThrottlePosition;
    }

    void setEngineLoad(int currentEngineLoad) {
        engineLoad = currentEngineLoad;
    }

    void setMaf(int currentMaf) {
        maf = currentMaf;
    }

    void setAcceX(int16_t currentAcceX) {
        acceX = currentAcceX;
    }

    void setAcceY(int16_t currentAcceY) {
        acceY = currentAcceY;
    }

    void setAcceZ(int16_t currentAcceZ) {
        acceZ = currentAcceZ;
    }

    void setGyroX(int16_t currentGyroX) {
        gyroX = currentGyroX;
    }

    void setGyroY(int16_t currentGyroY) {
        gyroY = currentGyroY;
    }

    void setGyroZ(int16_t currentGyroZ) {
        gyroZ = currentGyroZ;
    }

    String getData() {
        String str;
        str = "RECORD";
        str += "@";
        str += "User1";
        str += "@";
        str += time;
        str += "@";
        str += date;
        str += "@";
        str += String(latitude, 6);
        str += "@";
        str += String(longitude, 6);
        str += "@";
        str += String(altitude, 6);
        str += "@";
        str += satellitesNum;
        str += "@";

        str += speed;
        str += "@";
        str += rpm;
        str += "@";
        str += throttlePosition;
        str += "@";
        str += engineLoad;
        str += "@";
        str += maf;

        str += "@";
        str += acceX;
        str += "@";
        str += acceY;
        str += "@";
        str += acceZ;
        str += "@";
        str += gyroX;
        str += "@";
        str += gyroY;
        str += "@";
        str += gyroZ;
        return str;
    }

private:
    String time;
    String date;
    double latitude;
    double longitude;
    double altitude;
    int satellitesNum;
    int speed;
    int rpm;
    int throttlePosition;
    int engineLoad;
    int maf;
    int16_t acceX;
    int16_t acceY;
    int16_t acceZ;
    int16_t gyroX;
    int16_t gyroY;
    int16_t gyroZ;
};

#endif
