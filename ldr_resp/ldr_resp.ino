#define LDR A0
#define WAIT 80
unsigned long timeCounter = 0;

void printDebugWave(int N)
{
  Serial.print(N);
  Serial.print(" ");
  Serial.println((millis() - timeCounter)/1000);
  timeCounter = millis();
}

void setup() {
Serial.begin(115200);
pinMode(LDR,INPUT);
}

void loop() {
printDebugWave(analogRead(LDR));
delay(WAIT/2);  
}
