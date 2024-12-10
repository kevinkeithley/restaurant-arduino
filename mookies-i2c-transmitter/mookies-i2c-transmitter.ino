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

// Variables for time and battery life
int timeIncrement = 15; // Default value in minutes
const int minTime = 10; 
const int maxTime = 55;
int batteryPercent = 100; // Mock battery percentage

// Edge detection flags
bool upHeld = false; // Tracks if "up" action has been processed
bool downHeld = false; // Tracks if "down" action has been processed

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

  // Display the initial content
  displayContent();
}

void loop() {
  // Read the state of the pins
  int upState = digitalRead(upPin);
  int downState = digitalRead(downPin);

  // Increment when "up" is toggled
  if (upState == LOW && !upHeld) { // LOW means the switch is active
    timeIncrement += 5;
    if (timeIncrement > maxTime) timeIncrement = maxTime;
    upHeld = true; // Mark as held
    displayContent(); // Update the display
    sendTimeIncrement(); // Send the updated value
    } else if (upState == HIGH) {
      upHeld = false; // Reset when released
    }

  // Decrement when "down" is toggled
  if (downState == LOW and !downHeld) { // LOW means the switch is active
    timeIncrement -= 5;
    if (timeIncrement < minTime) timeIncrement = minTime;
    downHeld = true; // Mark as held
    displayContent(); // Update the display
    sendTimeIncrement(); // Send the updated value
  } else if (downState == HIGH) {
    downHeld = false; // Reset when released
  }

  delay(50); // Small debounce delay for switches
}

void displayContent() {
  // Clear the display
  display.clearDisplay();

  // Draw the top yellow bar (time and battery life)
  display.setTextSize(1);               // Small text for the yellow bar
  display.setTextColor(SSD1306_WHITE); // White text for monochrome screens
  display.setCursor(0, 0);              // Top-left corner
  display.print("Battery: ");
  display.print(batteryPercent);
  display.println("%");

  // Draw the number of minutes and "min" in the main blue area
  display.setTextSize(4);               // Larger text for the number
  display.setCursor(20, 20);            // Adjust position for the number
  display.print(timeIncrement);

  display.setTextSize(2);               // Smaller text for "min"
  display.setCursor(80, 34);           // Position for "min"
  display.print("min");

  // Display everything
  display.display();
}

void sendTimeIncrement() {
  // Send the time increment over HC-12
  Serial1.println(timeIncrement);
  Serial.print("Sent to HC-12: ");
  Serial.println(timeIncrement); 
}
