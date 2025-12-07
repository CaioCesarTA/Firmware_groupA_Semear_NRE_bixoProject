#include <ESP32Servo.h>
#include <PulseSensorPlayground.h>
#include <HCSR04.h>

//ROS
#include <ros.h>
#include <std_msgs/String.h>

ros::NodeHandle nh;

// Callback do ROS
void rosCallback(const std_msgs::String& msg);

// Subscriber
ros::Subscriber<std_msgs::String> sub("color_detected", &rosCallback);

//mensagem
volatile String cmd = "";

// Servos
Servo levantaquad_esquerdo;
Servo pe_esquerdo;
Servo levantaquad_direito;
Servo pe_direito;
Servo rotaquad_direito;
Servo rotaquad_esquerdo;

// Ultrassônico
int Trig = 19;
int Echo = 18;
UltraSonicDistanceSensor sensorUltrassonico(Trig, Echo);

// Distância compartilhada
volatile float distancia = 1000;  // valor inicial grande
float bpm;

int intervalo;

// RGB
int pinR = 32;
int pinG = 33;
int pinB = 25;
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

    vTaskDelay(50 / portTICK_PERIOD_MS); // 20 Hz
  }
}

// TASK PRINCIPAL (CORE 1)

void taskPrincipal(void *pvParameters) {
  for (;;) {
    nh.spinOnce();
    vTaskDelay(20 / portTICK_PERIOD_MS); // evita travar o núcleo
  }
}

void setup() {
  Serial.begin(115200);

  //ROS
  nh.getHardware()->setPort(&Serial);
  nh.getHardware()->setBaud(115200);
  delay(1000);
  nh.initNode();
  nh.subscribe(sub);

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

//posicao padrao dos servos
  levantaquad_esquerdo.write(90);
 pe_esquerdo.write(90);
 levantaquad_direito.write(90);
 pe_direito.weite(90);
 rotaquad_direito.write(90);
 rotaquad_esquerdo.write(90);
}

void loop() {
}

//CALLBACK DO ROS
void rosCallback(const std_msgs::String& msg) {
  cmd = msg.data; 
  nh.loginfo(msg.data);

  if (cmd == "Vermelho") {
    pulsaVermelho();
    AtivaPulseSensor();
  }
  else if (cmd == "Azul") {
    pulsaAzul();
    anda_para_frente();
  }
  else if (cmd == "Laranja") {
    pulsaLaranja();
    avanca_direita();
  }
  else if (cmd == "Verde") {
    pulsaVerde();
  }
  else if (cmd == "Amarelo") {
    parar();
  }
  else if (cmd == "Rosa") {
    pulsaRosa();
    avanca_direita();
  }
}

//funcao para o sensor de pulso
void AtivaPulseSensor(){
while(cmd != "Amarelo"){
  if(sendPulseSignal){
    delay(20);
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
  delay(intervalo);
  digitalWrite(pinR, LOW);
  delay(intervalo);

  cmd = "";
  nh.spinOnce();
  vTaskDelay(5 / portTICK_PERIOD_MS);
  // AQUI verifica somente o comando específico
  if (cmd == "Amarelo")  || (distancia < 5){
  parar();
  return;
    }
  }
}

//Código para andar 
void avanca_esquerda() {
  delay(100);
  quadril_esquerdo.write(90 - 15);
  delay(100);
  quadril_direito.write(90 - 15);
  delay(100);
  quadril_esquerdo.write(90);
  delay(100);
  quadril_direito.write(90);
  delay(100);

    cmd = "";
    nh.spinOnce();
    vTaskDelay(5 / portTICK_PERIOD_MS);
    // AQUI verifica somente o comando específico
    if (cmd == "Amarelo") || (distancia < 5){
    parar();
    return;
    }
}

void avanca_direita() {
  delay(100);
  quadril_direito.write(90 + 15);
  delay(100);
  quadril_esquerdo.write(90 - 15);
  delay(100);
  quadril_direito.write(90);
  delay(100);
  quadril_esquerdo.write(90);
      cmd = "";
      nh.spinOnce();
    vTaskDelay(5 / portTICK_PERIOD_MS);
    // AQUI verifica somente o comando específico
    if (cmd == "Amarelo")  || (distancia < 5){
      parar();
      return;
    }
}

void anda_para_frente() {
      cmd = "";
      nh.spinOnce();
    vTaskDelay(5 / portTICK_PERIOD_MS);
    // AQUI verifica somente o comando específico
    if (cmd == "Amarelo") || (distancia < 5){
      parar();
      return;
    }
}

void parar(){
  pulsaAmarelo();
 levantaquad_esquerdo.write(90);
 pe_esquerdo.write(90);
 levantaquad_direito.write(90);
 pe_direito.weite(90);
 rotaquad_direito.write(90);
 rotaquad_esquerdo.write(90);

}

void piscaVermelho(){
  analogWrite(pinG, 0);
  analogWrite(pinB, 0);
  analogWrite(pinR, 255);
    vTaskDelay(10 / portTICK_PERIOD_MS);
  analogWrite(pinR, 0);
}

void pulsaVermelho() {
  for (int i = 0; i <= 255; i++) {
    analogWrite(pinR, i); analogWrite(pinG, 0); analogWrite(pinB, 0);
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
  for (int i = 255; i >= 0; i--) {
    analogWrite(pinR, i); analogWrite(pinG, 0); analogWrite(pinB, 0);
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}

void pulsaVerde() {
  for (int i = 0; i <= 255; i++) {
    analogWrite(pinR, 0); analogWrite(pinG, i); analogWrite(pinB, 0);
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
  for (int i = 255; i >= 0; i--) {
    analogWrite(pinR, 0); analogWrite(pinG, i); analogWrite(pinB, 0);
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}

void pulsaAzul() {
  for (int i = 0; i <= 255; i++) {
    analogWrite(pinR, 0); analogWrite(pinG, 0); analogWrite(pinB, i);
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
  for (int i = 255; i >= 0; i--) {
    analogWrite(pinR, 0); analogWrite(pinG, 0); analogWrite(pinB, i);
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}

void pulsaLaranja() {
  for (int i = 0; i <= 255; i++) {
    analogWrite(pinR, i); analogWrite(pinG, i * 0.25); analogWrite(pinB, 0);
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
  for (int i = 255; i >= 0; i--) {
    analogWrite(pinR, i); analogWrite(pinG, i * 0.25); analogWrite(pinB, 0);
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}

void pulsaAmarelo() {
  for (int i = 0; i <= 255; i++) {
    analogWrite(pinR, i); analogWrite(pinG, i); analogWrite(pinB, 0);
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
  for (int i = 255; i >= 0; i--) {
    analogWrite(pinR, i); analogWrite(pinG, 0); analogWrite(pinB, 0);
  }
}

void pulsaRosa() {

  for (int i = 0; i <= 255; i++) {
    vermelho = i;          
    Azul = i * 0.6;        
    analogWrite(pinR, vermelho); analogWrite(pinB, Azul); analogWrite(pinG, 0);  
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }

  for (int i = 255; i >= 0; i--) {
    vermelho = i;
    Azul = i * 0.6;
    analogWrite(pinR, vermelho); analogWrite(pinB, Azul); analogWrite(pinG, 0);
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}