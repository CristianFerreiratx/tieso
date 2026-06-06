#include "PosturaSalud.h"

void setup() {
    Serial.begin(115200);
    Wire.begin(21, 22);
    inicializarModulos();
}

void loop() {
    mpu.update();
    float anguloCalculado = mpu.getAngleX() + 90;

    procesarPostura(anguloCalculado);
    procesarSalud();
    enviarDatosCSV(anguloCalculado);

    delay(20); 
}
