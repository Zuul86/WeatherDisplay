#include <Arduino.h>
#include "DEV_Config.h"
#include "EPD.h"
#include "GUI_Paint.h"
#include "ImageData.h"
#include <stdlib.h>

// Private method declarations
void initializeDisplay();
void createImageBuffers();
void showImageFromArray();
void weatherDisplayDemo();

void drawCurrentConditions(int margin);
void drawShapes();
void drawLocalHeader(int margin);
void demonstratePartialRefresh();
void drawBorders(int margin);
void cleanupDisplay();

UBYTE *BlackImage = NULL, *RYImage = NULL;
UWORD Imagesize = 0;

void setup()
{
  DEV_Module_Init();

  initializeDisplay();
  createImageBuffers();

#if 1
  //showImageFromArray();
#endif

#if 1
  //drawShapes();
#endif

#if 1
  weatherDisplayDemo();
#endif

#if 1
  //demonstratePartialRefresh();
#endif

  //cleanupDisplay();
}

void weatherDisplayDemo()
{
  const int margin = 20;
  EPD_7IN5B_V2_Init();

  Paint_Clear(WHITE);

  drawBorders(margin);
  drawLocalHeader(margin);
  drawCurrentConditions(margin);

  printf("EPD_Display\r\n");
  EPD_7IN5B_V2_Display(BlackImage, RYImage);
  DEV_Delay_ms(2000);
}

void drawLocalHeader(int margin)
{
  const int header_height = 50;

  // Draw header on black layer
  Paint_SelectImage(BlackImage);
  Paint_DrawString_EN(10 + margin, 10 + margin, "Local Weather", &Font16, WHITE, BLACK);
}

void drawBorders(int margin)
{
  const int split_point = EPD_7IN5B_V2_WIDTH / 4;

  // Draw both rectangles on the black layer
  Paint_SelectImage(BlackImage);
  
  Paint_DrawRectangle(margin, margin, split_point, EPD_7IN5B_V2_HEIGHT - margin,
                      BLACK, DOT_PIXEL_2X2, DRAW_FILL_EMPTY);
  Paint_DrawRectangle(split_point + margin, margin, EPD_7IN5B_V2_WIDTH - margin, EPD_7IN5B_V2_HEIGHT - margin,
                      BLACK, DOT_PIXEL_2X2, DRAW_FILL_EMPTY);

}


void initializeDisplay()
{
  printf("e-Paper Init and Clear...\r\n");
  EPD_7IN5B_V2_Init();
  EPD_7IN5B_V2_Clear();
  DEV_Delay_ms(500);
}

void createImageBuffers()
{
  Imagesize = ((EPD_7IN5B_V2_WIDTH % 8 == 0) ? (EPD_7IN5B_V2_WIDTH / 8) : (EPD_7IN5B_V2_WIDTH / 8 + 1)) * EPD_7IN5B_V2_HEIGHT;
  if ((BlackImage = (UBYTE *)malloc(Imagesize)) == NULL)
  {
    printf("Failed to apply for black memory...\r\n");
    while (1)
      ;
  }
  if ((RYImage = (UBYTE *)malloc(Imagesize)) == NULL)
  {
    printf("Failed to apply for red memory...\r\n");
    while (1)
      ;
  }

  printf("NewImage:BlackImage and RYImage\r\n");
  Paint_NewImage(BlackImage, EPD_7IN5B_V2_WIDTH, EPD_7IN5B_V2_HEIGHT, 0, WHITE);
  Paint_NewImage(RYImage, EPD_7IN5B_V2_WIDTH, EPD_7IN5B_V2_HEIGHT, 0, WHITE);

  Paint_SelectImage(BlackImage);
  Paint_Clear(WHITE);
  Paint_SelectImage(RYImage);
  Paint_Clear(WHITE);
}

void showImageFromArray()
{
  printf("show image for array\r\n");
  EPD_7IN5B_V2_Init_Fast();
  EPD_7IN5B_V2_Display(gImage_7in5_V2_b, gImage_7in5_V2_ry);
  DEV_Delay_ms(2000);
}

void drawShapes()
{
  EPD_7IN5B_V2_Init();

  // Draw black layer
  Paint_SelectImage(BlackImage);
  Paint_Clear(WHITE);
  Paint_DrawPoint(10, 80, BLACK, DOT_PIXEL_1X1, DOT_STYLE_DFT);
  Paint_DrawPoint(10, 90, BLACK, DOT_PIXEL_2X2, DOT_STYLE_DFT);
  Paint_DrawPoint(10, 100, BLACK, DOT_PIXEL_3X3, DOT_STYLE_DFT);
  Paint_DrawPoint(10, 110, BLACK, DOT_PIXEL_3X3, DOT_STYLE_DFT);
  Paint_DrawLine(20, 70, 70, 120, BLACK, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
  Paint_DrawLine(70, 70, 20, 120, BLACK, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
  Paint_DrawRectangle(20, 70, 70, 120, BLACK, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
  Paint_DrawRectangle(80, 70, 130, 120, BLACK, DOT_PIXEL_1X1, DRAW_FILL_FULL);
  Paint_DrawString_EN(10, 0, "waveshare", &Font16, BLACK, WHITE);
  Paint_DrawNum(10, 50, 987654321, &Font16, WHITE, BLACK);

  // Draw red layer
  Paint_SelectImage(RYImage);
  Paint_Clear(WHITE);
  Paint_DrawCircle(160, 95, 20, BLACK, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
  Paint_DrawCircle(210, 95, 20, BLACK, DOT_PIXEL_1X1, DRAW_FILL_FULL);
  Paint_DrawStar(160, 95, 20, BLACK);
  Paint_DrawLine(85, 95, 125, 95, BLACK, DOT_PIXEL_1X1, LINE_STYLE_DOTTED);
  Paint_DrawLine(105, 75, 105, 115, BLACK, DOT_PIXEL_1X1, LINE_STYLE_DOTTED);
  Paint_DrawString_EN(10, 20, "hello world", &Font12, WHITE, BLACK);
  Paint_DrawNum(10, 33, 123456789, &Font12, BLACK, WHITE);

  printf("EPD_Display\r\n");
  EPD_7IN5B_V2_Display(BlackImage, RYImage);
  DEV_Delay_ms(2000);
}

void demonstratePartialRefresh()
{
  EPD_7IN5B_V2_Init_Part();
  EPD_7IN5B_V2_Display_Base_color(WHITE);
  Paint_NewImage(BlackImage, Font20.Width * 7, Font20.Height, 0, WHITE);
  Debug("Partial refresh\r\n");
  Paint_SelectImage(BlackImage);
  Paint_Clear(WHITE);

  PAINT_TIME sPaint_time = {12, 34, 56};
  for (UBYTE num = 10; num > 0; num--)
  {
    sPaint_time.Sec = sPaint_time.Sec + 1;
    if (sPaint_time.Sec == 60)
    {
      sPaint_time.Min = sPaint_time.Min + 1;
      sPaint_time.Sec = 0;
      if (sPaint_time.Min == 60)
      {
        sPaint_time.Hour = sPaint_time.Hour + 1;
        sPaint_time.Min = 0;
        if (sPaint_time.Hour == 24)
        {
          sPaint_time.Hour = 0;
          sPaint_time.Min = 0;
          sPaint_time.Sec = 0;
        }
      }
    }
    Paint_ClearWindows(0, 0, Font20.Width * 7, Font20.Height, WHITE);
    Paint_DrawTime(0, 0, &sPaint_time, &Font20, WHITE, BLACK);
    EPD_7IN5B_V2_Display_Partial(BlackImage, 10, 130, 10 + Font20.Width * 7, 130 + Font20.Height);
    DEV_Delay_ms(500);
  }
}

void cleanupDisplay()
{
  printf("Clear...\r\n");
  EPD_7IN5B_V2_Init();
  EPD_7IN5B_V2_Clear();

  printf("Goto Sleep...\r\n");
  EPD_7IN5B_V2_Sleep();
  free(BlackImage);
  free(RYImage);
  BlackImage = NULL;
  RYImage = NULL;
}

void drawCurrentConditions(int margin)
{
  // Draw current conditions on black layer
  Paint_SelectImage(BlackImage);
  Paint_DrawString_EN(10 + margin, 70 + margin, "55 F", &Font32, WHITE, BLACK);
  Paint_DrawString_EN(10 + margin, 110 + margin, "Humidity: 45%", &Font12, WHITE, BLACK);
  Paint_DrawString_EN(10 + margin, 130 + margin, "Pressure: 5 mb", &Font12, WHITE, BLACK);
}

/* The main loop -------------------------------------------------------------*/
void loop()
{
  //
}