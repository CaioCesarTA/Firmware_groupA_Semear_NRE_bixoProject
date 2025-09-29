#include <ESP32Servo.h>


//definição dos servos
Servo levantaquad_direito;
Servo levantaquad_esquerdo;
Servo rotaquad_direito;
Servo rotaquad_esquerdo;
Servo pe_direito;
Servo pe_esquerdo;

//Ultrassônico
int Trig = 12;
int Echo = 13;
float TempoEcho = 0;
float Distancia = 0;

// RGB
int pinR = 34;
int pinG = 35;
int pinB = 32;
int ledcRbat = 0; //entrada PWM usada para o led
int ledcGbat = 1; //entrada PWM usada para o led
int ledcBbat = 2; //entrada PWM usada para o led

// sensor de pulso
int pinbat_in = 25; //entrada do cabo para receber os batimentos
int bat_GPIO;       //variavel do batimento
int bat_lixo 550;   //sinal a ser ignorada
int freq = 5000;  // frequencia de 5000 Hz
int res = 8;  // resolução de 8 bits

void setup() {
  Serial.begin(115200);

//Definição dos pinos dos servos
  levantaquad_esquerdo.attach(14);
  levantaquad_direito.attach(27);
  //rotaquad_esquerdo.attach();
  //rotaquad_direito.attach();
  pe_esquerdo.attach(26);
  //pe_direito.attach(25);

//Definição ângulo dos servos
  levantaquad_esquerdo.write(90);
  levantaquad_direito.write(90);
  //rotaquad_esquerdo.write(90);
  //rotaquad_direito.write(90);
  pe_esquerdo.write(90);
  //pe_direito.write(90);

//Sensor Ultrassônico 
  pinMode(Trig, OUTPUT);
  pinMode(Echo, INPUT);
  
  pinMode(pinR,OUTPUT);         // pin do led 
  ledcSetup(pinR, freq, res);   //setup do led PWM 
  ledcAttachPin(pinR, ledcRbat); //selecionando qual entrada vai funcionar para o led 

    pinMode(pinG,OUTPUT);         // pin do led
  ledcSetup(pinR, freq, res);   //setup do led PWM 
  ledcAttachPin(pinR, ledcGbat); //selecionando qual entrada vai funcionar para o led 

    pinMode(pinB,OUTPUT);         // pin do led
  ledcSetup(pinB, freq, res);   //setup do led PWM
  ledcAttachPin(pinB, ledcBbat); //selecionando qual entrada vai funcionar para o led
}


void loop(){

//Comunicação com a prog
  if (Serial.available()) {
    String cmd = Serial.readStringUntil('\n');
    if (cmd == "VERMELHO") {
      pulsaVermelho();
      AtivaPulseSensor();
    }
    else if (cmd == "AZUL"){
      pulsaAzul();
    }
    esle if (cmd == "LARANJA"){
      pulsaLaranja();
    }
    else if (cmd == "VERDE"){
      pulsaVerde();
    }
    else if (cmd == "AMARELO"){
      pulsaAmarelo();
    }
    else (cmd == "ROSA"){
      pulsaRosa()
    }
  }
  
  //funções soltas (sensor utrassônico)
  enviarPulso();
  calculaDistancia();
  imprimeDistancia();
}

//Código para andar 
void avanca_esquerda() {
  levantaquad_esquerdo.write(110);//levanta o quadril 
  delay(1000);
  pe_esquerdo.write(70);//abaixa o pé
  delay(2000);
//  pe_direito.write(70);levanta o pé
  levantaquad_direito.write(110);//perna para trás
  delay(1000);
}
/*
void avanca_direita() {
  levantaquad_direito.write(70);//levanta o quadril
  delay(1000);
  pe_direito.write(110);//abaixa o pé
  delay(2000);
  pe_esquerdo.write(110);
  levantaquad_esquerdo.write(70);// perna para trás
  delay(1000);
}

void peso_na_esquerda(){
  pe_direito.write(110);
  delay(1000);
  pe_esquerdo.write(90);
  levantaquad_esquerdo.write(90);//joga o peso para a perna esquerda
  delay(500);
  levantaquad_direito.write(90);
  pe_direito.write(90);//retorna tudo para posição inicial
}

void peso_na_direita(){
  pe_esquerdo.write(70);
  delay(1000);
  pe_direito.write(90);
  levantaquad_direito.write(90);//joga o peso para a perna esquerda
  delay(500);
  levantaquad_esquerdo.write(90);
  pe_esquerdo.write(90);//retorna tudo para posição inicial
}

void anda_para_frente() {
  avanca_direita();
  delay(2000);
  peso_na_direita();
  delay(2000);
  avanca_esquerda();
  delay(2000);
  peso_na_direita();
}

//curvas
void virar_direita() {

}

void virar_esquerda() {

}
*/
// sensor ultrassônico
void enviarPulso() {
  digitalWrite(Trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(Trig, LOW);
}

float calculaDistancia() {
  TempoEcho = pulseIn(Echo, HIGH);
  return((TempoEcho*0.0343)/2);
}

void imprimeDistancia() {
  Serial.print("Distância: ");
  Distancia = calculaDistancia();
  Serial.println(Distancia);
}
/*
void respostaDistancia() {
  if (Distancia > 50) {
    ligarVerde();
  }
  if (Distancia <= 50 && Distancia >= 10) {
    ligarAmarelo();
  }
  if (Distancia < 10) {
    ligarVermelho();
  }
    delay(500);
  
}
*/

void AtivaPulseSensor(){
while(cmd != ROSA){
if (bat_GPIO > bat_lixo){
    pulsaVermelho()
    }
    if (Serial.available()) cmd = Serial.readStringUntil('\n');
  }

  digitalWrite(pinR, 0);
  digitalWrite(pinB, 0);
    pulsaVerde()
    return 0;
    }


    //funções do LED
void pulsaVermelho(){
for (int i = 0; i<= 255; i++){
    ledcWrite(pinR, i);
    delay(10);
      }
  for (int i = 255; i>= 0; i--){
    ledcWrite(pinR, i);
    delay(10);
      }
    }

void pulsaVerde(){
for (int i = 0; i<= 255; i++){
    ledcWrite(pinG, i);
    delay(10);
      }
  for (int i = 255; i>= 0; i--){
    ledcWrite(pinG, i);
    delay(10);
      }
    }
  
void pulsaAzul(){
for (int i = 0; i<= 255; i++){
    ledcWrite(pinB, i);
    delay(10);
      }
  for (int i = 255; i>= 0; i--){
    ledcWrite(pinB, i);
    delay(10);
      }
    }
void pulsaLaranja(){
int verde;
digitalWrite(pinB, 0);
for (int i = 0; i<= 255; i++){
    ledcWrite(pinR, i);
    verde = (int)(i*0,55);
    ledcWrite(pinG, verde);
    delay(10);
      }
  for (int i = 255; i>= 0; i--){
    ledcWrite(pinR, i);
    verde = (int)(i*0,55);
    ledcWrite(pinG, verde);
    delay(10);
      }
    }

void pulsaAmarelo(){
int verde;
int Vermelho;
digitalWrite(pinB, 0);
for (int i = 0; i<= 255; i++){
    Vermelho = (int)(i*0,8);
    ledcWrite(pinG, Vermelho); 
    verde = (int)(i*0,8);
    ledcWrite(pinG, verde);
    delay(10);
      }
  for (int i = 255; i>= 0; i--){
    Vermelho = (int)(i*0,8);
    ledcWrite(pinG, Vermelho);
    verde = (int)(i*0,8);
    ledcWrite(pinG, verde);
    delay(10);
      }
    }

  void pulsaRosa(){
int Azul;
int verde;
digitalWrite(pinB, 0);
for (int i = 0; i<= 255; i++){
    ledcWrite(pinR, i);
    Azul = (int)(i*0,8);
    verde = (int)(i*0.75);
    ledcWrite(pinB, Azul);
    delay(10);
      }
  for (int i = 255; i>= 0; i--){
    ledcWrite(pinR, i);
    Azul = (int)(i*0,8);
    verde = (int)(i*0.75); 
    ledcWrite(pinB, Azul);
    delay(10);
      }
    }