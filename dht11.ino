// █████████████████████████████████████████████████████
//
// ESP32/ESP8266 BlackBook by PowerTGS (2024)
//
// author : Tomasz Majcher 
// e-mail : powerTGS440@gmail.com
// phone  : +48 668 082121
//
// █████████████████████████████████████████████████████
//
// DHT11 demo file
//
// source file      : sourcce/sensor/dht11/dht11.ino
// device tested    : esp32/esp2866
// code platform    : ArduinoIDE
// 
// █████████████████████████████████████████████████████
// W Y K O R Z Y S T A N I E    P A M I Ę C I   R A M   &   F L A S H
// █████████████████████████████████████████████████████
//
// Variables and constants in RAM (global, static), used 28212 / 80192 bytes (35%)
// Code in flash (default, ICACHE_FLASH_ATTR), used 243772 / 1048576 bytes (23%)
//
// █████████████████████████████████████████████████████
// M A K R O
// █████████████████████████████████████████████████████

#define SERIAL_MONITOR        true                          // SERIAL włączony
#define SERIAL_SPEED          115200                        // prędkość SERIAL
#define SETUP_DELAY           5000                          // opoznienie SETUP
#define LOOP_DELAY            10000                         // opóźnienie pętli LOOP 

#define FPSTR(pstr_pointer) (reinterpret_cast<const __FlashStringHelper *>(pstr_pointer))
#define F(string_literal) (FPSTR(PSTR(string_literal)))

// █████████████████████████████████████████████████████
// B I B L I O T E K I   D H T   1 1
// █████████████████████████████████████████████████████   
                       
#include "DHT.h"

#define DHTPIN 5                            // GPIO 5 (D1) dla ESP8266
#define DHTTYPE DHT11                       // definicja typu czujnika DHT11

// #define #define DHTTYPE DHT22            // w przypadku czujnika DHT22

// █████████████████████████████████████████████████████
// O B I E K T   K L A S Y   D H T
// █████████████████████████████████████████████████████

DHT dht(DHTPIN, DHTTYPE);

// █████████████████████████████████████████████████████
// Z M I E N N E   G L O B A L N E
// █████████████████████████████████████████████████████

struct DHT_SENSOR                     // struktura dla czujnika DHT11/DHT22
{
    float   temperatura_C = 0;        // zmienna dla przechowywania temp w st. C
    float   temperatura_F = 0;        // zmienna dla przechowywania temp w st. F
    float   wilgotnosc = 0;           // zmienna dla przechowywania wilgotnosci
}   dht_sensor;

bool F_Sensor_DHT_OK = false;         // flaga błedów dla DHT11/DHT22

// █████████████████████████████████████████████████████
// D E K L A R A C J E   F U N K C J I
// █████████████████████████████████████████████████████

void Read_DHT_Sensor (float &temp_C, float &temp_F, float &humi);

#if SERIAL_MONITOR
    void Show_DHT_Sensor (float &temp_C, float &temp_F, float &humi);
#endif

// █████████████████████████████████████████████████████
// S E T U P
// █████████████████████████████████████████████████████

void setup() 
{
    #if SERIAL_MONITOR
        Serial.begin(SERIAL_SPEED);
        Serial.printf_P ( PSTR ("\n\n[DHT11/DHT22] Sensor Test" ));
    #endif

    dht.begin();               // inicjalizacja czujnika DHT11/DHT22
    delay(SETUP_DELAY);        // dla tego kiepskiego czujnika trzeba dać opóźnienie
}

// █████████████████████████████████████████████████████
// L O O P
// █████████████████████████████████████████████████████

void loop() 
{
    Read_DHT_Sensor (dht_sensor.temperatura_C, dht_sensor.temperatura_F, dht_sensor.wilgotnosc);  

    #if SERIAL_MONITOR                
        Show_DHT_Sensor (dht_sensor.temperatura_C, dht_sensor.temperatura_F, dht_sensor.wilgotnosc);  
    #endif

    delay(LOOP_DELAY);
}

// █████████████████████████████████████████████████████
// R E A D   D H T   S E N S O R
// █████████████████████████████████████████████████████
// funkcja odczytuje wartości z czujnika DHT11/DHT22
// na wejsciu otrzymuje adresy zmiennych w strukturze dht_sensor
// █████████████████████████████████████████████████████

void Read_DHT_Sensor (float &temp_C, float &temp_F, float &humi)
{
    float t = dht.readTemperature();              // zmienna lokalna, tymczasowa
    float f = dht.readTemperature(true);          // zmienna lokalna, tymczasowa
    float h = dht.readHumidity();                 // zmianne lokalna, tymczasowa

     F_Sensor_DHT_OK = false;                     // resetujemy FLAGĘ, ustawiamy FALSE

    // --------------------------------------------------------------------------------- //
    // moduły DHT11/DHT22 nie słyną z bezawaryjnej i bezproblemowej pracy niestety       //
    // zanim zapiszemy do struktury nowe dane należy sprawdzić czy są prawidłowe :-)     //
    // ten prosty warunek załatwia nam sprawę, albo pozostaną poprzednie odczyty albo    //
    // po wyświetleniu ujrzymy tam wartości : NAN                                        //
    // W tym szkicu problemu większego nie będzie, program się nie wysypie, urządzenie   //
    // nie zrestartuje, ale w przypadku wysyłania danych na wyświetlacz LCD, TFT to już  //
    // zaczynają się schody. Zamiast zmiennej FLOAT wysyłamy wartość NAN, nie sprawdzamy //
    // co przybyło i próbujemy to wyświetlić, dlatego trzeba się zabezpieczyć. Inne      //
    // moduły też potrafią nabroić, szukamy problemu ale nie wiemy gdzie leży, więc      //
    // taka wskazówka, by sobie uprościć życie i walczyć z BUG'ami, które sami kreujemy  //
    //                                                                                   //
    // Nie dużo trzeba, a nawet DHT11/DHT22 będą pracować bezproblemowo!                 //    
    // --------------------------------------------------------------------------------- //

    if (isnan(humi) || isnan(t) || isnan(f))     // jeśli odczyt z czujnika nie prawidłowy
    {
        #if SERIAL_MONITOR
            Serial.printf_P( PSTR ("\n\n[DHT_Sensor] Wykryto błąd odczytu z czujnika.") );
            Serial.printf_P( PSTR ("\n[DHT_Sensor] Ustawiam Flagę: F_Sensor_DHT_OK na FALSE.") );
        #endif       
        // nie ustawiamy F_Sensor_DHT_OK na FALSE, zrobiliśmy to przed wejściem do warunku!
    }
    else
    {
        #if SERIAL_MONITOR
            Serial.printf_P( PSTR ("\n\n[DHT_Sensor] Dane z czujnika wyglądają na prawidłowe.") );
            Serial.printf_P( PSTR ("\n[DHT_Sensor] Ustawiam Flagę: F_Sensor_DHT_OK na TRUE.") );
        #endif
        
        F_Sensor_DHT_OK = true;   // dane wyglądają Okey, ustawiamy FLAGĘ na TRUE          
                      
        temp_C = t;               // kopiujemy do struktury wartości ze zmiennych tymczasowych
        temp_F = f;               // kopiujemy do struktury wartości ze zmiennych tymczasowych
        humi   = h;               // kopiujemy do struktury wartości ze zmiennych tymczasowych
    }    
}

// █████████████████████████████████████████████████████
// S H O W   D H T   S E N S O R
// █████████████████████████████████████████████████████
// funkcja odczytuje wartości z czujnika ATH20
// na wejsciu otrzymuje adresy struktury ath_sensor 
// █████████████████████████████████████████████████████

#if SERIAL_MONITOR
void Show_DHT_Sensor (float &temp_C, float &temp_F, float &humi)
{
    Serial.printf_P( PSTR ("\n[DHT_Sensor] Temperatura = %.02f ºC"), temp_C);
    Serial.printf_P( PSTR ("\n[DHT_Sensor] Temperatura = %.02f ºF"), temp_F);
    Serial.printf_P( PSTR ("\n[DHT Sensor] Wilgotnosc  = %.02f %RH"), humi); 
}
#endif

// █████████████████████████████████████████████████████
// END OF FILE : src/sensor/dht11/dht11.ino
// █████████████████████████████████████████████████████
