#pragma once

#include "Arduino.h"

#define IEEE754_VERSION "0.2.0"

// (un)comment lines to configure functionality / size
//#define IEEE754_ENABLE_MSB   // +78 bytes


// IEEE754 float layout; 
struct IEEEfloat
{
    uint32_t m:23; 
    uint8_t e:8;
    uint8_t s:1;
};


// Arduino UNO double layout: 
// the UNO has no 64 bit double, it is only able to map 23 bits of the mantisse
// a filler is added for the remaining bits. These might be useful in future?
struct _DBL
{
    uint32_t filler:29;
    uint32_t m:23;
    uint16_t e:11;
    uint8_t  s:1;
};

// for packing and unpacking a float
union _FLOATCONV
{
    IEEEfloat p;
    float f;
    byte b[4];
};

// for packing and unpacking a double
union _DBLCONV
{
    // IEEEdouble p;
    _DBL p;
    double d;           // !! is a 32bit float for UNO.
    byte b[8];
};


//
// DEBUG FUNCTIONS
//
// print float components

void dumpFloat(float number)
{
    IEEEfloat* x = (IEEEfloat*) ((void*)&number);
    
    Serial.print(" sign: "); Serial.print(x->s, BIN);
    Serial.print("  exp: "); Serial.print(x->e, BIN);
    Serial.print(" mant: "); Serial.println(x->m, BIN);

    String teste = String(x->s,BIN) + String(x->e,BIN) + String(x->m,BIN);
    Serial.println(teste);

    String pktBIN = "01000000111100000000000000000000"; //7.5
    int bitsArray[] =  {0,1,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    for(int i = 0; i < 32;i++)Serial.print(bitsArray[i]);
    Serial.println();

    //daqui pra frente bitsArray é o cara que eu quero reconstruir como 7.5

    float value = 0;
    for(int i = 0; i < 32; i++)
    {
      if(bitsArray[i])bitSet(value,i+1);
    }
    Serial.println(value);
}

// print "double" components
void dumpDBL(struct _DBL dbl)
{
    Serial.print(dbl.s, HEX);
    Serial.print("\t");
    Serial.print(dbl.e, HEX);
    Serial.print("\t");
    Serial.println(dbl.m, HEX);
}

//
// mapping to/from 64bit double - best effort
//

// converts a float to a packed array of 8 bytes representing a 64 bit double
// restriction exponent and mantisse.
// float;  array of 8 bytes;  LSBFIRST; MSBFIRST
void float2DoublePacked(float number, byte* bar, int byteOrder = LSBFIRST)  
{
    _FLOATCONV fl;
    fl.f = number;
    _DBLCONV dbl;
    dbl.p.filler = 0;
    dbl.p.s = fl.p.s;
    dbl.p.e = fl.p.e-127 +1023;  // exponent adjust
    dbl.p.m = fl.p.m;
    
#ifdef IEEE754_ENABLE_MSB
    if (byteOrder == LSBFIRST)
    {
#endif
        for (int i = 0; i < 8; i++)
        {
            bar[i] = dbl.b[i];
        }
#ifdef IEEE754_ENABLE_MSB
    }
    else
    {
        for (int i = 0; i < 8; i++)
        {
            bar[i] = dbl.b[7-i];
        }
    }
#endif
}

// converts a packed array of bytes into a 32bit float.
// there can be an exponent overflow
// the mantisse is truncated to 23 bits.
float doublePacked2Float(byte* bar, int byteOrder = LSBFIRST)
{
    _FLOATCONV fl;
    _DBLCONV dbl;
    
#ifdef IEEE754_ENABLE_MSB
    if (byteOrder == LSBFIRST)
    {
#endif
        for (int i = 0; i < 8; i++)
        {
            dbl.b[i] = bar[i];
        }
#ifdef IEEE754_ENABLE_MSB
    }
    else
    {
        for (int i = 0; i < 8; i++)
        {
            dbl.b[i] = bar[7-i];
        }
    }
#endif
    
    int e = dbl.p.e - 1023 + 127;  // exponent adjust 
    // TODO check exponent overflow.
    if (e >=0 || e <= 255) 
    {
        fl.p.s = dbl.p.s;
        fl.p.e = e;  
        fl.p.m = dbl.p.m;  // note this one clips the mantisse 
        return fl.f;
    }
    return NAN;  // OR +-INF?
    // return (fl.p.s) ? -INFINITY : INFINITY;
}

//
// TEST FUNCTIONS
//

// ~1.7x faster
int IEEE_NAN(float number)  
{
    uint16_t* x = ((uint16_t*) &number + 1);
    return ((*x) == 0x7FC0); 
}

// ~3.4x faster
int IEEE_INF(float number)  
{
    uint8_t* x = ((uint8_t*) &number);
    if (*(x+2) != 0x80) return 0;
    if (*(x+3) == 0x7F) return 1;
    if (*(x+3) == 0xFF) return -1;
    return 0;
}

// for the real speed freaks, the next two
boolean IEEE_PosINF(float number)  
{
    return (* ((uint16_t*) &number + 1) ) == 0x7F80; 
}

boolean IEEE_NegINF(float number)  
{
    return (* ((uint16_t*) &number + 1) ) == 0xFF80; 
}




//
// PROPERTIES
// 
uint8_t IEEE_Sign(float number)
{
  IEEEfloat* x = (IEEEfloat*) ((void*)&number);
  return x->s;
}

int IEEE_Exponent(float number)
{
  IEEEfloat* x = (IEEEfloat*) ((void*)&number);
  return x->e - 127;
}

uint32_t IEEE_Mantisse(float number)
{
  IEEEfloat* x = (IEEEfloat*) ((void*)&number);
  return x->m;
}

//
// MATH FUNCTIONS
//

// factor ~2.7; (tested with *16) more correct than the faster one
float IEEE_POW2(float number, int n)
{
    _FLOATCONV fl;
    fl.f = number;
    int e = fl.p.e + n;
    if (e >= 0 && e < 256)
    {
        fl.p.e = e;
        return fl.f;
    }
    return (fl.p.s) ? -INFINITY : INFINITY;
}

// WARNING no overflow detection in the SHIFT (factor ~3.5)
float IEEE_POW2fast(float number, int n)
{
    _FLOATCONV fl;
    fl.f = number;
    fl.p.e += n;
    return fl.f;
}



//
// NOT TESTED FUNCTIONS
//

//
// get truncated part as separate float.
// 
void doublePacked2Float2(byte* bar, int byteOrder, float* value, float* error)
{
    _FLOATCONV fl;
    _DBLCONV dbl;
    
#ifdef IEEE754_ENABLE_MSB
    if (byteOrder == LSBFIRST)
    {
#endif
        for (int i = 0; i < 8; i++)
        {
            dbl.b[i] = bar[i];
        }
#ifdef IEEE754_ENABLE_MSB
    }
    else
    {
        for (int i = 0; i < 8; i++)
        {
            dbl.b[i] = bar[7 - i];
        }
    }
#endif
    
    int e = dbl.p.e - 1023 + 127;  // exponent adjust 
    // TODO check exponent overflow.
    if (e >=0 || e <= 255) 
    {
        fl.p.s = dbl.p.s;
        fl.p.e = e;  
        fl.p.m = dbl.p.m;  // note this one clips the mantisse 
        *value = fl.f;
        
        fl.p.s = dbl.p.s;
        fl.p.e = e-23;  
        fl.p.m = dbl.p.filler;  // note this one clips the mantisse 
        *error = fl.f;
    }
    *value = (dbl.p.s) ? -INFINITY : INFINITY;
    *error = 0;
}
