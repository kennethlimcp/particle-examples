/*
Written by kennethlimcp on 25 November 2018
Argon and Xenon battery voltage calculation

Official docs: https://docs.particle.io/reference/device-os/firmware#battery-voltage

GND ---- R2 ---- R1 --- Vbatt
             |
             |
            Vbatt_adc (P0.05, BATT)

R1 =  806 kOhm
R2 = 2000 kOhm

ADC resolution is 3.3 volts / 4096 units or, 0.0008 volts (0.8 mV) per unit

Vbatt_adc = Vbatt * R2/(R1 + R2) * 0.0008
Vbatt = Vbatt_adc * (R1 + R2)/R2 * 0.0008
Vbatt = Vbatt_adc * (2000 + 806)/2000 * 0.0008
Vbatt = Vbatt_adc * 1.403 * 0.0008
Vbatt = Vbatt_adc * 0.0011224

*/

void setup() {
    pinMode(BATT, INPUT);
}

unsigned long old_time = 0;

void loop() {
    if(millis() - old_time > 10000) {
        float voltage = analogRead(BATT) * 0.0011224;
        
        Particle.publish("voltage", String::format("%.2f",voltage), PRIVATE);
        old_time = millis();
    }
}
