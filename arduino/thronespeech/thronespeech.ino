#include <SoftwareSerial.h>

const int hubSensor = 0; // Anolog 0
const int remoteSensorTx = 2; // Uno
const int remoteSensorRx = 3; // Uno

const int debugLED = 13; // as ever

const int average = 10;

long voltage, serData, distance = 0; // Distance in In1ches
bool pulse = true;
bool stringComplete = false;

// Mode filtering
int rangeValue[average];

SoftwareSerial remoteSensor(remoteSensorRx, remoteSensorTx, true);

void setup() {
 Serial.begin(9600);
 remoteSensor.begin(9600);
 delay(500);
}

void loop() {
    showRate();    
    voltage = getAveragedAnalog();
    serData = getAveragedRemote();
    sendSample(voltage,'A');
    if(serData>0) sendSample(serData,'B');
}

void showRate() {
    pulse = !pulse;
    if(pulse) digitalWrite(debugLED,HIGH);
    else digitalWrite(debugLED,LOW);
}

void sendSample(int _data, int _id) {
    int data = map(_data,0,512,0,255);
    Serial.write('R');
    Serial.write(_id);
    Serial.write('D');
    Serial.write(data);
    Serial.write('\n');
}

long getAveragedAnalog() {
    // Sensor info!
    // ~9.8mV/in at 5v (Vcc/5   12 per inch)
    // Smooth samples
    int summedVoltage,averagedVoltage = 0;
//    for(int i=0;i<average;i++) {
//        averagedVoltage = analogRead(hubSensor)/2;    // Arduino analog is 0-1024 so divide by 2
//        summedVoltage += averagedVoltage;
//        delay(5);
//    }
    
    for(int i=0;i<average;i++) {
        rangeValue[i] = analogRead(hubSensor)/2;
        delay(5);
    }
    isort(rangeValue,average);
    
    return long( calcMode(rangeValue,average) );
}

long getAveragedRemote() {
    // Via http://forum.arduino.cc/index.php/topic,114808.0.html
    int result = 0;
    char inData[4]; //char array to read data into
    int index = 0;
    
    
    remoteSensor.flush(); // Clear cache ready for next reading
    if (remoteSensor.available() > 3) {
        char rByte = remoteSensor.read(); //read serial input for "R" to mark start of data
        if(rByte == 'R') {
            while (index < 3)  //read next three character for range from sensor
            {
                inData[index] = remoteSensor.read();
                //Serial.println(inData[index]); //Debug line
                
                index++;  // Increment where to write next
                if(index>4) break; // Precaution
            }
            inData[index] = 0x00; //add a padding byte at end for atoi() function
        }
        
        rByte = 0; //reset the rByte ready for next reading
        index = 0; // Reset index ready for next reading
        result = atoi(inData); // Changes string data into an integer for use
    }
    return result;
}

//Sorting function
// sort function (Author: Bill Gentles, Nov. 12, 2010)
void isort(int *a, int n){
    // *a is an array pointer function
    for (int i = 1; i < n; ++i)
    {
        int j = a[i];
        int k;
        for (k = i - 1; (k >= 0) && (j < a[k]); k--)
        {
            a[k + 1] = a[k];
        }
        a[k + 1] = j;
    }
}

int calcMode( int *x, int n){
    // Mode filter from http://playground.arduino.cc/Main/MaxSonar
    int i = 0;
    int count = 0;
    int maxCount = 0;
    int mode = 0;
    int bimodal;
    int prevCount = 0;
    while(i<(n-1)){
        prevCount=count;
        count=0;
        while(x[i]==x[i+1]){
            count++;
            i++;
        }
        if(count>prevCount&count>maxCount){
            mode=x[i];
            maxCount=count;
            bimodal=0;
        }
        if(count==0){
            i++;
        }
        if(count==maxCount){//If the dataset has 2 or more modes.
            bimodal=1;
        }
        if(mode==0||bimodal==1){//Return the median if there is no mode.
            mode=x[(n/2)];
        }
        return mode;
    }
}