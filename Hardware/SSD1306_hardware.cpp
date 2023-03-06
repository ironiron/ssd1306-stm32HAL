/**
 ******************************************************************************
 * @file    SSD1306_hardware.cpp
 * @author  Rafał Mazurkiewicz
 * @date    13.08.2019
 * @brief   This file contains hardware related functions
 ******************************************************************************
 * @attention
 * <h2><center>&copy; COPYRIGHT(c) 2019 Rafał Mazurkiewicz </center></h2>
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

#include <stdint.h>
#include <array>
#include "SSD1306.hpp"

void SSD1306::Write_Command(uint8_t command)
{
    auto temp = HAL_I2C_Mem_Write(conn, address, control_b_command, 1, &command,
            1, 1000);
    if (temp != 0)
    {
        last_error = temp;
    }
}

void SSD1306::Write_Data(std::array<uint8_t, SSD1306::buffer_size> &data)
{
    auto temp = HAL_I2C_Mem_Write(conn, address, control_b_data, 1,
            data.begin(), height * (width) / 8, 1000);
    if (temp != 0)
    {
        last_error = temp;
    }
}
