#include <WiFi.h>
#include "time.h"
#include <HTTPClient.h>
#include <SoftwareSerial.h>
#include <SolarPosition.h>


#define MYPORT_TX 17
#define MYPORT_RX 16

int period = 1000;
unsigned long time_now = 0;


int ElevRefPin1 =0; //pin que recibira la senal de uno de los limit switch en el eje de elevacion
int ElevRefPin2 =0;//pin que recibira la senal de uno de los limit switch en el eje de elevacion
int AzmthRefPin = 0; //pin que recibira la senal de uno del limit switch en el eje del agulo azimuth

int ElevRef1_deg = 30; //angulo constante en el que esta ubicado uno de los limit switch del eje de elevacion
int ElevRef2_deg = 150; //angulo constante en el que esta ubicado uno de los limit switch del eje de elevacion
int AzmthRef_deg = 0; //angulo constante en el que esta ubicado el limit switch del eje del angulo azimuth

int ElevAxisDeg = 0;  //degrees of elevation axis with respect to the ground angle
int AzmthAxisDeg = 0; //degrees of azimuth axis with respecto to the south

bool able_to_stop1 = true;
bool able_to_stop2 = true;


SoftwareSerial TRexJr; //puerto serial para mandar comandos a la tarjeta Trex Jr


SolarPosition ReynosaUT(26.050425, -98.260485); //latitud y longitud

const uint8_t digits = 3;

unsigned long degree_converter = 76.444444444444; //use this var to scale degrees and convert it to time (used to control motors by degrees)

const char* ssid = "INFINITUM7EDE_2.4";
const char* password =  "3KU99ZC64q";

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 0;
const int   daylightOffset_sec = 0;

struct Motor { //estructura para el objeto "Motor" (solo almacenara comandos para su control)
    int FORWARD_CMD;
    int REVERSE_CMD;
    int STOP_CMD;
};

Motor ElevMotor = { //struct con los comandos basicos para controlar el motor del eje de elevacion
    0xC2,
    0xC1,
    0xC0
};

Motor AzmthMotor = { //struct con los comandos basicos para controlar el motor del eje de elevacion
    0xCA,
    0xC9,
    0xC8
};

time_t getTime() {
  time_t now;
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    return(0);
  }
  time(&now);
  return now;
}



void setup() {
    
    
    //los comandos para cada accion podrian estar mal, revisar despues


  

  Serial.begin(115200);

  pinMode(MYPORT_RX, INPUT);
  pinMode(MYPORT_TX, OUTPUT);
  
  pinMode(ElevRefPin1, INPUT);
  pinMode(ElevRefPin2, INPUT);
  pinMode(AzmthRefPin, INPUT);
  

  setTime(SECS_YR_2000);

    // Init and get the time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

    // establece el provedor del tiempo
  SolarPosition::setTimeProvider(getTime);

  TRexJr.begin(19200, SWSERIAL_8N1, MYPORT_RX, MYPORT_TX, false);
  if (!TRexJr) { // If the object did not initialize, then its configuration is invalid
    Serial.println("Invalid SoftwareSerial pin configuration, check config"); 
    while (1) { // Don't continue with invalid configuration
      delay (1000);
    }
  }

  // Connect to Wi-Fi
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected."); 




  
  //secuencia inicial para mover los ejes a los puntos de referencia
  //MoveMotorsToReferences();

  Serial.println("about to set motor");
  SetMotor(ElevMotor, 180, true);
  


  
}

void loop() {
//   delay(1000);
//   printSolarPosition(ReynosaUT.getSolarPosition(), digits);

  if ((ElevRefPin1 == HIGH) && able_to_stop1){
      ElevAxisDeg = ElevRef1_deg;
      StopMotor(ElevMotor);
      able_to_stop1 = false;
  }
  
  if (ElevRefPin1 == LOW){
      able_to_stop1 = true;
  }
  
  if ((ElevRefPin2 == HIGH) && able_to_stop2){
      ElevAxisDeg = ElevRef2_deg;
      StopMotor(ElevMotor);
      able_to_stop2 = false;
  }
  
  if (ElevRefPin2 == LOW){
      able_to_stop2 = true;
  }
  
  if (AzmthRefPin == HIGH){
      AzmthAxisDeg = AzmthRef_deg;
      StopMotor(AzmthMotor);
  }

  if(millis() >= time_now + period){
      time_now += period;
      if ((getTime() % (3600/2)) == 0){
        Serial.println("5 seconds");
        //UpdateAxis();
      }
  }
  
}


//los bytes de los comandos para los motores pueden estar mal, comprobarlos despues
//tambien hay que analizar que referencia va de acuerdo al movimiento (en marcha o reversa)

void MoveMotorsToReferences(){
    
    while (!(ElevRefPin1 == HIGH) && !(ElevRefPin2 == HIGH)){
        TRexJr.write(0xC2);
        TRexJr.write(127);
    }
    
    StopMotor(ElevMotor);
    
    if (ElevRefPin1 == HIGH) {
        ElevAxisDeg = ElevRef1_deg;
    }
    else {
        ElevAxisDeg = ElevRef2_deg;
    }
    
    
    while (!AzmthRefPin == HIGH){
        TRexJr.write(0xCA);
        TRexJr.write(127);
    }
    
    StopMotor(AzmthMotor);
    AzmthAxisDeg = AzmthRef_deg;
}

void MoveToLimit(int limitSignalPin){
    if (limitSignalPin == ElevRefPin1){
        TRexJr.write(0xC2);
        TRexJr.write(127);
    }
    else{
        TRexJr.write(0xCA);
        TRexJr.write(127);
    }
}





//funcion que se pretende usar cada determinado tiempo para actualizar la posicion de los ejes
//de manera que se ajusten para apuntar al sol (se ajustara un eje a la vez, no al mismo tiempo)
//los bytes de los comandos para los motores pueden estar mal, comprobarlos despues


void UpdateAxis(){
    unsigned long elev = ReynosaUT.getSolarPosition().elevation;
    unsigned long azmth = ReynosaUT.getSolarPosition().azimuth;
    
    if ((elev > ElevRefPin1) && (elev < ElevRefPin2)){
        int elev_to_move = elev - ElevAxisDeg;

        if (elev_to_move < 0){
          SetMotor(ElevMotor, abs(elev_to_move), false);
        }

        else {
          SetMotor(ElevMotor, elev_to_move, true);
        }
        ElevAxisDeg = elev;
    }
    
    
    delay(15000); //delay para asegurar que los motores no se enciendan al mismo tiempo(se puede ajustar despues)
    int azmth_to_move = abs(azmth - AzmthAxisDeg);
    SetMotor(AzmthMotor, azmth_to_move, true);
    AzmthAxisDeg = azmth;
}

    




//funciones para controlar los motores por grados y detenerlos
//el estado actual de estas funciones, no se sabe si se esta
//usando correctamente el tipo "struct", hay que revisar eso

void SetMotor(struct Motor motor, int degrees, bool is_forward){

    if (is_forward){
      Serial.println("is forward");
      TRexJr.write(motor.FORWARD_CMD);
      TRexJr.write(127); //127 is maximum speed
      delay(degrees * degree_converter);
      Serial.println("se detiene");
      StopMotor(motor);
    }
    else {
      TRexJr.write(motor.REVERSE_CMD);
      TRexJr.write(127); //127 is maximum speed
      delay(degrees * degree_converter);
      StopMotor(motor);
    }
}





//funcion para obtener el tiempo (segundos desde 1900 hasta el momento actual)


//funcion para imprimir la elevacion y angulo azimuth del sol 

//funcion para conectarse a una red WIFI
void Connect(){
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("IP: " + (String)WiFi.localIP());  
}

void StopMotor(struct Motor motor){
    TRexJr.write(motor.STOP_CMD);
    TRexJr.write(127);
}

