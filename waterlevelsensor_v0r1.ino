/*
This module is for a water level sensor usingf an HC-SR04 ultrasonic distance
measuring device.

Xbee Protocol:
<CHOLAS>SourceMAC,DestMAC,version,MSGType,length,msg</CHOLAS>

SourceMAC: is the MAC of the device sending the message
DestMAC: Is the MAC of the device the message is intended to receive. MAC aadress of 0xffffffff or 0xFFFFFFFF
is a broadcast message for all devices attached.
version: Version of the protocol
MSGType:
0 = ackknowledge
1 = request liquid level (gallons)
2 = set high threshold (the line at which the valve should be shut off), msg = level in gallons
3 = set low threshold( the line at which the valve should be turned on), msg = level in gallons
4 = set warning level threshold (the point at which the device alerts the system that it has passed this position)
5 = set critical level threshold (the point at which an alarm is generated to the system of the liquid level)
6 = request valve status
7 = turn valve on
8 = turn valve off
9 = Get total tank capacity
*/

#define TANK_RADIUS 3 //In feet
#define TANK_HEIGHT 5 //In feet
#define HEADER "<CHOLAS>"
#define TERMINATOR "</CHOLAS>"


const int echoPin = 13;
const int trigPin = 12;
int photoSense = A15;
String xbeeString = ""; // a string to hold incoming data
String xbeePktString = "";
boolean xbeePktComplete = false; // whether the string is complete
int valveControl = A0;


/**************************************
Xbee MAC Address
**************************************/
char m_MACaddress[17];

float tankLevel = 0;
float highThreshold = 2000;
float lowThreshold = 0;
float warningThreshold = 0;
float criticalThreshold = 0;
float tankVolume = 0;
float waterHeight = 0;
float totalTankVolume = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin (9600);
  Serial2.begin(9600);

  Serial.println("Initialize sensor pins.....");
   
   pinMode(valveControl,OUTPUT);
   pinMode(trigPin,OUTPUT);
   pinMode(echoPin,INPUT);
   digitalWrite(trigPin,LOW);
   digitalWrite(valveControl,LOW);
   pinMode(photoSense, INPUT);
   
   GetXbeeSerialNumber( &m_MACaddress[0] );
   Serial.print("Xbee Serial Number: "); Serial.println(m_MACaddress);
   
   //Calulcate the total tank volume
   totalTankVolume = (3.14 * (TANK_RADIUS * TANK_HEIGHT))*7;
   Serial.print("Water tank Maximum Capacity: "); Serial.print(totalTankVolume); Serial.println(" Gallons");
}

void loop() {
  char str[256];
  long distance = 0;
  long duration = 0;

  // put your main code here, to run repeatedly:
  while(1){
      //Is there a packet to process?
      if(xbeePktComplete==true){
          xbeePktComplete = false;
          Serial.println(xbeePktString);
          xbeePktString = "";
          
          String myMac(m_MACaddress);
          
          String temp = "";
int tempPosition = 0;
          int packetLength = 0;
          packetLength = xbeePktString.length();
          temp = xbeePktString;

          //Get the header string from the packet
          int commaPosition = temp.indexOf(",");

          //This shoudl be the sourceMAC
          String sourceMAC = temp.substring(0, commaPosition);

          //Get the destination MAC
          tempPosition = commaPosition + 1;
          temp = temp.substring(tempPosition, packetLength);
          packetLength = temp.length();
          
          //Get destination MAC address
          commaPosition = temp.indexOf(",");
          String destMAC = temp.substring(0, commaPosition );
          tempPosition = commaPosition + 1;
          temp = packet.substring(tempPosition, packetLength);
          packetLength = temp.length();

          int isThisForMe = destMAC.indexOf(myMac);
          int isThisABroadcast = destMAC.indexOf("FFFFFFFF");
          if( isThisForMe != -1 || isThisABroadcast != -1){
            //Get protocol version
            commaPosition = temp.indexOf(",");
            String blah = temp.substring(0, commaPosition );
            int version = blah.toInt();
            tempPosition = commaPosition + 1;
            temp = packet.substring(tempPosition, packetLength);
            packetLength = temp.length();
            
             //Get protocol version
            commaPosition = temp.indexOf(",");
            blah = temp.substring(0, commaPosition );
            int msgtype = blah.toInt();
            tempPosition = commaPosition + 1;
            temp = packet.substring(tempPosition, packetLength);
            packetLength = temp.length();
            
            //Get protocol version
            commaPosition = temp.indexOf(",");
            blah = temp.substring(0, commaPosition );
            int msgLength = blah.toInt();
            tempPosition = commaPosition + 1;
            temp = packet.substring(tempPosition, packetLength);
            packetLength = temp.length();
            
            char reply[256];
            switch(msgtype){
                //request liquid level (gallons)
                case 1:
                  sprintf(reply, "%s,%s,%s,1,1,1,%lf,%s", HEADER, &m_MACaddress[0], sourceMAC.toCharArray(), tankLevel, TERMINATOR );
                  Serial2.println(reply);
                  break;
                  
                //2 = set high threshold (the line at which the valve should be shut off), msg = level in gallons
                case 2:
                  //Get protocol version
                  commaPosition = temp.indexOf(",");
                  blah = temp.substring(0, commaPosition );
                  char threshdoldStr[blah.length()];
                  threshdoldStr = blah.toCharArray();
                  highThreshold = atop(threshdoldStr);
                  sprintf(reply, "%s,%s,%s,1,0,0,%s", HEADER, &m_MACaddress[0], sourceMAC.toCharArray(),TERMINATOR );
                  Serial2.println(reply);
                  break;
                  
                //3 = set low threshold( the line at which the valve should be turned on), msg = level in gallons
                case 3:
                  //Get protocol version
                  commaPosition = temp.indexOf(",");
                  blah = temp.substring(0, commaPosition );
                  char threshdoldLowStr[blah.length()];
                  threshdoldLowStr = blah.toCharArray();
                  lowThreshold = atop(threshdoldLowStr);
                  sprintf(reply, "%s,%s,%s,1,0,0,%s", HEADER, &m_MACaddress[0], sourceMAC.toCharArray(),TERMINATOR );
                  Serial2.println(reply);
                  break;
                  
                //4 = set warning level threshold (the point at which the device alerts the system that it has passed this position)
                case 4:
                  //Get protocol version
                  commaPosition = temp.indexOf(",");
                  blah = temp.substring(0, commaPosition );
                  char threshdoldWarnStr[blah.length()];
                  threshdoldWarnStr = blah.toCharArray();
                  warningThreshold = atop(threshdoldWarnStr);
                  sprintf(reply, "%s,%s,%s,1,0,0,%s", HEADER, &m_MACaddress[0], sourceMAC.toCharArray(),TERMINATOR );
                  Serial2.println(reply);
                  break;
                
                case 5:
                  //Get protocol version
                  commaPosition = temp.indexOf(",");
                  blah = temp.substring(0, commaPosition );
                  char threshdoldCrtiStr[blah.length()];
                  threshdoldCrtiStr = blah.toCharArray();
                  criticalThreshold = atop(threshdoldCrtiStr);
                  sprintf(reply, "%s,%s,%s,1,0,0,%s", HEADER, &m_MACaddress[0], sourceMAC.toCharArray(),TERMINATOR );
                  Serial2.println(reply);
                  break;
              
              case 6:
                  sprintf(reply, "%s,%s,%s,1,1,1,%d,%s", HEADER, &m_MACaddress[0], sourceMAC.toCharArray(), digitalRead(valveControl),TERMINATOR );
                  Serial2.println(reply);
                  break;
                  
              case 7:
                  digitalWrite(valveControl,HIGH)
                  sprintf(reply, "%s,%s,%s,1,0,0,%s", HEADER, &m_MACaddress[0], sourceMAC.toCharArray(),TERMINATOR );
                  Serial2.println(reply);
                  break;
              
              case 8:
                  digitalWrite(valveControl,LOW)
                  sprintf(reply, "%s,%s,%s,1,0,0,%s", HEADER, &m_MACaddress[0], sourceMAC.toCharArray(),TERMINATOR );
                  Serial2.println(reply);
                  break;
                  
             case 9:
                  sprintf(reply, "%s,%s,%s,1,1,1,%lf,%s", HEADER, &m_MACaddress[0], sourceMAC.toCharArray(), totalTankVolume,TERMINATOR );
                  Serial2.println(reply);
                  break;
              
              default:
                Serial.println("Unknown Message!");
                break;
                
              xbeePktString = "";
            }
            
            else{
              //Clear the packet buffer
              xbeePktString = "";
            }
          }
          
          

      }
    
      //Read the distance
      triggerMeasurement();
      duration = pulseIn(echoPin, HIGH);
      distance = (((duration/2) / 29.1) * 0.39);
      sprintf(str, "%ld us, %ld inches", duration, distance);
      Serial.println(str);
      
      //Calculate the total water hieght
      waterHeight = (60 - distance)/12;
      
      //Tank volume in cubic feet (ft^3)
      tankVolume = 3.14 * (TANK_RADIUS * waterHeight);
      
      //Tankl level in gallons
      tankLevel = tankVolume * 7;
      Serial.print("Tank Level: "); Serial.print(tankLevel); Serial.println(" Gallons");
      
      //Turn off the valve when hitting this threshold
      if( tankLevel >= highThreshold){
        Serial.print("Shut off valve!");
        digitalWrite(valveControl,LOW);
      }
      
      //Turn on the valve when hitting this threshold
      if( tankLevel <= lowThreshold){
        Serial.print("Turn on valve!");
        digitalWrite(valveControl,HIGH);
      }
      
      //Warn that we have hit this level
      if( tankLevel <= warningThreshold){
        Serial.print("Tank is at a warning level!");
      }
      
      //Warn that we have hit this level
      if( tankLevel <= criticalThreshold){
        Serial.print("Tank is at a critical level!");
      }

      Serial.print("Light Sensor: "); Serial.print(GetLightSensor()); Serial.println(" V");
      Serial.print("Low water threshold: "); Serial.print(lowThreshold); Serial.println(" Gallons(Valve turns on below this level)");
      Serial.print("High water threshold: "); Serial.print(highThreshold); Serial.println(" Gallons(Valve turns off above this level)");
      Serial.print("Warning threshold: "); Serial.print(lowThreshold); Serial.println(" Gallons(Warning of tank level)");
      Serial.print("Critical threshold: "); Serial.print(criticalThreshold); Serial.println(" Gallons(Critical warning of tank level)");
  }
}

void serialEvent2() {
  while (Serial2.available()) {
    // get the new byte:
    char inChar = (char)Serial2.read();
    // add it to the inputString:
    xbeeString += inChar;
    Serial.println(xbeeString);
    // if the incoming character is a newline, set a flag
    // so the main loop can do something about it:
    int headerlocation = xbeeString.indexOf(HEADER);
    int terminatorlocation = xbeeString.indexOf(TERMINATOR);

    //We found a header and terminator in the buffer
    if( headerlocation>-1 && terminatorlocation>-1){
      xbeePktString = xbeeString.substring(headerlocation+8,terminatorlocation);
      Serial.println(xbeePktString);
      
      xbeeString = xbeeString.substring(terminatorlocation+9);
      Serial.println(xbeeString);
      
      xbeePktComplete = true;
    }
  }
}

void triggerMeasurement(){
  digitalWrite(trigPin, LOW); // Added this line
  delayMicroseconds(2); // Added this line
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10); // Added this line
  digitalWrite(trigPin, LOW);
}

float GetLightSensor(){
  int lightSensorADCValue = 0; // variable to store the value coming from the sensor

  //Read analog input 0 to read the photo sensor voltage
  lightSensorADCValue = analogRead(photoSense);
  
  ////Multiply the reading from the sensor by 3.223 mV (3.3V / 10 bits)
  float lightSensorVoltage = (lightSensorADCValue * .003223);

  return lightSensorVoltage;
}

boolean GetXbeeSerialNumber( char *mac ){
    delay(1);
    
    //Send the open command mode to the Xbee module
    Serial2.write(43);
    Serial2.write(43);
    Serial2.write(43);

    //Wait for the prompt to come back
    long startReadMillis = millis();
    long nowReadMillis = millis();
    long diffMillis = 0;
    int timeout = 5000;

    boolean loop = true;
    boolean byteAvail = false;
    String cLi = "";
    char inputChar = 0x00;

    //Serial.println("Sent +++ Wait for carriage return!");
    while(loop){
        byteAvail = Serial2.available();
        if(byteAvail){
            inputChar = Serial2.read();
            //Serial.print("Rx Char: "); Serial.println(inputChar);
            
            //Did we get a carriage return?
            if(inputChar == 13){
               loop = false;
            }

            else{
                cLi += String(inputChar);
            }
        }
        
        nowReadMillis = millis();
        diffMillis = nowReadMillis-startReadMillis;
        if( diffMillis >= timeout){
            Serial.print("Timeout: "); Serial.println(diffMillis);
            sprintf(mac,"");
            loop = false;
            return false;
        }
    }

    //Serial.print("cLi: "); Serial.println(cLi);

    //Now Read the Serial number
    //ATSH
    Serial2.write(65);
    Serial2.write(84);
    Serial2.write(83);
    Serial2.write(72);
    Serial2.write(13);

    startReadMillis = millis();
    nowReadMillis = 0;
    diffMillis = 0;

    loop = true;
    byteAvail = false;
    String atsh = "";
    inputChar = 0x00;

    //Serial.println("ATSH Wait for carriage return!");

    while(loop){
        byteAvail = Serial2.available();
        if(byteAvail){
            inputChar = Serial2.read();
            //Serial.print("Rx Char: "); Serial.println(inputChar);
    
            
            //Did we get a carriage return?
            if(inputChar == 13){
               loop = false;
            }

            else{
                atsh += String(inputChar);
            }
        }
        
        nowReadMillis = millis();
        diffMillis = nowReadMillis-startReadMillis;
        if( diffMillis >= timeout){
            //Serial.print("Timeout: "); Serial.println(diffMillis);
            sprintf(mac,"");
            loop = false;
            return false;
        }
    }
    //Serial.print("ATSH: "); Serial.println(atsh);

    //ATSL
    Serial2.write(65);
    Serial2.write(84);
    Serial2.write(83);
    Serial2.write(76);
    Serial2.write(13);

    startReadMillis = millis();
    nowReadMillis = 0;
    diffMillis = 0;

    loop = true;
    byteAvail = false;
    String atsl = "";
    inputChar = 0x00;
    //Serial.println("ATSL Wait for carriage return!");

    while(loop){
        byteAvail = Serial2.available();
        if(byteAvail){
            inputChar = Serial2.read();
            
            //Did we get a carriage return?
            if(inputChar == 13){
               loop = false;
            }

            else{
                atsl += String(inputChar);
            }
        }
        
        nowReadMillis = millis();
        diffMillis = nowReadMillis-startReadMillis;
        if( diffMillis >= timeout){
            //Serial.print("Timeout: "); Serial.println(diffMillis);
            //sprintf(mac,"");
            loop = false;
            return false;
        }
    }
    //Serial.print("ATSL: "); Serial.println(atsl);

    //Send ATCN close CLI command
    Serial2.write(65);
    Serial2.write(84);
    Serial2.write(67);
    Serial2.write(78);
    Serial2.write(13);

    char atshStr[9];
    char atslStr[9];
    atsh.toCharArray(atshStr, 9);
    atsl.toCharArray(atslStr, 9);
    sprintf(mac,"%s%s", atshStr,atslStr );
    return true;
}

