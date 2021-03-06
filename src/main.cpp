#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <AsyncElegantOTA.h>
#include <WS2812FX.h>
#include <OneButton.h>
#include <secrets.h>

#define LED_COUNT 8
#define LED_PIN D5

#define TIMER_MS 5000

AsyncWebServer server(80);

WS2812FX ws2812fx = WS2812FX(LED_COUNT, LED_PIN, NEO_RGB + NEO_KHZ800);
OneButton button(D2, true);

unsigned long last_change = 0;
unsigned long now = 0;

ICACHE_RAM_ATTR void checkTicks()
{
  // include all buttons here to be checked
  button.tick(); // just call tick() to check the state.
}

void singleClick()
{
  Serial.println("singleClick() detected.");
} // singleClick


// this function will be called when the button was pressed 2 times in a short timeframe.
void doubleClick()
{
  Serial.println("doubleClick() detected.");

  //ledState = !ledState; // reverse the LED
  //digitalWrite(PIN_LED, ledState);
} // doubleClick


// this function will be called when the button was pressed multiple times in a short timeframe.
void multiClick()
{
  Serial.print("multiClick(");
  Serial.print(button.getNumberClicks());
  Serial.println(") detected.");

//  ledState = !ledState; // reverse the LED
//  digitalWrite(PIN_LED, ledState);
} // multiClick

void setup(void) {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/plain", "Hi! I am ESP8266.");
  });

  AsyncElegantOTA.begin(&server);    // Start ElegantOTA
  server.begin();
  Serial.println("HTTP server started");

  ws2812fx.init();
  ws2812fx.setBrightness(255);
  ws2812fx.setSpeed(1000);
  ws2812fx.setColor(0x007BFF);
  ws2812fx.setMode(FX_MODE_STATIC);
  ws2812fx.start();
  Serial.println("FIM");

  attachInterrupt(digitalPinToInterrupt(D2), checkTicks, CHANGE);

  // link the xxxclick functions to be called on xxxclick event.
  button.attachClick(singleClick);
  button.attachDoubleClick(doubleClick);
  button.attachMultiClick(multiClick);

  button.setPressTicks(1000);
}

void loop(void) {
  now = millis();

  ws2812fx.service();

  if(now - last_change > TIMER_MS) {
    ws2812fx.setMode((ws2812fx.getMode() + 1) % ws2812fx.getModeCount());
    last_change = now;
  }

  button.tick();
}