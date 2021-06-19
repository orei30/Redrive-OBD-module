#ifndef Gyro_Reader_h
#define Gyro_Reader_h

#include "global.h"
#include <Wire.h>

class GyroReader
{
public:
    GyroReader() : MPU_ADDR(0x68),
                   dateItemRequestTime(0)
    {
    }
    void init()
    {
        Wire.begin();
        Wire.beginTransmission(MPU_ADDR); // Begins a transmission to the I2C slave (GY-521 board)
        Wire.write(0x6B);                 // PWR_MGMT_1 register
        Wire.write(0);                    // set to zero (wakes up the MPU-6050)
        Wire.endTransmission(true);
        SerialMon.println("gyro initialized");
    }
    int process()
    {
        SerialMon.println("start processing gyro data.");
        int result = 0;
        // If it is time to query
        if (dateItemRequestTime == 0)
        {
            dateItemRequestTime = millis();
        }
        // On timeout
        else if (dateItemRequestTime + DATE_QUERY_TIMEOUT <= millis())
        {
            SerialMon.println("gyro processing timedout.");
            dateItemRequestTime = 0;
            result = 1;
        }
        // Reading data
        else
        {
            SerialMon.println("getting gyro data");
            Wire.beginTransmission(MPU_ADDR);
            Wire.write(0x3B);                        // starting with register 0x3B (ACCEL_XOUT_H) [MPU-6000 and MPU-6050 Register Map and Descriptions Revision 4.2, p.40]
            Wire.endTransmission(false);             // the parameter indicates that the Arduino will send a restart. As a result, the connection is kept active.
            Wire.requestFrom(MPU_ADDR, 7 * 2, true); // request a total of 7*2=14 registers

            ax = Wire.read() << 8 | Wire.read(); // reading registers: 0x3B (ACCEL_XOUT_H) and 0x3C (ACCEL_XOUT_L)
            ay = Wire.read() << 8 | Wire.read(); // reading registers: 0x3D (ACCEL_YOUT_H) and 0x3E (ACCEL_YOUT_L)
            az = Wire.read() << 8 | Wire.read(); // reading registers: 0x3F (ACCEL_ZOUT_H) and 0x40 (ACCEL_ZOUT_L)
            gx = Wire.read() << 8 | Wire.read(); // reading registers: 0x43 (GYRO_XOUT_H) and 0x44 (GYRO_XOUT_L)
            gy = Wire.read() << 8 | Wire.read(); // reading registers: 0x45 (GYRO_YOUT_H) and 0x46 (GYRO_YOUT_L)
            gz = Wire.read() << 8 | Wire.read(); // reading registers: 0x47 (GYRO_ZOUT_H) and 0x48 (GYRO_ZOUT_L)
            record.setAcceX(ax);
            record.setAcceY(ay);
            record.setAcceZ(az);
            record.setGyroX(gx);
            record.setGyroY(gy);
            record.setGyroZ(gz);
            SerialMon.print("aX = ");
            SerialMon.print(ax);
            SerialMon.print(" | aY = ");
            SerialMon.print(ay);
            SerialMon.print(" | aZ = ");
            SerialMon.print(az);
            SerialMon.print(" | gX = ");
            SerialMon.print(gx);
            SerialMon.print(" | gY = ");
            SerialMon.print(gy);
            SerialMon.print(" | gZ = ");
            SerialMon.print(gz);
            result = 1;
        } // end if
        return result;
    }

private:
    const int MPU_ADDR;
    unsigned long dateItemRequestTime;
    int16_t ax, ay, az;
    int16_t gx, gy, gz;
};

#endif