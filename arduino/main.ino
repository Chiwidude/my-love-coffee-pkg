#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include <Wire.h>
#include <Adafruit_AHTX0.h> // Librería para el AHT20
// Nota: También incluirías la del BMP280 para presión

// Configuración de tu red Wi-Fi
#define WIFI_SSID "NOMBRE_DE_TU_WIFI"
#define WIFI_PASSWORD "CONTRASEÑA_DE_TU_WIFI"

// Configuración de Firebase
#define FIREBASE_HOST "https://my-love-coffee-pkg-default-rtdb.firebaseio.com/"
#define FIREBASE_AUTH "4o0DFJMyIxe1n9cM0nsZvvCBtIq4ToQy2A5J571Q" 

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;
Adafruit_AHTX0 aht;

void setup() {
  Serial.begin(115200);
  Wire.begin(); // Inicializa bus I2C
  
  if (!aht.begin()) {
    Serial.println("No se encontró el sensor AHT20");
    while (1);
  }

  // Conexión Wi-Fi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  // Configuración de Firebase
  config.host = FIREBASE_HOST;
  config.signer.tokens.legacy_token = FIREBASE_AUTH;
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}

void loop() {
  sensors_event_t humidity, temp;
  aht.getEvent(&humidity, &temp); // Lee el sensor

  float h = humidity.relative_humidity;
  float t = temp.temperature;
  float p = 1013.25; // Aquí iría la lectura real del BMP280

  if (Firebase.ready()) {
    // 1. Guardar el estado ACTUAL (sobrescribe)
    Firebase.Json.setFloat(fbdo, "/monitoreo_cafe/actual/humedad", h);
    Firebase.Json.setFloat(fbdo, "/monitoreo_cafe/actual/temperatura", t);
    Firebase.Json.setFloat(fbdo, "/monitoreo_cafe/actual/presion", p);
    Firebase.RTDB.setJSON(&fbdo, "/monitoreo_cafe/actual", &fbdo.jsonObject);

    // 2. Guardar en el HISTORIAL (agrega una nueva lectura)
    String pathHistorial = "/monitoreo_cafe/historial/" + String(millis());
    Firebase.RTDB.setJSON(&fbdo, pathHistorial, &fbdo.jsonObject);
    
    Serial.println("Datos enviados a Firebase con éxito");
  }

  delay(5000); // Envía datos cada 5 segundos
}