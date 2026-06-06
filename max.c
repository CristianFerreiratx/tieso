#ifndef POSTURA_SALUD_H
#define POSTURA_SALUD_H

#include <Arduino.h>
#include <Wire.h>
#include <MPU6050_tockn.h>
#include "MAX30105.h"
#include "spo2_algorithm.h"

const int PIN_BUZZER = 25;
const float UMBRAL_POSTURA = 170.0; 
const unsigned long TIEMPO_MAX_POSTURA = 3000; 

// variables compartidas con el modulo principal
extern MPU6050 mpu;
extern MAX30105 particleSensor;
extern int32_t heartRate;
extern int32_t spo2;
extern int8_t validHeartRate;
extern int8_t validSPO2;
extern int32_t ultimoPulsoValido;
extern int32_t ultimoOxigenoValido;

void inicializarModulos();
void procesarPostura(float angulo);
void procesarSalud();
void enviarDatosCSV(float angulo);

#endif
