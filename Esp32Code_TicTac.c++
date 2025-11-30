#include <ESP32Servo.h>
#include <PulseSensorPlayground.h>
#include <HCSR04.h>

// Servos
Servo levantaquad_esquerdo;
Servo pe_esquerdo;
Servo levantaquad_direito;
Servo pe_direito;

// Ultrassônico
int Trig = 19;
int Echo = 18;
UltraSonicDistanceSensor sensorUltrassonico(Trig, Echo);

// Distância compartilhada
volatile float distancia = 1000;  // valor inicial grande
float bpm;

int intervalo;

// RGB
int pinR = 25;
int pinG = 33;
int pinB = 32;
int vermelho = 0;
int verde = 0;
int Azul = 0;

// Pulse Sensor
PulseSensorPlayground pulseSensor;
int pinbat_in = 34;
int bat_GPIO = 120;
int bat_lixo = 1200;
bool sendPulseSignal = false;

// HANDLES DAS TASKS
TaskHandle_t TaskUltrassonico;
TaskHandle_t TaskPrincipal;


// TASK DO ULTRASSONICO (CORE 0)

void taskUltrassonico(void *pvParameters) {
  for (;;) {
    float d = sensorUltrassonico.measureDistanceCm();

    if (d > 0) {
      distancia = d;
    } else {
      distancia = 999; // erro → distância gigante
    }

    Serial.print("DISTANCIA = ");
    Serial.print(distancia);
    Serial.println(" cm");

    vTaskDelay(50 / portTICK_PERIOD_MS); // 20 Hz
  }
}


// TASK PRINCIPAL (CORE 1)

void taskPrincipal(void *pvParameters) {
  for (;;) {

    if (distancia > 5) {
      pulsaAzul();
      andar();
    }

      if(sendPulseSignal){
    vTaskDelay(pdMS_TO_TICKS(20));
    Serial.println(pulseSensor.getLatestSample());
  }

    if (pulseSensor.sawStartOfBeat()) {
      if(!sendPulseSignal){
        bat_GPIO = pulseSensor.getBeatsPerMinute();
        if (bat_GPIO >= 120) Serial.print("120");
        else if (bat_GPIO <= 80) Serial.print("80");
        else if (bat_GPIO <= 120 && bat_GPIO >= 80) Serial.print(bat_GPIO);
        Serial.println(" bpm");
      }
    }
  intervalo = (60000 / bat_GPIO) / 2;
  digitalWrite(pinR, HIGH);
vTaskDelay(pdMS_TO_TICKS(intervalo));
  digitalWrite(pinR, LOW);
vTaskDelay(pdMS_TO_TICKS(intervalo));

    vTaskDelay(20 / portTICK_PERIOD_MS); // evita travar o núcleo
  }
}


void setup() {
  Serial.begin(115200);

  // Pulse sensor
  pulseSensor.analogInput(pinbat_in);
  pulseSensor.blinkOnPulse(pinR);
  pulseSensor.setSerial(Serial);
  pulseSensor.setThreshold(bat_lixo);

  // Servos
  levantaquad_esquerdo.attach(12);
  pe_esquerdo.attach(14);
  levantaquad_direito.attach(22);
  pe_direito.attach(26);

  levantaquad_esquerdo.write(10);
  pe_esquerdo.write(90);
  levantaquad_direito.write(130);
  pe_direito.write(25);
  delay(200);

  // LEDs
  pinMode(pinR, OUTPUT);
  pinMode(pinG, OUTPUT);
  pinMode(pinB, OUTPUT);

  // Criar task do ultrassônico → CORE 0
  xTaskCreatePinnedToCore(
    taskUltrassonico,
    "Ultrassonico",
    4096,
    NULL,
    1,
    &TaskUltrassonico,
    0
  );

  // Criar task principal → CORE 1
  xTaskCreatePinnedToCore(
    taskPrincipal,
    "Principal",
    8192,
    NULL,
    1,
    &TaskPrincipal,
    1
  );
}

void loop() {
  // NÃO USADO — tudo está nas tasks
}



void andar() {
  levantaquad_esquerdo.write(30);
  delay(1000);
  pe_esquerdo.write(75);
  delay(1000);
  levantaquad_esquerdo.write(10);
  delay(1000);
  pe_esquerdo.write(90);
  delay(1000);

  levantaquad_direito.write(110);
  delay(1000);
  pe_direito.write(40);
  delay(1000);
  levantaquad_direito.write(130);
  delay(1000);
  pe_direito.write(25);
  delay(1000);
}

void piscaVermelho(){
  analogWrite(pinG, 0);
  analogWrite(pinB, 0);
  analogWrite(pinR, 255);
  delay(10);
  analogWrite(pinR, 0);
}

void pulsaVermelho() {
  for (int i = 0; i <= 255; i++) {
    analogWrite(pinR, i); analogWrite(pinG, 0); analogWrite(pinB, 0);
    delay(10);
  }
  for (int i = 255; i >= 0; i--) {
    analogWrite(pinR, i); analogWrite(pinG, 0); analogWrite(pinB, 0);
    delay(10);
  }
}

void pulsaVerde() {
  for (int i = 0; i <= 255; i++) {
    analogWrite(pinR, 0); analogWrite(pinG, i); analogWrite(pinB, 0);
    delay(10);
  }
  for (int i = 255; i >= 0; i--) {
    analogWrite(pinR, 0); analogWrite(pinG, i); analogWrite(pinB, 0);
    delay(10);
  }
}

void pulsaAzul() {
  for (int i = 0; i <= 255; i++) {
    analogWrite(pinR, 0); analogWrite(pinG, 0); analogWrite(pinB, i);
    delay(10);
  }
  for (int i = 255; i >= 0; i--) {
    analogWrite(pinR, 0); analogWrite(pinG, 0); analogWrite(pinB, i);
    delay(10);
  }
}

void pulsaLaranja() {
  for (int i = 0; i <= 255; i++) {
    analogWrite(pinR, i); analogWrite(pinG, i * 0.25); analogWrite(pinB, 0);
    delay(10);
  }
  for (int i = 255; i >= 0; i--) {
    analogWrite(pinR, i); analogWrite(pinG, i * 0.25); analogWrite(pinB, 0);
    delay(10);
  }
}

void pulsaAmarelo() {
  for (int i = 0; i <= 255; i++) {
    analogWrite(pinR, i); analogWrite(pinG, i); analogWrite(pinB, 0);
    delay(10);
  }
  for (int i = 255; i >= 0; i--) {
    analogWrite(pinR, i); analogWrite(pinG, 0); analogWrite(pinB, 0);
  }
}