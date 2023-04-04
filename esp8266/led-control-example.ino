#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>

const char* ssid = "Wifi-ssid";
const char* password = "wifi-password";

// Telegram bot token
#define BotToken "telegram-bot-token"


// Admins id
String chat_ids[] = {"5187014948", "760028258", "727789704"};
// Admins count
int admins = 3;


X509List cert(TELEGRAM_CERTIFICATE_ROOT);


WiFiClientSecure client;
UniversalTelegramBot bot(BotToken, client);

int botRequestDelay = 100;
unsigned long lastTimeBotRan;

// Pins for led
const int RocketLed = D1;
const int OutLed = D0;

bool RocketState = LOW;
bool OutState = LOW;

void handleNewMessages(int numNewMessages) {
  for (int i=0; i<numNewMessages; i++) {
    String chat_id = String(bot.messages[i].chat_id);
    bool is_admin = false;
    for (int j=0; j<admins; j++) {
      if (chat_id == chat_ids[j]) {
        is_admin = true;
        break;
      }
    }
    if (!is_admin){
      // if you are not admin he will not answer you
      bot.sendMessage(chat_id, "Hey, sən admin deyilsən bu əmrdən istifadə edə biməzsən.", "");
      continue;
    }
    String text = bot.messages[i].text;
    Serial.println(text);

    String from_name = bot.messages[i].from_name;



    // Commands 

    if (text == "/start") {
      String keyboardJson = "[[{ \"text\" : \"Yardım\", \"callback_data\" : \"/help\" }], [{ \"text\" : \"Əmrlər\", \"callback_data\" : \"/commands\" }]]";
        bot.sendMessageWithInlineKeyboard(chat_id, "Salam, " + from_name + ".\nMən Qiyas üçün hazırlanmış bir botam.\nMüəyyən edilmiş əmrlər vasitəsilə bəzi funksiyaları yerinə yetirə bilirəm.\nHələ ki, yetərincə funksiyam olmasa da yaxında birçox funksiyaya sahib olacam.\nAşağıdaki butonlar vasitəsilə əmrlər və mənim haqqımda bilgi əldə edə bilərsiniz.", "", keyboardJson);
    }

    // Rocket led üçün
    if (text == "/rocketled_on") {
      if(digitalRead(RocketLed))  {
        bot.sendMessage(chat_id, "Rocket Led onsuz da aktivdir.", "");
      } else {
        RocketState = HIGH;
        digitalWrite(RocketLed, RocketState);
        
        if(digitalRead(RocketLed)) {
          bot.sendMessage(chat_id, "Rocket Led aktiv edildi.", "");
        } else {
          bot.sendMessage(chat_id, "Problem var.", "");
        }
      }
    }

    if (text == "/rocketled_off") {
      if(!digitalRead(RocketLed)) {
        bot.sendMessage(chat_id, "Rocket Led onsuz da deaktivdir.", "");
      } else {
        RocketState = LOW;
        digitalWrite(RocketLed, RocketState);
        
        if(!digitalRead(RocketLed)) {
          bot.sendMessage(chat_id, "Rocket Led deaktiv edildi.", "");
        } else {
          bot.sendMessage(chat_id, "Problem var.", "");
        }
      }
    }
    
    if (text == "/outled_on") {
      if(digitalRead(OutLed)) {
        bot.sendMessage(chat_id, "Xaricdəki Led onsuz da aktivdir.", "");
      } else {
        OutState = HIGH;
        digitalWrite(OutLed, OutState);
        
        if(digitalRead(OutLed)) {
          bot.sendMessage(chat_id, "Xaricdəki Led aktiv edildi.", "");
        } else {
          bot.sendMessage(chat_id, "Problem var.", "");
        }
      }
    }
    
    if (text == "/outled_off") {
      if(!digitalRead(OutLed)) {
        bot.sendMessage(chat_id, "Xaricdəki Led onsuz da deaktivdir.", "");
      } else {
        OutState = LOW;
        digitalWrite(OutLed, OutState);
        
        if(!digitalRead(OutLed)) {
          bot.sendMessage(chat_id, "Xaricdəki Led deaktiv edildi.", "");
        } else {
            bot.sendMessage(chat_id, "Problem var.", "");
        }
      }
    }

    if (text == "/leds_on") {
      if (digitalRead(RocketLed) && digitalRead(OutLed)){
        bot.sendMessage(chat_id, "Hər iki led onsuz da aktivdir.", "");
      } else if(digitalRead(RocketLed) && !digitalRead(OutLed)) {
        bot.sendMessage(chat_id, "Rocket led aktivdir.\nXarici Led aktiv edilir...", "");
        OutState = HIGH;
        digitalWrite(OutLed, OutState);
        bot.sendMessage(chat_id, "Aktiv edildi.", "");
      } else if(!digitalRead(RocketLed) && digitalRead(OutLed)) {
        bot.sendMessage(chat_id, "Xarici led aktivdir.\nRocket Led aktiv edilir...", "");
        RocketState = HIGH;
        digitalWrite(RocketLed, RocketState);
        bot.sendMessage(chat_id, "Aktiv edildi.", "");
      } else if (digitalRead(RocketLed) && digitalRead(OutLed)) {
        OutState = HIGH;
        RocketState = HIGH;
        digitalWrite(OutLed, OutState);
        digitalWrite(RocketLed, RocketState);
        bot.sendMessage(chat_id, "Aktiv edildi.", "");
      }
      
    }

    if (text == "/leds_off") {
      if (!digitalRead(RocketLed) && !digitalRead(OutLed)){
        bot.sendMessage(chat_id, "Hər iki led onsuz da deaktivdir.", "");
      } else if(!digitalRead(RocketLed) && digitalRead(OutLed)) {
        bot.sendMessage(chat_id, "Rocket led deaktivdir.\nXarici Led deaktiv edilir...", "");
        OutState = LOW;
        digitalWrite(OutLed, OutState);
        bot.sendMessage(chat_id, "Dektiv edildi.", "");
      } else if(digitalRead(RocketLed) && !digitalRead(OutLed)) {
        bot.sendMessage(chat_id, "Xarici led deaktivdir.\nRocket Led deaktiv edilir...", "");
        RocketState = LOW;
        digitalWrite(RocketLed, RocketState);
        bot.sendMessage(chat_id, "Deaktiv edildi.", "");
      } else if (digitalRead(RocketLed) && digitalRead(OutLed)) {
        OutState = LOW;
        RocketState = LOW;
        digitalWrite(OutLed, OutState);
        digitalWrite(RocketLed, RocketState);
        bot.sendMessage(chat_id, "Deaktiv edildi.", "");
      }
    }

    if (text == "/state") {

      if (digitalRead(RocketLed) && digitalRead(OutLed)){
        bot.sendMessage(chat_id, "Hər iki led aktivdir", "");
      } else if(!digitalRead(RocketLed) && !digitalRead(OutLed)) {
        bot.sendMessage(chat_id, "Hər iki led deaktivdir", "");
      } else if(digitalRead(RocketLed) && !digitalRead(OutLed)) {
        bot.sendMessage(chat_id, "Rocket led: aktiv.\nXarici Led: deaktiv.", "");
      } else if(!digitalRead(RocketLed) && digitalRead(OutLed)) {
        bot.sendMessage(chat_id, "Rocket led: deaktiv.\nXarici Led: aktiv.", "");
      }       
    }

    if (text == "/help") {
      String keyboardJson = "[[{ \"text\" : \"AA\", \"url\" : \"https://t.me/qiyascc\" }]]";
      bot.sendMessageWithInlineKeyboard(chat_id, "Bot TexnoPark üçün nəzərdə tutulmuş idarə botudur.\n\nHaqqında:\nQaynaq kodları: https:\nDil: C++\n\nBotda olmasını istədiyiniz bir funksiya varsa butona klikləyərək yaza bilərsiniz.", "", keyboardJson);
    }

    if(text == "/commands") {
      String commands = "Əmrlərin siyahısı: \n";
      commands += "/rocketled_on - Roketin ledini aktiv edər.\n";
      commands += "/rocketled_off - Roketin ledini deaktiv edər.\n";
      commands += "/outled_on - Xaricdəki ledi aktiv edər.\n";
      commands += "/outled_off - Xaricdəki ledi deaktiv edər.\n";
      commands += "/leds_on - Bütün ledləri eyni anda aktivləşdirər.\n";
      commands += "/leds_off - Bütün ledləri eyni anda deaktivləşdirər.\n";
      commands += "/state - Durumu bildirər.\n";      
      bot.sendMessage(chat_id, commands, "");    
    }
    


    // Button list
    if(text == "/functions") {
      String keyboardJson = "[[{ \"text\" : \"OutLed\", \"callback_data\" : \"/outled_off_and_on\" }], [{ \"text\" : \"RocketLed\", \"callback_data\" : \"/rocketled_off_and_on\" }], [{ \"text\" : \"State\", \"callback_data\" : \"/state\" }], [{ \"text\" : \"All\", \"callback_data\" : \"/leds_off_and_on\" }]]";
      bot.sendMessageWithInlineKeyboard(chat_id, "Butonlar vasitəsilə rahat bir şəkildə funksiyaları istifadə edə bilərsiniz.", "", keyboardJson);
    }
    
    
    // Commands for buttons
    // hidden

    if (text == "/outled_off_and_on") {
      if(digitalRead(OutLed)) {
        OutState = LOW;
        digitalWrite(OutLed, OutState);
      } else {
        OutState = HIGH;
        digitalWrite(OutLed, OutState);
      }
    }
    if (text == "/rocketled_off_and_on") {
      if(digitalRead(RocketLed)) {
        RocketState = LOW;
        digitalWrite(RocketLed, RocketState);
      } else {
        RocketState = HIGH;
        digitalWrite(RocketLed, RocketState);
      }
    }
    if(text == "/leds_off_and_on") {
      if (digitalRead(RocketLed) && digitalRead(OutLed)){
        OutState = LOW;
        RocketState = LOW;
        digitalWrite(RocketLed, RocketState);
        digitalWrite(OutLed, OutState);
      } else if(!digitalRead(RocketLed) && !digitalRead(OutLed)) {
        OutState = HIGH;
        RocketState = HIGH;
        digitalWrite(RocketLed, RocketState);
        digitalWrite(OutLed, OutState);       
      } else if(digitalRead(RocketLed) && !digitalRead(OutLed)) {
        OutState = HIGH;
        RocketState = LOW;
        digitalWrite(RocketLed, RocketState);
        digitalWrite(OutLed, OutState);
      } else if(!digitalRead(RocketLed) && digitalRead(OutLed)) {
        OutState = LOW;
        RocketState = HIGH;
        digitalWrite(RocketLed, RocketState);
        digitalWrite(OutLed, OutState);
      }  
      
    }
  }
}

void setup() {
  Serial.begin(115200);
  
  configTime(0, 0, "pool.ntp.org");
  client.setTrustAnchors(&cert);
 
  pinMode(RocketLed, OUTPUT);
  digitalWrite(RocketLed, RocketState);

  pinMode(OutLed, OUTPUT);
  digitalWrite(OutLed, OutState);
 
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Wifi'a qoşulur...");
  }
  Serial.println(WiFi.localIP());
}

void loop() {  
  if (millis() > lastTimeBotRan + botRequestDelay)  {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    while(numNewMessages) {
      Serial.println("M-m-mesaj.");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    lastTimeBotRan = millis();
  }
}
