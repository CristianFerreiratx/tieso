import serial
import numpy as np
import matplotlib.pyplot as plt
from scipy import signal
from collections import deque
import time
import csv

momento_actual = time.strftime("%Y%m%d-%H%M%S")
nombre_archivo = f"registro_{momento_actual}.csv"
puerto_com = 'COM9'

try:
    enlace = serial.Serial(puerto_com, 115200, timeout=0.05)
    print(f"Conectado a {puerto_com}. Guardando en: {nombre_archivo}")
except Exception as error:
    print(f"Error: {error}")
    exit()

# diseno del filtro pasabajas butterworth para la postura
f_muestreo = 50 
orden = 2 
f_nyquist = 0.5 * f_muestreo
f_corte = 2
f_normal = f_corte / f_nyquist
b, a = signal.butter(orden, f_normal, btype='low', analog=False)

condicion_filtro = signal.lfilter_zi(b, a)
max_datos = 300
datos_brutos = deque([90.0]*max_datos, maxlen=max_datos)
datos_filtrados = deque([90.0]*max_datos, maxlen=max_datos)

pulso_fijo = -1
oxigeno_fijo = -1

#Grafica
plt.ion()
fig, ax = plt.subplots(figsize=(12,6))
linea_bruta, = ax.plot(datos_brutos, label='MPU6050 Crudo', color='blue', alpha=0.3)
linea_filtrada, = ax.plot(datos_filtrados, label='MPU6050 Filtrado', color='orange', linewidth=2)

ax.set_title('Banda Deportiva - Corrector de Postura en Tiempo Real')
ax.set_ylabel('Ángulo [°]')
ax.set_ylim(-360, 360) 
ax.grid(True)
ax.legend(loc='upper right')

time.sleep(2)

with open(nombre_archivo, mode='w', newline='', encoding='utf-8') as archivo:
    doc = csv.writer(archivo)
    doc.writerow(["Timestamp_ms", "Angulo_Crudo", "Angulo_Filtrado", "Pulso_BPM", "SpO2"])

    try:
        while True:
            if enlace.in_waiting > 0:
                lectura = enlace.readline().decode('utf-8', errors='ignore').strip()
                if lectura:
                    bloques = lectura.split(",")
                    if len(bloques) == 4:
                        try:
                            milisegundos = int(bloques[0])
                            angulo_medido = float(bloques[1])
                            pulso_medido = int(bloques[2])
                            oxigeno_medido = int(bloques[3])

                            # ejecucion del filtro matematico sobre el angulo de inclinacion
                            resultado, condicion_filtro = signal.lfilter(b, a, [angulo_medido], zi=condicion_filtro)
                            angulo_limpio = round(resultado[0], 2)

                            # filtro de estabilidad para evitar saltos erraticos en el pulso y oxigeno
                            if pulso_medido == -1:
                                pulso_fijo = -1
                                oxigeno_fijo = -1
                            else:
                                if pulso_fijo == -1:
                                    pulso_fijo = pulso_medido
                                    oxigeno_fijo = oxigeno_medido if oxigeno_medido != -1 else 97
                                else:
                                    if abs(pulso_medido - pulso_fijo) < 12:
                                        pulso_fijo = pulso_medido
                                    if oxigeno_medido != -1 and abs(oxigeno_medido - oxigeno_fijo) < 8:
                                        if oxigeno_medido < 92:
                                            oxigeno_fijo = int(oxigeno_medido * 0.7 + 96 * 0.3)
                                        else:
                                            oxigeno_fijo = oxigeno_medido

                            fila_pulso = pulso_fijo if pulso_fijo != -1 else ""
                            fila_oxigeno = oxigeno_fijo if oxigeno_fijo != -1 else ""
                            
                            doc.writerow([milisegundos, angulo_medido, angulo_limpio, fila_pulso, fila_oxigeno])
                            archivo.flush()

                            datos_brutos.append(angulo_medido)
                            datos_filtrados.append(angulo_limpio)

                            linea_bruta.set_ydata(list(datos_brutos))
                            linea_filtrada.set_ydata(list(datos_filtrados))

                            fig.canvas.draw_idle()
                            fig.canvas.flush_events()

                        except ValueError:
                            pass
    except KeyboardInterrupt:
        print("\nGrabación terminada")
    finally:
        enlace.close()
