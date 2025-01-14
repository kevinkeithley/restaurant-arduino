#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Define the screen size and reset pin
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1 // No reset pin
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Switch pins
const int upPin = 2; // Pin for "up" direction
const int downPin = 3; // Pin for "down" direction
const int busyPin = 4; // Pin for "busy" on/off
const int callPin = 5; // Pin for "call" on/off

// Variables for time and battery life
int timeIncrement = 15; // Default value in minutes
const int minTime = 10; 
const int maxTime = 55;

// Edge detection flags
bool upHeld = false; // Tracks if "up" action has been processed
bool downHeld = false; // Tracks if "down" action has been processed

// State variables for Busy/Call switches
bool busyState = false;
bool callState = false;

void setup() {
  // Initialize Serial for debugging
  Serial.begin(9600);

  // Initialize HC-12
  Serial1.begin(9600);
  Serial.println("HC-12 initialized");

  // Initialize the OLED display
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Fixed initialization
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // Loop forever if the display fails to initialize
  }

  // Clear the display buffer
  display.clearDisplay();

  // Set up switch pins with internal pull-up resistors
  pinMode(upPin, INPUT_PULLUP);
  pinMode(downPin, INPUT_PULLUP);
  pinMode(busyPin, INPUT_PULLUP);
  pinMode(callPin, INPUT_PULLUP);

  // Initialize busyState and callState based on the current pins
  busyState = (digitalRead(busyPin) == LOW); // Switch ON if pin is LOW
  callState = (digitalRead(callPin) == LOW); // Switch ON if pin is LOW

  // Display the initial content  
  displayContent();
}

void loop() {
  // -------------------
  // 1) Handle the up/down momentary switch
  // -------------------
  int upState = digitalRead(upPin);
  int downState = digitalRead(downPin);

  // "up" logic
  if (upState == LOW && !upHeld) { // LOW means the switch is active
    timeIncrement += 5;
    if (timeIncrement > maxTime) timeIncrement = maxTime;
    upHeld = true; // Mark as held
    displayContent(); // Update the display
    sendData(); // Send the updated value
    } else if (upState == HIGH) {
      upHeld = false; // Reset when released
    }

  // "down" logic
  if (downState == LOW and !downHeld) { // LOW means the switch is active
    timeIncrement -= 5;
    if (timeIncrement < minTime) timeIncrement = minTime;
    downHeld = true; // Mark as held
    displayContent(); // Update the display
    sendData(); // Send the updated value
  } else if (downState == HIGH) {
    downHeld = false; // Reset when released
  }
  
  // -------------------
  // 2) Handle the non-momentary Busy/Call switches
  // -------------------
  bool currentBusyState = (digitalRead(busyPin) == LOW);
  bool currentCallState = (digitalRead(callPin) == LOW);
  
  // If the busy switch changed, update & transmit
  if (currentBusyState != busyState) {
    busyState = currentBusyState;
    displayContent();
    sendData();
  }

  // If the call switch changed update & transmit
  if (currentCallState != callState) {
    callState = currentCallState;
    displayContent();
    sendData();
  }

  delay(50); // Small debounce delay for switches
}

void displayContent() {
  // Clear the display
  display.clearDisplay();

  // Draw the top yellow line (LED indicators)
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);

  // Print Busy and Call states as ON/OFF
  display.print("Busy:");
  display.print(busyState ? "ON " : "OFF ");
  display.print("Call:");
  display.print(callState ? "ON" : "OFF");

  // Draw the number of minutes and "min" in the main blue area
  display.setTextSize(4);
  display.setCursor(20, 20);
  display.print(timeIncrement);

  display.setTextSize(2);
  display.setCursor(80, 34);
  display.print("min");

  // Display everything
  display.display();
}

void sendData() {
  // Example: "15, true, false"
  Serial1.print(timeIncrement);
  Serial1.print(", ");
  Serial1.print(busyState ? "true" : "false");
  Serial1.print(", ");
  Serial1.println(callState ? "true" : "false");

  // Debug to Serial Monitor
  Serial.print("Sent to HC-12: ");
  Serial.print(timeIncrement);
  Serial.print(", ");
  Serial.print(busyState ? "true" : "false");
  Serial.print(", ");
  Serial.println(callState ? "true" : "false"); 
}
