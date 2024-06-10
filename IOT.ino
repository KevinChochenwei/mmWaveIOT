#include <Firebase_ESP_Client.h>
#include <ESP8266WiFi.h>
//Provide the token generation process info.
#include "addons/TokenHelper.h"
//Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"



// Wi-Fi credentials
#define WIFI_SSID "jenny"
#define WIFI_PASSWORD "11221122"
#define TRUE 1
#define FALSE 0

// Insert Firebase project API Key
#define API_KEY "AIzaSyBzPG5onK71Oe62LrVEKiut9W_Sd4x04Rg"

// Insert RTDB URLefine the RTDB URL */
#define DATABASE_URL "https://ntut-iottest-default-rtdb.firebaseio.com/" 

//Define Firebase Data object
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;
int count = 0;
bool signupOK = false;
bool initialize = false;

const int radarPin = D1;
const int key_statusPin = D2;
const int ac_controlPin = D3;
const int windows_controlPin = D4;
const int camera_controlPin = D5;

int radarState = 0; // 用于存储读取的引脚状态
int keylessState = 0; // 用于存储读取的引脚状态
int windowsState = 0; // 用于存储读取的引脚状态
int acState = 0; // 用于存储读取的引脚状态
int cameraState = 0; // 用于存储读取的引脚状态

void setup() {

  Serial.begin(115200);
  Serial1.begin(115200);
  pinMode(ac_controlPin, OUTPUT);
  pinMode(windows_controlPin, OUTPUT);
  pinMode(camera_controlPin, OUTPUT);
  digitalWrite(ac_controlPin, 0);
  digitalWrite(windows_controlPin, 0);
  digitalWrite(camera_controlPin, 0);
  // Connect to Wi-Fi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);

    Serial.println("Connecting...");
  }
  Serial.println("Connected!");

  /* Assign the api key (required) */
  config.api_key = API_KEY;

  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;

  /* Sign up */
  if (Firebase.signUp(&config, &auth, "", "")){
    Serial.println("ok");
    signupOK = true;
  }
  else{
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }
  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h
  
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}

void loop() {
  
  //Board status update to database, only run once
  if(!initialize){
    keylessState = TRUE;//digitalRead(key_statusPin); // 读取引脚状态
    if (Firebase.ready() && signupOK){
      if (Firebase.RTDB.setInt(&fbdo, "Test/Key Status", keylessState)){
      // Serial.println("PASSED");
      // Serial.println("PATH: " + fbdo.dataPath());
      // Serial.println("TYPE: " + fbdo.dataType());
      }
      
      if (Firebase.RTDB.setInt(&fbdo, "Test/Radar Enable", FALSE)){
      // Serial.println("PASSED");
      // Serial.println("PATH: " + fbdo.dataPath());
      // Serial.println("TYPE: " + fbdo.dataType());
      }
      
      if (Firebase.RTDB.setInt(&fbdo, "Test/Radar Detect", FALSE)){
      
      }
      
      if (Firebase.RTDB.setString(&fbdo, "Test/Windows Control", "0")){
      
      }
      
      if (Firebase.RTDB.setString(&fbdo, "Test/AirCondition Control", "0")){
      
      }
      
      if (Firebase.RTDB.setString(&fbdo, "Test/Camera Control", "0")){
      
      }
      // else {
      //   Serial.println("FAILED Initialize");
      //   Serial.println("REASON: " + fbdo.errorReason());
      // }


    }
    initialize = TRUE;
  }

  keylessState = digitalRead(key_statusPin); // 读取引脚状态

  if(keylessState){ //鑰匙在車上
    if (Firebase.ready() && signupOK) {
      if (Firebase.RTDB.setInt(&fbdo, "Test/Key Status", keylessState)){
          
      }
      else {
        Serial.println("FAILED Key Status True");
        Serial.println("REASON: " + fbdo.errorReason());
      }

      if (Firebase.RTDB.setInt(&fbdo, "Test/Radar Enable", FALSE)){
          
      }
      else {
        Serial.println("FAILED Radar Enable False");
        Serial.println("REASON: " + fbdo.errorReason());
      }

      if (Firebase.RTDB.setInt(&fbdo, "Test/Radar Detect", FALSE)){
          
      }
      else {
        Serial.println("FAILED Radar Detect False");
        Serial.println("REASON: " + fbdo.errorReason());
      }

    }

  }
  else { //鑰匙不在車上
    if (Firebase.ready() && signupOK) {
      if (Firebase.RTDB.setInt(&fbdo, "Test/Key Status", keylessState)){
          
      }
      else {
        Serial.println("FAILED Key Status False");
        Serial.println("REASON: " + fbdo.errorReason());
      }

      if (Firebase.RTDB.setInt(&fbdo, "Test/Radar Enable", TRUE)){
          
      }
      else {
        Serial.println("FAILED Radar Enable True");
        Serial.println("REASON: " + fbdo.errorReason());
      }
    }
  }


  // Read the command from the database
  if (Firebase.RTDB.getString(&fbdo, "Test/Radar Enable")) {
    if (fbdo.dataType() == "int") {
      bool radar_enble = fbdo.intData() ;
      String command = fbdo.stringData();
      Serial.println("Received command: " + command);

      if(radar_enble){
        radarState = !digitalRead(radarPin);
        Firebase.RTDB.setInt(&fbdo, "Test/Radar Detect", radarState);
        
        Firebase.RTDB.getString(&fbdo, "Test/Windows Control");
        if(fbdo.stringData() == "1")
          digitalWrite(windows_controlPin,1);
        else
          digitalWrite(windows_controlPin,0);

        Firebase.RTDB.getString(&fbdo, "Test/AC Control");
        if(fbdo.stringData() == "1")
          digitalWrite(ac_controlPin,1);
        else
          digitalWrite(ac_controlPin,0);
        Firebase.RTDB.getString(&fbdo, "Test/Camera Control");

        if(fbdo.stringData() == "1")
          digitalWrite(camera_controlPin,1);
        else
          digitalWrite(camera_controlPin,0);

        }
      }
  }
  else {
    Serial.println("Failed to get command: " + fbdo.errorReason());
  }

  delay(1000); // 每秒检查一次

}
