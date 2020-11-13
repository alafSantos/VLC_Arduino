#include <Wire.h> //INCLUSÃO DE BIBLIOTECA
#include <LiquidCrystal_I2C.h> //INCLUSÃO DE BIBLIOTECA

LiquidCrystal_I2C lcd(0x27,2,1,0,4,5,6,7,3, POSITIVE); //ENDEREÇO DO I2C E DEMAIS INFORMAÇÕES
 
void setup(){
 lcd.begin (16,2); //SETA A QUANTIDADE DE COLUNAS(16) E O NÚMERO DE LINHAS(2) DO DISPLAY
 lcd.setBacklight(HIGH); //LIGA O BACKLIGHT (LUZ DE FUNDO)
 lcd.setCursor(0, 0); // Go to home of 2nd line
 lcd.print("Inicializando");
 lcd.setCursor(0,1);
 lcd.print("Receptor...");
 delay(2000);
 lcd.clear();
}
 
void loop(){
  lcd.setCursor(0,1); 
  lcd.print("ERRO DE LEITURA"); 
  lcd.setCursor(0,0); 
  delay(1000); 
  lcd.print("BIT DE PARIDADE");
  delay(500);
  lcd.clear();
}
