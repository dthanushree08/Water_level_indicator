#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

int sensorPin = A0;
int greenLED  = 5;
int yellowLED = 6;
int redLED    = 7;
int buzzer    = 8;

// Custom solid block character for the bar graph
byte block[8] = {31, 31, 31, 31, 31, 31, 31, 31};

int raw, level, filled;
bool blinkState = false;
char line1[17];

void setup() {
  pinMode(greenLED, OUTPUT);
  pinMode(yellowLED, OUTPUT);
  pinMode(redLED, OUTPUT);
  pinMode(buzzer, OUTPUT);

  Serial.begin(9600);

  lcd.init();
  lcd.backlight();
  lcd.createChar(0, block);

  lcd.setCursor(0, 0);
  lcd.print("WATER TANK GAUGE");
  lcd.setCursor(0, 1);
  lcd.print("Starting...");
  delay(1500);
  lcd.clear();
}

void loop() {
  raw = analogRead(sensorPin);
  level = map(raw, 0, 1023, 0, 100);
  filled = map(level, 0, 100, 0, 16);   // number of bar blocks (0 to 16)

  const char* status;
  if (level < 10)       status = "EMPTY";
  else if (level < 30)  status = "LOW";
  else if (level < 70)  status = "MEDIUM";
  else if (level < 90)  status = "HIGH";
  else                  status = "FULL";

  // Everything off first
  digitalWrite(greenLED, LOW);
  digitalWrite(yellowLED, LOW);
  digitalWrite(redLED, LOW);
  noTone(buzzer);

  // LED and buzzer logic
  if (level < 10) {
    blinkState = !blinkState;            // red LED blinks when empty
    digitalWrite(redLED, blinkState);
  }
  else if (level < 30) {
    digitalWrite(redLED, HIGH);
  }
  else if (level < 70) {
    digitalWrite(yellowLED, HIGH);
  }
  else if (level < 90) {
    digitalWrite(greenLED, HIGH);
  }
  else {
    digitalWrite(greenLED, HIGH);
    tone(buzzer, 1000);                  // overflow alert
  }

  // LCD line 1: percentage + status
  snprintf(line1, 17, "Lvl:%3d%% %-6s", level, status);
  lcd.setCursor(0, 0);
  lcd.print(line1);

  // LCD line 2: bar graph
  lcd.setCursor(0, 1);
  for (int i = 0; i < 16; i++) {
    if (i < filled) lcd.write(byte(0));  // filled block
    else lcd.print("-");                 // empty part
  }

  Serial.print("Level: ");
  Serial.print(level);
  Serial.print("% - ");
  Serial.println(status);

  delay(300);
}