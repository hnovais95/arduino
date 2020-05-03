/*
  Controle das luminárias do LAP por meio internet utilizando biblioteca MySQL Connector, idealizada por Dr. Charles Bell.

  Feito por Heitor Novais.
*/

#include "Ethernet.h"
#include <MySQL_Connection.h>
#include <MySQL_Cursor.h>

#define quente1 3   // Definir PIN 3 para variável "quente" da luminária 1
#define frio1   5   // Definir PIN 5 para variável "frio" da luminária 1
#define quente2 6   // Definir PIN 6 para variável "quente" da luminária 2
#define frio2   9   // Definir PIN 9 para variável "frio" da luminária 2
#define quente3 10  // Definir PIN 10 para variável "quente" da luminária 3
#define frio3   11  // Definir PIN 11 para variável "frio" da luminária 3

/*Configuração do Ethernet Shield*/
byte mac_addr[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192, 168, 0, 101);       // Define o endereço IP
IPAddress gateway(192,168,0,1);       // Define o gateway
IPAddress subnet(255, 255, 255, 0);   // Define a máscara de rede

/*Configuração do banco de dados*/
IPAddress server_addr(192, 168, 0, 6); // IP do MySQL server
char user[] = "heitor";                // MySQL user login username
char password[] = "segredo";           // MySQL user login password

/*Configuração do MySQL Connector/Arduino*/
EthernetClient client;
MySQL_Connection conn((Client *)&client);
MySQL_Cursor cur = MySQL_Cursor(&conn);  // Cria uma instância do ponteiro passando na conexão

/*Comando SQL para consultar valor de "pot" na tabela "luminarias" do banco de dados*/
const char consulta1_1[] = "SELECT status FROM lap.luminarias WHERE id = 1";
const char consulta1_2[] = "SELECT pot FROM lap.luminarias WHERE id = 1";
const char consulta2_1[] = "SELECT status FROM lap.luminarias WHERE id = 2";
const char consulta2_2[] = "SELECT pot FROM lap.luminarias WHERE id = 2";
const char consulta3_1[] = "SELECT status FROM lap.luminarias WHERE id = 3";
const char consulta3_2[] = "SELECT pot FROM lap.luminarias WHERE id = 3";

/*Consulta banco de dados*/
void consultarMYSQL(byte &x, const char *consulta) {      
      row_values *row = NULL;
      MySQL_Cursor *cur_mem = new MySQL_Cursor(&conn); // Inicializando a instância da classe de consulta
      cur_mem->execute(consulta); // Executando consulta
      column_names *columns = cur_mem->get_columns(); // Buscando as colunas requeridas da tabela do BD
      
      /*Varrendo linhas da tabela do banco de dados*/
      do {
          row = cur_mem->get_next_row();
          if (row != NULL) {
          x = atol(row->values[0]);
          }   
      } while (row != NULL);
      delete cur_mem; // Deletando ponteiro dinamicamento alocado para liberar memória
}

/*Parâmetros do código de reinício*/
int num_fails;                  // variable for number of failure attempts
#define MAX_FAILED_CONNECTS 5   // maximum number of failed connects to MySQL

/*Função Reset*/
void soft_reset() {
  asm volatile("jmp 0");
}

void setup() {
    Serial.begin(115200);
    while(!Serial); // Aguardando conexão serial
    delay(1000);

    /*Inicializando interface de rede (Ethernet Shield)*/
    Ethernet.begin(mac_addr, ip, gateway, subnet); 
    
    /*Teste de conexão com banco de dados*/
    Serial.println("Conectando...");
    if (conn.connect(server_addr, 3306, user, password)) {
      Serial.println("Sucesso!");
    } else {
        Serial.println("Conexão falhou.");
      }
      
    pinMode(quente1, OUTPUT);
    pinMode(frio1, OUTPUT);
    pinMode(quente2, OUTPUT);
    pinMode(frio3, OUTPUT);
    pinMode(quente3, OUTPUT);
    pinMode(frio3, OUTPUT);
}

void loop() {
    /*Verificando conexão*/
    delay(1000);
    if (conn.connected()) { 
      byte estado1, estado2, estado3;
      byte pot1, pot2, pot3;
      String e1,e2,e3;
      delay(1000);

      /*Executando consulta dos valores de estado e potenciômetro das lumiráris 1,2 e 3*/
      consultarMYSQL(estado1, consulta1_1);
      consultarMYSQL(estado2, consulta2_1);
      consultarMYSQL(estado3, consulta3_1);
      consultarMYSQL(pot1, consulta1_2);
      consultarMYSQL(pot2, consulta2_2);
      consultarMYSQL(pot3, consulta3_2);

      /*Variáveis de controle*/
      byte f1 = 255 - pot1;
      byte q1 = 255 - f1;
      byte f2 = 255 - pot2;
      byte q2 = 255 - f2;
      byte f3 = 255 - pot3;
      byte q3 = 255 - f3;

      /*Verificando estado das variávies. Ligado/Desligado*/
      if (estado1 == 1){
        e1 = "Ligado";
      } else {
          e1 = "Deligado";
          q1 = 0;
          f1 = 0;
        }

      if (estado2 == 1){
        e2 = "Ligado";
      } else {
          e2 = "Desligado";
          q2 = 0;
          f2 = 0;
        }
      
      if (estado3 == 1){
        e3 = "Ligado";
      } else {
          e3 = "Deligado";
          q3 = 0;
          f3 = 0;
        }
      
      analogWrite(quente1,q1);
      analogWrite(frio1,f1);
      analogWrite(quente2,q2);
      analogWrite(frio2,f2);
      analogWrite(quente3,q3);
      analogWrite(frio3,f3);
      
      /*Imprimindo status das luminárias no Monitor Serial*/
      Serial.print("\n\nID: ");
      Serial.print(1);
      Serial.print("\nEstado: ");
      Serial.print(e1);            
      if (estado1 == 1){
        Serial.print("\nPot: ");
        Serial.print(pot1);
        Serial.print("\nFrio: ");
        Serial.print(f1);
        Serial.print("\nQuente: ");
        Serial.print(q1);
      }
      
      Serial.print("\n\nID: ");
      Serial.print(2);
      Serial.print("\nEstado: ");
      Serial.print(e2);            
      if (estado2 == 1){
        Serial.print("\nPot: ");
        Serial.print(pot2);
        Serial.print("\nFrio: ");
        Serial.print(f2);
        Serial.print("\nQuente: ");
        Serial.print(q2);
      }
      
      Serial.print("\n\nID: ");
      Serial.print(3);
      Serial.print("\nEstado: ");
      Serial.print(e3);            
      if (estado3 == 1){
        Serial.print("\nPot: ");
        Serial.print(pot3);
        Serial.print("\nFrio: ");
        Serial.print(f3);
        Serial.print("\nQuente: ");
        Serial.print(q3);
        Serial.print("\n\n****************");
      } else
        Serial.print("\n\n****************");
      
      num_fails = 0;
  
  /*Restabelecendo conexão em caso de falha*/
  } else {
    Serial.println("Reconectando...");
    if (conn.connect(server_addr, 3306, user, password)) {
      delay(500);
      Serial.println("Reconectado com sucesso!");
    } else {
      num_fails++;
      Serial.println("Reconexão falhou!");
      if (num_fails == MAX_FAILED_CONNECTS) {
        Serial.println("Ok, não foi possível reconectar. Reiniciando Arduino...");
        delay(2000);
        soft_reset();
      }
      }
    }
}
