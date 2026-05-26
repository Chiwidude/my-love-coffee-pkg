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
  // Capturar las variables del sensor de humedad (AHT20)
  sensors_event_t humidity, temp;
  aht.getEvent(&humidity, &temp); 
  
  // Leer los valores de temperatura y presión (BMP280)
  float t = bmp.readTemperature();       // Temperatura en °C
  float p = bmp.readPressure() / 100.0F; // Convierte la presión de Pascales a hPa
  float h = humidity.relative_humidity;  // Humedad ambiente en %

  // ─── 1. MOSTRAR DATOS EN EL EMPAQUE (PANTALLA LCD) ───
  // Fila 1: Temperatura y Humedad Ambiente
  lcd.setCursor(0, 0);
  lcd.print("T:");
  lcd.print(t, 1); // Muestra 1 decimal para que quepa bien
  lcd.print("C ");
  lcd.print("H:");
  lcd.print(h, 1);
  lcd.print("%");

  // Fila 2: Presión Atmosférica
  lcd.setCursor(0, 1);
  lcd.print("P:");
  lcd.print(p, 1);
  lcd.print(" hPa");

  // ─── 2. ENVIAR DATOS A LA COMPUTADORA POR USB ───
  // Enviamos los datos en formato "temperatura,humedad,presion"
  Serial.print(t, 2);
  Serial.print(",");
  Serial.print(h, 2);
  Serial.print(",");
  Serial.println(p, 2); // println incluye el salto de línea que Python espera

  delay(2000); // Tomar lecturas en bucle cada 2 segundos
}