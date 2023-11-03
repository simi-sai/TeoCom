#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <RF24.h>
#include <nRF24L01.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#define PIN_HUM A0
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

// Declaracion de variables
uint64_t Adress = 0xB3B4B5B6CDLL;
unsigned long Tiempo;
Data mediciones;

// Retorna la Temperatura
float getTemperatura() {
  return dht.readTemperature();
}

// Retorna la Humedad
float getHumedad() {
  return map(analogRead(PIN_HUM), 1024, 300, 0, 100);
}

// Imprime los datos
void ImprimirMensaje(bool aux, Data datos){
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

void Medir() {
  mediciones.Modulo_id = 2;
  mediciones.Temperatura = getTemperatura();
  mediciones.Humedad = getHumedad();
}

// ----------------------

// Configuraciones
void setup() {
  Serial.begin(9600);
  radio.begin();
  dht.begin();

  radio.setPALevel(RF24_PA_LOW);
  radio.openWritingPipe(Adress);
  radio.setDataRate(RF24_250KBPS);
  radio.setChannel(100);

  Tiempo = millis();

  lcd.init();
  lcd.backlight();
  lcd.clear();
}

// Programa Principal
void loop() {           
  if(millis() >= Tiempo) {      // Analiza la bandera
    Medir();
    int i = 0;
    bool OK = false;

    do {
      OK = radio.write(&mediciones, sizeof(mediciones));
      i++;
    } while(!OK && i < 5);

    ImprimirMensaje(OK, mediciones);
    imprimirLCD(mediciones);
    Tiempo += 3600000;          // Aumento la bandera 1 hora
  }
}