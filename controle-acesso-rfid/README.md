# Controle de acesso com RFID
Controle de acesso implementado em Arduino Uno.

Por meio do módulo MRFC522 é possível ler tags nfc e cartões que seguem o padrão Mifare.

O sistema identifica a tag aproximada e verifica na memória EEPROM do Arduino Uno se tag está cadastra. Se sim, relé é acionado liberando o acesso, LED RGB fica verde e uma voz gravada emite a mensagem "Bem-vindo!", por meio do módulo gravador de voz. Caso contrário LED RGB fica vermelho e buzzer sinaliza o acesso negado. Todos os status são aprensetados na tela do LCD, que fica mostrando a hora atual em stand-by.

## Materiais utilizados
* 1x Arduino Uno
* 1x Módulo RFID MRFC522
* 1x Tag Mifare 13.56mhz
* 1x Display LCD 16x2
* 1x Módulo Gravador de Voz e Player ISD1820
* 1x Módulo RTC DS1307
* 1x LED RGB
* 3x Resistores de 330k ohms
* 1x Buzzer 5V
* 1x Relé 5V
