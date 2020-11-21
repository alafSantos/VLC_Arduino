/*
 * Codigo para Receptor VLC
 * line code that maybe I'll need (it is just about an error that I got): sudo chmod a+rw /dev/ttyACM0
 */

#include <LiquidCrystal_I2C.h>  //classe do lcd
#include <Wire.h> 

#define LDR A5
#define vectorSize 82 //usaremos somente 41 no melhor caso
#define pktSize 41 // 4 bits de start, 32 do float i3e754, 1 paridade e 4 de stop
#define WAIT 5//200Hz

float N0 = 0, N1 = 0, N2 = 0;

LiquidCrystal_I2C lcd(0x27,2,1,0,4,5,6,7,3, POSITIVE);

void defineBitsArray(int *samples, int *bitsArray);

int  searchStart(int *samples);
int  searchStop(int *samples);

void LCD_write(int linha, String text);
void LCD_Update(float value, int *bitsArray);

unsigned int numberOfOnes(int *bitsArray);
float floatI3E754(int *bitsArray);

void setup()
{
  Serial.begin(115200);
  pinMode(LDR, INPUT);

  lcd.begin(16, 2);                 // 20x4 LCD module
  lcd.setBacklight(HIGH);

  lcd.setCursor(0, 0); // Go to home of 2nd line
  lcd.print("Inicializando Receptor...");
  delay(1000);
}

void loop()
{
  
  int *samples, *bitsArray;
  samples = (int*)malloc(sizeof(int)*vectorSize);
  bitsArray = (int*)malloc(sizeof(int)*pktSize);
  for(int i = 0; i < vectorSize; i++)
  {
    samples[i] = analogRead(LDR);
    delay(WAIT/2);   
  }
  
  defineBitsArray(samples, bitsArray);

  float sensorValue = floatI3E754(bitsArray);

  LCD_Update(sensorValue, bitsArray);

  free(samples);
  free(bitsArray);
}

void LCD_write(int linha, String text)
{
  int max_chars = 19;
  for(int i = 0; i!=max_chars && i!=text.length(); i++)
  {
    lcd.setCursor(i, linha);
    lcd.print(text[i]);
  }
}

void LCD_Update(float value, int *bitsArray)
{
    if(bitsArray[36] == !(numberOfOnes(bitsArray)%2))
    {
      lcd.setCursor(0,1); 
      lcd.print("ERRO DE LEITURA"); 
      lcd.setCursor(0,0); 
      delay(1000); 
      lcd.print("BIT DE PARIDADE");
      delay(500);
      lcd.clear();
    }
    else
    {
      lcd.clear();
      String msg = "Sensor: " + String(value);
      LCD_write(0, msg);
    }
}

unsigned int numberOfOnes(int *bitsArray)
{
  int counter = 0;
  for(int i = 4; i <= 35; i++)
    if(bitsArray[i] == 1)
      counter++;
  return counter;
}


//implementando
void defineBitsArray(int *samples, int *bitsArray)
{

  int startP = searchStart(samples);
  int stopP = searchStop(samples);

  
  
  for(int i = 0; i < pktSize; i++)
  {
    if(samples[i] == N2 && samples[i+1] == N0) bitsArray[i] = 1;
    if(samples[i] == N0 && samples[i+1] == N2) bitsArray[i] = 0;
  }


  /*
    A ideia é desfazer o manchester com a cara do pacote que montamos no transmissor
    primeiro ele tem de cair por 2 waits em N0 e no fim tem de se manter em N1 (no bit)
    as variações de 1 e 0 sao definidas como N2/N0 e N0/N2 respectivamente. Para tal temos de
    definir N0, N1 e N2 e trabalhar com uma margem de erro de pelo menos X% (X experimental).
  */
}



//implementar
int searchStart(int *samples)
{
  int startPosition = 0;
  return startPosition;
}

//implementar
int searchStop(int *samples)
{
  int stopPosition = 0;
  return stopPosition;
}

//implementar
float floatI3E754(int *bitsArray)
{
  float value;
  return value;
}
