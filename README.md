# Banda Deportiva Inteligente T.I.E.S.O 🚀

Este proyecto consiste en una **banda deportiva inteligente** diseñada para monitorear la postura corporal, la frecuencia cardíaca y la saturación de oxígeno en sangre ($SpO_2$) en tiempo real. Utiliza un microcontrolador ESP32 para la recolección de datos biofísicos y un script de Python para el procesamiento, filtrado digital y visualización gráfica de los datos.

Proyecto desarrollado para la asignatura de **Programación Superior** en la *Universidad Católica Boliviana San Pablo - Sede Tarija*.

## 🛠️ Componentes de Hardware

* **ESP32 WROOM:** Microcontrolador principal encargado del procesamiento local, control de sensores y transmisión de datos.
* **MPU6050:** Acelerómetro y giroscopio de 6 ejes utilizado para medir la inclinación de la columna y detectar malas posturas.
* **MAX30102:** Sensor óptico de alta sensibilidad (PPG) para medir el pulso cardíaco y el oxígeno en sangre mediante fotometría reflectante.
* **Zumbador (Buzzer Piezoceléctrico):** Sistema de alerta física intermitente que se activa tras 3 segundos de mala postura continua.

## 💻 Características del Software

### 1. Firmware del ESP32 (Arduino C++)
Organizado de forma modular en tres archivos para facilitar su mantenimiento y escalabilidad:
* `PosturaSalud.h`: Definición de constantes, pines de hardware, variables globales compartidas y prototipos de funciones.
* `PosturaSalud.cpp`: Desarrollo lógico del control del zumbador, calibración del MPU6050 y procesamiento de luz roja/infrarroja mediante el algoritmo de Maxim Integrated.
* `ProyectoBanda.ino`: Hilo principal que coordina la inicialización (`setup`) y el bucle cíclico de lectura y envío (`loop`) a través de un bus I2C de alta velocidad.

### 2. Interfaz y Filtrado en Python
El script `main.py` procesa los datos crudos enviados por el cable serie/USB:
* **Filtro Butterworth (IIR de paso bajo):** Limpia el ruido físico, temblores musculares o vibraciones del movimiento, aislando el ángulo real de la espalda.
* **Gráfica Interactiva (Matplotlib):** Muestra de forma visual y dinámica la comparación en tiempo real entre el ángulo crudo y el ángulo filtrado.
* **Datalogger Automático:** Guarda cada sesión con marcas de tiempo en un archivo `.csv` compatible con Excel para su posterior análisis estadístico.

## 🚀 Instalación y Uso rápido

### Requisitos previos
* **Arduino IDE** con las librerías `Wire`, `MPU6050_tockn` y `SparkFun MAX3010x`.
* **Python 3.x** con los siguientes paquetes instalados:
    ```bash
    pip install pyserial numpy matplotlib scipy
    ```

### Instrucciones de ejecución
1.  Realiza las conexiones físicas de los sensores al ESP32 a través de los pines I2C (`SDA -> Pin 21`, `SCL -> Pin 22`) y el Buzzer al `Pin 25`.
2.  Carga el código de Arduino en tu placa ESP32 usando el Arduino IDE.
3.  Verifica el puerto COM asignado a tu ESP32 (ej. `COM9`) y actualízalo en la variable `puerto_com` dentro del archivo `main.py`.
4.  Ejecuta el script de Python en tu computadora:
    ```bash
    python main.py
    ```
5.  Mantén una postura erguida durante los primeros 2 segundos para permitir la correcta inicialización del sistema.

## 📈 Futuras Actualizaciones (Versión 2)
* **Inalámbrico mediante Bluetooth Serial (SPP):** Migración del envío por cable hacia la antena interna del ESP32 para mayor comodidad deportiva.
* **Portabilidad Autónoma:** Integración de batería LiPo de 3.7V con circuito de carga TP4056 para evitar el apagado automático por bajo consumo presente en los Powerbanks comerciales.

## 👥 Autores
* Gesem Callapa
* Cristian Ferreira
* *Carrera de Ingeniería Mecatrónica - Biomédica*
