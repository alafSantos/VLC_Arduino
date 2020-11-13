/*
 * Alunos: Alaf Santos / Lucas Chemelli / Matheus Boy
 * Disciplina: Sistemas de Telecomunicacoes
 * Codigo para Transmissor VLC
 * line code that maybe I'll need (it is just about an error that I got): sudo chmod a+rw /dev/ttyACM0
 */

#define SENSOR A0   //usar um potenciometro pra simular um sensor qualquer
#define LED 10     //Pino onde saira o sinal pros leds
#define N2  255
#define N1  227 
#define N0  200     
#define WAIT 5      //200Hz - sincronizado com o receptor (pseudo clk)

String StringI3E754(float sensor);
void sendData(String pktIEEE754);
void sendPar(String pktIEEE754);
void sendPKT754(String pktIEEE754);
void sendStart();
void sendStop();


void setup() 
{
  Serial.begin(115200);
  pinMode(LED, OUTPUT);
  pinMode(SENSOR, INPUT);
}

void loop() 
{
 analogWrite(LED, N1);
 delay(500);
 float sensor = analogRead(SENSOR);
 Serial.print("Leitura Sensor: ");
 Serial.println(sensor);

 String pktIEEE754 = StringI3E754(sensor); //neste ponto ja temos a string de bits
 sendData(pktIEEE754);

}

String StringI3E754(float sensor)
{
  //implementar
  return "0";
}

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
     analogWrite(LED, N0);
     Serial.println("N0");
     delay(WAIT);   
  }
}

void sendStop()
{
  for(int j = 0; j < 4; j++)
  {
     analogWrite(LED, N1);
     Serial.println("N1");
     delay(WAIT);   
  }
}

void sendPar(String pktIEEE754)
{
  int counter = 0;
  for(int i = 0; i < pktIEEE754.length(); i++)
    if(pktIEEE754[i] == '1')
      counter++;
  if(!(counter%2))
  {
      Serial.println("UM");
      analogWrite(LED, N2);
      delay(WAIT/2);
      analogWrite(LED, N0);
      delay(WAIT/2);
  }
  else
  {
      Serial.println("ZERO");
      analogWrite(LED, N0);
      delay(WAIT/2);
      analogWrite(LED, N2);
      delay(WAIT/2); 
  }
}

void sendPKT754(String pktIEEE754)
{
  for(int i = 0; i < pktIEEE754.length(); i++)
  {
    if(pktIEEE754[i] == '1')
    { 
      Serial.println("UM");
      analogWrite(LED, N2);
      delay(WAIT/2);
      analogWrite(LED, N0);
      delay(WAIT/2);
    }
    else 
    {
      Serial.println("ZERO");
      analogWrite(LED, N0);
      delay(WAIT/2);
      analogWrite(LED, N2);
      delay(WAIT/2);
    }  
  }
}
