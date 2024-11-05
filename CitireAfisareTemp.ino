#include <LiquidCrystal.h>

#define LM35_SENSOR_CHANNEL 0  // Setare pin senzor de temp A0
#define ADC_REF_VOLTAGE 5.0    // Tensiune de referință 5V

// Setare pini LCD
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;

// Declarare lcd
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// Variabilă pentru a ține evidența timpului de start
unsigned long start_time;

// Funcție inițializare ADC
void init_adc() 
{
    // Setează referința de tensiune a ADC la AVCC
    ADMUX |= (1 << REFS0);
    // Setează prescaler-ul ADC la 128
    ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
    // Activează ADC-ul
    ADCSRA |= (1 << ADEN);
}

// Funcție citire date din ADC
float read_temperature() {
    ADMUX &= 0xF0; // Resetează pinii din ADMUX pentru citire
    ADMUX |= LM35_SENSOR_CHANNEL; // Setează pinul la care este legat senzorul de temperatură (A0)
    // Pornește conversia ADC
    ADCSRA |= (1 << ADSC);
    // Așteaptă finalizarea conversiei
    while (ADCSRA & (1 << ADSC)) {}
    // Obține rezultatul ADC și calculează temperatura
    uint16_t adc_value = ADC;
    // Transformă valoarea analogică în tensiune
    float voltage = (float) adc_value * ADC_REF_VOLTAGE / 1024.0;
    // Transformă tensiunea în grade Celsius pentru LM35
    float temperature = voltage * 100.0;
    return temperature;
}

void setup() 
{
    lcd.begin(16, 2);
    init_adc();
    Serial.begin(9600);
    start_time = millis(); // Înregistrează timpul de start
}

void loop()
{
    float temp_float = read_temperature(); // Citește valoarea temperaturii
    int tmp = (int)(temp_float + 0.5); // Convertește temperatura la un număr întreg (rotunjit)

    // Calculează timpul scurs
    unsigned long elapsed_time = millis() - start_time;
    unsigned int total_seconds = elapsed_time / 1000;
    unsigned int minutes = total_seconds / 60;
    unsigned int seconds = total_seconds % 60;

    lcd.clear();

    // Afiseaza temperatura pe prima linie
    lcd.setCursor(0, 0);
    lcd.print("Temp: ");
    lcd.print(tmp);
    lcd.print(" C"); // Opțional: adaugă unitatea de măsură

    // Afiseaza timpul pe a doua linie
    lcd.setCursor(0, 1);
    lcd.print("Timp: ");
    if (minutes < 10) lcd.print("0");
    lcd.print(minutes);
    lcd.print(":");
    if (seconds < 10) lcd.print("0");
    lcd.print(seconds);

    delay(1000);
    Serial.println(tmp); // Vizualizare temperatura 
}
