/*
 * VLC Receiver Based on OOk modulation and Manchester Code
 * line code that maybe I'll need (it is just about an error that I got): sudo chmod a+rw /dev/ttyACM0
 */

/*
	ERROR TABLE
	1 - Stop was not found
	2 - parity error
	3 - Others

*/

#include <LiquidCrystal_I2C.h>  //LCD Class
#include <Wire.h>               //to use I2C protocol

#define LDR A5                  //our input is based on this light sensor
#define pktSize 33              //this is the length of the bits vector (with pk754 + parity)
#define WAIT 100                //timing value, it needs to be the same in the transmitter

#define Margin 0.002			//acceptable margin of error in variations of values


const int vectorSize = 2*pktSize; //this is the length of the light values vector - it must be at least >= 2*pktSize
unsigned int ERRO = 0;

/* Manchester levels */
float N0 = 0, N1 = 0, N2 = 0;

/* To use the display */
LiquidCrystal_I2C lcd(0x27,2,1,0,4,5,6,7,3, POSITIVE);

/* Functions and methods that we'll use later */

void defineSamplesArray(int *samples);
void defineBitsArray(int *samples, int *bitsArray);

void LCD_write(int linha, String text);
void LCD_Update(float value, int *bitsArray);

void checkParity(int *bitsArray);
float floatI3E754(int *bitsArray);

/*system boot*/
void setup()
{
  Serial.begin(115200);
  pinMode(LDR, INPUT);

  lcd.begin(16, 2);                 // 16x2 LCD module
  lcd.setBacklight(HIGH);

  lcd.setCursor(0, 0); // Go to home of 2nd line
  lcd.print("System Starting...");
  delay(1000);
}

/*main function - loop*/
void loop()
{
  
  int *samples, *bitsArray;
  float sensorValue;
  samples = (int*)malloc(sizeof(int)*vectorSize); //to light values
  bitsArray = (int*)malloc(sizeof(int)*pktSize);  //to bits (1s and 0s)
 
  defineSamplesArray(samples);
  defineBitsArray(samples, bitsArray);
  
  if(ERRO) sensorValue = -127.00;
  else
  {
  	 checkParity(bitsArray);
  	 sensorValue = floatI3E754(bitsArray);
  }
  
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
    if(!ERRO)
    {
      lcd.clear();
      String msg = "Sensor: " + String(value);
      LCD_write(0, msg); 
    }
    else
    {    
      switch(ERRO)
      {
      	case 1:
      		  lcd.setCursor(0,1); 
			  lcd.print("ERROR"); 
			  lcd.setCursor(0,0); 
			  delay(1000); 
			  lcd.print("Stop was not found");
			  delay(500);
			  lcd.clear();	
			  break;      	
      	case 2:
      		  lcd.setCursor(0,1); 
			  lcd.print("ERROR"); 
			  lcd.setCursor(0,0); 
			  delay(1000); 
			  lcd.print("Incompatible parity");
			  delay(500);
			  lcd.clear();
			  break;	      	
      	default:
      		  lcd.setCursor(0,1); 
			  lcd.print("ERROR"); 
			  lcd.setCursor(0,0); 
			  delay(1000); 
			  lcd.print("Restart the system");
			  delay(500);
			  lcd.clear();	
      
      }	
    }
}

void checkParity(int *bitsArray)
{
  int counter = 0;
  for(int i = 4; i <= 34; i++)
    if(bitsArray[i] == 1)
      counter++;
  
  if(bitsArray[pktSize-1] == !(counter%2))	ERRO = 0;
  else ERRO = 2;
}

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
   		
   		//looking 4 2 the start
	  	if(samples[i-1] < samples[i])
	  	{
	  		N1 = samples[i-1];
	  		N2 = samples[i];
	  		
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

//implementar
void defineBitsArray(int *samples, int *bitsArray)
{
  
 /* for(int i = 0; i < pktSize; i++)
  {
    if(samples[i] == N2 && samples[i+1] == N0) bitsArray[i] = 1;
    if(samples[i] == N0 && samples[i+1] == N2) bitsArray[i] = 0;
  }

 
    A ideia é desfazer o manchester com a cara do pacote que montamos no transmissor
    primeiro ele tem de cair por 2 waits em N0 e no fim tem de se manter em N1 (no bit)
    as variações de 1 e 0 sao definidas como N2/N0 e N0/N2 respectivamente. Para tal temos de
    definir N0, N1 e N2 e trabalhar com uma margem de erro de pelo menos X% (X experimental).
  */
}


//implementar
float floatI3E754(int *bitsArray)
{
  float value;
  return value;
}
