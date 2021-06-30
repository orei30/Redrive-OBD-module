#ifndef Gsm_Connector_h
#define Gsm_Connector_h

#include "global.h"
#include <TinyGsmClient.h>
#include <PubSubClient.h>

unsigned long dateItemRequestTime = 0;
const char *userName = "User1";
const char apn[] = "internetg";
const char gprsUser[] = "";
const char gprsPass[] = "";
const char *broker = "broker.hivemq.com";
const char *topicOfNewRecord = "redrive/records";
const char *topicOfDriveStatus = "redrive/drives";
uint32_t lastReconnectAttempt = 0;

#define RESTART 1
#define No_RESTART 0

class GsmConnector
{
public:
    GsmConnector() : modem(SerialGSM),
                     client(modem),
                     mqtt(client),
                     startDrive(1),
                     dateItemRequestTime(0)
    {
    }
    PubSubClient* init()
    {
        SerialGSM.begin(9600);
        return &mqtt;
    }
    void connect(int restart)
    {
        SerialMon.println("Wait...");
        SerialMon.println("Initializing modem...");
        if (restart)
            modem.restart();
        else
            modem.init();
        String modemInfo = modem.getModemInfo();
        SerialMon.print("Modem Info: ");
        SerialMon.println(modemInfo);
        if (GSM_PIN && modem.getSimStatus() != 3)
        {
            modem.simUnlock(GSM_PIN);
        }
        SerialMon.print("Waiting for network...");
        if (!modem.waitForNetwork())
        {
            SerialMon.println(" fail");
            return;
        }
        SerialMon.println(" success");
        if (modem.isNetworkConnected())
        {
            SerialMon.println("Network connected");
        }
        SerialMon.print(F("Connecting to "));
        SerialMon.print(apn);
        if (!modem.gprsConnect(apn, gprsUser, gprsPass))
        {
            SerialMon.println(" fail");
            return;
        }
        SerialMon.println(" success");

        if (modem.isGprsConnected())
        {
            SerialMon.println("GPRS connected");
        }
        // MQTT Broker setup
        mqtt.setServer(broker, 1883);
    }
    int process()
    {
        int result = 0;
        // If it is time to query
        if (dateItemRequestTime == 0)
        {
            dateItemRequestTime = millis();
        }
        // On timeout
        else if (dateItemRequestTime + DATE_QUERY_TIMEOUT <= millis())
        {
            dateItemRequestTime = 0;
        }
        // Reading data
        else
        {
            if (!mqtt.connected())
            {
                SerialMon.println("=== MQTT NOT CONNECTED ===");
                // Reconnect every 10 seconds
                unsigned long t = millis();
                if (t - lastReconnectAttempt > 10000L)
                {
                    lastReconnectAttempt = t;
                    if (mqttConnect())
                    {
                        lastReconnectAttempt = 0;
                    } // end if
                }     // end if
                delay(100);
                return;
            } // end if
        }     // end if
        if (startDrive == 1)
        {
            mqtt.publish(topicOfDriveStatus, "START_DRIVE@User1");
            startDrive = 0;
        } // end if
        return result;
    }

    void sendRecord(const char *record)
    {
        mqtt.publish(topicOfNewRecord, record);
    }

private:
    TinyGsm modem;
    TinyGsmClient client;
    PubSubClient mqtt;
    unsigned long dateItemRequestTime;
    int startDrive;

    boolean mqttConnect()
    {
        SerialMon.print("Connecting to ");
        SerialMon.print(broker);

        // Connect to MQTT Broker
        boolean status = mqtt.connect("GsmClientTest");

        // Or, if you want to authenticate MQTT:
        //boolean status = mqtt.connect("GsmClientName", "mqtt_user", "mqtt_pass");

        if (status == false)
        {
            SerialMon.println(" fail");
            return false;
        }
        SerialMon.println(" success");
        return mqtt.connected();
    }
};
#endif