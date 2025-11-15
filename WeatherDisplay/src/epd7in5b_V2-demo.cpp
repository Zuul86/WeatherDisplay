#include <Arduino.h>
#include <WiFi.h>
#include "time.h"
#include "DEV_Config.h"
#include "EPD.h"
#include "GUI_Paint.h"
#include <stdlib.h>

// Private method declarations
void syncTimeWithNTP();
void initializeDisplay();
void createImageBuffers();
void weatherDisplayDemo();

void drawCurrentConditions(int margin);
void drawShapes();
void drawLocalHeader(int margin);
void demonstratePartialRefresh();
void drawBorders(int margin);
void partiallyRefreshCurrentConditions();
void cleanupDisplay();

UBYTE *BlackImage = NULL, *RYImage = NULL;
UWORD Imagesize = 0;

// --- WiFi and NTP Configuration ---
const char* ssid = "";
const char* password = "";

const char* ntpServer = "pool.ntp.org";
// Update with your timezone offset in seconds. E.g., PST is UTC-8 -> -8 * 3600 = -28800
const long gmtOffset_sec = 0; 
const int daylightOffset_sec = 3600; // For daylight saving

void setup()
{
  DEV_Module_Init();

  syncTimeWithNTP();

  initializeDisplay();
  createImageBuffers();

#if 1
  //drawShapes();
#endif

#if 1
  weatherDisplayDemo();
#endif

#if 1
  partiallyRefreshCurrentConditions();
#endif

#if 1
  //demonstratePartialRefresh();
#endif

  //cleanupDisplay();
}

void syncTimeWithNTP()
{
  printf("Connecting to WiFi: %s\n", ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    printf(".");
  }
  printf("\nWiFi connected.\n");

  // Init and get the time
  printf("Syncing time with NTP server...\n");
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    printf("Failed to obtain time\n");
    return;
  }
  printf("Time synced successfully.\n");
  WiFi.disconnect(true);
  printf("WiFi disconnected.\n");
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

  // --- Add Date and Time ---
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    printf("Failed to obtain time for display\n");
    return;
  }

  PAINT_TIME sPaint_time;
  sPaint_time.Year = timeinfo.tm_year + 1900; // Year is offset from 1900
  sPaint_time.Month = timeinfo.tm_mon + 1;    // Month is 0-11
  sPaint_time.Day = timeinfo.tm_mday;
  sPaint_time.Hour = timeinfo.tm_hour;
  sPaint_time.Min = timeinfo.tm_min;
  sPaint_time.Sec = timeinfo.tm_sec;
  Paint_DrawDateTime(10 + margin, 30 + margin, &sPaint_time, &Font12, WHITE, BLACK);
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
    Paint_DrawDateTime(0, 0, &sPaint_time, &Font20, WHITE, BLACK);
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

void partiallyRefreshCurrentConditions()
{
  printf("Starting partial refresh for current conditions...\r\n");
  EPD_7IN5B_V2_Init_Part();

  // Define the area for current conditions
  const int margin = 20;
  UWORD x_start = 10 + margin;
  UWORD y_start = 70 + margin;
  UWORD y_end = 160 + margin;

  // Calculate the end coordinate based on the column layout
  const UWORD x_end = (EPD_7IN5B_V2_WIDTH / 4);
  UWORD area_width = x_end - x_start;
  const UWORD area_height = y_end - y_start;

  // Create a smaller buffer for the partial update area
  UWORD partial_imagesize = ((area_width % 8 == 0) ? (area_width / 8) : (area_width / 8 + 1)) * area_height;
  UBYTE *PartialImage;
  if ((PartialImage = (UBYTE *)malloc(partial_imagesize)) == NULL)
  {
    printf("Failed to apply for partial memory...\r\n");
    return;
  }

  Paint_NewImage(PartialImage, area_width, area_height, 0, WHITE);
  Paint_SelectImage(PartialImage);
  Paint_Clear(WHITE);

  // Simulate data updates
  int temp = 55;
  for (int i = 0; i < 5; i++)//this is the cadence of updates
  {
    char temp_str[10];
    sprintf(temp_str, "%d F", temp + i);

    Paint_Clear(WHITE); // Clear the small buffer
    // Draw into the small buffer at relative coordinates (0,0)
    Paint_DrawString_EN(0, 0, temp_str, &Font32, WHITE, BLACK);
    Paint_DrawString_EN(0, 40, "Humidity: 45%", &Font12, WHITE, BLACK);
    Paint_DrawString_EN(0, 60, "Pressure: 5 mb", &Font12, WHITE, BLACK);

    printf("Updating temperature to %s\r\n", temp_str);
    EPD_7IN5B_V2_Display_Partial(PartialImage, x_start, y_start, x_end, y_end);
    DEV_Delay_ms(3000);
  }

  free(PartialImage);
}

/* The main loop -------------------------------------------------------------*/
void loop()
{
  //
}