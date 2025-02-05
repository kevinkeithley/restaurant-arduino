#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Define the screen size and reset pin
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Pins for LEDs
const int busyLedPin = 2;   // LED for "busy" state
const int callLedPin = 3;  // LED for "call" state

// Variables for time
int timeIncrement = -1; 
const int minTime = 10;
const int maxTime = 55;

// State variables for Busy/Call
bool busyState = false;
bool callState = false;

// Blink parameters
unsigned long lastBlinkMillis = 0;
const unsigned long blinkInterval = 500; // 500 ms => 2 blinks/second
bool callLedIsOn = false; // Tracks the LED's current on/off state

void setup() {
  // Initialize Serial for debugging
  Serial.begin(9600);

  // Initialize Serial1 for HC-12 communication
  Serial1.begin(9600);
  Serial.println("HC-12 initialized as receiver");

  // Initialize the OLED display
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // Loop forever if the display fails
  }

  // Clear the display buffer
  display.clearDisplay();

  // Set LED pins as outputs
  pinMode(busyLedPin, OUTPUT);
  pinMode(callLedPin, OUTPUT);

  // Ensure LEDs are off initially
  digitalWrite(busyLedPin, LOW);
  digitalWrite(callLedPin, LOW);

  // Display the initial content
  displayContent();
}

void loop() {
  // ----------------------------------------------------------------
  // 1) Read incoming data from the HC-12 (if any) and parse CSV
  // ----------------------------------------------------------------
  if (Serial1.available() > 0) {
    String receivedData = Serial1.readStringUntil('\n'); // up to newline
    
    Serial.print("Received from HC-12: ");
    Serial.println(receivedData);

    // Example format: "timeIncrement, busyState, callState"
    int firstComma  = receivedData.indexOf(',');
    int secondComma = receivedData.indexOf(',', firstComma + 1);

    if (firstComma != -1 && secondComma != -1) {
      String timeString  = receivedData.substring(0, firstComma);
      String busyString   = receivedData.substring(firstComma + 1, secondComma);
      String callString  = receivedData.substring(secondComma + 1);

      timeString.trim();
      busyString.trim();
      callString.trim();

      int newTime = timeString.toInt();
      bool newBusyState  = (busyString.equalsIgnoreCase("true"));
      bool newCallState = (callString.equalsIgnoreCase("true"));

      // Validate time range
      if (newTime >= minTime && newTime <= maxTime) {
        timeIncrement = newTime;
      } else {
        Serial.println("Invalid time received, ignoring...");
      }

      // Update states
      busyState  = newBusyState;
      callState = newCallState;

      // Update the display
      displayContent();
    }
    else {
      Serial.println("CSV format error: expected two commas");
    }
  }

  // ----------------------------------------------------------------
  // 2) Update LEDs in real-time
  //    - busy is either ON or OFF
  //    - call blinks if callState == true
  // ----------------------------------------------------------------
  updateLEDs();

  // Small delay for stability (but not too large or blinking slows)
  delay(20);
}

// ------------------------------------------------------------------
// Helper function to handle the busy (solid) + call (blinking) logic
// ------------------------------------------------------------------
void updateLEDs() {
  // busy LED: solid ON if busyState == true, else OFF
  digitalWrite(busyLedPin, busyState ? HIGH : LOW);

  // call LED: blink if callState == true, else OFF
  if (callState) {
    unsigned long currentMillis = millis();
    // Check if it's time to toggle
    if (currentMillis - lastBlinkMillis >= blinkInterval) {
      lastBlinkMillis = currentMillis;
      callLedIsOn = !callLedIsOn; // toggle the state
      digitalWrite(callLedPin, (callLedIsOn ? HIGH : LOW));
    }
  }
  else {
    // If callState is false, LED stays off
    callLedIsOn = false;
    digitalWrite(callLedPin, LOW);
  }
}

// ------------------------------------------------------------------
// Display the "timeIncrement" in a simple manner
// ------------------------------------------------------------------
void displayContent() {
  display.clearDisplay();

  display.setTextColor(SSD1306_WHITE);
  
  display.setTextSize(4);
  display.setCursor(20, 20);
  display.print(timeIncrement);

  display.setTextSize(2);
  display.setCursor(80, 34);
  display.print("min");

  display.display();
}
