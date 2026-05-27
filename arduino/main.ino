#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Adafruit_AHTX0.h>    // Librería para la Humedad Ambiente
#include <Adafruit_BMP280.h>   // Librería para Temperatura y Presión

// Inicializamos la pantalla de Oxdea (Dirección estándar 0x27)
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Inicializamos los objetos de los dos sensores físicos
Adafruit_AHTX0 aht;
Adafruit_BMP280 bmp;

void setup() {
  // 1. Abrimos el canal USB para hablar con la PC (Script de Python)
  Serial.begin(9600);
  
  // 2. Iniciamos el protocolo I2C
  Wire.begin(); 
  
  // 3. Encendemos la pantalla
  lcd.init();         
  lcd.backlight();
  lcd.print("Monitoreo Cafe");
  lcd.setCursor(0, 1);
  lcd.print("Iniciando IoT...");
  
  // 4. Arrancamos los sensores reales y verificamos que respondan
  // Nota: 0x77 es la dirección física estándar del chip BMP280
  if (!aht.begin() || !bmp.begin(0x77)) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Error Hardware!");
    lcd.setCursor(0, 1);
    lcd.print("Revisar cables");
    while (1); // Si algo quedó flojo, el Arduino se congela aquí como seguridad
  }
  
  delay(2000);
  lcd.clear();
}

void loop() {
  sensors_event_t humidity, temp;
  aht.getEvent(&humidity, &temp); // El AHT20 calcula humedad y temperatura exacta
  
  // Usamos la temperatura del AHT20 porque su precisión es de ±0.3°C (Mejor que la del BMP280)
  float t = temp.temperature;            
  float h = humidity.relative_humidity;  // Humedad ambiente (Tarda 8s en responder)
  
  // El BMP280 lee la presión y usa su temperatura interna solo para compensar el cálculo
  float p = bmp.readPressure() / 100.0F; 

  // 1. Mostrar en LCD (Optimizado para 1 decimal por la resolución)
  lcd.setCursor(0, 0);
  lcd.print("T:"); lcd.print(t, 1); lcd.print("C ");
  lcd.print("H:"); lcd.print(h, 1); lcd.print("%");

  lcd.setCursor(0, 1);
  lcd.print("P:"); lcd.print(p, 1); lcd.print(" hPa");

  // 2. Enviar a Python por USB
  Serial.print(t, 2);
  Serial.print(",");
  Serial.print(h, 2);
  Serial.print(",");
  Serial.println(p, 2); 

  // 3. Esperar 10 segundos (Suficiente para el tiempo de respuesta de 8s del AHT20)
  delay(10000); 
}