#include <EEPROM.h>

int addr = 0;
byte value;

void setup() {
  EEPROM.write(32, ' ');
  EEPROM.write(33, '!');
  EEPROM.write(34, '\"');
  EEPROM.write(35, '#');
  EEPROM.write(36, '$');
  EEPROM.write(37, '%');
  EEPROM.write(38, '&');
  EEPROM.write(39, '\'');
  EEPROM.write(40, '(');
  EEPROM.write(41, ')');
  EEPROM.write(42, '*');
  EEPROM.write(43, '+');
  EEPROM.write(44, ',');
  EEPROM.write(45, '-');
  EEPROM.write(46, '.');
  EEPROM.write(47, '/');
  EEPROM.write(48, '0');
  EEPROM.write(49, '1');
  EEPROM.write(50, '2');
  EEPROM.write(51, '3');
  EEPROM.write(52, '4');
  EEPROM.write(53, '5');
  EEPROM.write(54, '6');
  EEPROM.write(55, '7');
  EEPROM.write(56, '8');
  EEPROM.write(57, '9');
  EEPROM.write(58, ':');
  EEPROM.write(59, ';');
  EEPROM.write(60, '<');
  EEPROM.write(61, '=');
  EEPROM.write(62, '>');
  EEPROM.write(63, '?');
  EEPROM.write(64, '@');
  EEPROM.write(65, 'A');
  EEPROM.write(66, 'B');
  EEPROM.write(67, 'C');
  EEPROM.write(68, 'D');
  EEPROM.write(69, 'E');
  EEPROM.write(70, 'F');
  EEPROM.write(71, 'G');
  EEPROM.write(72, 'H');
  EEPROM.write(73, 'I');
  EEPROM.write(74, 'J');
  EEPROM.write(75, 'K');
  EEPROM.write(76, 'L');
  EEPROM.write(77, 'M');
  EEPROM.write(78, 'N');
  EEPROM.write(79, 'O');
  EEPROM.write(80, 'P');
  EEPROM.write(81, 'Q');
  EEPROM.write(82, 'R');
  EEPROM.write(83, 'S');
  EEPROM.write(84, 'T');
  EEPROM.write(85, 'U');
  EEPROM.write(86, 'V');
  EEPROM.write(87, 'W');
  EEPROM.write(88, 'X');
  EEPROM.write(89, 'Y');
  EEPROM.write(90, 'Z');
  EEPROM.write(91, '[');
  EEPROM.write(92, '\\');
  EEPROM.write(93, ']');
  EEPROM.write(94, '^');
  EEPROM.write(95, '_');
  EEPROM.write(96, '`');
  EEPROM.write(97, 'a');
  EEPROM.write(98, 'b');
  EEPROM.write(99, 'c');
  EEPROM.write(100, 'd');
  EEPROM.write(101, 'e');
  EEPROM.write(102, 'f');
  EEPROM.write(103, 'g');
  EEPROM.write(104, 'h');
  EEPROM.write(105, 'i');
  EEPROM.write(106, 'j');
  EEPROM.write(107, 'k');
  EEPROM.write(108, 'l');
  EEPROM.write(109, 'm');
  EEPROM.write(110, 'n');
  EEPROM.write(111, 'o');
  EEPROM.write(112, 'p');
  EEPROM.write(113, 'q');
  EEPROM.write(114, 'r');
  EEPROM.write(115, 's');
  EEPROM.write(116, 't');
  EEPROM.write(117, 'u');
  EEPROM.write(118, 'v');
  EEPROM.write(119, 'w');
  EEPROM.write(120, 'x');
  EEPROM.write(121, 'y');
  EEPROM.write(122, 'z');
  EEPROM.write(123, '{');
  EEPROM.write(124, '|');
  EEPROM.write(125, '}');
  EEPROM.write(126, '~');

  Serial.begin(9600);
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
