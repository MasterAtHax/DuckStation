
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// ========================================
// DUCKSTATION v0.1.1
// DUCKCODE INTERPRETER
// ========================================

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

#define OLED_SDA 8
#define OLED_SCL 9

Adafruit_SSD1306 display(
  SCREEN_WIDTH,
  SCREEN_HEIGHT,
  &Wire,
  OLED_RESET
);

// ========================================
// DISPLAY SYSTEM
// ========================================

void showScreen(String heading, String message) {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setTextWrap(true);

  display.setCursor(0, 0);
  display.println(heading);
  display.println("----------------");

  display.println(message);
  display.display();
}

// ========================================
// DUCKCODE INTERPRETER
// ========================================

bool executeDuckLine(String line, int lineNumber) {
  line.trim();

  // Ignore empty lines and comments
  if (line.length() == 0 || line.startsWith("#")) {
    return true;
  }

  // SHOW command
  if (line.startsWith("show ")) {
    String message = line.substring(5);
    message.trim();

    if (message.length() < 2 ||
        message[0] != '"' ||
        message[message.length() - 1] != '"') {
      showScreen("DUCKCODE ERROR",
                 "Invalid SHOW command. Line " + String(lineNumber));
      return false;
    }

    message = message.substring(1, message.length() - 1);

    showScreen("DUCKCODE", message);
    return true;
  }

  // WAIT command
  if (line.startsWith("wait ")) {
    String arguments = line.substring(5);
    arguments.trim();

    int separator = arguments.indexOf(' ');

    if (separator < 0) {
      showScreen("DUCKCODE ERROR",
                 "Invalid WAIT command. Line " + String(lineNumber));
      return false;
    }

    String amountText = arguments.substring(0, separator);
    String unit = arguments.substring(separator + 1);
    unit.trim();

    int amount = amountText.toInt();

    if (amount <= 0 || amount > 60 ||
        (unit != "second" && unit != "seconds")) {
      showScreen("DUCKCODE ERROR",
                 "Invalid WAIT value. Line " + String(lineNumber));
      return false;
    }

    delay(amount * 1000UL);
    return true;
  }

  // Unknown command
  showScreen("DUCKCODE ERROR",
             "Unknown command. Line " + String(lineNumber));

  return false;
}

// ========================================
// SCRIPT RUNNER
// ========================================

void runDuckScript(const char* source) {
  String currentLine = "";
  int lineNumber = 1;

  while (*source) {
    char character = *source++;

    if (character == '\n') {
      if (!executeDuckLine(currentLine, lineNumber)) {
        return;
      }

      currentLine = "";
      lineNumber++;
    } else if (character != '\r') {
      currentLine += character;
    }
  }

  if (currentLine.length() > 0) {
    executeDuckLine(currentLine, lineNumber);
  }
}

// ========================================
// FIRST DUCKCODE TEST SCRIPT
// ========================================

const char* demoScript = R"DUCK(
# My first DuckStation script

show "Hello!"

wait 1 second

show "Welcome to DuckStation!"

wait 2 seconds

show "DuckCode is working!"
)DUCK";

// ========================================
// STARTUP
// ========================================

void setup() {
  Wire.begin(OLED_SDA, OLED_SCL);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    while (true) {
      delay(100);
    }
  }

  showScreen("DUCKSTATION", "SYSTEM ONLINE\nVERSION 0.1.1");

  delay(2000);

  showScreen("DUCKCODE", "Starting interpreter...");
  delay(1000);

  runDuckScript(demoScript);

  delay(1000);
  showScreen("DUCKSTATION", "SCRIPT COMPLETE!");
}

void loop() {
}
