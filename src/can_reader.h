#ifndef Can_Reader_h
#define Can_Reader_h

#include "global.h"
#include <SPI.h>
#include <mcp2515.h>

#define VEHICLE_SPEED 0x0D
#define ENGINE_RPM 0x0C
#define THROTTLE_POSITION 0x11
#define ENGINE_LOAD 0x04
#define MAF 0x10

class CanReader
{
public:
    CanReader() : mcp2515(53),
                  dataItemIndex(0),
                  speedDefined(0),
                  rpmDefined(0),
                  throttleDefined(0),
                  engineLoadDefined(0),
                  mafDefined(0),
                  dateItemRequestTime(0),
                  dataItems{VEHICLE_SPEED, ENGINE_RPM, THROTTLE_POSITION, ENGINE_LOAD, MAF}
    {
    }

    int process()
    {
        int result = 0;
        // If it is time to query
        if (dateItemRequestTime == 0)
        {
            setMessageAndSend(dataItems[dataItemIndex]);
            dateItemRequestTime = millis();
            speedDefined = 0;
            rpmDefined = 0;
            throttleDefined = 0;
            engineLoadDefined = 0;
            mafDefined = 0;
        }
        // On timeout
        else if (dateItemRequestTime + DATE_QUERY_TIMEOUT <= millis())
        {
            switch (dataItems[dataItemIndex])
            {
            case VEHICLE_SPEED:
                if (!speedDefined)
                    record.setSpeed(-1);
                break;
            case ENGINE_RPM:
                if (!rpmDefined)
                    record.setRPM(-1);
                break;
            case THROTTLE_POSITION:
                if (!throttleDefined)
                    record.setThrottlePosition(-1);
                break;
            case ENGINE_LOAD:
                if (!engineLoadDefined)
                    record.setEngineLoad(-1);
                break;
            case MAF:
                if (!mafDefined)
                    record.setMaf(-1);
                result = 1;
                break;
            default:
                SerialMon.println("something went wromg with can reader switch");
                break;
            } // end switch
            dataItemIndex = 0;
            dateItemRequestTime = 0;
            result = 1;
        }
        // Reading data
        else
        {
            if (mcp2515.readMessage(&returnedMsg) == MCP2515::ERROR_OK && returnedMsg.data[2] == dataItems[dataItemIndex])
            {
                switch (dataItems[dataItemIndex])
                {
                case VEHICLE_SPEED:
                    record.setSpeed(returnedMsg.data[3]);
                    speedDefined = 1;
                    break;
                case ENGINE_RPM:
                    record.setRPM(((returnedMsg.data[3] * 256) + returnedMsg.data[4]) / 4);
                    rpmDefined = 1;
                    break;
                case THROTTLE_POSITION:
                    record.setThrottlePosition(returnedMsg.data[3] * (100 / 255));
                    throttleDefined = 1;
                    break;
                case ENGINE_LOAD:
                    record.setEngineLoad(returnedMsg.data[3] * (100 / 255));
                    engineLoadDefined = 1;
                    break;
                case MAF:
                    record.setMaf(((returnedMsg.data[3] * 256) + returnedMsg.data[4]) / 100);
                    mafDefined = 1;
                    result = 1;
                    break;
                default:
                    SerialMon.println("something went wromg with can reader switch");
                    break;
                } // end switch
                dataItemIndex = (dataItemIndex + 1) % (sizeof(dataItems) / sizeof(int));
                dateItemRequestTime = 0;
            } // end if
        }     // end if
        return result;
    }

private:
    MCP2515 mcp2515;
    struct can_frame msgToSend;
    struct can_frame returnedMsg;
    int dataItems[5];
    int speedDefined;
    int rpmDefined;
    int throttleDefined;
    int engineLoadDefined;
    int mafDefined;
    int dataItemIndex;
    unsigned long dateItemRequestTime;

    void setMessageAndSend(unsigned char pid)
    {
        msgToSend.data[2] = pid;
        mcp2515.sendMessage(&msgToSend);
    }
};
#endif