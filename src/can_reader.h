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

    void init()
    {
        mcp2515.reset();
        mcp2515.setBitrate(CAN_500KBPS, MCP_8MHZ); // tals car
        // mcp2515.setBitrate(CAN_1000KBPS);          // omris car
        mcp2515.setNormalMode();
        msgToSend.can_id = 0x7DF;
        msgToSend.can_dlc = 8;
        msgToSend.data[0] = 0x02;
        msgToSend.data[1] = 0x01;
        msgToSend.data[3] = 0x00;
        msgToSend.data[4] = 0x00;
        msgToSend.data[5] = 0x00;
        msgToSend.data[6] = 0x00;
        msgToSend.data[7] = 0x00;
    }

    int process()
    {
        int result = 0;
        // if it is time to query
        if (dateItemRequestTime == 0)
        {
            // send message to can bus
            setMessageAndSend(dataItems[dataItemIndex]);
            // initialize the start time for this query
            dateItemRequestTime = millis();
            // reset flags
            speedDefined = 0;
            rpmDefined = 0;
            throttleDefined = 0;
            engineLoadDefined = 0;
            mafDefined = 0;
        }
        // on timeout
        else if (dateItemRequestTime + DATE_QUERY_TIMEOUT <= millis())
        {
            // check the current data item
            // if failed to get the data, set it to -1
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
            // reset request time for the next query
            dateItemRequestTime = 0;
        }
        // Reading data
        else
        {
            // if data arrived from can bus
            // check if it is the data we ask for
            // if it is set to record
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
        } // end if
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

    /**
     * send message to can bus to get the relevant data
    */
    void setMessageAndSend(unsigned char pid)
    {
        msgToSend.data[2] = pid;
        mcp2515.sendMessage(&msgToSend);
    }
};
#endif