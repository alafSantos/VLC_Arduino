/*
 * VLC Transmitter Based on OOk modulation and Manchester Code
 * line code that maybe I'll need (it is just about an error that I got): sudo chmod a+rw /dev/ttyACM0
 */

#define SENSOR A0   //using a variable resistor to simulate a sensor
#define LED 10     //the gate where the data will be available

/* Manchester levels */
#define N2  255
#define N1  228 
#define N0  200

/* Timing - T = (1/f) [ms] */     
#define WAIT 100      //200Hz - sincronizado com o receptor (pseudo clk)


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


/*system boot*/
void setup() 
{
  Serial.begin(115200);
  pinMode(LED, OUTPUT);
  pinMode(SENSOR, INPUT);
}

/*main function - loop*/
void loop() 
{
 analogWrite(LED, N1); //turn LED on
 float sensor = analogRead(SENSOR); //reading the sensor gate
 String pktIEEE754 = StringI3E754(sensor); //in this point, we have the string of bits
 sendData(pktIEEE754); //sending data after get the bits array
 //maybe we need a timing here
}

/*Input = float number / output = string of bits in the format IEEE 754*/
String StringI3E754(float sensor)
{
  //not implemented yet
  return "01010101010101011111";
}

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
  Serial.println(millis()/1000);
}
