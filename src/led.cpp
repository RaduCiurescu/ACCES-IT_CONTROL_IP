#include <Arduino.h>
#include "DHT.h"
#include "thingProperties.h"
#include <Preferences.h>
#include <time.h>
#include <WiFi.h>
#include <ESP_Mail_Client.h>

// —————— Configurare WiFi ——————
const char* WIFI_SSID     = "bofadn2";
const char* WIFI_PASSWORD = "12345678";

// —————— Senzor DHT22 ——————
#define DHTPIN   26
#define DHTTYPE  DHT22
DHT dht(DHTPIN, DHTTYPE);

// —————— Stocare mesaje și evenimente ——————
Preferences prefs;     // "msg_store"
Preferences humPrefs;  // "hum_store"

// —————— Configurare SMTP ——————
#define SMTP_HOST       "smtp.gmail.com"
#define SMTP_PORT       465
#define AUTHOR_EMAIL    "raduciurescu75@gmail.com"
#define AUTHOR_PASSWORD "fwichghjpegqizph"
#define RECIPIENT_EMAIL "podean.beni@gmail.com"

SMTPSession smtp;
SMTP_Message message;
Session_Config smtpConfig;

void smtpCallback(SMTP_Status status) {
  Serial.println(status.info());
  if (status.success()) {
    Serial.println("Email trimis cu succes.");
    smtp.sendingResult.clear();
  } else {
    Serial.println("Eroare la trimiterea email-ului.");
  }
}

// Flag pentru evitarea trimitere multiple
bool emailSent = false;

void setup() {
  Serial.begin(115200);

  // Conectare WiFi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Conectare la WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(300);
    
  }
  Serial.println(" conectat!");

  // Inițializare DHT22
  dht.begin();

  // Inițializare NVS
  prefs.begin("msg_store", false);
  humPrefs.begin("hum_store", false);

  // Configurare SMTP
  smtp.debug(1);
  smtp.callback(smtpCallback);

  smtpConfig.server.host_name = SMTP_HOST;
  smtpConfig.server.port      = SMTP_PORT;
  smtpConfig.login.email      = AUTHOR_EMAIL;
  smtpConfig.login.password   = AUTHOR_PASSWORD;
  smtpConfig.time.ntp_server       = "pool.ntp.org";
  smtpConfig.time.gmt_offset       = 3;
  smtpConfig.time.day_light_offset = 0;

  // Mesaj static (expeditor, subiect, destinatar)
  message.sender.name  = "ESP32 Alert";
  message.sender.email = AUTHOR_EMAIL;
  message.subject      = "🚨 Umiditate ridicată (>75%)";
  message.addRecipient("Utilizator", RECIPIENT_EMAIL);
  message.text.charSet  = "utf-8";
  message.text.transfer_encoding = Content_Transfer_Encoding::enc_7bit;

  // Inițializare IoT Cloud
  configTime(0, 0, "pool.ntp.org", "time.nist.gov");
  initProperties();
  ArduinoCloud.begin(ArduinoIoTPreferredConnection);
}

void loop() {
  ArduinoCloud.update();

  // Citire umiditate
  float humidity = dht.readHumidity();
  if (isnan(humidity)) {
    Serial.println("Eroare citire DHT22!");
  } else {
    Serial.printf("Umiditate: %.1f %%\n", humidity);

    // Trimitere email dacă umiditate >75% o singură dată
    if (humidity > 75.0 && !emailSent) {
      // Conținut dinamic
      String body = "Atenție! Umiditatea actuală este de " + String(humidity, 1) + "%";
      message.text.content = body.c_str();

      // Conectare și trimitere email
      if (!smtp.connect(&smtpConfig)) {
        Serial.printf("Eroare conexiune SMTP: %d, %s\n", smtp.statusCode(), smtp.errorReason().c_str());
      } else if (!MailClient.sendMail(&smtp, &message)) {
        Serial.printf("Eroare trimitere email: %d, %s\n", smtp.statusCode(), smtp.errorReason().c_str());
      }
      emailSent = true;
    } else if (humidity <= 75.0) {
      emailSent = false;
    }
  }

  delay(2000);
}
