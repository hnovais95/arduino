/*
Programa: Controle de acesso com RFID
Autor: Heitor Novais
*/

#include <SPI.h>
#include <MFRC522.h>
#include <EEPROM.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include  "RTClib.h"

#define SS_PIN 10 //Define pino 10 para SDA do módulo RC522
#define RST_PIN 9 //Define pino 9 para RESET do módulo RC522

LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);

MFRC522 mfrc522(SS_PIN, RST_PIN); //Cria o objeto "mfrc522" para o módulo RC522
RTC_DS1307 rtc; //Cria o objeto "rtc" para o módulo RTC DS1307

/*Pinos*/
const int red = 6;
const int green = 5;
const int blue = 7;
const int buzzer = 3;
const int play = 2; //Pino responsável por dar "play" no aúdio gravado no módulo ISD1820
const int fechadura = 8;

String cartoesCadastrados = "";
String cartaoMaster = "F9 CE 78 89";
String userID = ""; //String que recebe código UID do cartão
String novoCartao = "";

unsigned long tempoAntigo = 0;
unsigned long tempoAntigoLED = 0;
unsigned long intervalo = 0;

bool estadoLED = false;

/*Declaração das funções*/
void relogio();
void carregarCartoesCadastrados(); //Carrega o banco de cartões cadastrados
void mensagemInicial(); //Imprime mensagem inicial
void leitura(); //Lê UID da tag
void autenticacao(); //Verifica permissão da tag e executa ação
void limparString(); //Limpa string que recebe UID para a próxima leitura
void liberarAcesso();
void negarAcesso();
void modoCadastro(); //Entra no modo cadastrar/remover cartão
void cadastrarNovoCartao();
void removerCartao();
void corRGB(); //Define cor do LED
void saudacao(); //Play em "Bem vindo a Automic"

void setup() 
{
  Serial.begin(9600);
  SPI.begin(); //Inicia barramento SPI
  mfrc522.PCD_Init(); //Inicia módulo RC522
  relogio();
  lcd.begin(16,2);
  lcd.setBacklight(HIGH);
  pinMode(play, OUTPUT);
  pinMode(red, OUTPUT);
  pinMode(green, OUTPUT);
  pinMode(blue, OUTPUT);
  pinMode(fechadura,  OUTPUT);
  digitalWrite(fechadura, !LOW);
  carregarCartoesCadastrados();
  mensagemInicial();
}

void loop() 
{   
  /*Verifica se há cartões presentes*/
  if (mfrc522.PICC_IsNewCardPresent()) 
  {
    /*Seleciona um dos cartões*/
    if (mfrc522.PICC_ReadCardSerial())
    {
      Serial.print("UID da tag :");
      leitura(); 
      autenticacao();
      mfrc522.PICC_HaltA(); //Termina leitura da tag atual
      mensagemInicial();
    }  
  }
}

/*############################# FIM DO PROGRAMA #############################*/

/*Escopo das funções utilizadas*/

void relogio()
{
  //rtc.begin();
  if (!rtc.begin()) 
  {
    lcd.print("RTC não encontrado");
    Serial.println("RTC não encontrado\n");
    while (1);
  }

  if (!rtc.isrunning()) 
  {
    lcd.print("RTC não está funcionando");
    Serial.println("RTC não está funcionando\n");
  }
  else
  
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); //Atualiza automaticamente com o relogio do computador
  //rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0)); //Para setar data/hora manualmente   
}

void carregarCartoesCadastrados()
{
  corRGB("laranja");
  
  lcd.clear();
  lcd.setCursor(3,0);
  lcd.print("AGUARDE...");
  Serial.println("Carregando cartoes cadastrados...\n");
  
  /*Percorre endereços da memória EEPROM*/
  for(int i = 0; i< 1024; i++)
  {
    int aux = EEPROM.read(i);
    
    if (aux != 0xFF) //Se endereço vazio...
    {
      String cartao = "";
      char auxChar;
      int espacoBranco = 0;
      
      /*Concatena caractéres percorridos com a string "cartao" até delimitador ";"*/
      do
      {
        auxChar = char(EEPROM.read(i));
        
        cartao = cartao + auxChar;
        
        /*Colocaa " " (espaço em branco) a cada 2 letras*/
        espacoBranco ++;
        if (espacoBranco >= 2){
          cartao = cartao + " ";
          espacoBranco = 0;
        }
       
        i++;
      } while(auxChar != ';');
      i--;
      
      cartoesCadastrados = cartoesCadastrados + cartao;     
    }
  }
        
  Serial.print("Cartões cadastrados: ");
  Serial.println(cartoesCadastrados);
  Serial.println("\n**********************************"); 
}

void mensagemInicial()
{

  DateTime now = rtc.now(); //Sincroniza relógio
  corRGB("azul");
  
  /*LDC*/
  lcd.clear();
  lcd.setCursor(4,0);
  lcd.print("AUTOMIC");
  lcd.setCursor(4,1);
  
  if (rtc.isrunning())
  {
    if (now.hour() < 10)
    {
      lcd.print("0");
      lcd.print(now.hour());
    }
    else
    lcd.print(now.hour());
    
    lcd.print(':');
    
    if (now.minute() < 10)
    {
      lcd.print("0");
      lcd.print(now.minute());
    }
    else
    lcd.print(now.minute());
    
    lcd.print(':');
    
    if (now.second() < 10)
    {
      lcd.print("0");
      lcd.print(now.second());
    }
    else
    lcd.print(now.second());
    
    /*Monitor Serial*/
    if (now.hour() < 10)
    {
      Serial.println();
      Serial.print("0");
      Serial.print(now.hour());
    }
    else
    {
      Serial.println();
      Serial.print(now.hour());
    }
    
    Serial.print(':');
    
    if (now.minute() < 10)
    {
      Serial.print("0");
      Serial.print(now.minute());
    }
    else
    Serial.print(now.minute());
    
    Serial.print(':');
    
    if (now.second() < 10)
    {
      Serial.print("0");
      Serial.print(now.second());
    }
    else
    Serial.print(now.second());
  }
  
  Serial.println("\nAproxime seu cartao do leitor...");
  Serial.println();
}

void leitura()
{
  corRGB("laranja");
  for (byte i = 0; i < mfrc522.uid.size; i++) 
  {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "); //Ex: Se byte em hexadecimal igual a "9" => "09".
    Serial.print(mfrc522.uid.uidByte[i], HEX);
    /*Concatena byte lido com a variável "carUID", formando UID da tag a cada iteração*/
    userID.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
    userID.concat(String(mfrc522.uid.uidByte[i], HEX));
    userID.toUpperCase(); //Modifica caracteres da variável "userID" para caracteres maiúsculos.
  }  
}

void autenticacao()
{
  corRGB("laranja");
  Serial.println("\n\nAutenticando...");
  delay(1000);
  Serial.print("\nMensagem: ");
  
  if(userID.substring(1) == cartaoMaster)
  {
    modoCadastro();      
  }
  else if (cartoesCadastrados.indexOf(userID.substring(1)) >= 0)
  {
    liberarAcesso();
  }
  else
  {
    negarAcesso();
  }

  delay(1000);  
  limparString();
}
  
void limparString()
{
  userID = "";
}

void liberarAcesso()
{
  corRGB("verde");
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("ACESSO LIBERADO");
  Serial.println("Acesso liberado!\n          Bem vindo Heitor!\n\n**********************************");
  digitalWrite(fechadura, !HIGH);
  delay(1000);
  digitalWrite(fechadura, !LOW);
  saudacao();
}

void negarAcesso()
{
  corRGB("vermelho");
  lcd.clear();
  lcd.setCursor(1,0);
  lcd.print("ACESSO NEGADO");
  Serial.println("Acesso negado!\n\n**********************************");
  tone(buzzer, 800);
  delay(650);
  noTone(buzzer);
}

void modoCadastro()
{  
  limparString();
  lcd.clear();
  lcd.setCursor(1,0);
  lcd.print("MODO CADASTRO");
  Serial.println("MODO CADASTRO\n\nAproxime o cartão a ser cadastrado/removido...");

  /*Espera por novo cartão durante 5s*/
  tempoAntigo = millis();
  while (intervalo < 5000)
  {
    intervalo = millis() - tempoAntigo;

    piscaLED();
    
    /*Verifica se há novos cartões presentes*/
    if (mfrc522.PICC_IsNewCardPresent()) 
    {
      /*Seleciona um dos cartões*/
      if (mfrc522.PICC_ReadCardSerial()) 
      {
        Serial.print("\nUID do novo cartão: ");
        leitura();
        novoCartao = userID.substring(1);
        novoCartao.toUpperCase();
        novoCartao.trim();
        novoCartao = novoCartao + " ;";
        
        if (userID.length() > 0 && novoCartao.indexOf(cartaoMaster) < 0) //Se userID lida for diferente do cartão Master...
        {
          if(cartoesCadastrados.indexOf(novoCartao) < 0)
          {
            cadastrarNovoCartao(novoCartao);
            break;
          } else
          {
            removerCartao(novoCartao);
            break;
          }
          
          mfrc522.PICC_HaltA(); //Termina leitura da tag atual          
        }
      }
    }
  } 
  intervalo = 0;
  Serial.println("\n**********************************");
}

void cadastrarNovoCartao(String cartao)
{
  cartoesCadastrados = cartoesCadastrados + cartao;
 
  cartao.replace(" ",""); //Remove os espacos em branco do UID lido
  
  /*Adiciona o novo cartão na memória*/
  for(int i = 0; i< 1024; i++)
  {
    int aux = EEPROM.read(i);    
    if (aux == 0xFF) //Se endereço vazio...
    {
      for(int j = 0; j< cartao.length(); j++)
      {
        EEPROM.write(i, byte(cartao.charAt(j)));
        i++;
      }
      break;
    }
  }

  lcd.clear();
  lcd.setCursor(3,0);
  lcd.print("CADASTRADO!");
  Serial.println("\n\nCartão cadastrado!\n");
  Serial.print("Cartoes atuais: ");
  Serial.println(cartoesCadastrados);
}

void removerCartao(String cartao)
{    
  cartoesCadastrados.replace(cartao, "");
  
  /*Remove cartão e reordena memória */
  String auxCartoesCadastrados = cartoesCadastrados;
  auxCartoesCadastrados.replace(" ","");
  for(int i = 0; i< 1024; i++)
  {
    if(i < auxCartoesCadastrados.length()){
      EEPROM.write(i, byte(auxCartoesCadastrados.charAt(i)));
    }
    else
    {
      EEPROM.write(i, 0xFF);
    }
  }
  
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("CARTAO REMOVIDO!");
  Serial.println("\n\nCartão removido!");
  Serial.print("\nCartoes atuais: ");
  Serial.println(cartoesCadastrados);
}

void corRGB(String cor)
{
  if (cor == "vermelho")
  {
    digitalWrite(red, !HIGH);
    digitalWrite(green, !LOW);
    digitalWrite(blue, !LOW);
  }

  if (cor == "verde")
  {
    digitalWrite(red, !LOW);
    digitalWrite(green, !HIGH);
    digitalWrite(blue, !LOW);
  }
  
  if (cor == "laranja")
  {
    analogWrite(red, 0);
    analogWrite(green, 90);
    analogWrite(blue, 255);
  }

  if (cor == "azul")
  {
    digitalWrite(red, !LOW);
    digitalWrite(green, !LOW);
    digitalWrite(blue, !HIGH);
  }
}

void piscaLED()
{
  if (millis() - tempoAntigoLED > 300)
  {
    tempoAntigoLED = millis();
    if (estadoLED == false)
    {
      estadoLED = true;
      corRGB("laranja");
    }
    else
    {
      estadoLED = false;
      digitalWrite(red, !LOW);
      digitalWrite(green, !LOW);
      digitalWrite(blue, !LOW);
    }
  }
}

void saudacao()
{
  digitalWrite(play, HIGH);
  digitalWrite(play, LOW);
}
