/*
 * VLC Receiver Based on OOk modulation and Manchester Code
 * line code that maybe I'll need (it is just about an error that I got): sudo chmod a+rw /dev/ttyACM0
 */

/*
	ERROR TABLE
	1 - Start was not found
  2 - parity error
	3 - Stop was not fount
	4 - Others

*/

#include <LiquidCrystal_I2C.h>  //LCD Class
#include <Wire.h>               //to use I2C protocol
#define LDR A0                  //our input is based on this light sensor
#define pktSize 33              //this is the length of the bits vector (with pk754 + parity)
#define WAIT 100                //timing value, it needs to be the same in the transmitter

#define Margin 0.0295      			//acceptable margin of error in variations of values


const int vectorSize = 2*pktSize; //this is the length of the light values vector - it must be at least >= 2*pktSize
unsigned int ERRO = 0;

/* Manchester levels */
float N0 = 0, N1 = 0, N2 = 0, Ni = 0; //Ni for unknown values

/* To use the display */
LiquidCrystal_I2C lcd(0x27,2,1,0,4,5,6,7,3, POSITIVE);

/* Functions and methods that we'll use later */

void defineSamplesArray(int *samples);
void defineBitsArray(int *samples, int *bitsArray);

void LCD_write(int linha, String text);
void LCD_Update(float value, int *bitsArray);

void checkParity(int *bitsArray);
float floatI3E754(int *bitsArray);

void printDebugWave(int N);

void (*resetFunc)(void) = 0; //declare reset function @ address 0

unsigned long timeCounter = 0, timeError = 0;

/*system boot*/
void setup()
{
  Serial.begin(115200);
  pinMode(LDR, INPUT);

  lcd.begin(16, 2);                 // 16x2 LCD module
  lcd.setBacklight(HIGH);

  lcd.setCursor(0, 0); // Go to home of 2nd line
  lcd.print("Starting...");
  delay(500);
}

/*main function - loop*/
void loop()
{

//-----------------------------------------------------------------------------------
 /*  int tam = 100;
   int luz[tam];
   for(int i = 0; i < tam; i++)
   {
    luz[i] = analogRead(LDR);
    delay(WAIT/2);
   }
   
    for (int i = 0; i < tam - 1; i++)
    {
        for (int j = i; j < tam - 1; j++)
        {
            if (luz[i] < luz[j])
            {
                int temp = luz[i];
                luz[i] = luz[j];
                luz[j] = temp;
            }
        }
    }

    int mediaMaior = 0, mediaMenor = 0, mediaMeio = 0;
    for(int i = 0; i < tam; i++)
    {
      if(i<(tam/3)) mediaMeio += luz[i];
      else if(i > (2*tam/3)) mediaMenor += luz[i];
      else mediaMaior += luz[i];
    }

    mediaMenor = mediaMenor/(tam/3);
    mediaMeio = mediaMeio/(tam/3);
    mediaMaior = mediaMaior/(tam/3);

    Serial.println(String(mediaMenor) + " < " + String(mediaMeio) + " < " + String(mediaMaior)); 
   */
   //printDebugWave(analogRead(LDR));
   
  // delay(WAIT/2);

//----------------------------------------------------------------------------------
  //reserving space for bit and light vectors
  int *samples, *bitsArray;
  float sensorValue = -127.00;
  samples = (int*)malloc(sizeof(int)*vectorSize); //to light values
  bitsArray = (int*)malloc(sizeof(int)*pktSize);  //to bits (1s and 0s)
//--------------------------------------------------------------------------------
 //calling the functions that will deal with the light samples
 // defineSamplesArray(samples);
//--------------------------------------------------------------------------------

  /*definir samples na mão para fins de testes*/
  //defini que 900 será equivalente a N0 e 1023 equivalente a N2

  samples[0] = 900;
  samples[1] = 1023;
  samples[2] = 1023;
  samples[3] = 900;
  for(int i = 4; i < vectorSize; i+=4)
  {
    for(int j = 0; j < 4; j++)
    {
      samples[i+j] = samples[j];
    }
  }

  // paridade
  samples[vectorSize-2] = 900;
  samples[vectorSize-1] = 1023;
  
  //segue o algoritmo normalmente agora
  
//--------------------------------------------------------------------------------
  defineBitsArray(samples, bitsArray);
//--------------------------------------------------------------------------------
  if(ERRO) sensorValue = -127.00;
  else
  {
  	 checkParity(bitsArray);
  	 sensorValue = floatI3E754(bitsArray);
  }
//--------------------------------------------------------------------------------
  LCD_Update(sensorValue, bitsArray);
//--------------------------------------------------------------------------------
  free(samples);
  free(bitsArray);
//-------------------------------------------------------------------------------- 
  if(timeCounter > 2147483648)resetFunc();

}

/*Tratar ruidos e tudo mais ainda - lembrar de só sair com as 66 posicoes de luz referentes a 
  pkt754 + parity */
void defineSamplesArray(int *samples)
{
  samples[0] = analogRead(LDR); //the first position will be filled
  
  unsigned int counter = 0;
  
  for(int i = 1; i < vectorSize; i++)
  {
    if(counter == 3) //if counter is equal to 3, we have already passed the start
    {
    	for(int k = 0; k < vectorSize; k++) //picking up values of light respectives to Manchester levels
    	{
  			samples[k] = analogRead(LDR);
  		  delay(WAIT/2);	
    	}
    	//here, we'll verify if stop timing was respected
    	for(int w = 0; w < 4; w++)
    	{
			if(analogRead(LDR) == N1) //tratar margem de erro depois
			{
				counter++;
				delay(WAIT);				
			}
			else
			{
				counter = 0;
				break;
			}
		}
		
		if(counter < 4) ERRO = 1;
    	
    	break; //it interrupts the loop based on i counter
    }
    else
    {
    	samples[i] = analogRead(LDR);
   		
   		//looking for the start
	  	if(samples[i-1] < samples[i])
	  	{
	  		N1 = samples[i-1];
	  		N2 = samples[i];

        N0 = N1 - (N2 - N1);
	  		
	  		for(int j = 0; j < 3; j++) //4T for start, but theoretically the first one was in the i position 
	  		{
	  			if(analogRead(LDR) == samples[i])
	  			{
	  				counter++;   //we need the value N2 repeted 4 times (i, i+1, i+2, i+3)
	  				delay(WAIT); //clk time
	  			}
	  			else
	  			{
	  				counter = 0;
	  				break; //stop the loop and go back to the searching of start
	   			}
	   		}
	   		
	  	} 
    }
  }
}

/*OK*/
void defineBitsArray(int *samples, int *bitsArray)
{
  int i = 1, j = 0;
  while(i < vectorSize)
  {
    if(samples[i] < samples[i-1]) bitsArray[j] = 0;
    else bitsArray[j] = 1; 
    i = 2 + i;
    j++;
  }
}

/*OK*/
void checkParity(int *bitsArray)
{
  int counter = 0;
  for(int i = 0; i < (pktSize-1); i++)
    if(bitsArray[i] == 1)
      counter++;
  if(bitsArray[pktSize-1] == !(counter%2))  ERRO = 0;
  else ERRO = 2;
}

//implementar
float floatI3E754(int *bitsArray)
{
  float value = 1.1;
  return value;
}




/*OK*/
void LCD_write(int linha, String text)
{
  int max_chars = 19;
  for(int i = 0; i!=max_chars && i!=text.length(); i++)
  {
    lcd.setCursor(i, linha);
    lcd.print(text[i]);
  }
}

/*OK*/
void LCD_Update(float value, int *bitsArray)
{
  if((millis() - timeError) > 1000)
  {
    lcd.clear();
    if(!ERRO)
    {
      String msg = "Sensor: " + String(value);
      LCD_write(0, msg); 
    }
    else
    {   
      switch(ERRO)
      {
        case 1:
            lcd.setCursor(0,0); 
            lcd.print("ERROR"); 
            lcd.setCursor(0,1);           
            lcd.print("Start");
            break;        
        case 2:
            lcd.setCursor(0,0); 
            lcd.print("ERROR"); 
            lcd.setCursor(0,1); 
            lcd.print("Parity");
            if((millis() - timeError) >= 100)
            break;
        case 3:
            lcd.setCursor(0,0); 
            lcd.print("ERROR"); 
            lcd.setCursor(0,1); 
            lcd.print("Stop");
            break;       
        default:
            lcd.setCursor(0,0); 
            lcd.print("ERROR"); 
            lcd.setCursor(0,1);
            lcd.print("Restart, pls");           
      } 
    }
    timeError = millis();
  }
}

/*OK*/
/*It will be useful to draw a wave from the signal input*/
void printDebugWave(int N)
{
  Serial.print(N);
  Serial.print(" ");
  Serial.println((millis() - timeCounter)/1000);
  timeCounter = millis();
}
