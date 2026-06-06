#include "PosturaSalud.h"

MPU6050 mpu(Wire);
MAX30105 particleSensor;

uint32_t bufferInfrarrojo[100];
uint32_t bufferRojo[100];
int indiceMuestras = 0;

int32_t heartRate = -1;
int32_t spo2 = -1;
int8_t validHeartRate = 0;
int8_t validSPO2 = 0;

int32_t ultimoPulsoValido = -1;
int32_t ultimoOxigenoValido = -1;

unsigned long cronometroPostura = 0;
bool malaPosturaActiva = false;

void inicializarModulos() {
    pinMode(PIN_BUZZER, OUTPUT);
    digitalWrite(PIN_BUZZER, LOW);

    mpu.begin();
    mpu.calcGyroOffsets(true);

    if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) {
        Serial.println("0,-1,-1,-1");
        while(1);
    }

    // configuracion de frecuencias de muestreo y registros internos del sensor
    particleSensor.setup(0x24, 4, 2, 200, 411, 4096);

    // balance de amplitudes para compensar la opacidad del tejido en el pecho
    particleSensor.setPulseAmplitudeRed(0x3A); 
    particleSensor.setPulseAmplitudeIR(0x1F);  
}

void procesarPostura(float angulo) {
    if (angulo > UMBRAL_POSTURA) {
        if (!malaPosturaActiva) {
            cronometroPostura = millis();
            malaPosturaActiva = true;
        } else {
            if (millis() - cronometroPostura >= TIEMPO_MAX_POSTURA) {
                if ((millis() / 250) % 2) {
                    tone(PIN_BUZZER, 1000); 
                } else {
                    noTone(PIN_BUZZER);    
                }
            }
        }
    } else {
        noTone(PIN_BUZZER);
        malaPosturaActiva = false;
    }
}

void procesarSalud() {
    particleSensor.check();

    if(particleSensor.available()) {
        uint32_t valorInfrarrojo = particleSensor.getIR();
        uint32_t valorRojo = particleSensor.getRed();

        // descarte automatico si el sensor esta midiendo el aire ambiental
        if (valorInfrarrojo < 50000) {
            heartRate = -1;
            spo2 = -1;
            validHeartRate = 0;
            validSPO2 = 0;
            ultimoPulsoValido = -1;
            ultimoOxigenoValido = -1;
            indiceMuestras = 0;
        } else {
            bufferRojo[indiceMuestras] = valorRojo;
            bufferInfrarrojo[indiceMuestras] = valorInfrarrojo;
            indiceMuestras++;

            if(indiceMuestras >= 100) {
                maxim_heart_rate_and_oxygen_saturation(
                    bufferInfrarrojo, 100, bufferRojo, &spo2, &validSPO2, &heartRate, &validHeartRate
                );

                // validacion de rangos biologicos aceptables en reposo
                if(validHeartRate && heartRate >= 50 && heartRate <= 115) {
                    ultimoPulsoValido = heartRate;
                }
                if(validSPO2 && spo2 >= 80 && spo2 <= 100) {
                    ultimoOxigenoValido = spo2;
                }

                indiceMuestras = 0;
            }
        }
        particleSensor.nextSample();
    }
}

void enviarDatosCSV(float angulo) {
    Serial.print(millis());
    Serial.print(",");
    Serial.print(angulo);
    Serial.print(",");
    Serial.print(ultimoPulsoValido);
    Serial.print(",");
    Serial.print(ultimoOxigenoValido);
    Serial.println();
}
