
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include <Adafruit_BME280.h>
#include <SPI.h>
#include <ESP8266WiFi.h>
#include <Wire.h>

#define TFT_CS         D2
#define TFT_RST        D3
#define TFT_DC         D4


// For 1.44" and 1.8" TFT with ST7735 (including HalloWing) use:
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);
// define device I2C address: 0x76 or 0x77 (0x77 is library default address)
#define BME280_I2C_ADDRESS  0x77
// initialize Adafruit BME280 library
Adafruit_BME280  bme280;

// Replace with your network credentials
const char* ssid     = "*********";
const char* password = "*********";

// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String header;

float p = 3.1415926;

void setup(void)
{
  Serial.begin(9600);

  tft.initR(INITR_BLACKTAB);     // initialize a ST7735S chip, black tab
  tft.fillScreen(ST7735_BLACK);  // fill screen with black color
  tft.drawFastHLine(0, 30,  tft.width(), ST7735_WHITE);   // draw horizontal white line at position (0, 30)
 
  tft.setTextColor(ST7735_WHITE, ST7735_BLACK);  // set text color to white and black background
  tft.setTextSize(1);                // text size = 1
  tft.setCursor(4, 0);               // move cursor to position (4, 0) pixel
  //tft.print("NODEMCU + ST7735 TFT");
  tft.setCursor(19, 15);              // move cursor to position (19, 15) pixel
  //tft.print("WEATHER STATION");
 
  // initialize the BME280 sensor
  Wire.begin(D6, D1);  // set I2C pins [SDA = D8, SCL = D1], default clock is 100kHz
  if( bme280.begin(BME280_I2C_ADDRESS) == 0 )
  {  // connection error or device address wrong!
    tft.setTextColor(ST7735_RED, ST7735_BLACK);   // set text color to red and black background
    tft.setTextSize(2);         // text size = 2
    tft.setCursor(5, 76);       // move cursor to position (5, 76) pixel
    tft.print("Connection");
    tft.setCursor(35, 100);     // move cursor to position (35, 100) pixel
    tft.print("Error");
    while(1)  // stay here
      delay(1000);
  }
 
  tft.drawFastHLine(0, 76,  tft.width(), ST7735_WHITE);   // draw horizontal white line at position (0, 76)
  tft.drawFastHLine(0, 122,  tft.width(), ST7735_WHITE);  // draw horizontal white line at position (0, 122)
  tft.setTextColor(ST7735_RED, ST7735_BLACK);     // set text color to red and black background
  tft.setCursor(5, 39);              // move cursor to position (25, 39) pixel
  tft.print("TEMPERATURE =");
  tft.setTextColor(ST7735_CYAN, ST7735_BLACK);  // set text color to cyan and black background
  tft.setCursor(15, 85);              // move cursor to position (34, 85) pixel
  tft.print("HUMIDITY =");
  tft.setTextColor(ST7735_GREEN, ST7735_BLACK);  // set text color to green and black background
  tft.setCursor(34, 131);              // move cursor to position (34, 131) pixel
  tft.print("PRESSURE =");
  tft.setTextSize(2);                 // text size = 2
  // print °C
  tft.drawCircle(89, 56, 2, ST7735_YELLOW);  // print degree symbol ( ° )
  tft.setCursor(95, 54);                     // move cursor to position (95, 54) pixel
  tft.setTextColor(ST7735_YELLOW, ST7735_BLACK);  // set text color to yellow and black background
  tft.print("C");

  WiFi.mode(WIFI_STA);
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
   // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
}

void loop()
{
  // read temperature, humidity and pressure from the BME280 sensor
  float temp = bme280.readTemperature();    // get temperature in °C
  float humi = bme280.readHumidity();       // get humidity in %
  float pres = bme280.readPressure();       // get pressure in Pa
 
  // print data on the display
  // print temperature (in °C)
  tft.setCursor(11, 54);
  tft.setTextColor(ST7735_YELLOW, ST7735_BLACK);  // set text color to yellow and black background
  if(temp < 0)    // if temperature < 0
    tft.printf( "-%02u.%02u", (int)abs(temp), (int)(abs(temp) * 100) % 100 );
  else            // temperature >= 0
    tft.printf( " %02u.%02u", (int)temp, (int)(temp * 100) % 100 );
 
  // 2: print humidity
  tft.setCursor(23, 100);
  tft.setTextColor(ST7735_MAGENTA, ST7735_BLACK);  // set text color to magenta and black background
  tft.printf( "%02u.%02u %%", (int)humi, (int)(humi * 100) % 100 );
 
  // 3: print pressure (in hPa)
  tft.setCursor(3, 146);
  tft.setTextColor(0xFD00, ST7735_BLACK);  // set text color to orange and black background
  tft.printf( "%04u.%02u", (int)(pres/100), (int)((uint32_t)pres % 100) );
  tft.setCursor(91, 146);
  tft.print("hPa");

  WiFiClient client = server.available();

    if (client) {                             // If a new client connects,
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:application/json");
            client.println("Connection: close");
            client.println();
            
            client.print("{\"temperature\":");
            client.print(temp);
            client.print(", \"humidity\":");
            client.print(humi);
            client.print( ", \"pressure\": ");
            client.print(pres);
            client.print("}");
            
            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
    delay(1000); 
}
