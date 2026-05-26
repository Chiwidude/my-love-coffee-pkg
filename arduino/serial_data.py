from datetime import datetime  # <-- Asegúrense de tener este import al inicio del archivo
import serial
import time
import requests

# ... (El resto de sus configuraciones iniciales se mantiene igual) ...

while True:
    try:
        if arduino.in_waiting > 0:
            # Leer la línea del Arduino
            linea = arduino.readline().decode('utf-8').strip()
            datos = linea.split(',')
            
            if len(datos) == 3:
                temp = float(datos[0])
                humedad = float(datos[1])
                presion = float(datos[2])
                
                # OBTENER TIMESTAMP ACTUAL: Formato "AAAA-MM-DD HH:MM:SS"
                timestamp_actual = datetime.now().strftime('%Y-%m-%d %H:%M:%S')
                
                print(f"[{timestamp_actual}] USB -> Temp: {temp}°C | Hum: {humedad}%")
                
                # Agregamos la clave "timestamp" al paquete que viaja a Firebase
                payload = {
                    "temperatura": temp,
                    "humedad": humidity, # (O la variable h que hayan definido)
                    "presion": presion,
                    "timestamp": timestamp_actual  # <-- ¡Listo!
                }
                
                # Enviar los datos a Firebase
                respuesta = requests.put(FIREBASE_URL, json=payload)
                
                if respuesta.status_code == 200:
                    print("¡Datos con marca de tiempo subidos a Firebase!")
                else:
                    print(f"Error en Firebase: Código {respuesta.status_code}")
                    
    except KeyboardInterrupt:
        print("\nPuente detenido.")
        arduino.close()
        break
    except Exception as e:
        print(f"Error: {e}")
        time.sleep(1)