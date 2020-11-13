/*
 * Alunos: Alaf Santos / Lucas Chemelli / Matheus Boy
 * Disciplina: Sistemas de Telecomunicacoes
 * Codigo para Receptor VLC
 * line code that maybe I'll need (it is just about an error that I got): sudo chmod a+rw /dev/ttyACM0
 */

#include <LiquidCrystal_I2C.h>  //classe do lcd
#include <Wire.h> 

#define LDR A5
#define vectorSize 200
#define LIM 25 //200/8 - NUMERO DE CARACTERES NO DADO
#define WAIT 5//200Hz

LiquidCrystal_I2C lcd(0x27,2,1,0,4,5,6,7,3, POSITIVE);

void defineThreshold(int *samples, int *threshold);
void defineBitsArray(int *samples, int *threshold);

void printOutput(int *bitsArray);

int  searchAHeader(int *bitsArray);
int  searchAFooter(int *bitsArray);

void LCD_write(int linha, String text);
void LCD_Update(float value, int bitPar, int *bitsArray);

unsigned int numberOfOnes(int *bitsArray);

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
 /* int *samples;
  samples = (int*)malloc(sizeof(int)*vectorSize);
  for(int i = 0; i < vectorSize; i++)
  {
    samples[i] = analogRead(LDR);
    delay(WAIT);   
  }
  
  int *threshold = (int*)malloc(sizeof(int)*LIM);
  defineThreshold(samples, threshold);
  defineBitsArray(samples, threshold);
  printOutput(samples);
  free(samples);
  free(threshold);
*/}

void LCD_write(int linha, String text)
{
  int max_chars = 19;
  for(int i = 0; i!=max_chars && i!=text.length(); i++)
  {
    lcd.setCursor(i, linha);
    lcd.print(text[i]);
  }
}

void LCD_Update(float value, int bitPar, int *bitsArray)
{
    if(bitPar == !(numberOfOnes(bitsArray)%2))
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
  return 10;
}


void defineThreshold(int *samples, int *threshold)
{
  unsigned long int sum=0;
  for(int j = 0; j < LIM; j++)
  {
   for(int i = 0; i < 7; i++) sum+=samples[i+j];
   threshold[j] = sum/8;

   //Serial.print("Threshold: ");
  // Serial.println(threshold[j]);
  }
}

void defineBitsArray(int *samples, int *threshold)
{
  for(int j = 0; j < LIM; j++)
  {
    for(int i = 0; i < 7; i++)
    {
      if(samples[i+j] >= threshold[j]) samples[i+j] = 1;
      else samples[i+j] = 0;
      Serial.print("Bits: ");
      Serial.println(samples[i+j]);
      delay(500);
    } 
  }
}

void printOutput(int *bitsArray)
{
  for(int i = 0; i < vectorSize; i++)Serial.println(bitsArray[i]);
/*  int *output;
  output = (int*)malloc(sizeof(int)*LIM);
  
  int startOutput = searchAHeader(bitsArray);
  int stopOutput  = searchAFooter(bitsArray);

  if((startOutput > 7) && (stopOutput > 0))
  {
    for(int j = 0; j < LIM; j++)
    for(int i = startOutput; i < stopOutput; i++) 
      output[j+i] += pow(2, 7-i)*bitsArray[i];  
  }
  
  

  Serial.print("Valor Decimal Recebido: ");
  for(int j = 0; j < LIM; j+=8)
    Serial.print(output[j]);

  Serial.println(" "); //pula uma linha

  delay(500);
  free(output);*/
}


int searchAHeader(int *bitsArray) //00100011
{
  int HeaderPosition = 0;
  for(int i = 0; i < vectorSize; i++)
  {
    if(i < (vectorSize-24))
    {
        if(bitsArray[i] == "0")
        {
          if(bitsArray[i+1] == "0")
          {
            if(bitsArray[i+2] == "1")
            {
              if(bitsArray[i+3] == "0")
              {
                if(bitsArray[i+4] == "0")
                {
                  if(bitsArray[i+5] == "0")
                  {
                    if(bitsArray[i+6] == "1")
                    {
                      if(bitsArray[i+7] == "1")
                      {
                        HeaderPosition = i;
                        Serial.print("Header Encontrado -- ");
                        Serial.println(i);
                        delay(1000);
                      }
                    }
                    
                  }
                }
              }
            }
          }
        } 
    }
  }
  return (HeaderPosition + 7);
}

int searchAFooter(int *bitsArray) //00100100
{
  int FooterPosition = 0;
  for(int i = 0; i < vectorSize; i++)
  {
    if(i < (vectorSize-24))
    {
        if(bitsArray[i] == "0")
        {
          if(bitsArray[i+1] == "0")
          {
            if(bitsArray[i+2] == "1")
            {
              if(bitsArray[i+3] == "0")
              {
                if(bitsArray[i+4] == "0")
                {
                  if(bitsArray[i+5] == "1")
                  {
                    if(bitsArray[i+6] == "0")
                    {
                      if(bitsArray[i+7] == "0")
                      {
                        FooterPosition = i;
                        Serial.print("Footer Encontrado -- ");
                        Serial.println(i);
                        delay(1000);
                      }
                    }
                    
                  }
                }
              }
            }
          }
        } 
    }
  }
  return (FooterPosition);
}
