#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Define the screen size and reset pin
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1 // No reset pin
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Variables for time and battery life
int timeIncrement = 15; // Default value in minutes
const int minTime = 10;
const int maxTime = 55;
int batteryPercent = 100; // Mock battery percentage

void setup() {
  // Initialize Serial for debugging
  Serial.begin(9600);

  // Initialize Serial1 for HC-12 communication
  Serial1.begin(9600);
  Serial.println("HC-12 initialized as receiver");

  // Initialize the OLED display
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // Loop forever if the display fails to initialize
  }

  // Clear the display buffer
  display.clearDisplay();

  // Display the initial content
  displayContent();
}

void loop() {
  // Check if the data is available from the HC-12
  if (Serial1.available()) {
    // Read the incoming data
    String receivedData = Serial1.readStringUntil('\n'); // Read until newline character

    //Debugging: Print the received data
    Serial.print("Received from HC-12: ");
    Serial.println(receivedData);

    // Convert the received string to an integer
    int newTime = receivedData.toInt();

    // Validate the received value
    if (newTime >= minTime && newTime <= maxTime) {
      timeIncrement = newTime; // Update the time increment
      displayContent(); // Update the display
    } else {
      Serial.println("Invalid data received");
    }
  }

  delay(500); // Small delay for stability
}

void displayContent() {
  // Clear the display
  display.clearDisplay();

  // Draw the top yellow bar (battery life)
  display.setTextSize(1); // Small text for the yellow bar
  display.setTextColor(SSD1306_WHITE); // White text for monochrome screens
  display.setCursor(0, 0);  // Top-left corner
  display.print("Battery: ");
  display.print(batteryPercent);
  display.println("%");

  // Draw the number of minutes and "min" in the main blue area
  display.setTextSize(4); // Larger text for the number
  display.setCursor(20, 20); // Adjust position for the number
  display.print(timeIncrement);

  display.setTextSize(2);  // Smaller text for "min"
  display.setCursor(80, 34);  // Position for "min"
  display.print("min");

  // Display everything
  display.display();
}
