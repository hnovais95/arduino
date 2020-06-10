/**
 * Estação Meteorológica
 * Autor: Heitor Novais
 */

/**
 * DEFINIÇÃO DE PINOS
 */
#define PIN_PLUVIOMETRO 2
#define PIN_ANEMOMETRO 3

/**
 * CONSTANTES
 */
const float MILIMETROS_PULSO = 0.25; // mm/pulso do pluviômetro
const int RAIO = 147; // Raio do anemômetro (mm)
const int PERIODO_PLUVIOMETRO = 2000;
const int PERIODO_ANEMOMETRO = 5000;

/**
 * VARIÁVEIS
 */
volatile unsigned int contPluviometro = 0;
volatile unsigned int contAnemometro = 0;
unsigned long tsUltimaMedicaoPluviometro = 0; // Timestamp
unsigned long tsUltimaMedicaoAnemometro = 0; // Timestamp

/**
 * PROTÓTIPOS
 */
void calculaVolumeChuva();
void calculaVelocidadeVento();

/**
 * INTERRUPÇÕES
 */
void pulsoPluviometro() {
  contPluviometro++;
}

void pulsoAnemometro() {
  contAnemometro++;
}

/**
 * PROGRAMA
 */
void setup() {
  Serial.begin(9600);
  Serial.println("Iniciando...\n");
  
  pinMode(PIN_PLUVIOMETRO, INPUT_PULLUP);
  pinMode(PIN_ANEMOMETRO, INPUT_PULLUP);
  
  attachInterrupt(digitalPinToInterrupt(PIN_PLUVIOMETRO), pulsoPluviometro, FALLING);
  attachInterrupt(digitalPinToInterrupt(PIN_ANEMOMETRO), pulsoAnemometro, FALLING);
}

void loop() {
  calculaVolumeChuva();
  calculaVelocidadeVento();
}

/*
 * FUNÇÕES
 */
void calculaVolumeChuva() {
  if (millis() - tsUltimaMedicaoPluviometro >= PERIODO_PLUVIOMETRO) {
    float volumeChuva = contPluviometro * MILIMETROS_PULSO;
    
    tsUltimaMedicaoPluviometro = millis();
    
    Serial.print("Volume de chuva: ");
    Serial.print(volumeChuva);
    Serial.println("mm/m2");
  }
}

void calculaVelocidadeVento() {
  if (millis() - tsUltimaMedicaoAnemometro >= PERIODO_ANEMOMETRO) {
    // Desabilita interrupção durante o cálculo
    detachInterrupt(PIN_ANEMOMETRO);
    
    int rpm = ((contAnemometro * 60) / ((millis() - tsUltimaMedicaoAnemometro) / 1000));
    float velocidadeKmH = ((4 * 3.14 * RAIO * rpm)/60) / 1000;
    float velocidadeMS = velocidadeKmH * 3.6;
    
    tsUltimaMedicaoAnemometro = millis();
    contAnemometro = 0;
    
    // Mostra o valor de RPM no serial monitor
    Serial.print("Velocidade do vento: ");
    Serial.print(rpm, DEC);
    Serial.print("rpm  ");
    Serial.print(velocidadeKmH);
    Serial.print("km/h  ");
    Serial.print(velocidadeMS);
    Serial.println("m/s");
    
    //Habilita interrupção
    attachInterrupt(digitalPinToInterrupt(PIN_ANEMOMETRO), pulsoAnemometro, FALLING);
  }
}
