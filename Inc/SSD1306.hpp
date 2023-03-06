/**
 ******************************************************************************
 * @file    SSD1306.hpp
 * @author  Olivier Van den Eede - ovde.be
 * @author  Rafał Mazurkiewicz
 * @date    13.08.2019
 * @brief   Library for OLED display
 ******************************************************************************
 * <h2><center>&copy;
 * Original work COPYRIGHT (c) 2016 Olivier Van den Eede - ovde.be
 * Modified work COPYRIGHT(c) 2019 Rafał Mazurkiewicz
 * </center></h2>
 *
 *Permission is hereby granted, free of charge,
 *to any person obtaining a copy of this software and associated documentation files
 *(the "Software"), to deal in the Software without restriction,
 *including without limitation the rights to use, copy, modify,
 *merge, publish, distribute, sublicense, and/or sell copies of
 *the Software, and to permit persons to whom the Software is furnished to do so,
 *subject to the following conditions:
 *
 *The above copyright notice and this permission notice shall be included in
 *all copies or substantial portions of the Software.
 *
 *THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 *INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
 *PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE
 *FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 *OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 *OTHER DEALINGS IN THE SOFTWARE.
 *******************************************************************************
 */

/**
 * @mainpage
 * Library for controlling OLED displays based on SSD1306 drivers.
 * ===============================================================
 * It is based on library made by Olivier Van den Eede.
 * One of the main modifications include separation of hardware related functions.
 * Additionally there are some extra functions not present in original work
 * It is developed with stm32 and I2C in mind, but can be easily modified for other microcontrollers,
 * by modifying SSD1306_hardware.cpp and SSD1306_hardware_conf.hpp files only
 *
 *
 */

#ifndef SSD1306_HPP_
#define SSD1306_HPP_

#include <string>
#include <array>
#include <stdint.h>
#include "fonts.h"
#include "SSD1306_hardware_conf.hpp"

/*! @class SSD1306
 *  @brief This class is controlling display.
 */
class SSD1306
{
public:
	/// This enum is for setting hardware configuration. Devices from different vendors can have
	/// OLED segments internally mapped to different SSD1306 pins.
	/// Try ALT_NOREMAP for 128x64, and  SEQ_NOREMAP for 128x32 as a rule of thumb.
	enum HardwareConf : uint8_t
	{
		SEQ_NOREMAP = 0x02,
		SEQ_REMAP = 0x22,
		ALT_NOREMAP = 0x12,
		ALT_REMAP = 0x32
	};

	/// Enum for colors. White means pixel is ON.
	enum Color : uint8_t
	{
		BLACK = 0, WHITE = 0xff
	};

	/**@brief Constructor configure class. If height>64 last error=0xff;.
	 * @param connection_port: I2C class object for HW connection.
	 * @param screen_height: height in pixels.
	 * @param hardware_configuration: Can be a value of SSD1306::HardwareConf.
	 * @param device_address: address of device on I2C line. usually 0x78 is OK.
	 */
    SSD1306(SSD1306_I2C_Typedef *connection_port, const uint8_t screen_height,
            HardwareConf hardware_configuration = ALT_NOREMAP,
            uint8_t device_address = 0x78) :
            conn(connection_port), height(screen_height),
                    hard_conf(hardware_configuration), address(device_address)
    {
        if (screen_height > 64)
        {
            last_error = 0xff;
        }
    }

	/**@brief Initialize device, cleans display
	 * @retval True if initialized without errors.
	 */
	bool Initialize(void);

	/**@brief This function have to be called to refresh screen with new data
	 */
	void Update_Screen(void);

	/**@brief Cleans display. Synonymous to calling "Fill(BLACK);"
	 */
	void Clean(void);

	/**@brief Fill whole display with one color (turn on or off pixels)
	 * @param color: Black means display is OFF.
	 */
	void Fill(SSD1306::Color color);

	/**@brief set cursor to given coordinates
	 * @param x: X Coordinate
	 * @param y: Y Coordinate
	 * @note Coordinates goes from top to bottom and left right x=0,y=0 means top left corner
	 */
	void Set_Cursor(uint8_t x, uint8_t y);

	/**@brief Writes normal string at coordinates set in SSD1306::Set_Cursor.
	 * @param str: string to be written
	 */
	void Write_String(char const *str);

	/**@brief Similar function to Write_String but fills the background with color.
	 * @param str: string to be written
	 */
	void Write_String_Inverted(char const *str);

	/**@brief Turns ON single pixel at given coordinate.
	 * @param x: X Coordinate
	 * @param y: Y Coordinate
	 * @param c: Color to draw
	 */
	void Draw_Pixel(uint8_t x, uint8_t y, SSD1306::Color c);

	/**@brief Copy Image to internal buffer.
	 * @param image: array of size 128*64=1024(\a buffer_size) containing image
	 * @note if image size is too big additional bytes will be ignored
	 */
	void Draw_Image(const uint8_t *image);

	/**@brief Draws Horizontal line
	 * @param x: X Coordinate
	 * @param y: Y Coordinate
	 * @param width: width of line (in pixels)
	 * @param c: Color to draw
	 * @note Moves from left to right
	 */
	void Draw_Line_H(uint8_t x, uint8_t y, uint8_t width, SSD1306::Color c);

	/**@brief Draws Vertical line
	 * @param x: X Coordinate
	 * @param y: Y Coordinate
	 * @param height: height of line (in pixels)
	 * @param c: Color to draw
	 * @note Moves from up to bottom
	 */
	void Draw_Line_V(uint8_t x, uint8_t y, uint8_t height, SSD1306::Color c);

	/**@brief Draws Square
	 * @param x: X Coordinate
	 * @param y: Y Coordinate
	 * @param x2: end of square Y Coordinate
	 * @param y2: end of square Y Coordinate
	 * @param c: Color to draw
	 * @note Moves from up to bottom and left to right
	 */
	void Draw_Square(uint8_t x, uint8_t y, uint8_t x2, uint8_t y2,
			SSD1306::Color c);

	/**@brief Draws Waveform in a plot-like fashion
	 * @param x: X Start Coordinate
	 * @param y: Y Start Coordinate
	 * @param buffer: pointer to buffer containing waveform data
	 * @param size: length of buffer array
	 * @param c: Color to draw
	 * @note Moves from top to bottom and left to right (value of 0 in buffer data results in
	 *  drawing pixels in the given \a y coordinates)
	 * @warning It does not check inputs - User should ensure that buffer<y and size<x!
	 */
	void Draw_Waveform(uint8_t x, uint8_t y, uint8_t *buffer, uint8_t size,
			SSD1306::Color c);

	/**@brief Put displays in Sleep mode.
	 */
	void Display_Off(void);

	/**@brief Wake up display.
	 */
	void Display_On(void);

	/**@brief Sets brightness of display.
	 * @param brightness: brightness- 0xff means full lit.
	 */
	void Set_Brightness(uint8_t brightness);

	/**@brief Function for inverting colors.
	 * @param inverted: TRUE- colors are inverted.
	 */
	void Invert_Colors(bool inverted);

	/**@brief Function for flip upside down display.
	 * @param flipped: TRUE- display is flipped.
	 */
	void Flip_Screen(bool flipped);

	/**@brief Function for mirroring (left- right) display.
	 * @param mirrored: TRUE- display is mirrored.
	 */
	void Mirror_Screen(bool mirrored);

	/**@brief Sets size of font.
	 * @param font: Can be a value of Fonts::FontDef.
	 */
	void Set_Font_size(Fonts::FontDef font);

	/**@brief Informs if device is initialized
	 * @retval True if initialized without errors.
	 */
	bool IsInitialized(void) const;

	/**@brief Returns error of underlying I2C interface
	 * @retval 0 if none error occured since last SSD1306::Clean_Errors call.
	 */
	int Get_Last_Error(void) const;

	/**@brief Clean errors
	 */
	void Clean_Errors(void);

private:
	SSD1306_I2C_Typedef *conn;
	const uint8_t height;
	const uint8_t width = 128;
	const uint8_t hard_conf;
	const uint8_t address;

	Fonts::FontDef font = Fonts::font_7x10;  ///<font size

	const static uint32_t buffer_size = 64 / 8 * 128; ///< size of internal buffer. Can be lower if used ONLY with 128x32
	std::array<uint8_t, buffer_size> buffer; ///<internal buffer used for displaying data
	bool isinitialized = false;
	int last_error = 0;

	const uint8_t control_b_command = 0x00; ///<required for I2C to indicate type of message
	const uint8_t control_b_data = 0x40; ///<required for I2C to indicate type of message

	struct
	{
		uint8_t X;
		uint8_t Y;
	} Coordinates;

	/**@brief HW related sends command thru I2C interface.
	 * @param command: byte to send.
	 */
	void Write_Command(uint8_t command);

	/**@brief HW related sends command thru I2C interface.
	 * @param data: byte array size of SSD1306::buffer_size to send.
	 */
	void Write_Data(std::array<uint8_t, buffer_size> &data);

    /**@brief Used internaly by \ref Write_String to draw one character at the time
     * @param chr: character
     * @param color: Color of character
     */
	void Write_Char(char chr, SSD1306::Color color);
};

#endif /* SSD1306_HPP_ */
