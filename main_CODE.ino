#include <LiquidCrystal.h>

#define LM35_SENSOR_CHANNEL 0  // Setare pin senzor de temp A0
#define ADC_REF_VOLTAGE 5.0    // Tensiune de referinta 5V

// Setare pini LCD
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;

// Declarare lcd
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// Variabila pentru a tine evidenta timpului de start
unsigned long start_time;

// Pini pentru controlul intensitatii becului
#define PWM_PIN 9               // PWM output to H-bridge
#define BUTTON_INCREASE_PIN 6   // Buton pentru cresterea intensitatii
#define BUTTON_DECREASE_PIN 7   // Buton pentru scaderea intensitatii

int pwm_value = 0; // Valoarea initiala a intensitatii (0-255)

void init_adc() 
{
    // Seteaza referinta de tensiune a ADC la AVCC
    ADMUX |= (1 << REFS0);
    // Seteaza prescaler-ul ADC la 128
    ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
    // Activeaza ADC-ul
    ADCSRA |= (1 << ADEN);
}

float read_temperature() {
    ADMUX &= 0xF0; // Reseteaza pinii din ADMUX pentru citire
    ADMUX |= LM35_SENSOR_CHANNEL; // Seteaza pinul la care este legat senzorul de temperatura (A0)
    // Porneste conversia ADC
    ADCSRA |= (1 << ADSC);
    // Asteapta finalizarea conversiei
    while (ADCSRA & (1 << ADSC)) {}
    // Obtine rezultatul ADC si calculeaza temperatura
    uint16_t adc_value = ADC;
    // Transforma valoarea analogica in tensiune
    float voltage = (float) adc_value * ADC_REF_VOLTAGE / 1024.0;
    // Transforma tensiunea in grade Celsius pentru LM35
    float temperature = voltage * 100.0;
    return temperature;
}

void setup() 
{
    lcd.begin(16, 2);
    init_adc();
    Serial.begin(9600);
    start_time = millis(); // Inregistreaza timpul de start

    // Configurare pini pentru butoane si PWM
    pinMode(PWM_PIN, OUTPUT);
    pinMode(BUTTON_INCREASE_PIN, INPUT_PULLUP);
    pinMode(BUTTON_DECREASE_PIN, INPUT_PULLUP);
}

void loop()
{
    // Citire butoane si ajustare intensitate
    if (digitalRead(BUTTON_INCREASE_PIN) == LOW) {
        pwm_value += 5; // Creste intensitatea cu 5 unitati
        if (pwm_value > 255) pwm_value = 255; // Limiteaza la 255
        delay(200); // Debounce
    }

    if (digitalRead(BUTTON_DECREASE_PIN) == LOW) {
        pwm_value -= 5; // Scade intensitatea cu 5 unitati
        if (pwm_value < 0) pwm_value = 0; // Limiteaza la 0
        delay(200); // Debounce
    }

    // Aplica semnalul PWM la puntea H
    analogWrite(PWM_PIN, pwm_value);

    float temp_float = read_temperature(); // Citeste valoarea temperaturii
    int tmp = (int)(temp_float + 0.5); // Converteste temperatura la un numar intreg (rotunjit)

    // Calculeaza timpul scurs
    unsigned long elapsed_time = millis() - start_time;
    unsigned int total_seconds = elapsed_time / 1000;
    unsigned int minutes = total_seconds / 60;
    unsigned int seconds = total_seconds % 60;

    lcd.clear();

    // Afiseaza temperatura pe prima linie
    lcd.setCursor(0, 0);
    lcd.print("Temp: ");
    lcd.print(tmp);
    lcd.print(" C");

    // Afiseaza intensitatea becului ca procent
    lcd.setCursor(10, 0);
    lcd.print("Int:");
    lcd.print(map(pwm_value, 0, 255, 0, 100)); // Mapare la procentaj
    lcd.print("%");

    // Afiseaza timpul pe a doua linie
    lcd.setCursor(0, 1);
    lcd.print("Timp: ");
    if (minutes < 10) lcd.print("0");
    lcd.print(minutes);
    lcd.print(":");
    if (seconds < 10) lcd.print("0");
    lcd.print(seconds);

    Serial.println(tmp); // Vizualizare temperatura 
    delay(100); // Mic delay pentru stabilitate
}
