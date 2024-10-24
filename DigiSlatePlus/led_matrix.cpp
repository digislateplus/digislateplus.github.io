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
			_write(_port[contr], module, OP_DISPLAYTEST, 0x01);

			// end display test
			// delay(500/8);

			_write(_port[contr], module, OP_DISPLAYTEST, 0x00);

			// activate all on startup
			_write(_port[contr], module, OP_SHUTDOWN, 0x01);  //turn on chip
	
		}
	}

	clear();
}


// home cursor
void LED_MATRIX::home(void) {
	_cursor = 0;
}


bool LED_MATRIX::cursor(uint8_t pos) {
	if (pos < _width) {
		_cursor = pos;
		return true;
	}

	return false;
}


// print text starting at x = 0
void LED_MATRIX::print(char* text, uint8_t chars) {
	print(text, chars, 0);
}


// print text starting at position (0-64)
void LED_MATRIX::print(char* text, uint8_t chars, int8_t pos) {

	uint8_t start = pos;
	uint8_t end = pos;
	uint8_t i;
	uint8_t pixels;


	// position is alignment
	if (pos < 0) {

		// ALIGN LEFT = default
		if (pos == ALIGN_LEFT) {
			pos = 0;
		}

		// get width and align
		else {

			pixels = length(text, chars);

			// ALIGN RIGHT
			if (pos == ALIGN_RIGHT) {
				pos = _width - pixels;
			}

			// ALIGN CENTER
			else {
				pos = (_width - pixels) / 2;
			}
		}
	}


	_cursor = pos;

	// iterate text
	for (i = 0;i < chars; i++) {

		uint16_t offset = text[i] * 9;
		uint8_t kerning = charTable[offset] & 0x0F;


		// write character into display
		for (int col = 0; col < kerning; col++) {

			// check for display end > end output
			if (_cursor >= _width) {
				break;
			}

			uint8_t data = pgm_read_byte(charTable + offset + col + 1);

			// save in buffer
			_buffer[_cursor] = data;


			// increment cursor
			_cursor ++;
		}

		// add space after character
		_cursor ++;
	}

	// send to display
	// write only data between start and end
	end = _cursor - 1;

	_send(pos, end);
}


// get text length
uint8_t LED_MATRIX::length(char* text, uint8_t length) {

	uint8_t kerning;
	uint8_t spacing;
	uint8_t l = 0;

	// iterate characters
	// add sizes
	for (uint8_t i = 0; i < length; i++) {

		uint16_t offset = text[i] * 9;
		kerning = pgm_read_byte(charTable + offset) & 0x0F;
		spacing = pgm_read_byte(charTable + offset) > 4;

		l += kerning + spacing;
	}

	return l - spacing;
}


// clear all arrays
void LED_MATRIX::clear(void) {

	for (int contr = 0; contr < _controller_count; contr++) {

		for (int module = 0; module < _count; module++) {
			clear(_port[contr], module);
		}
	}

	_cursor = 0;
}


// clear display with module
void LED_MATRIX::clear(uint8_t port, uint8_t address) {

	for (int i = 0; i < 8; i++) {
		_write(port, address, OP_DIGIT0 + i, 0);
	}
}


// send data
void LED_MATRIX::_send(uint8_t start, uint8_t end) {

	#ifdef DEBUG
		Serial.print("send data to display (");
		Serial.print(start);
		Serial.print("-");
		Serial.print(end);
		Serial.println(")");
	#endif


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