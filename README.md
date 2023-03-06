# About 
This is a modified version of library from [https://github.com/4ilo/ssd1306-stm32HAL](https://github.com/4ilo/ssd1306-stm32HAL) which is made by Olivier Van den Eede.

I have ported this library to C++, separeted hardware related functions to different files, added some functions for primitive drawings (like draw horizontal line etc.) 
and for display controll (flip screen etc.). 

## Usage

Small example:

```
#include "ssd1306.h"

I2C_HandleTypeDef hi2c1; // from stm32 HAL library

SSD1306 oled(&hi2c1, 64);
// At this point I2C interface should be initialized by user.
oled.Initialize();
oled.Set_Cursor(0, 0);
oled.Set_Font_size(Fonts::font_16x26);
oled.Write_String("Hello!");
oled.Update_Screen();

// Copy all data from local screenbuffer to the screen
oled.Update_Screen();
```
I2C interface have to be initialized before using this library

### Using 128x32 displays

Some vendors provides displays with different internal hardware configuration so if your displays shows some artefacts, try using e.g.
```
SSD1306 oled(&hi2c1, 32, SSD1306::SEQ_NOREMAP);
```
### File structure and font

Files under *Inc*, *Src* and *Hardware* directories are compulsory. In *Examples* there's a simple showcase. *Tests* directory contains unit tests in catch2 framework for development machine. 
You can safely ommit *Examples*, *Tests*, *docs*.

If you need a font generator to add custom fonts you can find it here: [the-this-pointer/glcd-font-calculator](https://github.com/the-this-pointer/glcd-font-calculator).
However maximal font width is hardcoded to 16.

## Porting to other microcontroller or HAL library

Only *SSD1306_hardware.cpp* and *SSD1306_hardware_conf.hpp* files have to be modified. 
*SSD1306_hardware_conf.hpp* holds type definition of underlying connection socket (be this I2C or SPI) and include header of HAL library.
*SSD1306_hardware.cpp* provides 2 functions to write command and data into displays controller. You can use constant member `control_b_data` and
`control_b_command` to indicate type of message (this is memory address). 

