void setup() {
 Serial.begin(9600);
}
void loop() {
//int bitsArray[] =  {0,1,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}; //7.5
int bitsArray[] =  {0,1,0,0,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};//9.0
float value = 0;

//lidando com sinal
int sign = 1;
if(bitsArray[0]) sign = -1;

//lidando com expoente
int exp754 = bitsArray[1]*pow(2,7) + bitsArray[2]*pow(2,6) + bitsArray[3]*pow(2,5) + bitsArray[4]*pow(2,4) + bitsArray[5]*pow(2,3) + bitsArray[6]*pow(2,2) + bitsArray[7]*pow(2,1) + bitsArray[8]*pow(2,0);
exp754 = exp754 - 127;

//lidando com mantissa
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
Serial.println(value);

}
