#include <EEPROM.h>

int addr = 0;
byte value;

void setup() {

  /* 0 */char s1[] = "APRS"; //5
  EEPROM.put(addr, s1);
  addr += sizeof(s1);

  /* 5*/ char s2[] = "HIPT"; //5
  EEPROM.put(addr, s2);
  addr += sizeof(s2);

  /* 10 */ char s3[] = "WIDE2";  //6
  EEPROM.put(addr, s3);
  addr += sizeof(s3);

  /* 16 */ char s4[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";  //27
  EEPROM.put(addr, s4);
  addr += sizeof(s4);

  /* 43 */ char s5[] = "/=";  //3
  EEPROM.put(addr, s5);
  addr += sizeof(s5);

  /* 46 */ char s6[] = "abcdefghijqlmnopqrstuvwxyz";  //27
  EEPROM.put(addr, s6);
  addr += sizeof(s6);

  /* 73 */ char s10[] = "LAT=";  //5
  EEPROM.put(addr, s10);
  addr += sizeof(s10);

  /* 78 */ char s11[] = "LON=";  //5
  EEPROM.put(addr, s11);
  addr += sizeof(s11);

  /* 83 */ char s12[] = "HEA=";  //5
  EEPROM.put(addr, s12);
  addr += sizeof(s12);

  /* 88 */ char s13[] = "SPE=";  //5
  EEPROM.put(addr, s13);
  addr += sizeof(s13);

  /* 93 */ char s14[] = "ALT=";  //5
  EEPROM.put(addr, s14);
  addr += sizeof(s14);

  /* 98 */ char s15[] = "Tin=";  //5
  EEPROM.put(addr, s15);
  addr += sizeof(s15);

  /* 103 */ char s16[] = "Tex=";  //5
  EEPROM.put(addr, s16);
  addr += sizeof(s16);
  
  addr=0;
  Serial.begin(9600);
  
  //Read alphabet
  char c[27];
  EEPROM.get(16,c);
  Serial.println(c);
  
}

void loop() {
  // read a byte from the current address of the EEPROM
  value = EEPROM.read(addr);

  Serial.print(addr);
  Serial.print("\t");
  Serial.print(value, DEC);
  Serial.println();

  /***
    Advance to the next address, when at the end restart at the beginning.

    Larger AVR processors have larger EEPROM sizes, E.g:
    - Arduno Duemilanove: 512b EEPROM storage.
    - Arduino Uno:        1kb EEPROM storage.
    - Arduino Mega:       4kb EEPROM storage.

    Rather than hard-coding the length, you should use the pre-provided length function.
    This will make your code portable to all AVR processors.
  ***/
  addr = addr + 1;
  if (addr == EEPROM.length()) {
    addr = 0;
  }

  /***
    As the EEPROM sizes are powers of two, wrapping (preventing overflow) of an
    EEPROM address is also doable by a bitwise and of the length - 1.

    ++address &= EEPROM.length() - 1;
  ***/

  delay(500);

}
