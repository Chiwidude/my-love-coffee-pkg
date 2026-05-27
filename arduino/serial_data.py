import serial
import time
import requests  # Librería para enviar datos a internet

# =====================================================================
# CONFIGURACIÓN INICIAL Y ENLACES
# =====================================================================
# ⚠️ CAMBIEN ESTOS 2 VALORES CON SUS DATOS REALES:
PUERTO_SERIAL = 'COM3' 

# Copien la URL de su Realtime Database (debe terminar en ".firebaseio.com/")
URL_FIREBASE = "https://my-love-coffee-pkg-default-rtdb.firebaseio.com/monitoreo_cafe.json"

BAUDIOS = 9600

def enviar_a_firebase(t, h, p):
    """Envía un paquete de datos acumulativo a Firebase"""
    # CAMBIO: Metemos los datos dentro de la "llave" del nodo histórico
payload = {
        "temperatura": t,
        "humedad": h,
        "presion": p,
        "timestamp": time.strftime('%Y-%m-%d %H:%M:%S')
    }
    
    try:
        # Al hacer POST a la raíz con esta estructura, Firebase generará 
        # automáticamente los identificadores únicos dentro de 'monitoreo_cafe'
        respuesta = requests.post(URL_FIREBASE, json=payload, timeout=5)
        
        if respuesta.status_code == 200:
            print(" -> [OK] Historial actualizado en Firebase.")
        else:
            print(f" -> [⚠️] Firebase respondió con código de error: {respuesta.status_code}")
            
    except requests.exceptions.RequestException as e:
        print(f" -> [❌] Error de red: {e}")

def iniciar_puente():
    print("=== PUENTE SERIAL-USB A FIREBASE INICIADO ===")
    print(f"Buscando transmisiones en el puerto: {PUERTO_SERIAL}...")
    
    try:
        arduino = serial.Serial(PUERTO_SERIAL, BAUDIOS, timeout=1)
        time.sleep(2) # Esperar estabilización del Arduino
        print("¡Conexión serial establecida con éxito!\n")
        
    except serial.SerialException as e:
        print(f"\n❌ ERROR DE CONEXIÓN SERIAL: No se pudo abrir el puerto {PUERTO_SERIAL}.")
        print("Asegúrense de cerrar el Monitor Serie del Arduino IDE.")
        return

    # =====================================================================
    # BUCLE PRINCIPAL
    # =====================================================================
    while True:
        try:
            linea = arduino.readline().decode('utf-8').strip()
            
            if linea:
                datos = linea.split(',')
                
                if len(datos) == 3:
                    try:
                        temperatura = float(datos[0])
                        humedad = float(datos[1])
                        presion = float(datos[2])
                        
                        # Mostrar localmente en la consola de Miniconda
                        print(f"[{time.strftime('%Y-%m-%d %H:%M:%S')}] Recibido -> "
                              f"Temp: {temperatura:.2f}°C | Hum: {humedad:.2f}% | Presión: {presion:.2f} hPa", end="")
                        
                        # 🚀 ¡EL ESLABÓN PERDIDO! Enviamos los datos a la nube:
                        enviar_a_firebase(temperatura, humedad, presion)
                        
                    except ValueError:
                        print(f"\n⚠️ Alerta: Datos corruptos recibidos: '{linea}'")
                else:
                    print(f"\n⚠️ Alerta: Formato inesperado en el bus: '{linea}'")
                    
        except KeyboardInterrupt:
            print("\n👋 Cerrando el puente serial de forma segura...")
            arduino.close()
            break
            
        except Exception as e:
            print(f"\n❌ Ocurrió un error inesperado: {e}")
            if 'arduino' in locals() and arduino.is_open:
                arduino.close()
            break

if __name__ == '__main__':
    iniciar_puente()