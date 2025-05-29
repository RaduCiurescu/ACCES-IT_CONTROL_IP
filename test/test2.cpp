#include <BluetoothSerial.h>
#include <WiFi.h>

// Inițializare Bluetooth Serial
BluetoothSerial SerialBT;

// Configurare date Wi-Fi
const char* ssid = "bofadn2";          // înlocuiește cu numele rețelei Wi-Fi
const char* password = "12345678";       // înlocuiește cu parola rețelei

// Configurare server (laptop) – se presupune că laptopul rulează un server care primește date
const char* server_ip = "192.168.1.100";         // adresa IP a laptopului
const uint16_t server_port = 80;                 // portul serverului (ex: 80 pentru HTTP)

// Definirea pinilor
#define BARRIER_PIN 13      // pinul de control pentru barieră (de exemplu, un releu)
#define SENSOR_PIN 12       // pinul pentru senzorul de limitator (ex: comutator mecanic)

// Codul de acces valid (poate fi extins sau verificat dinamic)
String validAccessCode = "1234";

void setup() {
  Serial.begin(115200);
  
  // Inițializare Bluetooth cu numele "ESP32_Barrier"
  SerialBT.begin("ESP32_Barrier");
  Serial.println("Bluetooth a pornit.");

  // Inițializare Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Conectare la Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWi-Fi conectat!");
  Serial.print("Adresa IP: ");
  Serial.println(WiFi.localIP());

  // Configurare pini
  pinMode(BARRIER_PIN, OUTPUT);
  digitalWrite(BARRIER_PIN, LOW); // Bariera inițial este închisă
  pinMode(SENSOR_PIN, INPUT_PULLUP); // Se presupune că senzorul este activ LOW când este declanșat
}

void loop() {
  // Verifică dacă a sosit un mesaj prin Bluetooth
  if (SerialBT.available()) {
    // Se citește mesajul până la caracterul de sfârșit de linie
    String received = SerialBT.readStringUntil('\n');
    received.trim(); // Elimină spațiile în plus
    Serial.print("Cod primit: ");
    Serial.println(received);
    
    // Verificarea codului de acces
    if (received == validAccessCode) {
      Serial.println("Acces valid. Se deschide bariera...");
      
      // Deschide bariera (activează releul sau motorul)
      digitalWrite(BARRIER_PIN, HIGH);
      
      // Așteaptă până când senzorul de limitator este declanșat (presupus activ LOW)
      while (digitalRead(SENSOR_PIN) == HIGH) {
        delay(100);
      }
      
      Serial.println("Limitator detectat. Se închide bariera...");
      // Închide bariera
      digitalWrite(BARRIER_PIN, LOW);
      
      // Trimite datele către laptop prin Wi-Fi
    //  sendDataToLaptop(received);
    }
    else {
      Serial.println("Acces invalid!");
    }
  }
}

// Funcție pentru transmiterea datelor către laptop printr-o cerere HTTP GET
void sendDataToLaptop(String data) {
  WiFiClient client;
  
  Serial.println("Se încearcă conexiunea la serverul laptopului...");
  if (!client.connect(server_ip, server_port)) {
    Serial.println("Conexiunea la server a eșuat!");
    return;
  }
  
  // Construiește URL-ul pentru cererea GET, de exemplu: /update?data=1234
  String url = "/update?data=" + data;
  
  // Trimitere cerere HTTP
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + server_ip + "\r\n" +
               "Connection: close\r\n\r\n");
  Serial.println("Datele au fost trimise către server.");
  
  delay(10);
  
  // Opțional: se citește răspunsul de la server
  while(client.available()){
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }
  
  client.stop();
}