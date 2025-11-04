#include <ESP32Servo.h>


//definição dos servos
Servo levantaquad_direito;
Servo levantaquad_esquerdo;
Servo rotaquad_direito;
Servo rotaquad_esquerdo;
Servo pe_direito;
Servo pe_esquerdo;


String cmd;

//Ultrassônico
int Trig = 19;
int Echo = 18;
const float VELOCIDADE_SOM_CM_US = 0.0343; 
const long MAX_TIMEOUT_US = 25000; 

// RGB
int pinR = 25;
int pinG = 32;
int pinB = 33;

// sensor de pulso
int pinbat_in = 34; //entrada do cabo para receber os batimentos
int bat_GPIO;       //variavel do batimento
int bat_lixo = 550;   //sinal a ser ignorada
int freq = 5000;  // frequencia de 5000 Hz
int res = 8;  // resolução de 8 bits

void setup() {
  Serial.begin(115200);

//Definição dos pinos dos servos
  levantaquad_esquerdo.attach(12);
  levantaquad_direito.attach(13);
  rotaquad_esquerdo.attach(14);
  rotaquad_direito.attach(22);
  pe_esquerdo.attach(23);
  pe_direito.attach(26);

//Definição ângulo dos servos
  levantaquad_esquerdo.write(90);
  levantaquad_direito.write(90);
  rotaquad_esquerdo.write(90);
  rotaquad_direito.write(90);
  pe_esquerdo.write(90);
  pe_direito.write(90);

//Sensor Ultrassônico 
  pinMode(Trig, OUTPUT);
  pinMode(Echo, INPUT);
  
//Definição dos leds
  pinMode(pinR,OUTPUT);         // pin do led 
  pinMode(pinG,OUTPUT);         // pin do led
  pinMode(pinB,OUTPUT);         // pin do led
}

void loop(){

//Comunicação com a prog
  if (Serial.available()) {
    cmd = Serial.readStringUntil('\n');
    if (cmd == "Vermelho") {
      pulsaVermelho();
      AtivaPulseSensor();
    }
    else if (cmd == "Azul"){
      pulsaAzul();
    }
    else if (cmd == "Laranja"){
      pulsaLaranja();
    }
    else if (cmd == "Verde"){
      pulsaVerde();
    }
    else if (cmd == "Amarelo"){
      pulsaAmarelo();
    }
    else if (cmd == "Rosa"){
      pulsaRosa();
    }
  }

//Ultrassônico
  float Distancia = medirDistancia(); 

  Serial.print("Distância: ");
  
  if (Distancia > 0) {
    Serial.print(Distancia);
    Serial.println(" cm");
  } else {
    Serial.println("Erro");
  }
}

//Código para andar 
void avanca_esquerda() {
  levantaquad_esquerdo.write(110);//levanta o quadril 
  delay(1000);
  pe_esquerdo.write(70);//abaixa o pé
  delay(2000);
  pe_direito.write(70); //levanta o pé
  delay(10);
  levantaquad_direito.write(110);//perna para trás
  delay(1000);
}

void avanca_direita() {
  levantaquad_direito.write(70);//levanta o quadril
  delay(1000);
  pe_direito.write(110);//abaixa o pé
  delay(2000);
  pe_esquerdo.write(110);
  delay(10);
  levantaquad_esquerdo.write(70);// perna para trás
  delay(1000);
}

void peso_na_esquerda(){
  pe_direito.write(110);
  delay(1000);
  pe_esquerdo.write(90);
  delay(10);
  levantaquad_esquerdo.write(90);//joga o peso para a perna esquerda
  delay(500);
  levantaquad_direito.write(90);
  delay(10);
  pe_direito.write(90);//retorna tudo para posição inicial
  delay(200);
}

void peso_na_direita(){
  pe_esquerdo.write(70);
  delay(1000);
  pe_direito.write(90);
  delay(10);
  levantaquad_direito.write(90);//joga o peso para a perna esquerda
  delay(500);
  levantaquad_esquerdo.write(90);
  delay(10);
  pe_esquerdo.write(90);//retorna tudo para posição inicial
  delay(200);
}

void anda_para_frente() {
  avanca_direita();
  delay(200);
  peso_na_direita();
  delay(200);
  avanca_esquerda();
  delay(200);
  peso_na_direita();
  delay(200);
}

//curvas
void virar_direita() {
  rotaquad_direito.write(110);
  delay(100);
  rotaquad_esquerdo.write(70);
  delay(100);
}

void virar_esquerda() {
  rotaquad_esquerdo.write(110);
  delay(100);
  rotaquad_direito.write(70);
  delay(100);
}


// sensor ultrassônico
float medirDistancia() {
  long duracao_us; 
  float distancia;


  digitalWrite(Trig, LOW);
  delayMicroseconds(2);
  digitalWrite(Trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(Trig, LOW);

  duracao_us = pulseIn(Echo, HIGH, MAX_TIMEOUT_US); 

  if (duracao_us == 0) {
    return 0.0; // Retorna 0.0 para ser tratado como 'Erro' no loop()
  }

  distancia = (duracao_us * VELOCIDADE_SOM_CM_US) / 2;

  if (distancia < 2.0 || distancia > 400.0) {
      return 0.0;
  }

  if (distancia < 5){
    peso_na_direita();
    delay(100);
    peso_na_esquerda();
    delay(100);
  }
  
  return distancia;
}


//funcao para o sensor de pulso
void AtivaPulseSensor(){
while(cmd != "Amarelo"){
      bat_GPIO = analogRead(pinbat_in);
  if (bat_GPIO > bat_lixo){
    piscaVermelho;
  }

  if (Serial.available()) cmd = Serial.readStringUntil('\n');
  }

  digitalWrite(pinR, 0);
  digitalWrite(pinB, 0);
  pulsaVerde();
}


void piscaVermelho(){

analogWrite(pinG, 0);
analogWrite(pinB, 0);
analogWrite(pinR, 255);
delay(10);
analogWrite(pinR, 0);

}
//funções do LED
void pulsaVermelho() {
  int vermelho;

  for (int i = 0; i <= 255; i++) {
    vermelho = i;

    analogWrite(pinR, vermelho);
    analogWrite(pinG, 0);
    analogWrite(pinB, 0);   
    delay(10);
  }


  for (int i = 255; i >= 0; i--) {
    vermelho = i;

    analogWrite(pinR, vermelho);
    analogWrite(pinG, 0);
    analogWrite(pinB, 0);
    delay(10);
  }
}

void pulsaVerde() {
  int verde;

  for (int i = 0; i <= 255; i++) {
    verde = i;

    analogWrite(pinR, 0);
    analogWrite(pinG, verde);
    analogWrite(pinB, 0);   
    delay(10);
  }

  for (int i = 255; i >= 0; i--) {
    verde = i;

    analogWrite(pinR, 0);
    analogWrite(pinG, verde);
    analogWrite(pinB, 0);
    delay(10);
  }
}
  
void pulsaAzul() {
  int Azul;

  for (int i = 0; i <= 255; i++) {
    Azul = i;

    analogWrite(pinR, 0);
    analogWrite(pinG, 0);
    analogWrite(pinB, Azul);   
    delay(10);
  }

  for (int i = 255; i >= 0; i--) {
    Azul = i;

    analogWrite(pinR, 0);
    analogWrite(pinG, 0);
    analogWrite(pinB, Azul);
    delay(10);
  }
}

void pulsaLaranja() {
  int vermelho, verde;

  for (int i = 0; i <= 255; i++) {
    vermelho = i;           
    verde = i * 0.5;        

    analogWrite(pinR, vermelho);
    analogWrite(pinG, verde);
    analogWrite(pinB, 0);
    delay(10);
  }

  for (int i = 255; i >= 0; i--) {
    vermelho = i;
    verde = i * 0.5;

    analogWrite(pinR, vermelho);
    analogWrite(pinG, verde);
    analogWrite(pinB, 0);
    delay(10);
  }
}

void pulsaAmarelo() {
  int vermelho, verde;

  for (int i = 0; i <= 255; i++) {
    vermelho = i;        
    verde = i;           
    analogWrite(pinR, vermelho);
    analogWrite(pinG, verde);
    analogWrite(pinB, 0);  
    delay(10);
  }

  for (int i = 255; i >= 0; i--) {
    vermelho = i;
    verde = i;
    analogWrite(pinR, vermelho);
    analogWrite(pinG, verde);
    analogWrite(pinB, 0);
    delay(10);
  }
}

void pulsaRosa() {
  int vermelho, azul;

  for (int i = 0; i <= 255; i++) {
    vermelho = i;          
    azul = i * 0.6;        
    analogWrite(pinR, vermelho);
    analogWrite(pinB, azul);
    analogWrite(pinG, 0);  
    delay(10);
  }

  for (int i = 255; i >= 0; i--) {
    vermelho = i;
    azul = i * 0.6;
    analogWrite(pinR, vermelho);
    analogWrite(pinB, azul);
    analogWrite(pinG, 0);
    delay(10);
  }
}