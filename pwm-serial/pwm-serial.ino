/*TÓPICOS ESPECIAIS - CONTROLE DISCRETO*/

#include "TimerOne.h"

#define PIN_u 10
#define PIN_x1 A1
#define PIN_x2 A0
#define runEvery(t) for (static uint16_t _lasttime; (uint16_t)((uint16_t)millis() - _lasttime) >= (t); _lasttime += (t))

/*Variáveis*/
int X1, X2, U, Ts=5;
float x1, x2, u, r;

/*Declaração de funções*/
String leituraSerial(); //Executa leitura do buffer serial
float recebeSerial(); //Armazena referencia passada no buffer serial
float StrToFloat(); //Converte variável String em Float

void setup()
{
  Serial.begin(115200);
  while (!Serial); //Aguarda comunicação serial
  pinMode(PIN_u, OUTPUT);
  pinMode(PIN_x1, INPUT);
  pinMode(PIN_x2, INPUT);

  Timer1.initialize(1000/25);
  Timer1.pwm(PIN_u, 0);
}

void loop() {
  runEvery(50)
  {
    r = recebeSerial();
    Serial.println(r);
  }
  
  runEvery(Ts)
  {  
    X1 = analogRead(A1);
    x1 = (X1*5/1023) - 2.5;
    X2 = analogRead(A0);
    x2 = (X2*5/1023) - 2.5;
    u = -0.1094*x1 -05727*x2 + 0.9896*r;
    U = int((u+2.5)*1024/5);
    Timer1.setPwmDuty(PIN_u, U);
    
    /*Serial.print(u);
    Serial.print(",");
    Serial.print(x1);
    Serial.print(",");
    Serial.println(x2);*/
  }
}

String leituraSerial()
{
  String codigo = "";
  char caractere;
  
  while(Serial.available() > 0)
  {
    caractere = Serial.read();
    if (caractere != '\n')
      codigo.concat(caractere);
      
    delay(10);
  }

  Serial.print("Código recebido: ");
  Serial.println(codigo);
  return codigo;
}

float recebeSerial()
{
  float ref;
  
  if (Serial.available() > 0)
  {
    String leitura = leituraSerial();
    leitura.toUpperCase();

    if (leitura.substring(0,1) == "r") // prefixo da referência a ser passada no buffer do Serial Monitor
    {  
      if (leitura.length() > 1) //Verifica se há caracteres após prefixo digitado no buffer do Serial Monitor
      {
        ref = StrToFloat(leitura.substring(1)); //Recebe referencia desejada
      
        /*Define limites para referência devido a saturação do sistema*/
        if (ref > 2.5)
          ref = 2.5;
        
        if (ref < -2.5)
          ref=-2.5;        
       }
     }
   }
  return ref;
}

float StrToFloat(String str)
{
  char carray[str.length() + 1]; //Array de char
  str.toCharArray(carray, sizeof(carray)); //Copia conteúdo da string para o array de char
  return atof(carray); //Converte array de char em inteiro
}
