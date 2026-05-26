import serial
import time

# =====================================================================
# CONFIGURACIÓN INICIAL
# =====================================================================
# ⚠️ IMPORTANTE: Cambien 'COM3' por el puerto real que vieron en su Arduino IDE
PUERTO_SERIAL = 'COM3' 
BAUDIOS = 9600

def iniciar_puente():
    print("=== PUENTE SERIAL-USB INICIADO ===")
    print(f"Buscando transmisiones en el puerto: {PUERTO_SERIAL}...")
    
    try:
        # Abrimos la conexión con el cable USB del Arduino
        arduino = serial.Serial(PUERTO_SERIAL, BAUDIOS, timeout=1)
        # Le damos 2 segundos al Arduino para que se reinicie tras la conexión
        time.sleep(2) 
        print("¡Conexión serial establecida con éxito!\n")
        
    except serial.SerialException as e:
        print(f"\n❌ ERROR DE CONEXIÓN: No se pudo abrir el puerto {PUERTO_SERIAL}.")
        print("Revisen que:")
        print("  1. El Arduino esté conectado por USB.")
        print("  2. El Monitor Serie del Arduino IDE esté CERRADO.")
        print(f"Detalles del error: {e}")
        return

    # =====================================================================
    # BUCLE PRINCIPAL: ESCUCHAR EL USB DE FORMA CONTINUA
    # =====================================================================
    while True:
        try:
            # Leer una línea completa que venga del Arduino
            linea = arduino.readline().decode('utf-8').strip()
            
            # Si la línea no está vacía, procesamos los datos
            if linea:
                # El Arduino manda: "temperatura,humedad,presion" (Ej: "24.50,55.30,1011.20")
                datos = linea.split(',')
                
                # Verificamos que vengan exactamente las 3 variables del sensor
                if len(datos) == 3:
                    try:
                        temperatura = float(datos[0])
                        humedad = float(datos[1])
                        presion = float(datos[2])
                        
                        # Imprimimos en la terminal de Miniconda para ver que todo va bien
                        print(f"[{time.strftime('%Y-%m-%d %H:%M:%S')}] Recibido -> "
                              f"Temp: {temperatura:.2f}°C | "
                              f"Hum: {humedad:.2f}% | "
                              f"Presión: {presion:.2f} hPa")
                        
                        # -------------------------------------------------------
                        # 🚀 AQUÍ IRÁ LA CONEXIÓN A INTERNET POSTERIORMENTE
                        # Por ahora, dejamos el espacio listo para mandar a Firebase:
                        # enviar_a_firebase(temperatura, humedad, presion)
                        # -------------------------------------------------------
                        
                    except ValueError:
                        print(f"⚠️ Alerta: Se recibieron datos corruptos o incompletos: '{linea}'")
                else:
                    print(f"⚠️ Alerta: Formato inesperado en el bus: '{linea}'")
                    
        except KeyboardInterrupt:
            # Si presionan Ctrl + C en la terminal, el programa se cierra limpiamente
            print("\n👋 Cerrando el puente serial de forma segura...")
            arduino.close()
            break
            
        except Exception as e:
            print(f"\n❌ Ocurrió un error inesperado en la lectura: {e}")
            if 'arduino' in locals() and arduino.is_open:
                arduino.close()
            break

if __name__ == '__main__':
    iniciar_puente()