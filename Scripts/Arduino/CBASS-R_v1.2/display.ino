


void startDisplay() {
    tft.begin();
    tft.setRotation(TFT_LAND); 
    tft.fillScreen(BLACK);
    // A start point for text size and color.  If changed elsewhere, change it back.
    tft.setTextSize(2);  // A reasonable size. 1-3 (maybe more) are available.
    tft.setTextColor(WHITE);
}

void displayTemperatureStatus() {
    tft.fillScreen(BLACK);
    tft.setCursor(0, 0);
    tft.print("T1SP ");
    dtostrf(T1SetPoint, 4, 1, T1SetPointStr);
    tft.print(T1SetPointStr);
    tft.print(" T1 ");
    dtostrf(T1TempInput, 4, 1, T1TempInputStr);
    tft.print(T1TempInputStr);
    box(T1RelayStateStr, 0, LINEHEIGHT);


    tft.setCursor(0, LINEHEIGHT);
    tft.print("T2SP ");
    dtostrf(T2SetPoint, 4, 1, T2SetPointStr);
    tft.print(T2SetPointStr);
    tft.print(" T2 ");
    dtostrf(T2TempInput, 4, 1, T2TempInputStr);
    tft.print(T2TempInputStr);
    box(T2RelayStateStr, 1, LINEHEIGHT);

    tft.setCursor(0, LINEHEIGHT*2);
    tft.print("T3SP ");
    dtostrf(T3SetPoint, 4, 1, T3SetPointStr);
    tft.print(T3SetPointStr);
    tft.print(" T3 ");
    dtostrf(T3TempInput, 4, 1, T3TempInputStr);
    tft.print(T3TempInputStr);
    box(T3RelayStateStr, 2, LINEHEIGHT);

    tft.setCursor(0, LINEHEIGHT*3);
    tft.print("T4SP ");
    dtostrf(T4SetPoint, 4, 1, T4SetPointStr);
    tft.print(T4SetPointStr);
    tft.print(" T4 ");
    dtostrf(T4TempInput, 4, 1, T4TempInputStr);
    tft.print(T4TempInputStr);
    box(T4RelayStateStr, 3, LINEHEIGHT);
}

void displayTemperatureStatusBold() {
    tft.setTextSize(2);
    // Only clear below the heading for less flashing.  Also don't clear the boxes, which are refreshed anyway.
    //tft.fillRect(LINEHEIGHT*2, LINEHEIGHT3, TFT_WIDTH-LINEHEIGHT3*4, TFT_HEIGHT-LINEHEIGHT3, BLACK);
    //Header
    tft.setCursor(0,0);
    tft.print("     SETPT  INTEMP   RELAY");

    tft.setTextSize(3);
    tft.fillRect(LINEHEIGHT*2, LINEHEIGHT3, TFT_WIDTH-LINEHEIGHT3*4, LINEHEIGHT3, BLACK);
    tft.setCursor(0, LINEHEIGHT3);
    tft.print("T1 ");
    dtostrf(T1SetPoint, 4, 1, T1SetPointStr);
    tft.print(T1SetPointStr);
    tft.print(" ");
    dtostrf(T1TempInput, 4, 1, T1TempInputStr);
    tft.print(T1TempInputStr);
    box(T1RelayStateStr, 1, LINEHEIGHT3);

    tft.fillRect(LINEHEIGHT*2, LINEHEIGHT3*2, TFT_WIDTH-LINEHEIGHT3*4, LINEHEIGHT3, BLACK);
    tft.setCursor(0, LINEHEIGHT3*2);
    tft.print("T2 ");
    dtostrf(T2SetPoint, 4, 1, T2SetPointStr);
    tft.print(T2SetPointStr);
    tft.print(" ");
    dtostrf(T2TempInput, 4, 1, T2TempInputStr);
    tft.print(T2TempInputStr);
    box(T2RelayStateStr, 2, LINEHEIGHT3);

    tft.fillRect(LINEHEIGHT*2, LINEHEIGHT3*3, TFT_WIDTH-LINEHEIGHT3*4, LINEHEIGHT3, BLACK);
    tft.setCursor(0, LINEHEIGHT3*3);
    tft.print("T3 ");
    dtostrf(T3SetPoint, 4, 1, T3SetPointStr);
    tft.print(T3SetPointStr);
    tft.print(" ");
    dtostrf(T3TempInput, 4, 1, T3TempInputStr);
    tft.print(T3TempInputStr);
    box(T3RelayStateStr, 3, LINEHEIGHT3);

    tft.fillRect(LINEHEIGHT*2, LINEHEIGHT3*4, TFT_WIDTH-LINEHEIGHT3*4, LINEHEIGHT3, BLACK);
    tft.setCursor(0, LINEHEIGHT3*4);
    tft.print("T4 ");
    dtostrf(T4SetPoint, 4, 1, T4SetPointStr);
    tft.print(T4SetPointStr);
    tft.print(" ");
    dtostrf(T4TempInput, 4, 1, T4TempInputStr);
    tft.print(T4TempInputStr);
    box(T4RelayStateStr, 4, LINEHEIGHT3);
}

// Draw a box with color based on relay state.
// Place on the zero-based line specified.
void box(char* s, int line, int lineSize) {
  word color;
  if (strcmp(s, "OFF") == 0) {
    color = BLACK;
  } else if (strcmp(s, "HTR") == 0) {
    color = RED;
  } else if (strcmp(s, "CHL") == 0) {
    color = BLUE;
  } else {
    color = YELLOW; // Yellow = caution.  This should not happen.
  }
  // x, y, width, height, color
  tft.fillRect(TFT_WIDTH-1.5*lineSize, lineSize*line, lineSize-1, lineSize-1, color);
}
