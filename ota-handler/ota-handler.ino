#define DEBUG_LED D7

void setup() {
	pinMode(DEBUG_LED, OUTPUT);
	digitalWrite(DEBUG_LED, LOW);

	System.disableUpdates();
 System.on(firmware_update_pending, otaHandler);
}


void otaHandler () {
	doBeforeOTA();

	System.enableUpdates();
	digitalWrite(DEBUG_LED, HIGH);
}

void doBeforeOTA() {
	// Add stuff that you want to execute before an OTA update happens
	// eg. Turn off lights and motor
}
