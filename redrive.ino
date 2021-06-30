#include "src/can_reader.h"
#include "src/gps_reader.h"
#include "src/gyro_reader.h"
#include "src/gsm_connector.h"

char *buf;
int canFinished = 0;
int gpsFinished = 0;
int gyroFinished = 0;

GsmConnector gsmConnector;
PubSubClient mqtt;
CanReader canReader;
GpsReader gpsReader;
GyroReader gyroReader;

void setup()
{
	SerialMon.begin(115200);
	mqtt = *gsmConnector.init();
	SerialMon.println("gsm initialized");
	canReader.init();
	gpsReader.init();
	gyroReader.init();
	gsmConnector.connect(RESTART);
}

void loop()
{
	if (!canFinished)
		// try to get data from the can bus
		canFinished = canReader.process();
	if (!gpsFinished)
		// try to get data from the GPS sensor
		gpsFinished = gpsReader.process();
	if (!gyroFinished)
		// try to get data from the gyro
		gyroFinished = gyroReader.process();
	if (canFinished && gpsFinished && gyroFinished)
	{
		SerialMon.println(record.getData());
		// if all data arrived, send it to the server
		gsmConnector.sendRecord(record.getData().c_str());
		canFinished = 0;
		gpsFinished = 0;
		gyroFinished = 0;
	} // end if
	mqtt.loop(); // Make mqtt work, dont move it!
}
