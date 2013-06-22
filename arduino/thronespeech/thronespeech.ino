
const int hubSensor = 0; // Anolog 0
const int remoteSensorTx = 1; // Uno tx0
const int remoteSensorRx = 0; // Uno rx1

const int debugLED = 13; // as ever

long averagedVoltage, summedVoltage, distance = 0; // Distance in Inches
int average = 10;
bool pulse = true;

void setup() {
 Serial.begin(9600);
 
}

void loop() {
    showRate();    
    summedVoltage = 0;
    
    // Sensor info!
    // ~9.8mV/in at 5v (Vcc/5   12 per inch)
    // Smooth samples
    for(int i=0;i<average;i++) {
        averagedVoltage = analogRead(hubSensor)/2;    // Arduino analog is 0-1024 so divide by 2
        summedVoltage += averagedVoltage;
        delay(10);
    }
    
    distance = (summedVoltage/average);
    sendSample(distance);
}

void showRate() {
    pulse = !pulse;
    if(pulse) digitalWrite(debugLED,HIGH);
    else digitalWrite(debugLED,LOW);
}

void sendSample(int _data) {
    int data = map(_data,0,512,0,255);
    Serial.write(data);
}