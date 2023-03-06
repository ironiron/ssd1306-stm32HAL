/**
 ******************************************************************************
 * @file    SSD1306.cpp
 * @author  Olivier Van den Eede - ovde.be
 * @author  Rafał Mazurkiewicz
 * @date    13.08.2019
 * @brief   Library for OLED display
 ******************************************************************************
 * @attention
 * <h2><center>&copy; COPYRIGHT(c) </center></h2>
 *
 * Original work COPYRIGHT (c) 2016 Olivier Van den Eede - ovde.be
 * Modified work COPYRIGHT(c) 2019 Rafał Mazurkiewicz
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
#include "SSD1306.hpp"

bool SSD1306::Initialize(void)
{
    Display_Off();
    Write_Command(0xD5); //--set display clock divide ratio/oscillator frequency
    Write_Command(0x80); //--set divide ratio  <default value> //*
    Write_Command(0xA8); //--set multiplex ratio(1 to 64) (display height)
    Write_Command(height - 1);
    Write_Command(0xD3); //-set display offset
    Write_Command(0x00); //-no offset
    Write_Command(0x40); //--set start line address
    Write_Command(0x8D); //--set DC-DC enable
    Write_Command(0x14); //	enable charge pump
    Mirror_Screen(false);
    Flip_Screen(false);
    Write_Command(0xDA); //--set com pins hardware configuration
    Write_Command(hard_conf);
    Set_Brightness(150);
    Write_Command(0xD9); //--set pre-charge period
    Write_Command(0x22); //can be 0xf1 if not working.
    Write_Command(0xDB); //--set vcomh
    Write_Command(0x40); //0x20,0.77xVcc
    Write_Command(0xA4); //0xa4,Output follows RAM content;0xa5,Output ignores RAM content
    Invert_Colors(false);
    Write_Command(0x20); //Set Memory Addressing Mode
    Write_Command(0x00); //00,Horizontal Addressing Mode;01,Vertical Addressing Mode;10,Page Addressing Mode (RESET);11,Invalid

    Write_Command(0x21); //Column address
    Write_Command(0x00);
    Write_Command(width - 1);
    Write_Command(0x22); //Page address
    Write_Command(0x00);
    Write_Command((height / 8) - 1);

    Display_On();
    Clean();
    Update_Screen();

    if (last_error == 0)
    {
        isinitialized = true;
    }
    else
    {
        isinitialized = false;
    }

    return isinitialized;
}

void SSD1306::Clean(void)
{
    Fill(BLACK);
}

void SSD1306::Update_Screen(void)
{
    Write_Command(0x21); //Column address
    Write_Command(0x00);
    Write_Command(127);

    Write_Command(0x22); //Page address
    Write_Command(0x00);
    Write_Command((height / 8) - 1);

    Write_Data(buffer);
}

void SSD1306::Fill(SSD1306::Color color)
{
    if (color == Color::BLACK)
    {
        for (auto &b : buffer)
        {
            b = Color::BLACK;
        }
    }
    else
    {
        for (auto &b : buffer)
        {
            b = Color::WHITE;
        }
    }
}

void SSD1306::Write_String(char const *str)
{
    int i = 0;
    while (str[i])
    {
        Write_Char(str[i], Color::WHITE);
        i++;
    }
}

void SSD1306::Write_String_Inverted(char const *str)
{
    int i = 0;
    while (str[i])
    {
        Write_Char(str[i], Color::BLACK);
        i++;
    }
}

int SSD1306::Get_Last_Error(void) const
{
    return last_error;
}

void SSD1306::Clean_Errors(void)
{
    last_error = 0;
}

void SSD1306::Write_Char(char chr, SSD1306::Color color)
{
    for (uint8_t y = 0; y < font.FontHeight; y++)
    {
        uint16_t row = font.data[(chr - 32) * font.FontHeight + y];
        for (uint8_t x = 0; x < font.FontWidth; x++)
        {
            if (color == Color::BLACK)
            {
                if ((row << x) & 0x8000)
                {
                    Draw_Pixel(Coordinates.X + x, (Coordinates.Y + y), BLACK);
                }
                else
                {
                    Draw_Pixel(Coordinates.X + x, (Coordinates.Y + y), WHITE);
                }
            }
            else
            {
                if ((row << x) & 0x8000)
                {
                    Draw_Pixel(Coordinates.X + x, (Coordinates.Y + y), WHITE);
                }
                else
                {
                    Draw_Pixel(Coordinates.X + x, (Coordinates.Y + y), BLACK);
                }
            }
        }
    }
    Coordinates.X += font.FontWidth;
}

void SSD1306::Set_Font_size(Fonts::FontDef font)
{
    this->font = font;
}

void SSD1306::Set_Brightness(uint8_t brightness)
{
    Write_Command(0x81);
    Write_Command(brightness);
}

void SSD1306::Draw_Pixel(uint8_t x, uint8_t y, SSD1306::Color c)
{
    if (x >= width || y >= height)
    {
        // Don't write outside the buffer
        return;
    }

    if (c == WHITE)
    {
        buffer[x + width * (y / 8)] |= uint8_t(1 << (y % 8));
    }
    else
    {
        buffer[x + width * (y / 8)] &= uint8_t(~ (1 << (y % 8)));
    }
}

void SSD1306::Draw_Line_H(uint8_t x, uint8_t y, uint8_t width, SSD1306::Color c)
{
    for (uint8_t i = 0; i < width; i++)
    {
        Draw_Pixel(x + i, y, c);
    }
}

void SSD1306::Draw_Line_V(uint8_t x, uint8_t y, uint8_t height,
        SSD1306::Color c)
{
    for (uint8_t i = 0; i < height; i++)
    {
        Draw_Pixel(x, y + i, c);
    }
}

void SSD1306::Draw_Square(uint8_t x, uint8_t y, uint8_t x2, uint8_t y2,
        SSD1306::Color c)
{
    Draw_Line_H(x, y, (uint8_t) (x2 - x + 1), c);
    Draw_Line_H(x, y2, (uint8_t) (x2 - x + 1), c);

    Draw_Line_V(x, y, (uint8_t) (y2 - y + 1), c);
    Draw_Line_V(x2, y, (uint8_t) (y2 - y + 1), c);
}

void SSD1306::Draw_Waveform(uint8_t x, uint8_t y, uint8_t *buffer, uint8_t size,
        SSD1306::Color c)
{
    for (uint8_t i = 0; i < size; i++)
    {
        Draw_Pixel(i + x, y - buffer[i], c);
    }
}

void SSD1306::Display_Off(void)
{
    Write_Command(0xAE);
}

void SSD1306::Display_On(void)
{
    Write_Command(0xAF);
}

void SSD1306::Flip_Screen(bool flipped)
{
    if (flipped == false)
    {
        Write_Command(0xC8); //Set COM Output Scan Direction
    }
    else
    {
        Write_Command(0xC0);
    }
}

void SSD1306::Invert_Colors(bool inverted)
{
    if (inverted == true)
    {
        Write_Command(0xA7); //inverted colours
    }
    else
    {
        Write_Command(0xA6); //normal colours
    }

}

void SSD1306::Draw_Image(const uint8_t *image)
{
    for (uint32_t i = 0; i < buffer_size; i++)
    {
        buffer[i] = image[i];
    }
}

void SSD1306::Set_Cursor(uint8_t x, uint8_t y)
{
    if (x >= width)
    {
        x = width;
    }
    if (y >= height)
    {
        y = height;
    }
    Coordinates.X = x;
    Coordinates.Y = y;
}

void SSD1306::Mirror_Screen(bool mirrored)
{
    if (mirrored == 0) //--set segment re-map 0 to 127
    {
        Write_Command(0xA1);
    }
    else
    {
        Write_Command(0xA0);
    }
}

bool SSD1306::IsInitialized(void) const
{
    return isinitialized;
}
