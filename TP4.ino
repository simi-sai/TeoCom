#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <RF24.h>
#include <nRF24L01.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#define Pin_HUM A0
#define PIN_TEMP D2
#define PIN_CE D9
#define PIN_CS D10
#define Boton D3

// Objetos
RF24 radio(PIN_CE, PIN_CS);
DHT dht(PIN_TEMP, DHT11);
LiquidCrystal_I2C lcd(0x27,16,2);

// Definicion de datos
struct Data {
  int Modulo_id;
  float Temperatura;
  float Humedad;
};

// Declaracion de variables
uint64_t Adress = 0xB3B4B5B6CDLL;
unsigned long Tiempo;
Data recoleccion[24][3];
int Indice_1;
int Indice_2;

// Retorna la Temperatura
float getTemperatura() {
  return dht.readTemperature();
}

// Retorna la Humedad
float getHumedad() {
  return map(analogRead(Pin_HUM), 1024, 300, 0, 100);
}

// Imprime los datos
void ImprimirMensaje(bool aux, Data datos){
  if(aux) {
    Serial.println("Datos Enviados con Exito: ");
  } else {
    Serial.println("Error al mandar los datos: ");
  }
  Serial.println("Temperatura: " + String(datos.Temperatura));
  Serial.println("Humedad: " + String(datos.Humedad));
}

// Mostrar datos por LCD
void imprimirLCD(Data mediciones){
  lcd.setCursor(0,0);
  lcd.print("Temp: ");
  lcd.print(mediciones.Temperatura);
  lcd.setCursor(0,1);
  lcd.print("Humedad: ");
  lcd.print(mediciones.Humedad);
  lcd.print("%");
}

// Guarda los Datos
void GuardarDatos(Data datos) {
  recoleccion[Indice_1][Indice_2] = datos;
  Indice_1 += 1;
  // Para evitar el stack overflow
  if(Indice_1 == 24) {
    Indice_1 = 0;
    Indice_2 += 1;
    if(Indice_2 == 3) {
      Indice_2 = 0;
    }
  }
}

// Envia por serial la matriz guardada
void EnviarSerial() {
  for(int i = 0; i < 24; i++) {
    Serial.println(String(recoleccion[i][0].Temperatura) + "  " + String(recoleccion[i][1].Temperatura) + "  " + String(recoleccion[i][2].Temperatura));
    Serial.println(String(recoleccion[i][0].Humedad) + "  " + String(recoleccion[i][1].Humedad) + "  " + String(recoleccion[i][2].Humedad));
  }
}

// ----------------------

// Configuraciones
void setup() {
  Serial.begin(9600);
  radio.begin();
  dht.begin();

  //pinMode(Boton, INPUT);
  //attachInterrupt(digitalPinToInterrupt(Boton), EnviarSerial, CHANGE);

  radio.setPALevel(RF24_PA_LOW);
  radio.openWritingPipe(Adress);
  radio.setDataRate(RF24_250KBPS);
  radio.setChannel(100);

  Tiempo = millis();
  Indice_1 = 0;
  Indice_2 = 0;

  //Imprimir LCD
  lcd.init();
  lcd.backlight();
  lcd.clear();
  
}

// Programa Principal
void loop() {    // Se realiza cada 1 hora
  if(millis() >= Tiempo) {  // Analiza la bandera
    Data mediciones; 
    mediciones.Modulo_id = 2;
    mediciones.Temperatura = getTemperatura();
    mediciones.Humedad = getHumedad();

    bool OK = radio.write(&mediciones, sizeof(mediciones));
    ImprimirMensaje(OK, mediciones);
    GuardarDatos(mediciones);
    imprimirLCD(mediciones);
    Tiempo += 5000;    // Aumento la bandera 1 minuto
  }
}