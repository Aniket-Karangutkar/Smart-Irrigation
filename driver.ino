#include <DHT.h>
//#include <DHT_U.h>

#include <DHT.h>
//#include <DHT_U.h>

 #include<LiquidCrystal.h>
LiquidCrystal lcd(12,11,5,4,3,2);
#define relay 7
#include <SoftwareSerial.h>
SoftwareSerial espSerial =  SoftwareSerial(8,9);  
#include <DHT.h>        

int sensor_pin = A1;
int output_value;
float t;
float h;
float s;

#define DHTPIN A0         
#define DHTTYPE DHT11 
DHT dht(DHTPIN, DHTTYPE);


String apiKey = " ";  //api Key      
String ssid=" "; //ssid   
String password =""; //password  

boolean DEBUG=true;


void showResponse(int waitTime){
    long t=millis();
    char c;
    while (t+waitTime>millis()){
      if (espSerial.available()){
        c=espSerial.read();
        if (DEBUG) Serial.print(c);
      }
    }
                   
}

boolean thingSpeakWrite(float value1, float value2, float value3){
  String cmd = "AT+CIPSTART=\"TCP\",\"";                  
  cmd += "184.106.153.149";                              
  cmd += "\",80";
  espSerial.println(cmd);
  if (DEBUG) Serial.println(cmd);
  if(espSerial.find("Error")){
    if (DEBUG) Serial.println("AT+CIPSTART error");
    return false;
  }
  String getStr = "GET /update?api_key="; 
  getStr += apiKey;
  getStr +="&field1=";
  getStr += String(value1);
  getStr +="&field2=";
  getStr += String(value2);
  getStr +="&field3=";
  getStr += String(value3);
  getStr += "\r\n\r\n";
  cmd = "AT+CIPSEND=";
  cmd += String(getStr.length());
  espSerial.println(cmd);
  if (DEBUG)  Serial.println(cmd);
  delay(100);
  if(espSerial.find(">")){
    espSerial.print(getStr);
    if (DEBUG)  Serial.print(getStr);
  }
  else{
    espSerial.println("AT+CIPCLOSE");
    if (DEBUG)   Serial.println("AT+CIPCLOSE");
    return false;
  }
  return true;
}

void setup() {                
  DEBUG=true;          
  Serial.begin(115200); 
  pinMode(relay, OUTPUT);
  analogWrite(6,50);
  lcd.begin(16,2);
  dht.begin();       
  espSerial.begin(115200); 
  espSerial.println("AT+CWMODE=1");   
  showResponse(1000);
  espSerial.println("AT+CWJAP=\""+ssid+"\",\""+password+"\"");  
  showResponse(5000);
  if (DEBUG)  Serial.println("Setup completed");
  lcd.setCursor(0,0);
  lcd.print("Smart Irrigation");
  delay(5000);
  lcd.clear();
}


//====================================================================================
void loop() {
   t = dht.readTemperature();
   h = dht.readHumidity();
   sm();
   s = output_value;
        if (isnan(t) || isnan(h) || isnan(s)) {
        if (DEBUG) Serial.println("Failed to read from Sensors");
      }
      else {
          if (DEBUG)  Serial.println("Temp="+String(t)+" *C");
          if (DEBUG) Serial.println("Humidity="+String(h)+" %");
          if (DEBUG) Serial.println("Soil Moisture="+String(s)+" %");
          thingSpeakWrite(t,h,s);                                      
      }    
      relay1();
      delay(1000);
      l();
      delay(16000);  
}




//====================================================================================
void sm(){
   output_value=analogRead(sensor_pin);
   output_value=map(output_value,1024,10,0,100);
}
//====================================================================================
void l(){

  lcd.setCursor(0,0);  
  lcd.print("Temp=");
  lcd.print(String(t)+" *C");
  delay(3000);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Humidity=");
  lcd.setCursor(0,1);
  lcd.print(String(h)+" %");
  delay(3000);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Soil Moisture");
  lcd.setCursor(0,1);
  lcd.print(String(s)+" %");
  delay(3000);
  lcd.clear();
}
//====================================================================================
void relay1(){
  if(output_value<50)
 {
  digitalWrite(relay, LOW); 
  Serial.print("Motor started");
 }
 else
 {
  
  digitalWrite(relay, HIGH);
  Serial.println("NO need to start motor");
 }
}
//====================================================================================
