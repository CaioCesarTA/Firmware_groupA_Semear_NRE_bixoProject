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

// Callback 2 do ROS
void rosCallback_2(const std_msgs::String& msg);

// Subscriber
ros::Subscriber<std_msgs::String> sub("comandos", &rosCallback_2);


//mensagem
String cmd = "";

// Servos
Servo quadril_direito;
Servo quadril_esquerdo;
Servo pe_direito;
Servo pe_esquerdo;
Servo joelho_direito;
Servo joelho_esquerdo;

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
    
  if (cmd == "Vermelho"|| cmd == "PARAR"){  
    pulsaVermelho();
    parar();
  }
  else if (cmd == "Azul"|| cmd == "DIREITA") {
    pulsaAzul();
    avanca_direita();
  }
  else if (cmd == "Laranja" || cmd == "FRENTE"){
    pulsaLaranja();
    anda_para_frente();
  }
  else if (cmd == "Verde" || cmd == "DESATIVAR"){
    pulsaVerde();
  }
  else if (cmd == "Amarelo") {
    pulsaAmarelo();
  AtivaPulseSensor();
  }
  else if (cmd == "Rosa") || cmd == "FRENTE"){
    pulsaRosa();
    avanca_esquerda();
  }
  else if (cmd == "SAMBA" || cmd == "PRESIDENTE"){
    Samba();
  }
    vTaskDelay(10 / portTICK_PERIOD_MS); // evita travar o núcleo
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
  quadril_direito.attach(22);
  quadril_esquerdo.attach(12);
  pe_direito.attach(26);
  pe_esquerdo.attach(14);
  joelho_direito.attach(23);
  joelho_esquerdo.attach(13);



  quadril_direito.write(85);
  quadril_esquerdo.write(80);
  pe_direito.write(95);
  pe_esquerdo.write(90);
  joelho_direito.write(90);
  joelho_esquerdo.write(95);
  delay(5000);

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
}

//CALLBACK DO ROS
void rosCallback(const std_msgs::String& msg) {
  cmd = msg.data; 
nh.loginfo(msg.data);
}

//CALLBACK 2 DO ROS
void rosCallback_2(const std_msgs::String& msg) {
  cmd = msg.data; 
nh.loginfo(msg.data);
}


//funcao para o sensor de pulso
void AtivaPulseSensor(){
while(cmd != "Vermelho"){
  if(sendPulseSignal){
      vTaskDelay(20 / portTICK_PERIOD_MS);
    Serial.println(pulseSensor.getLatestSample());
  }

  if (pulseSensor.sawStartOfBeat()) {
    if(!sendPulseSignal){
      bat_GPIO = pulseSensor.getBeatsPerMinute();
      if (bat_GPIO >= 120) {
        Serial.print("120");
      bat_GPIO = 120;
    }
      else if (bat_GPIO <= 80){
      Serial.print("80");
      bat_GPIO = 80;
      }
      else if (bat_GPIO <= 120 && bat_GPIO >= 80) Serial.print(bat_GPIO);
    }
  }
if (bat_GPIO < 40) bat_GPIO = 40;
intervalo = (60000 / bat_GPIO) / 2;
  digitalWrite(pinR, HIGH);
      vTaskDelay(intervalo / portTICK_PERIOD_MS);
  digitalWrite(pinR, LOW);
      vTaskDelay(intervalo / portTICK_PERIOD_MS);

nh.spinOnce();
if (cmd == "Vermelho" || distancia < 5) {
    parar();
    return;
    }
  }
}

//funcao que danca
void Samba(){
    while(1){
  pe_direito.write(90 - 10);
      vTaskDelay(100 / portTICK_PERIOD_MS);
  joelho_direito.write(90 + 10);
      vTaskDelay(100 / portTICK_PERIOD_MS);
  pe_esquerdo.write(90 - 10);
      vTaskDelay(100 / portTICK_PERIOD_MS);
  joelho_esquerdo.write(90 + 10);
      vTaskDelay(100 / portTICK_PERIOD_MS);
  pe_direito.write(90);
      vTaskDelay(100 / portTICK_PERIOD_MS);
  joelho_direito.write(90);
      vTaskDelay(100 / portTICK_PERIOD_MS);
  pe_esquerdo.write(90);
      vTaskDelay(100 / portTICK_PERIOD_MS);
  joelho_esquerdo.write(90);
      vTaskDelay(100 / portTICK_PERIOD_MS);

    nh.spinOnce();
if (cmd == "Vermelho" || distancia < 5 || cmd == "FRENTE") {
    parar();
    return;
      }
    }
}

//Código para andar 
void avanca_esquerda() {
  while(1){
  vTaskDelay(100 / portTICK_PERIOD_MS);
  quadril_esquerdo.write(80 - 15);
  vTaskDelay(100 / portTICK_PERIOD_MS);
  quadril_direito.write(90 - 15);
  vTaskDelay(100 / portTICK_PERIOD_MS);
  quadril_esquerdo.write(80);
  vTaskDelay(100 / portTICK_PERIOD_MS);
  quadril_direito.write(90);
  vTaskDelay(100 / portTICK_PERIOD_MS);

  nh.spinOnce();
  if (cmd == "Vermelho" || distancia < 5|| cmd == "FRENTE") {
    parar();
    return;
    }
  }
}

void avanca_direita() {
  while(1){
  quadril_direito.write(90 + 15);
      vTaskDelay(100 / portTICK_PERIOD_MS);
  quadril_esquerdo.write(80 + 15);
      vTaskDelay(100 / portTICK_PERIOD_MS);
  quadril_direito.write(90);
      vTaskDelay(100 / portTICK_PERIOD_MS);
  quadril_esquerdo.write(80);
nh.spinOnce();
if (cmd == "Vermelho" || distancia < 5|| cmd == "FRENTE") {
    parar();
    return;
    }
  }
}

void anda_para_frente() {
    while(1){
  joelho_direito.write(90 + 20);
  pe_direito.write(95 - 20);
      vTaskDelay(200 / portTICK_PERIOD_MS);
  joelho_esquerdo.write(95 - 20); 
  pe_esquerdo.write(90 + 20);
      vTaskDelay(200 / portTICK_PERIOD_MS);
  joelho_direito.write(90);
  pe_direito.write(95);
      vTaskDelay(200 / portTICK_PERIOD_MS);
  joelho_esquerdo.write(95); 
  pe_esquerdo.write(90);
      vTaskDelay(200 / portTICK_PERIOD_MS);
  
  joelho_esquerdo.write(95 - 20); 
  pe_esquerdo.write(90 + 20);
      vTaskDelay(200 / portTICK_PERIOD_MS);
  joelho_direito.write(90 + 20);
  pe_direito.write(95 - 20);
      vTaskDelay(200 / portTICK_PERIOD_MS);
  joelho_esquerdo.write(95); 
  pe_esquerdo.write(90);
      vTaskDelay(200 / portTICK_PERIOD_MS);
  joelho_direito.write(90);
  pe_direito.write(95);
      vTaskDelay(200 / portTICK_PERIOD_MS);

nh.spinOnce();
if (cmd == "Vermelho" || distancia < 5|| cmd == "FRENTE") {
    parar();
    return;
    }
}
}

void parar(){
  pulsaVermelho(); 
  quadril_direito.write(90);
  quadril_esquerdo.write(90);
  pe_direito.write(90);
  pe_esquerdo.write(90);
  joelho_direito.write(90);
  joelho_esquerdo.write(90);
vTaskDelay(200 / portTICK_PERIOD_MS);
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
    analogWrite(pinR, i); analogWrite(pinG, i); analogWrite(pinB, 0);
    vTaskDelay(10 / portTICK_PERIOD_MS);
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