#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>

//#define __DEBUG__
// Initialize Telegram BOT
#define BotToken "360711541:AAGyd9LzIPHnege0CpkmIZ15vfpUIRmpCCU"

// userId
const char* antonio = "34919692";
const char* jolien = "48126411";

// Initialize Wifi connection to the router
const int maxIndex = 2;
const char* ssid[maxIndex]     = {"Julien", "Yolin"};
const char* password[maxIndex] = {"jolientje", "jolientje"};

// variables
int alarm = 0;
int updateInterval = 1000; //mean time between scan messages
long lastUpdate;

WiFiClientSecure sslClient;
UniversalTelegramBot bot(BotToken, sslClient);

void setup() {
  // IO pins
  pinMode(2, INPUT_PULLUP);
  pinMode(LED_BUILTIN, OUTPUT); //status led

  #ifdef __DEBUG__
  // Serial
  Serial.begin(115200);
  while (!Serial);
  #endif

  //connect to wifi
  connectWifi();
  
  // send one time msg to owner
  bot.sendMessage(antonio, "I am online");
}

void loop() {
  //checken of wifi nogf verbonden is
  if (WiFi.status() != WL_CONNECTED) {
    connectWifi();
  }

  // Klep open
  if(digitalRead(2) == LOW) {
    if(alarm == 0) {
      alarm = 1;    
      bot.sendMessage(antonio, "You've got mail");
      bot.sendMessage(jolien, "You've got mail");
    } 
  }

  // Deur open
  if(digitalRead(2) == HIGH) {
    if(alarm == 1) {  
      alarm = 0;    
      bot.sendMessage(antonio, "Mailbox empty");
      bot.sendMessage(jolien, "Mailbox empty");
    }
  }

  if (millis() > lastUpdate + updateInterval)  {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    while(numNewMessages) {
      #ifdef __DEBUG__
      Serial.println("New message(s)");
      #endif
      for (int i=0; i<numNewMessages; i++) {
        telegramMessage m = bot.messages[i];
        if ( m.chat_id != 0 ){ // Checks if there are some updates
          if (m.chat_id != antonio) {
            bot.sendMessage(antonio, "Message from: " + m.chat_id + " : " + m.text);
          }
          else {
            bot.sendMessage(m.chat_id, m.text);  // Reply to the same chat with the same text
          }
        }
      }
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    lastUpdate = millis();
    #ifdef __DEBUG__
    Serial.println("Checking...");
    #endif
    toggleLED(LED_BUILTIN);
  }
}

void toggleLED(int led) {
  if(digitalRead(led) == LOW)
    digitalWrite(led, HIGH);
  else
    digitalWrite(led, LOW);
}

void connectWifi(){
  int wifiIndex = 0;
  while (WiFi.status() != WL_CONNECTED) {
    // Set WiFi to station mode and disconnect from AP if previously connected
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(100);
  
    // attempt to connect to Wifi network:
    #ifdef __DEBUG__
    Serial.print("Connecting Wifi: ");
    #endif

    WiFi.begin(ssid[wifiIndex], password[wifiIndex]);
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 20) {
      delay(500);
      attempts++;
          
      #ifdef __DEBUG__
      Serial.print(".");
      #endif
      
      toggleLED(LED_BUILTIN);
    }
    if(++wifiIndex == maxIndex) {
      wifiIndex = 0;
    }
  }
  digitalWrite(LED_BUILTIN, HIGH);
  
  #ifdef __DEBUG__
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  #endif
}

