#include <iostream>
#include <limits.h>
#include <math.h>
#include <bitset>

using namespace std;

string GetBinary32(float value)
{
  union
  {
    float input;   // assumes sizeof(float) == sizeof(int)
    int output;
  } data;

  data.input = value;

  bitset<sizeof(float) * CHAR_BIT>   bits(data.output);

  string mystring = bits.to_string<char, char_traits<char>, allocator<char> >();

  return mystring;
}

float GetFloat32(string Binary)
{
  bitset<32> set(Binary);      
  int hexNumber = set.to_ulong();
    
  bool negative  = !!(hexNumber & 0x80000000);
  int  exponent  =   (hexNumber & 0x7f800000) >> 23;    
  int sign = negative ? -1 : 1;

  // Subtract 127 from the exponent
  exponent -= 127;

  // Convert the mantissa into decimal using the
  // last 23 bits
  int power = -1;
  float total = 0.0;
  for ( int i = 0; i < 23; i++ )
  {
    int c = Binary[ i + 9 ] - '0';
    cout << Binary[ i + 9 ] << " / "<< c << endl;
    total += (float) c * (float) pow( 2.0, power );
    power--;
  }
  total += 1.0;

  cout << "mantissa lidada: "<< total << endl;

  float value = sign * (float) pow( 2.0, exponent ) * total;

  return value;
}

int main () {
  string str = GetBinary32(7.5);
  float f = GetFloat32(str);
  cout << "Float P/ String: " << str << endl;
  cout << "String P/Float: " << f << endl;

  return 0;

}
