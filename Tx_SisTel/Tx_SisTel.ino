/*
 * Alunos: Alaf Santos / Matheus Boy
 * Disciplina: Sistemas de Telecomunicacoes
 * Codigo para Transmissor VLC
 */
 
#define LED 10        //saida analogica na qual o led esta conectado
#define ONE   255     //maximo para analogWrite (8 bits) - 5V
#define ZERO  200     //minimo para nao termos uma variacao visivel e condizer com a sensibilidade do receptor - funfa pra teste
#define WAIT 5      //200Hz - sincronizado com o receptor (pseudo clk)

String header = "00" + String('#', BIN); //(#)10 = (00100011)2, somei os zeros do inicio para termos 8 bits
String footer = "00" + String('$', BIN); //($)10 = (00100100)2, somei o zero do inicio para termos 8 bits
String input = "00000101"; //5 em binario de 8 bits

void setup() 
{
  Serial.begin(115200);
  pinMode(LED, OUTPUT);
}

void loop() 
{
 if(Serial.available()) //o envio comeca ao entrarmos com algo no serial
 {
  for(int j = 0; j < header.length(); j++)
  {
      Serial.println(header);
      if(header[j] == '1')
      { 
        analogWrite(LED, ONE);
        Serial.println("UM");
        delay(WAIT);
      }
      else 
      {
        analogWrite(LED, ZERO);
        Serial.println("ZERO");
        delay(WAIT);
      }  
      
  }
     
  for(int i = 0; i < input.length(); i++)
  {
    Serial.println(input);
    if(input[i] == '1')
    { 
      analogWrite(LED, ONE);
      Serial.println("UM");
      delay(WAIT);
    }
    else 
    {
      analogWrite(LED, ZERO);
      Serial.println("ZERO");
      delay(WAIT);
    }  
  }

  for(int j = 0; j < footer.length(); j++)
  {
      Serial.println(footer);
      if(footer[j] == '1')
      { 
        analogWrite(LED, ONE);
        Serial.println("UM");
        delay(WAIT);
      }
      else 
      {
        analogWrite(LED, ZERO);
        Serial.println("ZERO");
        delay(WAIT);
      }  
      
  }
    Serial.println(" "); //pula uma linha
  }
}
