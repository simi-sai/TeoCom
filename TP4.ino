#include <LiquidCrystal_I2C.h>

#include <SPI.h>
#include <RF24.h>
#include <nRF24L01.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#define PinHUM A0
#define PIN_TEMP D2
#define PIN_CE D9
#define PIN_CS D10

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

unsigned long long Adress = 0xB3B4B5B6CDLL;
unsigned long Tiempo;
Data recoleccion[24];

// Retorna la Temperatura
float getTemperatura() {
  return dht.readTemperature();
}

// Retorna la Humedad
float getHumedad() {
  return float(analogRead(PinHUM));
}

// Imprime los datos
void ImprimirMensaje(bool aux, Data datos){
  if(aux) {
    Serial.println("Datos Enviados con Exito: ");
  } else {
    Serial.println("Error al mandar los datos");
  }
  Serial.println("Temperatura: " + String(datos.Temperatura));
  Serial.println("Humedad: " + String(datos.Humedad));
}
//mostrar datos por LCD
void imprimirLCD(Data mediciones){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Temperatura: ");
  lcd.print(temp);
  lcd.setCursor(0,1);
  lcd.print("Humedad: ");
  lcd.print(humedad);
  lcd.print("%");
}

// Guarda los datos

// ----------------------

// Configuraciones
void setup() {
  Serial.begin(9600);
  radio.begin();
  dht.begin();

  radio.openWritingPipe(Adress);
  radio.setDataRate(RF24_250KBPS);
  Tiempo = millis();
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
    ImprimirMensaje(OK,mediciones);
    GuardarDatos(mediciones);
    imprimirLCD(mediciones);
    Tiempo += 3600000;    // Aumento la bandera 1 hora
  }
}

