#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <SPI.h>
#include <DHT.h>

char auth[] = " "; //Your Authentication token

char ssid[] = " ";  // Your WiFi credentials.
char pass[] = " ";  // Set password to "" for open networks.

BlynkTimer timer;

WidgetLED Vled1(V1);
WidgetLED Vled2(V2);

WidgetLCD lcd(V3);


#define wifiLed 16 //D0

#define RelayPin1 5  //D1
#define RelayPin2 4  //D2
#define RelayPin3 14 //D5
#define RelayPin4 12 //D6

#define dhtData 0 //D3
#define DHTTYPE DHT11 //Type of the DHT sensor
DHT dht(dhtData , DHTTYPE);

int AsoilData=A0;

#define VPIN_BUTTON_1    V6 //Relay1
#define VPIN_BUTTON_2    V7 //Relay2
#define VPIN_BUTTON_3    V8 //Relay3
#define VPIN_BUTTON_4    V9 //Relay4

#define VPIN_BUTTON_5    V5 // Auto or Manual modes


int toggleState_1=1;
int toggleState_2=1;
int toggleState_3=1;
int toggleState_4=1;
int modeState;

int analog;
int analogPercent;
int waterLimit=35;

int wifiFlag=0;

BLYNK_WRITE(VPIN_BUTTON_1) {
  toggleState_1 = param.asInt();
  digitalWrite(RelayPin1, toggleState_1);
}
    
BLYNK_WRITE(VPIN_BUTTON_2) {
  toggleState_2 = param.asInt();
  digitalWrite(RelayPin2, toggleState_2);
}
    
BLYNK_WRITE(VPIN_BUTTON_3) {
  toggleState_3 = param.asInt();
  digitalWrite(RelayPin3, toggleState_3);
}
    
BLYNK_WRITE(VPIN_BUTTON_4) {
  toggleState_4 = param.asInt();
  digitalWrite(RelayPin4, toggleState_4);
}

BLYNK_WRITE(VPIN_BUTTON_5) {
  modeState = param.asInt();
}

BLYNK_CONNECTED() {
  // Request the latest state from the server
  Blynk.syncVirtual(VPIN_BUTTON_1);
  Blynk.syncVirtual(VPIN_BUTTON_2);
  Blynk.syncVirtual(VPIN_BUTTON_3);
  Blynk.syncVirtual(VPIN_BUTTON_4);
  Blynk.syncVirtual(VPIN_BUTTON_5);
}

void checkBlynkStatus() { // called every 3 seconds by SimpleTimer

  bool isconnected = Blynk.connected();
  if (isconnected == false) {
    wifiFlag = 1;
    digitalWrite(wifiLed, HIGH);//Turn off WiFi LED
  }
  if (isconnected == true) {
    wifiFlag = 0;
    digitalWrite(wifiLed, LOW); //Turn on WiFi LED
  }
}

void dhtSensor()
{
  float h = dht.readHumidity();
  float t = dht.readTemperature(); // or dht.readTemperature(true) for Fahrenheit

  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  Serial.println(t);
  Serial.println(h);
  
  String text1= "Temp: " + String(t);
  String text2= "Humidity: " +String(h);
    
  lcd.clear();
  lcd.print(0, 0, text1);
  lcd.print(0, 1, text2);  
}

void soilSensor()
{
  analog = analogRead(AsoilData);
  analogPercent = map(analog,0,1024,100,0);
  //analogPercent = map(analog,0,1024,1024,0);
  Blynk.virtualWrite(V4, analogPercent);
  if(modeState==1 && analogPercent<waterLimit)
  {
    int toggle_1=0;
    int toggle_2=0;
    int toggle_3=0;
    int toggle_4=0;
    Blynk.virtualWrite(VPIN_BUTTON_1, toggle_1);
    digitalWrite(RelayPin1, toggle_1);
    Blynk.virtualWrite(VPIN_BUTTON_2, toggle_2);
    digitalWrite(RelayPin2, toggle_2);
    Blynk.virtualWrite(VPIN_BUTTON_3, toggle_3);
    digitalWrite(RelayPin3, toggle_3);
    Blynk.virtualWrite(VPIN_BUTTON_4, toggle_4);
    digitalWrite(RelayPin4, toggle_4);
  }
  else if(modeState==1 && analogPercent>waterLimit)
  {
    int toggle_1=1;
    int toggle_2=1;
    int toggle_3=1;
    int toggle_4=1;
    Blynk.virtualWrite(VPIN_BUTTON_1, toggle_1);
    digitalWrite(RelayPin1, toggle_1);
    Blynk.virtualWrite(VPIN_BUTTON_2, toggle_2);
    digitalWrite(RelayPin2, toggle_2);
    Blynk.virtualWrite(VPIN_BUTTON_3, toggle_3);
    digitalWrite(RelayPin3, toggle_3);
    Blynk.virtualWrite(VPIN_BUTTON_4, toggle_4);
    digitalWrite(RelayPin4, toggle_4);
  }
  else
  {
    Blynk.virtualWrite(VPIN_BUTTON_1, toggleState_1);
    digitalWrite(RelayPin1, toggleState_1);
    Blynk.virtualWrite(VPIN_BUTTON_2, toggleState_2);
    digitalWrite(RelayPin2, toggleState_2);
    Blynk.virtualWrite(VPIN_BUTTON_3, toggleState_3);
    digitalWrite(RelayPin3, toggleState_3);
    Blynk.virtualWrite(VPIN_BUTTON_4, toggleState_4);
    digitalWrite(RelayPin4, toggleState_4);
  }
}


void welcomeMSG()
{
  lcd.clear();
  lcd.print(4,0 ,"Wellcome");
  delay(3000);
  lcd.clear();
  lcd.print(7,0, "TO");
  delay(3000);
  lcd.print(3,1, "Smart Agri");
  delay(4000);
  lcd.clear();
}

void setup()
{
  Serial.begin(9600);
  dht.begin();
  Blynk.begin(auth, ssid, pass);
  pinMode(RelayPin1, OUTPUT);
  pinMode(RelayPin2, OUTPUT);
  pinMode(RelayPin3, OUTPUT);
  pinMode(RelayPin4, OUTPUT);

  pinMode(wifiLed, OUTPUT);
  
  //During Starting all Relays should TURN OFF
  digitalWrite(RelayPin1, toggleState_1);
  digitalWrite(RelayPin2, toggleState_2);
  digitalWrite(RelayPin3, toggleState_3);
  digitalWrite(RelayPin4, toggleState_4);
  
  Vled1.off();
  Vled2.off();
  
  lcd.clear();
  welcomeMSG();
  
  timer.setInterval(3000L, checkBlynkStatus); // check if Blynk server is connected every 3 seconds
  timer.setInterval(100L, dhtSensor);
  timer.setInterval(100L, soilSensor);
}

void loop()
{ 
  if(modeState==1)//Auto or manual control
  {
    Vled2.on();
  }
  else
  {
    Vled2.off();
  }
  
  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.println("WiFi Not Connected");
    Vled1.off();
  }
  else
  {
    Serial.println("WiFi Connected");
    Blynk.run();
    Vled1.on();
  }
   
  timer.run();
}
