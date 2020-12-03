#define LDR A0 
#define Margin 0.03  
#define pktSize 33
#define WAIT 80

const int vectorSize = 2*pktSize;
unsigned int counter = 0;
unsigned long timeCounter = 0;

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

void printDebugWave(int N)
{
  Serial.print(N);
  Serial.print(" ");
  Serial.println((millis() - timeCounter)/1000);
  timeCounter = millis();
}

void setup() 
{
  Serial.begin(115200);
  pinMode(LDR, INPUT);
}

void loop() 
{
  int *samples, *bitsArray, /**samples2,*/aux = 0;
  bool flag = LOW;
 // samples2   = (int*)malloc(sizeof(int)*vectorSize*2); //to light values
  samples   = (int*)malloc(sizeof(int)*vectorSize); //to light values
  bitsArray = (int*)malloc(sizeof(int)*pktSize);  //to bits (1s and 0s)

 /* aux = analogRead(LDR);
  delay(WAIT/2);
    
  if(aux < analogRead(LDR))
  {
  for(int i = 0; i < vectorSize*2; i++)
  {
    samples2[i] = analogRead(LDR);
    delay(WAIT/2);
  } */

int samples2[] = {
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
860};
    int aux3[219];  
    for(int i = 0; i < 219; i++)aux3[i]=samples2[i];
    
    for (int i = 0; i < vectorSize-1 ; i++)
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

  for(int i = 0; i < 219; i++)samples2[i]=aux3[i]; //recupera a ordem certa

  aux = 0;
  for(int i = 1; i < vectorSize*3; i++)
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
  {    
     for(int i = 0; i < vectorSize; i++)
     {
       samples[i] = samples2[aux+i];
     }
     
     defineBitsArray(samples, bitsArray);
     for(int i = 0; i < (pktSize-1); i++)Serial.print(bitsArray[i]);
     Serial.println(" ");
  }
  delay(1000);
  free(samples);
  free(bitsArray);
}
  
