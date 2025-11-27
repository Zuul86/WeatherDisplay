#include <Arduino.h>
#include <WiFi.h>
#include "time.h"
#include "DEV_Config.h"
#include "EPD.h"
#include "GUI_Paint.h"
#include <stdlib.h>
#include "config.h"

// Private method declarations
void syncTimeWithNTP();
void initializeDisplay();
void createImageBuffers();
void weatherDisplayDemo();

void drawCurrentConditions(int margin, const char* temp, const char* humidity, const char* pressure);
void drawLocalHeader(int margin);
void drawBorders(int margin);
void cleanupDisplay();

UBYTE *BlackImage = NULL, *RYImage = NULL;
UWORD Imagesize = 0;

const char* ntpServer = "pool.ntp.org";
const int daylightOffset_sec = 3600; // For daylight saving

void setup()
{
  DEV_Module_Init();
  syncTimeWithNTP();

  initializeDisplay();
  createImageBuffers();

  weatherDisplayDemo();
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
  Paint_SelectImage(BlackImage);
  drawBorders(margin);
  drawLocalHeader(margin);
  EPD_7IN5B_V2_Display_Partial(BlackImage, 0, 0, EPD_7IN5B_V2_WIDTH, EPD_7IN5B_V2_HEIGHT);
  DEV_Delay_ms(5000);

  int temp = 55;
  for (int i = 0; i < 5; i++)//this is the cadence of updates
  {
    char temp_str[10];
    sprintf(temp_str, "%d F", temp + i);    
    
    // Define the specific area for the temperature string to be updated
    UWORD x_start = 10 + margin;
    UWORD y_start = 70 + margin;
    UWORD x_end = x_start + (Font32.Width * 4); // Approx. width for "XX F"
    UWORD y_end = y_start + Font32.Height;
    
    // Clear just the temperature area and redraw the new value
    Paint_ClearWindows(x_start, y_start, x_end, y_end, WHITE);
    Paint_DrawString_EN(x_start, y_start, temp_str, &Font32, BLACK, WHITE);
    
    printf("Updating temperature to %s\r\n", temp_str);
    EPD_7IN5B_V2_Display_Partial(BlackImage, 0, 0, EPD_7IN5B_V2_WIDTH, EPD_7IN5B_V2_HEIGHT);

    DEV_Delay_ms(5000);
  }
  printf("EPD_Display\r\n");
  
  DEV_Delay_ms(2000);
}

void drawLocalHeader(int margin)
{
  Paint_DrawString_EN(10 + margin, 10 + margin, "Local Weather", &Font16, BLACK, WHITE);

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
  Paint_DrawDateTime(10 + margin, 30 + margin, &sPaint_time, &Font12, BLACK, WHITE);

  // Draw static parts of current conditions once
  drawCurrentConditions(margin, " ", "Humidity: 45%", "Pressure: 5 mb");
}

void drawBorders(int margin)
{
  const int split_point = EPD_7IN5B_V2_WIDTH / 4;

  Paint_DrawRectangle(margin, margin, split_point, EPD_7IN5B_V2_HEIGHT - margin,
                      BLACK, DOT_PIXEL_2X2, DRAW_FILL_EMPTY);
  Paint_DrawRectangle(split_point + margin, margin, EPD_7IN5B_V2_WIDTH - margin, EPD_7IN5B_V2_HEIGHT - margin,
                      BLACK, DOT_PIXEL_2X2, DRAW_FILL_EMPTY);

}

void initializeDisplay()
{
  printf("e-Paper Init and Clear...\r\n");

  EPD_7IN5B_V2_Init_Part();
  EPD_7IN5B_V2_Clear();
  EPD_7IN5B_V2_Display_Base_color(WHITE);

  DEV_Delay_ms(1000);
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

void drawCurrentConditions(int margin, const char* temp, const char* humidity, const char* pressure)
{
  Paint_DrawString_EN(10 + margin, 70 + margin, temp, &Font32, BLACK, WHITE);
  Paint_DrawString_EN(10 + margin, 110 + margin, humidity, &Font12, BLACK, WHITE);
  Paint_DrawString_EN(10 + margin, 130 + margin, pressure, &Font12, BLACK, WHITE);
}

/* The main loop -------------------------------------------------------------*/
void loop()
{
  //
}