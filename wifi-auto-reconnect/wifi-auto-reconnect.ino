/*
This will better manage Wifi connection and
automatically power cycle the Wifi module and perform reconnection attempts.
*/

SYSTEM_MODE(SEMI_AUTOMATIC);

uint8_t retry_count = 0;
unsigned long old_time = millis();

void setup(){
    WiFi.on();
}


void loop(){
     if(millis() - old_time >= 2000){
        if(retry_count < 10){
            if(!WiFi.ready()){
                WiFi.connect();
                retry_count++;

            }
            else if (!Particle.connected()){
                Particle.connect();
                retry_count++;
            }
        }
        else{
            WiFi.off();
            retry_count = 0;
            WiFi.on();
        }
        old_time = millis();
    }


    // Insert user code here
}
