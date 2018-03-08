#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <ArduinoJson.h>

#include <WiFi.h>
#include <HTTPClient.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);  // set the LCD address to 0x27 for a 16 chars and 2 line display
int show;
char copy[800];
const char* ssid = "xxx";
const char* password =  "xxx";

 int last_items0_statistics_viewCount = 0;
 int last_items0_statistics_subscriberCount = 0;

 int actual_items0_statistics_viewCount = 0;
 int actual_items0_statistics_subscriberCount = 0;

 const int ledPinmore = 16;
 const int ledPinequal = 17;

const char* kind ; // "youtube#channelListResponse"
const char* etag ; // "\"_gJQceDMxJ8gP-8T2HLXUoURK8c/m2tMRDbthsKril5Zsnhte9GLBJo\""

int pageInfo_totalResults ; // 1
int pageInfo_resultsPerPage ; // 1

const size_t bufferSize = JSON_ARRAY_SIZE(1) + JSON_OBJECT_SIZE(2) + 2*JSON_OBJECT_SIZE(4) + JSON_OBJECT_SIZE(5) + 400;
DynamicJsonBuffer jsonBuffer(bufferSize);


const char* items0_kind ; // "youtube#channel"
const char* items0_etag ; // "\"_gJQceDMxJ8gP-8T2HLXUoURK8c/j8MfOsgJKZaXS0EFCqbmvjIfFuQ\""
const char* items0_id ; // "UC7LN8YkAc85slbjy800eM4A"

const char* items0_statistics_viewCount; // "14412"
const char* items0_statistics_commentCount; // "0"
const char* items0_statistics_subscriberCount ; // "168"
bool items0_statistics_hiddenSubscriberCount ; // false
const char* items0_statistics_videoCount; // "14"

int res = 20;
 
void setup() {

  int error;
  Serial.begin(115200);
  while (! Serial);

  Serial.println("Dose: check for LCD");

  // See http://playground.arduino.cc/Main/I2cScanner
  Wire.begin();
  Wire.beginTransmission(0x27);
  error = Wire.endTransmission();
  Serial.print("Error: ");
  Serial.print(error);

  if (error == 0) {
    Serial.println(": LCD found.");
  } else {
    Serial.println(": LCD not found.");
  } // if

  lcd.begin(); // initialize the lcd
  show = 0;
  lcd.backlight();
 
  
  delay(10000);
  WiFi.begin(ssid, password);

  pinMode (ledPinmore, OUTPUT);
  pinMode (ledPinequal, OUTPUT);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Connecting WiFi..");
    Serial.println("Connecting WiFi");
  }

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Conected");
  Serial.println("Connected");
 
}
 
void loop() {

  if (res == 0){
    ESP.restart();
  }
  res = res - 1;
  
  if ((WiFi.status() == WL_CONNECTED)) { //Check the current connection status
 
    HTTPClient http;
 
    http.begin("https://www.googleapis.com/youtube/v3/channels?part=statistics&id=UC7LN8YkAc85slbjy800eM4A&key=AIzaSyD6jKvdy-cZ7sAfah5tnFQqYXFKd0pc8D4"); //Specify the URL
    int httpCode = http.GET();                                        //Make the request
 
    if (httpCode > 0) { //Check for the returning code
     Serial.println(httpCode);
     if (httpCode == 200){
        String payload = http.getString();

        
        payload.toCharArray(copy, 800);
        
        Serial.println(payload);
        
        JsonObject& root = jsonBuffer.parseObject(payload);
        if (!root.success()) {

          Serial.println("parseObject() failed");
          Serial.println(root.success());

          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Parse error");
          Serial.println("Parse error");
          
          delay(10000);

          //reset 
          ESP.restart();
          return;

        }

          kind = root["kind"]; // "youtube#channelListResponse"
          etag = root["etag"]; // "\"_gJQceDMxJ8gP-8T2HLXUoURK8c/m2tMRDbthsKril5Zsnhte9GLBJo\""
          
          pageInfo_totalResults = root["pageInfo"]["totalResults"]; // 1
          pageInfo_resultsPerPage = root["pageInfo"]["resultsPerPage"]; // 1
          
          JsonObject& items0 = root["items"][0];
          items0_kind = items0["kind"]; // "youtube#channel"
          items0_etag = items0["etag"]; // "\"_gJQceDMxJ8gP-8T2HLXUoURK8c/j8MfOsgJKZaXS0EFCqbmvjIfFuQ\""
          items0_id = items0["id"]; // "UC7LN8YkAc85slbjy800eM4A"
          
          JsonObject& items0_statistics = items0["statistics"];
          items0_statistics_viewCount = items0_statistics["viewCount"]; // "14412"
          items0_statistics_commentCount = items0_statistics["commentCount"]; // "0"
          items0_statistics_subscriberCount = items0_statistics["subscriberCount"]; // "168"
          items0_statistics_hiddenSubscriberCount = items0_statistics["hiddenSubscriberCount"]; // false
          items0_statistics_videoCount = items0_statistics["videoCount"]; // "14"
         //Serial.println(payload);


        //comprobar las last values y actualizarlas y hacer algo con un servo mismo

        actual_items0_statistics_viewCount = atoi( items0_statistics_viewCount );
        actual_items0_statistics_subscriberCount = atoi( items0_statistics_subscriberCount );

        if ( actual_items0_statistics_viewCount > last_items0_statistics_viewCount) {
          Serial.println("hay más visitas!");
          Serial.println(items0_statistics_viewCount);
          
        }

        if ( actual_items0_statistics_subscriberCount > last_items0_statistics_subscriberCount) {
          Serial.println("hay más subscriptores!");
          Serial.println(items0_statistics_subscriberCount);
          digitalWrite (ledPinmore , HIGH);  // turn on the LED
          digitalWrite (ledPinequal, LOW);  // turn off the LED
        } else if ( actual_items0_statistics_subscriberCount == last_items0_statistics_subscriberCount) {
          Serial.println("se mantiene");
          Serial.println(items0_statistics_subscriberCount);
          digitalWrite (ledPinmore , LOW);  // turn on the LED
          digitalWrite (ledPinequal, LOW);  // turn off the LED  
        } else {
          Serial.println("bajaron los subscriptores");
          Serial.println(items0_statistics_subscriberCount);
          digitalWrite (ledPinmore , LOW);  // turn on the LED
          digitalWrite (ledPinequal, HIGH);  // turn off the LED
        }

        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("ARitmoDeCarnaval");
        Serial.println("ARitmoDeCarnaval");
        lcd.setCursor(0, 1);
        lcd.print("Subs:" + String(actual_items0_statistics_subscriberCount) + "   V:" + String(actual_items0_statistics_viewCount/1000) + "k");
        Serial.println("Subs:"+ String(actual_items0_statistics_subscriberCount) +"   V:" + String(actual_items0_statistics_viewCount/1000) + "k");

        //actualizamos ultimo contador
        last_items0_statistics_viewCount = atoi( items0_statistics_viewCount );
        last_items0_statistics_subscriberCount = atoi( items0_statistics_subscriberCount );
     
    
  

     }
 
        
   }
 
    else {
      Serial.println("Error on HTTP request");
    }
 
    http.end(); //Free the resources
  }
 
  delay(30000);

}
