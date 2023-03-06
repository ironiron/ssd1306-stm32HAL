/**
 ******************************************************************************
 * @file    SSD1306.hpp
 * @author  Rafał Mazurkiewicz
 * @date    13.08.2019
 * @brief   Unit test for oled display driver
 ******************************************************************************
 * <h2><center>&copy; COPYRIGHT(c) 2019 Rafał Mazurkiewicz
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

#include <vector>
#include "catch.hpp"
#include "image.hpp"
#include "SSD1306.hpp"
#include "testing.hpp"


void *dummy;
SSD1306 oled64(&dummy,64);

TEST_CASE( "fills screen buffer within bounds")
{
  testing::ssd1306::data.clear(); // TODO refractor those initial cleanings

  oled64.Fill(SSD1306::Color::WHITE);
  oled64.Update_Screen();

  REQUIRE(testing::ssd1306::data.size()==1030);//1024 data+ 6 bytes of commands

  REQUIRE(testing::ssd1306::data[0]==0x21);//commands
  REQUIRE(testing::ssd1306::data[1]==0);
  REQUIRE(testing::ssd1306::data[2]==127);
  REQUIRE(testing::ssd1306::data[3]==0x22);
  REQUIRE(testing::ssd1306::data[4]==0);
  REQUIRE(testing::ssd1306::data[5]==7);

  REQUIRE(testing::ssd1306::data[6]==0xff);//data
  REQUIRE(testing::ssd1306::data[50]==0xff);
  REQUIRE(testing::ssd1306::data[100]==0xff);
  REQUIRE(testing::ssd1306::data[1029]==0xff);//last data
}

TEST_CASE( "draws pixels")
{
  testing::ssd1306::data.clear();

  oled64.Clean();
  oled64.Draw_Pixel(0,0,SSD1306::Color::WHITE);
  oled64.Draw_Pixel(1,3,SSD1306::Color::WHITE);
  oled64.Draw_Pixel(0,8,SSD1306::Color::WHITE);
  oled64.Draw_Pixel(127,63,SSD1306::Color::WHITE);
  oled64.Update_Screen();

  REQUIRE(testing::ssd1306::data.size()==1030);//1024 data+ 6 bytes of commands

  REQUIRE(testing::ssd1306::data[5]==7);//last byte of commands
  REQUIRE(testing::ssd1306::data[6]==0x01);//data
  REQUIRE(testing::ssd1306::data[7]==0x08);
  REQUIRE(testing::ssd1306::data[8]==0);
  REQUIRE(testing::ssd1306::data[6+128]==0x01);
  REQUIRE(testing::ssd1306::data[6+127]==0);
  REQUIRE(testing::ssd1306::data[6+129]==0);
  REQUIRE(testing::ssd1306::data[1029]==0x80);//last data

}

TEST_CASE( "writes character")
{
  testing::ssd1306::data.clear();

  oled64.Clean();
  oled64.Set_Font_size(Fonts::font_7x10);
  oled64.Write_String("8");
  oled64.Update_Screen();

  //This is from font.hpp file
  //0x3800, 0x4400, 0x4400, 0x3800, 0x4400, 0x4400, 0x4400, 0x3800, 0x0000, 0x0000,
  //which is displayed on oled as:
  //	0011100
  //	0100010
  //	0100010
  //	0011100
  //	0100010
  //	0100010
  //	0100010
  //	0011100
  //	0000000
  //	0000000

  REQUIRE(testing::ssd1306::data.size()==1030);//1024 data+ 6 bytes of commands

  REQUIRE(testing::ssd1306::data[5]==7);//last byte of commands
  REQUIRE(testing::ssd1306::data[6]==0b0);//data
  REQUIRE(testing::ssd1306::data[7]==0b01110110);
  REQUIRE(testing::ssd1306::data[8]==0b10001001);
  REQUIRE(testing::ssd1306::data[9]==0b10001001);
  REQUIRE(testing::ssd1306::data[10]==0b10001001);
  REQUIRE(testing::ssd1306::data[11]==0b01110110);
  REQUIRE(testing::ssd1306::data[12]==0b0);
  REQUIRE(testing::ssd1306::data[6+128]==0b0);//next line (2 bits evalate on next row)
  REQUIRE(testing::ssd1306::data[7 + 128] == 0b0);
  REQUIRE(testing::ssd1306::data[8 + 128] == 0b0);
  REQUIRE(testing::ssd1306::data[9 + 128] == 0b0);
  REQUIRE(testing::ssd1306::data[10 + 128] == 0b0);
  REQUIRE(testing::ssd1306::data[11 + 128] == 0b0);
  REQUIRE(testing::ssd1306::data[12 + 128] == 0b0);
}

TEST_CASE( "Draw Image")
{
  testing::ssd1306::data.clear();

  oled64.Clean();
  oled64.Draw_Image(Tables::sandals);
  oled64.Update_Screen();

  REQUIRE(testing::ssd1306::data.size()==1030);//1024 data+ 6 bytes of commands

  REQUIRE(testing::ssd1306::data[5]==7);//last byte of commands

  for (uint32_t i=0;i<1023;i++)
    {
      REQUIRE(testing::ssd1306::data[6+i]==Tables::sandals[i]);
    }
}

TEST_CASE( "Draw horizontal line")
{
  testing::ssd1306::data.clear();

  oled64.Clean();
  oled64.Draw_Line_H(2,3,10,SSD1306::Color::WHITE);
  oled64.Update_Screen();

  REQUIRE(testing::ssd1306::data.size()==1030);//1024 data+ 6 bytes of commands

  REQUIRE(testing::ssd1306::data[5]==7);//last byte of commands
  REQUIRE(testing::ssd1306::data[6]==0);
  REQUIRE(testing::ssd1306::data[7]==0);
  REQUIRE(int(testing::ssd1306::data[8])==0x08);
  REQUIRE(testing::ssd1306::data[9]==0x08);
  REQUIRE(testing::ssd1306::data[10]==0x08);
  REQUIRE(testing::ssd1306::data[11]==0x08);
  REQUIRE(testing::ssd1306::data[12]==0x08);
  REQUIRE(testing::ssd1306::data[13]==0x08);
  REQUIRE(testing::ssd1306::data[14]==0x08);
  REQUIRE(testing::ssd1306::data[15]==0x08);
  REQUIRE(testing::ssd1306::data[16]==0x08);
  REQUIRE(testing::ssd1306::data[17]==0x08);
  REQUIRE(testing::ssd1306::data[18]==0);

}

TEST_CASE( "Draw Vertical line")
{
  testing::ssd1306::data.clear();

  oled64.Clean();
  oled64.Draw_Line_V(1,1,5,SSD1306::Color::WHITE);
  oled64.Update_Screen();

  REQUIRE(testing::ssd1306::data.size()==1030);//1024 data+ 6 bytes of commands

  REQUIRE(testing::ssd1306::data[5]==7);//last byte of commands
  REQUIRE(testing::ssd1306::data[6]==0);
  REQUIRE(testing::ssd1306::data[7]==0b00111110);
  REQUIRE(testing::ssd1306::data[8]==0);


}

TEST_CASE( "Draw Square")
{
  testing::ssd1306::data.clear();

  oled64.Clean();
  oled64.Draw_Square(1,1,4,5,SSD1306::Color::WHITE);
  oled64.Draw_Square(6,0,6,0,SSD1306::Color::WHITE);//should draw one pixel
  oled64.Update_Screen();

  REQUIRE(testing::ssd1306::data.size()==1030);//1024 data+ 6 bytes of commands

  REQUIRE(testing::ssd1306::data[5]==7);//last byte of commands
  REQUIRE(testing::ssd1306::data[6]==0);
  REQUIRE(testing::ssd1306::data[7]==0b00111110);
  REQUIRE(testing::ssd1306::data[8]==0b00100010);
  REQUIRE(testing::ssd1306::data[9]==0b00100010);
  REQUIRE(testing::ssd1306::data[10]==0b00111110);
  REQUIRE(testing::ssd1306::data[11]==0);
  REQUIRE(testing::ssd1306::data[12]==0x01);
  REQUIRE(testing::ssd1306::data[13]==0);
}

TEST_CASE( "Draw Waveform")
{
  uint8_t waveform[]={1,3,4,0,7};
  testing::ssd1306::data.clear();

  oled64.Clean();
  oled64.Draw_Waveform(1,7,waveform,5,SSD1306::Color::WHITE);
  oled64.Update_Screen();

  REQUIRE(testing::ssd1306::data.size()==1030);//1024 data+ 6 bytes of commands

  REQUIRE(testing::ssd1306::data[5]==7);//last byte of commands
  REQUIRE(testing::ssd1306::data[6]==0);
  REQUIRE(testing::ssd1306::data[7]==0b01000000);
  REQUIRE(testing::ssd1306::data[8]==0b00010000);
  REQUIRE(testing::ssd1306::data[9]==0b00001000);
  REQUIRE(testing::ssd1306::data[10]==0b10000000);
  REQUIRE(testing::ssd1306::data[11]==0b00000001);
  REQUIRE(testing::ssd1306::data[12]==0);
}
