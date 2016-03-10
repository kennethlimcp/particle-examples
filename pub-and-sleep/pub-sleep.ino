/*
Demo for a Particle device to wake up periodically, read an analog pin and publish an event
*/

SYSTEM_MODE(SEMI_AUTOMATIC);

long sleep_duration = 10;

void setup(){
    pinMode(A0,INPUT);
    WiFi.on();
    Particle.connect();
}


void loop(){
   if(Particle.connected()){
       int pubValue = analogRead(A0);
       Particle.publish("test", String(pubValue));
       delay(1000);
       System.sleep(SLEEP_MODE_DEEP, sleep_duration);
   }

}
