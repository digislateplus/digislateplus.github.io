#include "led_matrix.h"


void LED_MATRIX::begin(SPIClass* spi, uint8_t width, uint8_t height) {

	_width = width;
	_height = height;

	_count = _width / 8;
	
	_controller_count = 0;

	home();
}


// home cursor
void LED_MATRIX::home(void) {
	_cursor = 0;
}


// add controller port to list
void LED_MATRIX::add_controller(uint8_t port) {
	if (_controller_count < MATRIX_CONTROLLERS) {
		_port[_controller_count++] = port;
	}
}


// send data
void LED_MATRIX::_send(uint8_t addr, uint8_t opcode, uint8_t data) {

	// //Create an array with the data to shift out
	// int offset = addr*2;
	// int maxbytes = _count * 2;

	// // clear output data
	// for(_i = 0; _i < _count; _i++) {
	// 	_spidata[_i] = (byte)0;
	// }

	// // put device data into the array
	// _spidata[offset+1] = opcode;
	// _spidata[offset] = data;

	// // ==============
	// // enable the line 
	// digitalWrite(_load_pin,LOW);

	// // shift out the data 
	// for(_i = maxbytes; _i > 0; _i--) {
	// 	_spi->transfer(_spidata[_i-1]);
	// }

	// // latch the data onto the display
	// digitalWrite(_load_pin,HIGH); 
	// // ==============
}



// print text starting at x = 0
void LED_MATRIX::print(char* text, uint8_t length) {
	print(text, length, 0);
}


// print text starting at position (0-64)
void LED_MATRIX::print(char* text, uint8_t length, uint8_t pos) {

	_cursor = pos;

	// iterate text
	for (int i = 0;i < length; i++) {

		uint16_t offset = text[i] * 9;
		uint8_t kerning = charTable[offset];

		// write character into display
		for (int col = 0; col < kerning; col++) {

			// check for display end > end output
			if (_cursor >= (_width)) {
				break;
			}

			uint8_t data = pgm_read_byte(charTable + offset + col + 1);
			_send(0, OP_DIGIT0 + col, data);

			// increment cursor
			_cursor ++;
		}

		// add space
		_cursor ++;
	}

	Serial.println();
}


// clear all arrays
void LED_MATRIX::clear(void) {

	for (int i = 0; i < _count; i++) {
		clear(i);
	}
}


void LED_MATRIX::clear(uint8_t address) {

	for (int col = 0; col < 8; col++) {
		_send(address, OP_DIGIT0 + col, 0);
	}

	_cursor = 0;
}


// set row of display
// void LED_MATRIX::_set_row(uint8_t address, uint8_t row, uint8_t data) {

// }