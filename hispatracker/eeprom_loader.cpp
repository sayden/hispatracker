#include <EEPROM.h>

#ifdef LOAD_EEPROM
	void loadEEPROM(){
	    EEPROM.write(0, 0x00);
	    EEPROM.write(1, 0x01);
	    EEPROM.write(2, 0x02);
	    EEPROM.write(3, 0x03);
	    EEPROM.write(4, 0x04);
	    EEPROM.write(5, 0x05);
	    EEPROM.write(6, 0x06);
	    EEPROM.write(7, 0x07);
	    EEPROM.write(8, 0x08);
	    EEPROM.write(9, 0x09);
	    EEPROM.write(10, 0x0a);
	    EEPROM.write(11, 0x0b);
	    EEPROM.write(12, 0x0c);
	    EEPROM.write(13, 0x0d);
	    EEPROM.write(14, 0x0e);
	    EEPROM.write(15, 0x0f);
	    EEPROM.write(16, 0x10);
	    EEPROM.write(17, 0x11);
	    EEPROM.write(18, 0x12);
	    EEPROM.write(19, 0x13);
	    EEPROM.write(20, 0x14);
	    EEPROM.write(21, 0x15);
	    EEPROM.write(22, 0x16);
	    EEPROM.write(23, 0x17);
	    EEPROM.write(24, 0x18);
	    EEPROM.write(25, 0x19);
	    EEPROM.write(26, 0x1a);
	    EEPROM.write(27, 0x1b);
	    EEPROM.write(28, 0x1c);
	    EEPROM.write(29, 0x1d);
	    EEPROM.write(30, 0x1e);
	    EEPROM.write(31, 0x1f);
	    EEPROM.write(32, 0x20);
	    EEPROM.write(33, 0x21);
	    EEPROM.write(34, 0x22);
	    EEPROM.write(35, '#');
	    EEPROM.write(36, '$');
	    EEPROM.write(37, '%');
	    EEPROM.write(38, '&');
	    EEPROM.write(39, 0x27);
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
	    EEPROM.write(92, 0x5c);
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
	}
#endif