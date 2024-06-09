
#include <PubSubClient.h>
#include <WiFi.h>
#include <ESP32Servo.h>

// Connection (MQTT & WiFi)
const char *SSID_MY_WIFI = "DJ PapoSeko";
const char *PASSWORD_MY_WIFI = "micael123";
//const char *SSID_MY_WIFI = "RedmiESP";
//const char *PASSWORD_MY_WIFI = "Bicker01";
//const char *SSID_MY_WIFI = "NOS-BBFF-5";
//const char *PASSWORD_MY_WIFI = "K7GRKEHG";
const char *MQTT_USER = "manel";
const char *MQTT_PASSWORD = "Manel0123@";
const char *BROKER_MQTT = "broker.hivemq.com"/* "753d9dbbe7694f54896483597c46d662.s1.eu.hivemq.cloud" */;
int BROKER_PORT = 1883/* 8883 */;
const char *TOPIC_USER_REQ_AUTHGARAGEM = "userreqauthgaragem-estanciunamentu4000"; // O USER PEDE A AUTENTICAÇÃO DE UMA GARAGEM
const char *TOPIC_USER_RES_AUTHGARAGEM = "userresauthgaragem-estanciunamentu4000"; // O MQTT RESPONDE SOBRE A AUTENTICAÇÃO DE UMA GARAGEM
const char *TOPIC_PORTA_ACTION = "portaacao-estanciunamentu4000"; // O USER INDICA A AÇÃO DA PORTA
const char *TOPIC_PORTA_STATE = "portaestado-estanciunamentu4000"; // O MQTT INFORMA O STATE DA PORTA
const char *TOPIC_ALERTA = "alerta-estanciunamentu4000"; // O MQTT LANÇA UM ALERTA
const char *TOPIC_IMAGEM_PEDIR = "imagempedir-estanciunamentu4000"; // O USER PEDE A IMAGEM DA CAMERA
const char *TOPIC_IMAGEM_ENVIA = "imagemenvia-estanciunamentu4000"; // O MQTT ENVIA A IMAGEM DA CAMERA
//
WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE (50)
char msg[MSG_BUFFER_SIZE];

//INPUTS
//PIR
const int PIR_DATA_PIN = 15;
int PIR_STATE = LOW;
//Botões
const int FIM_CURSO_FECHADO_PIN = 34;
const int FIM_CURSO_ABERTO_PIN = 35;

const int BOTAO_EMERGENCIA_PIN     = 33;


bool FIM_CURSO_FECHADO = false;
bool FIM_CURSO_ABERTO = false;

bool BOTAO_EMERGENCIA = false;
//Botões
const int BUT_EMERGENCIA = 100;

//OUTPUTS
//IR
//const uint16_t IR_RECIVER_PIN = 2;
//const uint16_t IR_EMITER_PIN  = 4;
//IRsend irsend(IR_RECIVER_PIN);
//IRrecv irrecv(IR_EMITER_PIN );
//decode_results results;
const int SERVO_PIN = 4;
float ANG_ABERTO = 90;
float ANG_FECHADO = 0;

//LEDs
const int LED_R_PIN = 25;
const int LED_G_PIN = 26;
const int LED_B_PIN = 27;

const int pwmFreq = 5000;
const int pwmResolution = 8;
const int redChannel = 0;
const int greenChannel = 1;
const int blueChannel = 2;

int RGB_CORES [2][3] = {
  { 0,  255, 0},
  {255,   0, 0}
};
//verde fechado ou aberto desocupado 
//vermelho ocupado fechado
 
//Motores 
const int R1D = 23;
const int R1E = 22;
const int R2D = 19;
const int R2E = 18;

//VARIAVIES
//OUTPUTS
int PORTA_STATE = 0; //fehaco - 0, aberto - 1, abrir - 2, 

//ESTADO GARAGEM
bool OCUPADO = false;


void setup() {
  //WiFi.mode(WIFI_STA);

  Serial.begin(115200);
  while(!Serial){

  }

  setup_wifi();

  //INPUTS
    //IR
      //irsend.begin();
      //irrecv.enableIRIn();
      //pinMode(IR_EMITER_PIN, OUTPUT);  
      //pinMode(IR_RECIVER_PIN, INPUT_PULLUP);
    
    //PIR 
      pinMode(PIR_DATA_PIN, INPUT);

    //FIM CURSO
      pinMode(FIM_CURSO_ABERTO_PIN, INPUT);
      pinMode(BOTAO_EMERGENCIA_PIN, INPUT);
      pinMode(FIM_CURSO_FECHADO_PIN, INPUT);

  //OUTPUT 
      pinMode(R1D, OUTPUT);  
      pinMode(R1E, OUTPUT); 
      pinMode(R2D, OUTPUT); 
      pinMode(R2E, OUTPUT);  

      //LED RGB
      pinMode(LED_R_PIN ,OUTPUT);
      pinMode(LED_G_PIN ,OUTPUT);
      pinMode(LED_B_PIN ,OUTPUT);

      /*ledcSetup(redChannel, pwmFreq, pwmResolution);
      ledcSetup(greenChannel, pwmFreq, pwmResolution);
      ledcSetup(blueChannel, pwmFreq, pwmResolution);

      ledcAttachPin(LED_R_PIN, redChannel);
      ledcAttachPin(LED_G_PIN, greenChannel);
      ledcAttachPin(LED_B_PIN , blueChannel);*/

  client.setServer(BROKER_MQTT, BROKER_PORT);
  client.setCallback(callback);
} 

void loop() {
  if(!client.connected()) reconnect();
  client.loop();

  /*digitalWrite(IR_EMITER_PIN,LOW);   // Liga o LED IR
  
  int sensorValue = digitalRead(IR_RECIVER_PIN); // Lê o valor do receptor IR
  
  if (sensorValue == LOW) {      // Se o sinal está interrompido (LOW), o receptor não está recebendo o sinal do emissor
    Serial.println("Objeto detectado!");
  } else {
    Serial.println("Caminho livre.");
  }

  delay(100);*/
    //Serial.println("EI EI");
  
  if(OCUPADO){
    RGB(1,0,0);
  }else{
    RGB(0,1,0);
  }

  //PIR 
  if(PORTA_STATE == 0 && OCUPADO){
    unsigned int pirValue = digitalRead(PIR_DATA_PIN);
    delay(100);

    if (PIR_STATE == LOW && pirValue == HIGH) {
      Serial.println("Motion detected!");
      PIR_STATE = HIGH;
      publish(TOPIC_ALERTA, "ta aqui gajo! 123", true);
    } 
    else if (PIR_STATE == HIGH && pirValue == LOW){
      PIR_STATE = LOW;
    }
  }

  FIM_CURSO_ABERTO = digitalRead(FIM_CURSO_ABERTO_PIN);
  FIM_CURSO_FECHADO= digitalRead(FIM_CURSO_FECHADO_PIN);

  if(digitalRead(FIM_CURSO_ABERTO_PIN) == HIGH){
     Serial.println("1");
  }else if(digitalRead(FIM_CURSO_FECHADO_PIN) == HIGH){
     Serial.println("2");
  }

  if(digitalRead(BOTAO_EMERGENCIA_PIN)== HIGH){
    Serial.println("3");
  }

  if(PORTA_STATE==2){
    Frente();
    Serial.println("AA");
  }else if(PORTA_STATE==3){
    Atras();
    Serial.println("BB");
  }else if(PORTA_STATE==0 || PORTA_STATE==1){
    Parou();
    Serial.println("CC");
  }

  //porta motor
  if(PORTA_STATE==2 && digitalRead(BOTAO_EMERGENCIA_PIN)){
     PORTA_STATE==1;
     //Parou();
     Serial.println("AI");
     publish(TOPIC_PORTA_STATE, "aberto", false);
  }else if(PORTA_STATE==3 && FIM_CURSO_FECHADO){
     PORTA_STATE==0;

     //Parou();
     publish(TOPIC_PORTA_STATE, "fechar", false);
  }

  /*Serial.print("DEBUG_PS: ");
  Serial.println(PORTA_STATE);

  if(FIM_CURSO_ABERTO){
       Serial.println("A");
  }
  if(FIM_CURSO_FECHADO){
       Serial.println("B");
  }*/
}

void RGB (int r, int g, int b){

      /*analogWrite(LED_R_PIN,  map(RGB_CORES[NUM_COR_LED][0],0,255,0,1023));
      analogWrite(LED_G_PIN,  map(RGB_CORES[NUM_COR_LED][1],0,255,0,1023));
      analogWrite(LED_B_PIN,  map(RGB_CORES[NUM_COR_LED][2],0,255,0,1023));*/

      digitalWrite(LED_R_PIN,  r);
      digitalWrite(LED_G_PIN,  g);
      digitalWrite(LED_B_PIN,  b);

      /*ledcWrite(redChannel  , RGB_CORES[NUM_COR_LED][0]);
      ledcWrite(greenChannel, RGB_CORES[NUM_COR_LED][1]);
      ledcWrite(blueChannel , RGB_CORES[NUM_COR_LED][2]);*/
}

void Frente()
{
  digitalWrite(R1D, HIGH);
  digitalWrite(R2D, HIGH);
  delay(2000);
  digitalWrite(R1D, LOW);
  digitalWrite(R2D, LOW);
}

void Atras()
{
  digitalWrite(R1E, HIGH);
  digitalWrite(R2E, HIGH);
  delay(2000);
  digitalWrite(R1E, LOW);
  digitalWrite(R2E, LOW);
}


void Parou(){
  digitalWrite(R1D, LOW);
  digitalWrite(R2E, LOW);
  delay(2000);
  digitalWrite(R1D, LOW);
  digitalWrite(R2E, LOW);
}


