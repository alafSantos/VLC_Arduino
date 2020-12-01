/*
 * VLC Transmitter Based on OOk modulation and Manchester Code
 * line code that maybe I'll need (it is just about an error that I got): sudo chmod a+rw /dev/ttyACM0
 */

#define SENSOR A0   //using a variable resistor to simulate a sensor
#define LED 10     //the gate where the data will be available

/* Manchester levels */
#define N2  250
#define N1  150 
#define N0  100

/* Timing - T = (1/f) [ms] */     
#define WAIT 250      //4Hz - sincronizado com o receptor (pseudo clk)

#define BUTTON 9

/* Functions and methods that we'll use later */
String StringI3E754(float sensor);


void sendData(String pktIEEE754);
void sendPar(String pktIEEE754);
void sendPKT754(String pktIEEE754);
void sendStart();
void sendStop();

void sendOne();
void sendZero();

void printDebugWave(int N);

void (*resetFunc)(void) = 0; //declare reset function @ address 0

bool lastButtonState = LOW, buttonState;

unsigned long timeCounter = 0, lastDebounceTime = 0, debounceDelay = 15;

/*system boot*/
void setup() 
{
  Serial.begin(115200);
  pinMode(LED, OUTPUT);
  pinMode(SENSOR, INPUT);
  pinMode(BUTTON, INPUT);
}

/*main function - loop*/
void loop() 
{
 analogWrite(LED, N1); //turn LED on
 printDebugWave(N1);

 /*DEBOUNCE*/
 bool reading = digitalRead(BUTTON);
 
 if(reading != lastButtonState)
  lastDebounceTime = millis();

 if((millis() - lastDebounceTime) > debounceDelay)
  if(reading != buttonState)
   buttonState = reading; 
 lastButtonState = reading;

 /* If buttonState == HIGH we do our transmission */
 if(buttonState)
 {
   float sensor = analogRead(SENSOR); //reading the sensor gate
   
   String pktIEEE754 = StringI3E754(sensor); //in this point, we have the string of bits
   sendData(pktIEEE754); //sending data after get the bits array

   buttonState = LOW;
   if(timeCounter > 2147483648) resetFunc();
 }
}


//------------------------------------------------------------------------------
struct IEEEfloat
{
    uint32_t m:23; 
    uint8_t e:8;
    uint8_t s:1;
};


/*Input = float number / output = string of bits in the format IEEE 754*/
String StringI3E754(float sensor)
{
    IEEEfloat* x = (IEEEfloat*) ((void*)&sensor);

    //Serial.print("Float: "); Serial.println(sensor);
    
   /* Serial.print(" sign: "); Serial.print(x->s, BIN);
    Serial.print("  exp: "); Serial.print(x->e, BIN);
    Serial.print(" mant: "); Serial.println(x->m, BIN);*/

    //we need to verify every components from the package before the sum
    //sign is just one bit
    //8 bits exp

    String pkt754_sign = String(x->s,BIN);
    String pkt754_exp  = String(x->e,BIN);
    String pkt754_mant = String(x->m,BIN);

    String aux1 = "";
    for(int i = 0; i < (8-pkt754_exp.length()); i++) aux1 += "0";

    String aux2 = "";
    for(int i = 0; i < (23-pkt754_mant.length()); i++) aux2 += "0";

    pkt754_exp = aux1 + pkt754_exp;
    pkt754_mant = aux2 + pkt754_mant;
        
    String pkt754 = pkt754_sign + pkt754_exp + pkt754_mant;
   // Serial.print("Package IEEE 754: ");Serial.println(pkt754);

    return pkt754;
}
//------------------------------------------------------------------------------


/*this is like the second main function, cuz here, we have the protocol form [start - pkt754 - parity - stop]*/
void sendData(String pktIEEE754)
{
 sendStart();
 sendPKT754(pktIEEE754);
 sendPar(pktIEEE754);
 sendStop();

}

void sendStart()
{
  for(int j = 0; j < 4; j++)
  {
     analogWrite(LED, N2);
     printDebugWave(N2);
     delay(WAIT);   
  }
}

void sendStop()
{
  for(int j = 0; j < 4; j++)
  {
     analogWrite(LED, N1);
     printDebugWave(N1);
     delay(WAIT);   
  }
}

void sendPar(String pktIEEE754)
{
  int counter = 0;
  for(int i = 0; i < pktIEEE754.length(); i++)
    if(pktIEEE754[i] == '1')
      counter++;
      
  if(!(counter%2)) sendOne();
  else sendZero();
}

void sendPKT754(String pktIEEE754)
{
  for(int i = 0; i < pktIEEE754.length(); i++)
  {
    if(pktIEEE754[i] == '1') sendOne();
    else sendZero();
  }
}


/* it's kind of a manchester code defined by IEEE 802.3*/
void sendOne()
{
      analogWrite(LED, N0);
      printDebugWave(N0);
      delay(WAIT/2);
 
      analogWrite(LED, N2);
      printDebugWave(N2);
      delay(WAIT/2);
}

void sendZero()
{
      analogWrite(LED, N2);
      printDebugWave(N2);
      delay(WAIT/2);
      
      analogWrite(LED, N0);
      printDebugWave(N0);
      delay(WAIT/2);
}

/*It will be useful to draw a wave from the signal output*/
void printDebugWave(int N)
{
  Serial.print(N);
  Serial.print(" ");
  Serial.println((millis() - timeCounter)/1000);
  timeCounter = millis();
}
