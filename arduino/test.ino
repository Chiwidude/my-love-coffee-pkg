#include <Wire.h>
#include <LiquidCrystal_I2C.h> // Librería para pantallas estándar como la de Oxdea

int seconds = 0;

// Configuramos la dirección real 0x27, con 16 columnas y 2 filas
LiquidCrystal_I2C lcd_1(0x27, 16, 2); 

void setup(){ 
  lcd_1.init();        // En pantallas reales se usa init() en vez de begin()
  lcd_1.backlight();   // Enciende la luz de fondo inicial
  lcd_1.print("hello world");
}

void loop() { 
  lcd_1.setCursor(0, 1);
  lcd_1.print(seconds);
  
  // Efecto de parpadeo adaptado para la pantalla real
  lcd_1.backlight();    // Enciende la luz
  delay(500); 
  
  lcd_1.noBacklight();  // Apaga la luz (equivalente a setBacklight(0))
  delay(500); 
  
  seconds += 1;
}