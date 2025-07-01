#include "led_matrix.h"


void LED_MATRIX::begin(SPIClass* spi, uint8_t width, uint8_t height, uint8_t load_1, uint8_t load_2) {

	_spi = spi;

	_width = width;
	_height = height;

	_controller_count = 2;
	_count = _width / 8 / _controller_count;

	_port[0] = load_1;
	_port[1] = load_2;

	pinMode(load_1, OUTPUT);
	pinMode(load_2, OUTPUT);

	digitalWrite(load_1, HIGH);
	digitalWrite(load_2, HIGH);

	// loop load lines
	for (uint8_t contr = 0; contr < _controller_count; contr++) {

		// loop controllers
		for (uint8_t module = 0; module < _count; module++) {

			#ifdef DEBUG
				Serial.print("init registers of MAX7219 port=");
				Serial.print(_port[contr]);

				Serial.print(", address=");
				Serial.println(module);
			#endif

			// clearDisplay(i);
			_write(_port[contr], module, OP_INTENSITY, 0x0F);  //max intensity

			//scanlimit is set to max on startup
			_write(_port[contr], module, OP_SCANLIMIT,0x07);

			// set decode type
			_write(_port[contr], module, OP_DECODEMODE, 0x00);  //disable onboard bit decode

			// displaytest
			// _write(_port[contr], module, OP_DISPLAYTEST, 0x01);

			// end display test
			// delay(500/8);

			_write(_port[contr], module, OP_DISPLAYTEST, 0x00);

			// activate all on startup
			_write(_port[contr], module, OP_SHUTDOWN, 0x01);  //turn on chip
	
		}
	}

	// set area to max and clear
	area();
	clear();
}


// =======================================================
// home cursor
void LED_MATRIX::home(void) {

	cursor(0);
}


// set/get cursor position
bool LED_MATRIX::cursor(uint8_t pos) {

	if (pos < _width) {
		_cursor = pos;
		return true;
	}

	return false;
}

// get cursor position
uint8_t LED_MATRIX::cursor(void) {
	return _cursor;
}

// increment cursor position
bool LED_MATRIX::next(void) {
	return next(1);
}

// increment cursor by steps
bool LED_MATRIX::next(int8_t steps) {

	if ((_cursor + steps) < 0) {
		_cursor = 0;

		return false;
	}

	else if ((_cursor + steps) < _width) {
		_cursor += steps;

		return true;
	}

	_cursor = _width - 1;

	return false;
}


// invert character output
void LED_MATRIX::invert(void) {
	invert(!_invert);
}

void LED_MATRIX::invert(bool status) {
	_invert = status;
}


// =======================================================
// PRINT CHARACTER
// print at cursor position
uint8_t LED_MATRIX::print_char(uint8_t c) {

	uint8_t col;
	uint8_t pos = cursor();

	uint16_t offset = c * 9;
	uint8_t kerning = charTable[offset] & 0x0F;
	uint8_t spacing = (charTable[offset] / 16) & 0x0F;

	// #ifdef DEBUG
	// 	Serial.print("char ");
	// 	Serial.print(c);
	// 	Serial.print(" at cursor ");
	// 	Serial.print(cursor());
	// 	Serial.print(" kerning ");
	// 	Serial.print(kerning);
	// 	Serial.print(" spacing ");
	// 	Serial.println(spacing);
	// #endif

	// write character into display
	for (col = 0; col < kerning; col++) {

		// check for display end > end output
		if (cursor() >= _width) {
			break;
		}

		uint8_t data = pgm_read_byte(charTable + offset + col + 1);

		// save in buffer
		if (_invert) {
			_buffer[cursor()] = data ^ 0xFF;
		}

		else {
			_buffer[cursor()] = data;
		}

		// increment cursor
		next();
	}

	// next(-1);

	// add space after character
	next(spacing);

	_send(pos, cursor());

	return spacing;
}


// =======================================================
// print text starting at x = 0
void LED_MATRIX::print(char* text) {
	print(text, 0);
}


// print text starting at position
// in area defined by min-max settings
// pos=0 at min
void LED_MATRIX::print(char* text, int8_t pos) {

	uint8_t c;
	uint8_t i;
	uint8_t pixels;

	// position is alignment
	if (pos < 0) {

		// ALIGN LEFT = default
		if (pos == ALIGN_LEFT) {
			pos = _min;
		}

		// get width of text and align
		else {

			pixels = length(text);

			// ALIGN RIGHT
			if (pos == ALIGN_RIGHT) {
				pos = _max - pixels + 1;
			}

			// ALIGN CENTER
			else {
				pos = _min + (_max - _min - pixels + 1) / 2;
			}
		}
	}

	// set cursor to start
	cursor(pos);

// Serial.print("pixels ");
// Serial.print(pixels);
// Serial.print(" min ");
// Serial.print(_min);
// Serial.print(" max ");
// Serial.print(_max);
// Serial.print(" pos ");
// Serial.println(cursor());

	// print characters of text
	i = 0;
	while ((c = text[i]) != 0) {
		print_char(text[i++]);
	}
}


// get text length
uint8_t LED_MATRIX::length(char* text) {

	uint8_t c;
	uint8_t kerning;
	uint8_t spacing;
	uint8_t l = 0;

	// iterate characters to \0
	// add sizes
	uint8_t i = 0;

	while ((c = text[i]) != 0) {

		uint16_t offset = c * 9;

		kerning = pgm_read_byte(charTable + offset) & 0x0F;
		spacing = pgm_read_byte(charTable + offset) > 4;

		l += kerning + spacing;

		i++;
	}

	// remove last spacing
	return l - spacing;
}


// clear all arrays
// restrict to _min and max
void LED_MATRIX::clear(void) {

	uint8_t c = 0;

	for (int contr = 0; contr < _controller_count; contr++) {

		for (int module = 0; module < _count; module++) {

			for (int i = 0; i < 8; i++) {

				if (c >= _min && c <= _max);
				_write(_port[contr], module, OP_DIGIT0 + i, 0);

				c++;
			}
		}
	}

	empty(_min, _max);
	cursor(0);
}


// reset print area
void LED_MATRIX::area(void) {
	area(0, _width);
}


// set print area
void LED_MATRIX::area(uint8_t min, uint8_t max)  {
	_min = min;
	_max = max;
}


// empty buffer
void LED_MATRIX::empty(void) {

	empty(0, _width);
}


// empty buffer
// from to
void LED_MATRIX::empty(uint8_t start, uint8_t end) {

	if (end >= _width) {
		end = _width;
	}

	for (int i = start; i < end; i++) {
		_buffer[i] = 0;
	}
}



// test display
void LED_MATRIX::test(void) {

	pattern();
	delay(150);

	invert();
	pattern();
	delay(150);

	invert(false);

}


// show testpattern
void LED_MATRIX::pattern(void) {

	clear();

	for (int i = 0; i < (_width / 8); i++) {
		print_char(0x7F);
	}
}



// send data
void LED_MATRIX::_send(uint8_t start, uint8_t end) {

	// #ifdef DEBUG
	// 	Serial.print("send data to display (");
	// 	Serial.print(start);
	// 	Serial.print("-");
	// 	Serial.print(end);
	// 	Serial.println(")");
	// #endif


	uint8_t pos = 0;

	for (uint8_t contr = _controller_count; contr > 0; contr--) {

		for (uint8_t module = 0; module < 4; module++) {

			uint8_t mod = 3 - module;

			for (uint8_t digit = OP_DIGIT0; digit <= OP_DIGIT7; digit++) {

				// if (pos >= start && pos <= end) {
					_write(_port[contr - 1], mod, digit, _buffer[pos]);
				// }

				pos++;
			}
		}

	}
}


// write one byte to address
void LED_MATRIX::_write(uint8_t load_pin, uint8_t addr, uint8_t opcode, uint8_t data) {

	//Create an array with the data to shift out
	int offset = addr*2;
	int maxbytes = _count * 2;


	// clear output data
	for(_i = 0; _i < _count; _i++) {
		_spidata[_i] = (byte)0;
	}

	// // put device data into the array
	_spidata[offset+1] = opcode;
	_spidata[offset] = data;

	// ==============
	// enable the line 
	digitalWrite(load_pin,LOW);

	// // shift out the data 
	for(_i = maxbytes; _i > 0; _i--) {
		_spi->transfer(_spidata[_i-1]);
	}

	// latch the data onto the display
	digitalWrite(load_pin,HIGH); 
	// ==============
}