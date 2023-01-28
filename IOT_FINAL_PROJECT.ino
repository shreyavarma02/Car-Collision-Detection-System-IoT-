#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>
#include <SoftwareSerial.h> 
#include <TinyGPS.h> 

// Set these to run example.
#define FIREBASE_HOST "iot-t2-default-rtdb.asia-southeast1.firebasedatabase.app/"      
#define FIREBASE_AUTH "V9Eyuw4MFOK3vo2esRCUkL6l3H2XVLOlFyvbsUu9"
#define WIFI_SSID "Hallothon"
#define WIFI_PASSWORD "12345678"

const int trigPin = 12;
const int echoPin = 14;
const int switchpin=13;
const int xpin = A0; 

int collide;
float t1;
float Xread;
float Xrest;
float Gx;
float lat = 12.9353,lon = 77.5336; 

TinyGPS gps;// create variable for latitude and longitude object  
SoftwareSerial gpsSerial(4,5);

//define sound velocity in cm/uS
#define SOUND_VELOCITY 0.034

long duration;
float distance;

void setup() 
{
  Serial.begin(115200); // Starts the serial communication
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input
  pinMode(switchpin,INPUT);

  // connect to wifi.
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("connected: ");
  Serial.println(WiFi.localIP());

  Serial.println();
  Serial.print("Connected");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());                               //prints local IP address
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);                 // connect to the firebase
}

int calculate_distance()
{

   digitalWrite(trigPin, LOW);        // Make sure that the TRIG is deactivated
   delayMicroseconds(2);              
   digitalWrite(trigPin, HIGH);       // Activate the output pulse 
   delayMicroseconds(10);             // Wait for 10µs, the pulse remains active during this time
   digitalWrite(trigPin, LOW);        //Stop the pulse and wait for ECHO 

  
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  
  // Calculate the distance
  distance = duration * SOUND_VELOCITY/2;  //Divide by 2 because we want to have only the “go” time, not the “go and back” timeound speed (1/SpeedSound) at cm/us

  return distance;
  
}

int GPS()
{
  if(gps.encode(gpsSerial.read()))// encode gps data 
  {  
    gps.f_get_position(&lat,&lon); // get latitude and longitude  
  } 
 
  String latitude = String(lat,6); 
  String longitude = String(lon,6); 
  Serial.println(latitude+";"+longitude); 

  Firebase.pushFloat("/collision/Location/Latitude",lat);
  Firebase.pushFloat("/collison/Location/Longitude",lon);

  if (Firebase.failed()) 
  {
      Serial.print("setting /number failed:");
      Serial.println(Firebase.error());  
      return 0;
  }
  else
  {
    return 1;
  }   
}

void collision_detect()
{
   if (digitalRead(switchpin) == 0)
    { 
      collide=digitalRead(switchpin);
      Firebase.pushInt("collision/isCollided",collide);
      GPS();
      Serial.println("Collision has occured, emergency services will be alerted");//print collision
      //delay(500);
    }
   else
    {
      Serial.println("No Collision");//print collision
      //delay(500);
    }
}

int acceleration()
{
  Serial.print("Time ");
  t1=millis();
  Serial.println(t1*0.001);
 
  Xread = analogRead(xpin)-Xrest;

 
  Gx=Xread/69.584;

  Serial.print("Acceleration X :");
  Serial.print(Gx);
  Serial.print("\n");
  
  Firebase.pushFloat("/Acceleration",Gx);
    // handle error
  if (Firebase.failed()) 
  {
      Serial.print("setting /number failed:");
      Serial.println(Firebase.error());  
      return 0;
  }
  else
  {
      return 1;
  }   
}


void loop() 
{

  if(acceleration()==1)
  {
    if ( calculate_distance() < 10)
    {
     
      collision_detect();
      //delay(1000);
    }
    Serial.println("Data Sent Successfully!!");
  }
  else
  {
    Serial.println("Failed To send Data");
  }
  delay(10000);
}