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
#define WAIT 80                //timing value, it needs to be the same in the transmitter

#define Margin 0.0295      			//acceptable margin of error in variations of values


const int vectorSize = 2*pktSize; //this is the length of the light values vector - it must be at least >= 2*pktSize
unsigned int ERRO = 0;
bool flag = LOW;

/* Manchester levels */
float N0 = 0, N1 = 0, N2 = 0, Ni = 0; //Ni for unknown values

/* To use the display */
LiquidCrystal_I2C lcd(0x27,2,1,0,4,5,6,7,3, POSITIVE);

/* Functions and methods that we'll use later */

void defineSamplesArray(int *samples, int *samples2);
void defineBitsArray(int *samples, int *bitsArray);

void LCD_write(int linha, String text);
void LCD_Update(float value);

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
  //reserving space for bit and light vectors
  int *samples, *samples2,*bitsArray, aux = 0;
  float sensorValue = -127.00;
  samples = (int*)malloc(sizeof(int)*vectorSize); //to light values
  samples2   = (int*)malloc(sizeof(int)*vectorSize*2);
  bitsArray = (int*)malloc(sizeof(int)*pktSize);  //to bits (1s and 0s)


VOLTA:
//leitura do sensor de luz utilizado (LDR no caso - ele estah causando muito ruido)
  for(int i = 0; i < (vectorSize*2); i++)
  {
    samples2[i] = analogRead(LDR);
    printDebugWave(samples2[i]);
    delay(WAIT/2);
  }

  

//Opcao para simular uma leitura com menos ruidos 
  /*int  samples2 = {
793,
792,
798,
782,
799,
781,
782,
782,
782,
782,
782,
782,
782,
797,
785,
798,
820,
821,
830,
835,
820,
821,
830,
829,
//---------------------------------------------------
//36.8
/*830,
790,
790,
830,
830,
790,
830,
790,
830,
790,
830,
790,
790,
830,
830,
790,
830,
790,
830,
790,
830,
790,
790,
830,
830,
790,
830,
790,
790,
830,
790,
830,
830,
790,
830,
790,
790,
830,
790,
830,
830,
790,
830,
790,
790,
830,
790,
830,
830,
790,
830,
790,
790,
830,
790,
830,
830,
790,
830,
790,
790,
830,
790,
830,*/
//7.5
/*830,
790,
790,
830,
830,
790,
830,
790,
830,
790,
830,
790,
830,
790,
830,
790,
790,
830,
790,
830,
790,
830,
790,
830,
830,
790,
830,
790,
830,
790,
830,
790,
830,
790,
830,
790,
830,
790,
830,
790,
830,
790,
830,
790,
830,
790,
830,
790,
830,
790,
830,
790,
830,
790,
830,
790,
830,
790,
830,
790,
830,
790,
830,
790,
//---------------------------------------------------
800,
795,
777,
795,
777,
795,
777,
795,
777,
795,
777,
795,
777,
795,
777,
795,
819,
829,
835,
799,
774,
857,
858,
858,
858,
859,
858,
859,
860,
860,
791,
763,
858,
860,
792,
860,
790,
860,
742,
729,
860,
860,
789,
860,
742,
859,
795,
813,
860,
760,
859,
746,
860,
860,
784,
857,
739,
750,
860,
860,
783,
757,
859,
756,
860,
833,
839,
810,
785,
798,
823,
833,
839,
808,
783,
797,
821,
830,
835,
803,
777,
795,
819,
829,
835,
799,
774,
857,
858,
858,
858,
859,
858,
859,
860,
860,
791,
763,
858,
860,
792,
860,
790,
860,
742,
729,
860,
860,
789,
860,
742,
859,
795,
813,
860,
760,
859,
746,
860,
860,
784,
857,
739,
750,
860,
860,
783,
757,
859,
756,
860};*/

  //calling the functions that will deal with the light samples
  defineSamplesArray(samples, samples2);  
  defineBitsArray(samples, bitsArray);
  
  ERRO = LOW;
  if(ERRO) sensorValue = -127.00;
  else if(flag)
  {    
  	 //checkParity(bitsArray);
   	 sensorValue = floatI3E754(bitsArray);

     //for(int i = 0; i < pktSize; i++)Serial.print(bitsArray[i]);
     //Serial.println(" ");
  }
  //delay(2000);


  Serial.print("Sensor: ");Serial.println(sensorValue);

  goto VOLTA;
  
  //LCD_Update(sensorValue);  
  
  free(samples);
  free(samples2);
  free(bitsArray);
  
  if(timeCounter > 2147483648)resetFunc();
}

/* "OK" */
void defineSamplesArray(int *samples, int *samples2)
{
    int aux3[vectorSize*2];  
    for(int i = 0; i < (vectorSize*2); i++)aux3[i]=samples2[i];
    
    for (int i = 0; i < ((vectorSize*2)-1) ; i++)
    {
        for (int j = i; j < vectorSize-1 ; j++)
        {
           if (samples2[i] < samples2[j])
           {
              int temp = samples2[i];
              samples2[i] = samples2[j];
              samples2[j] = temp;
           }
         }
    }
    int mediaMaior = 0, mediaMenor = 0, mediaMeio = 0;
    for(int i = 0; i < vectorSize; i++)
    {
      if(i<(vectorSize/3)) mediaMeio += samples2[i];
      else if(i > (2*vectorSize/3)) mediaMenor += samples2[i];
      else mediaMaior += samples2[i];
    }
    mediaMenor = mediaMenor/(vectorSize/3);
    mediaMeio = mediaMeio/(vectorSize/3);
    mediaMaior = mediaMaior/(vectorSize/3);

  for(int i = 0; i < vectorSize*2; i++)samples2[i]=aux3[i]; //recupera a ordem certa

  int aux = 0;
  for(int i = 1; i < vectorSize*2; i++)
  {
    if(aux == 8)
    {
      aux = i;
      flag = HIGH;
      break;
    }
    else if(mediaMeio*(1-Margin) <= samples2[i] && samples[i] <= mediaMaior) aux++;
    else aux = 0;
  }
  
  if(flag)
     for(int i = 0; i < vectorSize; i++)
       samples[i] = samples2[aux+i];
       
  flag = HIGH;
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

/*OK*/
float floatI3E754(int *bitsArray)
{
    float value = 0;
    
    int sign = 1;
    if(bitsArray[0]) sign = -1;
    
    int exp754 = bitsArray[1]*pow(2,7) + bitsArray[2]*pow(2,6) + bitsArray[3]*pow(2,5) + bitsArray[4]*pow(2,4) 
               + bitsArray[5]*pow(2,3) + bitsArray[6]*pow(2,2) + bitsArray[7]*pow(2,1) + bitsArray[8]*pow(2,0);
    exp754 = exp754 - 127;
    
    int power = -1;
    float mant = 0.0;
      for ( int i = 0; i < 23; i++ )
      {
        int c = bitsArray[ i + 9 ];
        mant += (float) c * (float) pow( 2.0, power );
        power--;
      }
    mant += 1.0;
    
    value = sign*(float)pow(2.0,exp754)*mant; 
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
void LCD_Update(float value)
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
 /* Serial.print(N);
  Serial.print(" ");
  Serial.println((millis() - timeCounter)/1000);
  timeCounter = millis();*/
}
