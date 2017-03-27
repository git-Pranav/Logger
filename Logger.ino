#include <SPI.h>
#include <SD.h>
#include <Adafruit_GPS.h>
Adafruit_GPS GPS(&Serial1);

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>

//Adafruit_BNO055 bno = Adafruit_BNO055(55);

#define cardSelect 4

File logfile;
// Set GPSECHO to 'false' to turn off echoing the GPS data to the Serial console
// Set to 'true' if you want to debug and listen to the raw GPS sentences
#define GPSECHO false
// this keeps track of whether we're using the interrupt
// off by default!
boolean usingInterrupt = false;

void setup()
{
  // connect at 115200 so we can read the GPS fast enough and echo without dropping chars
  // also spit it out
  Serial.begin(115200);
  Serial.println("Adafruit GPS library basic test!");

  // 9600 NMEA is the default baud rate for Adafruit MTK GPS's- some use 4800
  GPS.begin(9600);
  // uncomment this line to turn on RMC (recommended minimum) and GGA (fix data) including altitude
  //GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  // uncomment this line to turn on only the "minimum recommended" data
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCONLY);
 
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ); // 1 Hz update rate


  delay(1000);
  // Ask for firmware version
  //Serial1.println(PMTK_Q_RELEASE);
  if (!SD.begin(cardSelect)) {
    Serial.println("Card init. failed!");
    //  error(2);
  }

  
//     if(!bno.begin())
//    {
//      Serial.println("No BNO055 detected");
//    }
//    delay(1000);
//    bno.setExtCrystalUse(true);


  char filename[15];
  strcpy(filename, "RIDE00.TXT");
  for (uint8_t i = 0; i < 100; i++) {
    filename[4] = '0' + i / 10;
    filename[5] = '0' + i % 10;
    // create if does not exist, do not open existing, write, sync after write
    if (! SD.exists(filename)) {
      break;
    }
  }

  logfile = SD.open(filename, FILE_WRITE);
  if ( ! logfile ) {
    Serial.print("Couldnt create ");
    Serial.println(filename);
    // error(3);
  }
  Serial.print("Writing to ");
  Serial.println(filename);

  Serial.println("Ready!");
}



uint32_t timer = millis();
void loop() // run over and over again
{
  
  /* Get a new sensor event */ 
//  sensors_event_t event; 
//  bno.getEvent(&event);
  
  /* Display the floating point data */
//  Serial.print("X: ");
//  Serial.print(event.orientation.x, 4);
//  Serial.print("\tY: ");
//  Serial.print(event.orientation.y, 4);
//  Serial.print("\tZ: ");
//  Serial.print(event.orientation.z, 4);
//  Serial.println("");
  
  //delay(100);
  
  // read data from the GPS in the 'main loop'
  char c = GPS.read();
  // if you want to debug, this is a good time to do it!
  if (GPSECHO)
    if (c) Serial.print(c);
  // if a sentence is received, we can check the checksum, parse it...
  if (GPS.newNMEAreceived()) {
    // a tricky thing here is if we print the NMEA sentence, or data
    // we end up not listening and catching other sentences!
    // so be very wary if using OUTPUT_ALLDATA and trytng to print out data
    Serial.println(GPS.lastNMEA()); // this also sets the newNMEAreceived() flag to false
    if (!GPS.parse(GPS.lastNMEA())) // this also sets the newNMEAreceived() flag to false
      return; // we can fail to parse a sentence in which case we should just wait for another
  }
  // if millis() or timer wraps around, we'll just reset it
  if (timer > millis()) timer = millis();

  if (millis() - timer >2000) //Log values only every 2 seconds.
  {
    timer = millis(); //Reset the timer
//    Serial.print("\nTime: ");
//    Serial.print(GPS.hour, DEC); Serial.print(':');
//    Serial.print(GPS.minute, DEC); Serial.print(':');
//    Serial.print(GPS.seconds, DEC); Serial.print('.');
//    Serial.println(GPS.milliseconds);
//    Serial.print("Date: ");
//    Serial.print(GPS.day, DEC); Serial.print('/');
//    Serial.print(GPS.month, DEC); Serial.print("/20");
//    Serial.println(GPS.year, DEC);
//    Serial.print("Fix: "); Serial.print((int)GPS.fix);
//    Serial.print(" quality: "); Serial.println((int)GPS.fixquality); 
//    
    if (GPS.fix) //Log the values only if GPS has a fix
    {
      Serial.print("Location: ");
      Serial.print(GPS.latitude, 4); Serial.print(GPS.lat);
      Serial.print(", ");
      Serial.print(GPS.longitude, 4); Serial.println(GPS.lon);
      Serial.print("Speed (knots): "); Serial.println(GPS.speed);
      Serial.print("Angle: "); Serial.println(GPS.angle);
      Serial.print("Altitude: "); Serial.println(GPS.altitude);
      Serial.print("Satellites: "); Serial.println((int)GPS.satellites);

      logfile.print(GPS.lastNMEA());
      logfile.flush();
    }

//    else Serial.println("No Fix");
  }
}

