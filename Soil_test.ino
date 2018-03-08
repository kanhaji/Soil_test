
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <EEPROM.h>
#include <SoftwareSerial.h>
#include <TinyGPS.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#define BME_SCK 13
#define BME_MISO 12
#define BME_MOSI 11
#define BME_CS 10

#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_BME280 bme; // I2C
//Adafruit_BME280 bme(BME_CS); // hardware SPI
//Adafruit_BME280 bme(BME_CS, BME_MOSI, BME_MISO, BME_SCK); // software SPI

File dataFile;    
TinyGPS gps;
SoftwareSerial serial(9, 10);
bool sdStat=false;
void setup() {
    Serial.begin(9600);
    serial.begin(9600);
    sdStat=SD.begin(8);
    Serial.println(F("BME280 test"));

    //EEPROM.update(0,1);

    pinMode(7,INPUT);
    pinMode(6,OUTPUT);

    bool status;
    Serial.println("aaaa");
    status = bme.begin();
    Serial.println("bbb");  
    if (!status) {
        Serial.println("Could not find a valid BME280 sensor, check wiring!");
        while (1);
    }
    

}

struct data
    {
      int temp,pressure,humidity,soil_hum;
      float lon,lat,alt;
    }dataVal;


void loop() { 
    int count=0;
    while(digitalRead(7))
    {
      delay(10);
      count++;
      if(count>50)
      {
      
      digitalWrite(6,HIGH);
      saveVal();
      count=0;
      break;
      }
    }
    //Serial.println("called");
   // saveVal();
    
}

void saveVal()
{
int temperature,pressure,humidity,soil_hum;
    float lon,lat,alt;

    bmeValues(&temperature,&pressure,&humidity);

    readGPS(&lat,&lon,&alt);
    Serial.print(lon);Serial.print(lat);Serial.println(alt);
    dataVal.temp=temperature;
    dataVal.pressure=pressure;
    dataVal.humidity=humidity;
    dataVal.soil_hum=soil_hum;
    dataVal.lon=lon;
    dataVal.lat=lat;
    dataVal.alt=alt;
    
    writeSD();
    Serial.println("Data Written");
    digitalWrite(6,LOW);
  
}


void bmeValues(int* temp,int* pressure,int* humidity) 
{
    *temp=int(bme.readTemperature());
    *pressure=int(bme.readPressure()/100.0F);
    *humidity=int(bme.readHumidity()); 

     
    /*Serial.print("Temperature = ");
    Serial.print(*temp);
    Serial.println(" *C");

    Serial.print("Pressure = ");

    Serial.print(*pressure);
    Serial.println(" hPa");

    Serial.print("Approx. Altitude = ");
    Serial.print(bme.readAltitude(SEALEVELPRESSURE_HPA));
    Serial.println(" m");

    Serial.print("Humidity = ");
    Serial.print(*humidity);
    Serial.println(" %");

    Serial.println();*/
}

void writeSD()
{
if (!sdStat)
   Serial.println("SD card initialization failed...");
   return;
   dataFile=SD.open("data.txt",FILE_WRITE);

   int temp=dataVal.temp;
   int pressure=dataVal.pressure;
   int humidity=dataVal.humidity;
   int soil_hum=dataVal.soil_hum;
   int longitude=dataVal.lon;
   int lattitude=dataVal.lat;
   int alt=dataVal.alt;
   int sno=EEPROM.read(0);

   String data=(String(sno)+"     "+String(temp)+"     "+String(pressure)+"     "+String(humidity)+"     "+String(soil_hum)+"     "+String(longitude)+"     "+String(lattitude)+"     "+String(alt));
   if(dataFile)
   {
    dataFile.println(data);
    EEPROM.update(0,sno+1);
    dataFile.close();
   }
   
 }

 void readSD()
 {
  dataFile=SD.open("data.txt");
  if(dataFile)
  {
    
  }
 }



 void readGPS(float* lattitude,float* longitude,float* alt)
 {
  bool isData=false;
  while(!isData)
  {
    while(serial.available())
    {
      char c=serial.read();
      Serial.write(c);
      if(gps.encode(c))
       isData=true;
    }

    if(isData)
    {
      unsigned long age;
      gps.f_get_position(lattitude,longitude,&age);
      *alt=gps.f_altitude();  
    }
  }
  
 }

