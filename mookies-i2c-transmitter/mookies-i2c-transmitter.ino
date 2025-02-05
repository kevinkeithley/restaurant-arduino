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
const int callPin = 5; // Pin for "call" on/off

// Variables for inventory tracking
int inventory = 1000; // Initial quantity of wings
int decrementValue = 6; // Default decrement value

// Edge detection flags
bool upHeld = false; // Tracks if "up" action has been processed
bool downHeld = false; // Tracks if "down" action has been processed

// State variables for Call switch
bool callState = false;

void setup() {
    // Initialize Serial for debugging
    Serial.begin(9600);

    // Initialize the OLED display
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        Serial.println(F("SSD1306 allocation failed"));
        for (;;);
    }

    // Clear the display buffer
    display.clearDisplay();

    // Set up switch pins with internal pull-up resistors
    pinMode(upPin, INPUT_PULLUP);
    pinMode(downPin, INPUT_PULLUP);
    pinMode(callPin, INPUT_PULLUP);

    // Initialize callState based on the current pin
    callState = (digitalRead(callPin) == LOW);
    decrementValue = callState ? 25 : 6;

    // Display the initial content  
    displayContent();
}

void loop() {
    // Read call switch state
    bool currentCallState = (digitalRead(callPin) == LOW);
    if (currentCallState != callState) {
        callState = currentCallState;
        decrementValue = callState ? 25 : 6;
        displayContent();
    }

    // Handle up/down momentary switches (flipped direction)
    int upState = digitalRead(upPin);
    int downState = digitalRead(downPin);

    if (upState == LOW && !upHeld) {
        inventory += decrementValue;
        upHeld = true;
        displayContent();
    } else if (upState == HIGH) {
        upHeld = false;
    }

    if (downState == LOW && !downHeld) {
        inventory = max(0, inventory - decrementValue);
        downHeld = true;
        displayContent();
    } else if (downState == HIGH) {
        downHeld = false;
    }

    delay(50); // Small debounce delay for switches
}

void displayContent() {
    // Clear the display
    display.clearDisplay();

    // Display mode at the top in small text
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    display.print(callState ? "Mode: -25" : "Mode: -6");
    
    // Display inventory count in large text
    display.setTextSize(4);
    display.setCursor(20, 20);
    display.print(inventory);
    
    // Display everything
    display.display();
}
