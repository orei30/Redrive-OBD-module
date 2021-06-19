#include "src/can_reader.h"
#include "src/gps_reader.h"
#include "src/gyro_reader.h"
#include "src/gsm_connector.h"

char *buf;
int canFinished = 0;
int gpsFinished = 0;
int gyroFinished = 0;

GsmConnector gsmConnector;
// CanReader canReader;
GpsReader gpsReader;
GyroReader gyroReader;

void setup()
{
	SerialMon.begin(115200);
	// gsmConnector.init();
	// SerialMon.println("gsm initialized");
	gpsReader.init();
	gyroReader.init();
	// gsmConnector.connect(1);
}

void loop()
{
	// if (!canFinished)
	// 	canFinished = canReader.process();
	if (!gpsFinished)
		gpsFinished = gpsReader.process();
	if (!gyroFinished)
		gyroFinished = gyroReader.process();
	if (gpsFinished && gyroFinished)
	{
		SerialMon.println(record.getData());
		gsmConnector.sendRecord(record.getData().c_str());
		canFinished = 0;
		gpsFinished = 0;
		gyroFinished = 0;
	} // end if
	// mqtt.loop(); // Make mqtt work, dont move it!
}
