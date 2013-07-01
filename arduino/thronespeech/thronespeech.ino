#include <SoftwareSerial.h>

const int hubSensor = 0; // Anolog 0
const int remoteSensorTx = 2; // Uno
const int remoteSensorRx = 3; // Uno

const int debugLED = 13; // as ever

long voltage, distance = 0; // Distance in In1ches
int average = 10;
bool pulse = true;

SoftwareSerial remoteSensor(remoteSensorRx, remoteSensorTx);

void setup() {
 Serial.begin(9600);
 remoteSensor.begin(4800);
}

void loop() {
    showRate();    
    summedVoltage = 0;
    voltage = getAveragedAnalog();
    serial = getAverageRemote();
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

long getAveragedAnalog() {
    // Sensor info!
    // ~9.8mV/in at 5v (Vcc/5   12 per inch)
    // Smooth samples
    int summedVoltage,averagedVoltage = 0;
    for(int i=0;i<average;i++) {
        averagedVoltage = analogRead(hubSensor)/2;    // Arduino analog is 0-1024 so divide by 2
        summedVoltage += averagedVoltage;
        delay(5);
    }
    return summedVoltage;
}

long getAveragedRemote() {
    // Via http://forum.arduino.cc/index.php/topic,114808.0.html
    int result;
    char inData[4]; //char array to read data into
    int index = 0;
    
    
    sonarSerial.flush(); // Clear cache ready for next reading
    
    while (stringComplete == false) {
        //Serial.print("reading ");    //debug line
        
        if (sonarSerial.available())
        {
            char rByte = sonarSerial.read(); //read serial input for "R" to mark start of data
            if(rByte == 'R')
            {
                while (index < 3)  //read next three character for range from sensor
                {
                    if (sonarSerial.available())
                    {
                        inData[index] = sonarSerial.read();
                        //Serial.println(inData[index]); //Debug line
                        
                        index++;  // Increment where to write next
                    }
                }
                inData[index] = 0x00; //add a padding byte at end for atoi() function
            }
            
            rByte = 0; //reset the rByte ready for next reading
            
            index = 0; // Reset index ready for next reading
            stringComplete = true; // Set completion of read to true
            result = atoi(inData); // Changes string data into an integer for use
        }
    }
    
    return result;
}