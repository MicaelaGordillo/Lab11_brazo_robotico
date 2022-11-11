// Importamos las librerías
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include "SPIFFS.h"

#include "config.h"
 // Valores ajustados para el SG90
#define COUNT_LOW 1638
#define COUNT_HIGH 7864
#define TIMER_WIDTH 16
#define PWM1_Ch1    1
#define PWM1_Ch2    2
#define PWM1_Ch3    3
#define PWM1_Ch4    4
String pwmValue1, pwmValue2, pwmValue3, pwmValue4;
// Contadores
int aux = 0; //Para saber si el modo automatico esta activo o no.
int contador = 0; //Vector secuencia
int c1 = 0;
int c2 = 0;
int c3 = 0;
int c4 = 0;
// Vectores
int secuencia[10];
int servo1[10];
int servo2[10];
int servo3[10];
int servo4[10];

AsyncWebServer server(80);

String getRSSI(){
  return String(WiFi.RSSI());
}
void setup() {
  
  Serial.begin(115200);   
  
  ledcSetup(1, 50, TIMER_WIDTH); // canal 1, 50 Hz, 16-bit width
  ledcAttachPin(32, 1);   // GPIO 22 asignado al canal 1
  ledcSetup(2, 50, TIMER_WIDTH); // canal 2, 50 Hz, 16-bit width
  ledcAttachPin(33, 2);   // GPIO 19 asignado al canal 2
  ledcSetup(3, 50, TIMER_WIDTH); // canal 1, 50 Hz, 16-bit width
  ledcAttachPin(26, 3);   // GPIO 22 asignado al canal 3
  ledcSetup(4, 50, TIMER_WIDTH); // canal 2, 50 Hz, 16-bit width
  ledcAttachPin(25, 4);   // GPIO 19 asignado al canal 4

  // Iniciamos  SPIFFS
  if (!SPIFFS.begin()){ 
    Serial.println("ha ocurrido un error al montar SPIFFS");
    return;
  }
  
  // conectamos al Wi-Fi
  WiFi.begin(ssid, password);
  // Mientras no se conecte, mantenemos un bucle con reintentos sucesivos
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    // Esperamos un segundo
    Serial.println("Conectando a la red WiFi..");
  }
  Serial.println();
  Serial.println(WiFi.SSID());
  Serial.print("Direccion IP:\t");
  // Imprimimos la ip que le ha dado nuestro router
  Serial.println(WiFi.localIP());

  // Rutas
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html",String(), false, datos);
  });
  server.on("/index.html", HTTP_GET, [](AsyncWebServerRequest * request) {
    aux = 0;
    request->send(SPIFFS, "/index.html", String(), false, datos);
  });
  server.on("/manual.html", HTTP_GET, [](AsyncWebServerRequest * request) {
    aux = 0;
    request->send(SPIFFS, "/manual.html", String(), false, datos);
  });
  server.on("/automatico.html", HTTP_GET, [](AsyncWebServerRequest * request) {
    aux = 1;
    request->send(SPIFFS, "/automatico.html", String(), false, datos);
  });
  server.on("/joystick.html", HTTP_GET, [](AsyncWebServerRequest * request) {
    aux = 0;
    request->send(SPIFFS, "/joystick.html", String(), false, datos);
  });
  server.on("/index.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.css", "text/css");
  });
  server.on("/RSSI", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain",getRSSI().c_str());
  });
  
  //Primera pestaña
  server.on("/SLIDER1", HTTP_POST, [](AsyncWebServerRequest *request){
    pwmValue1 = request->arg("pwmValue1");
    ledcWrite(PWM1_Ch1,  pwmValue1.toInt()); 
    request->redirect("/index.html");    
  });
  server.on("/SLIDER2", HTTP_POST, [](AsyncWebServerRequest *request){
    pwmValue2 = request->arg("pwmValue2");
    ledcWrite(PWM1_Ch2,  pwmValue2.toInt());
    request->redirect("/index.html");     
  });
  server.on("/SLIDER3", HTTP_POST, [](AsyncWebServerRequest *request){
    pwmValue3 = request->arg("pwmValue3");
    ledcWrite(PWM1_Ch3,  pwmValue3.toInt());
    request->redirect("/index.html");     
   });  
  server.on("/SLIDER4", HTTP_POST, [](AsyncWebServerRequest *request){
    pwmValue4 = request->arg("pwmValue4");
    ledcWrite(PWM1_Ch4,  pwmValue4.toInt());  
    request->redirect("/index.html");   
  });
  
  // Segunda pestaña 
  server.on("/SLIDER5", HTTP_POST, [](AsyncWebServerRequest *request){
    pwmValue1 = request->arg("pwmValue1");
    ledcWrite(PWM1_Ch1,  pwmValue1.toInt());    

    pwmValue2 = request->arg("pwmValue2");
    ledcWrite(PWM1_Ch2,  pwmValue2.toInt());

    pwmValue3 = request->arg("pwmValue3");
    ledcWrite(PWM1_Ch3,  pwmValue3.toInt()); 

    pwmValue4 = request->arg("pwmValue4");
    ledcWrite(PWM1_Ch4,  pwmValue4.toInt()); 
    request->redirect("/manual.html");    
  }); 

  // Tercera pestaña
  server.on("/SLIDER6", HTTP_POST, [](AsyncWebServerRequest *request){
      pwmValue1 = request->arg("pwmValue1");
      aux = 1;
      servo1[c1] = pwmValue1.toInt(); 
      secuencia[contador] = 1;
      secuencia[contador+1] = pwmValue1.toInt();
      c1++;
      contador = contador+2;
      request->redirect("/automatico.html");
  });  
  server.on("/SLIDER7", HTTP_POST, [](AsyncWebServerRequest *request){
      pwmValue2 = request->arg("pwmValue2");
      aux = 1;
      servo2[c2] = pwmValue2.toInt();
      secuencia[contador] = 2;
      secuencia[contador+1] = pwmValue2.toInt();
      c2++;
      contador = contador+2;
      request->redirect("/automatico.html");
   });
   server.on("/SLIDER8", HTTP_POST, [](AsyncWebServerRequest *request){
      pwmValue3 = request->arg("pwmValue3");
      aux = 1;
      servo3[c3]= pwmValue3.toInt();
      secuencia[contador] = 3;
      secuencia[contador+1] = pwmValue3.toInt();
      c3++;
      contador = contador+2;
      request->redirect("/automatico.html");
  });  
  server.on("/SLIDER9", HTTP_POST, [](AsyncWebServerRequest *request){
      pwmValue4 = request->arg("pwmValue4");
      aux = 1;
      servo4[c4]= pwmValue4.toInt();
      secuencia[contador] = 4;
      secuencia[contador+1] = pwmValue4.toInt();
      c4++;
      contador = contador+2;
      request->redirect("/automatico.html");
  });
  server.on("/REINICIAR", HTTP_POST, [](AsyncWebServerRequest *request){
      aux = 0;
      c1 = 0; c2 = 0; c3 = 0; c4 = 0;
      contador = 0;
      secuencia[10];
      servo1[10]; servo2[10]; servo3[10]; servo4[10];
      ledcWrite(PWM1_Ch1,  1638);
      ledcWrite(PWM1_Ch2,  1638);
      ledcWrite(PWM1_Ch3,  1638);
      ledcWrite(PWM1_Ch4,  1638);
      request->redirect("/automatico.html");
  });
  
  server.begin();
}

String datos(const String& var) {
  if (var == "IP") {
    return WiFi.localIP().toString();
  }else if (var == "HOSTNAME") {
    return String(WiFi.SSID());
  }else if (var == "STATUS") {
    return String(WiFi.status());
  }else if (var == "PSK") {
    return String(WiFi.psk());
  }else if (var == "RSSI") {
    return String(WiFi.RSSI());
  }else if (var == "PWM1") {
    return String(pwmValue1);
  }else if (var == "PWM2") {
    return String(pwmValue2);
  }else if (var == "PWM3") {
    return String(pwmValue3);
  }else if (var == "PWM4") {
    return String(pwmValue4);
  }
  return var;
}

void loop() {
  while(aux == 1){
    for (int i = 0; i < contador; i = i+2) {
      if(secuencia[i] == 1){
        ledcWrite(PWM1_Ch1,  secuencia[i+1]);
        delay(1000);
      } else if (secuencia[i] == 2){
        ledcWrite(PWM1_Ch2,  secuencia[i+1]);
        delay(1000);
      } else if (secuencia[i] == 3){
        ledcWrite(PWM1_Ch3,  secuencia[i+1]);
        delay(1000);
      } else if (secuencia[i] == 4){
        ledcWrite(PWM1_Ch4,  secuencia[i+1]);
        delay(1000);
      } else {
        Serial.println("Ocurrio un error!");
      }
      ledcWrite(PWM1_Ch1,  1638);
      ledcWrite(PWM1_Ch2,  1638);
      ledcWrite(PWM1_Ch3,  1638);
      ledcWrite(PWM1_Ch4,  1638);
      delay(1000);
      Serial.print("Servo: ");
      Serial.print(secuencia[i]);
      Serial.print(" Valor: ");
      Serial.println(secuencia[i+1]);
    }
  }
}
