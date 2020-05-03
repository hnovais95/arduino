# Controle de Iluminação
Este projeto é um sistema que controla a temperatura de cor de três luminárias.

## Luminárias
As luminárias são constituídas de dois segmentos de fitas de LED, um de cor quente e o outro de cor fria. São envolvias por uma plástico difusor para homogeneizar a luz e uma haste que fixa-se no chão.

## Controle
O controle é realizado pelos pinos PWM do Arduino Uno que acionam os transitores das luminárias. Cada luminária tem dois transistores, um para o segmento de fita de LED de cor fria e outra para o segmento de cor quente.

## Interface com o Usuário
A interface é feita por uma página WEB. O usuário seleciona a luminária que deseja controlar e a temperara de cor. Os parâmetros são salvos em um banco MySQL e consultados diretamente pelo Arduino Uno.

![alt text](https://github.com/hnovais95/arduino/blob/master/controle-iluminacao/P%C3%A1gina%20WEB/screenshot.png)

Projeto concluído em 09/2018.