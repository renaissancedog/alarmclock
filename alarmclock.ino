#include <Wire.h>
#include <RTClib.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);
RTC_DS1307 RTC;

const int BUTTON_SET = 6;
const int BUTTON_PLUS = 7;
const int BUTTON_MINUS = 8;
const int BUTTON_ALARM_SW = 9;
const int LED_PIN = 13;
const int BUZZER_PIN = 2;

int hourSetting;
int minuteSetting;
int menu = 0;
int alarmState = 0;
bool dimmedDisplay = false;
uint8_t alarmHours = 7, alarmMinutes = 15;

void setup() {
  lcd.init();
  lcd.backlight();
  lcd.clear();

  pinMode(BUTTON_SET, INPUT_PULLUP);
  pinMode(BUTTON_PLUS, INPUT_PULLUP);
  pinMode(BUTTON_MINUS, INPUT_PULLUP);
  pinMode(BUTTON_ALARM_SW, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  printAlarmOff();
  Serial.begin(9600);
  Wire.begin();
  RTC.begin();
  RTC.adjust(DateTime(__DATE__, __TIME__));
  menu = 0;
}

void loop() {
  if (digitalRead(BUTTON_SET) == LOW) {
    menu++;
    lcd.clear();
  }
  if (digitalRead(BUTTON_PLUS) == LOW && digitalRead(BUTTON_MINUS) == LOW) {
    dimDisplay();
  }
  switch (menu) {
    case 0:
      displayDateTime();
      handleAlarm();
      break;
    case 1:
      displaySetHour();
      break;
    case 2:
      displaySetMinute();
      break;
    case 3:
      displayAlarmHour();
      break;
    case 4:
      displayAlarmMinute();
      break;
    case 5:
      storeSettings();
      delay(500);
      menu = 0;
      break;
    default:
      break;
  }
  delay(10);
}

void displayDateTime() {
  DateTime now = RTC.now();
  lcd.setCursor(0, 1);
  lcd.print((now.hour() < 10 ? "0" : ""));
  lcd.print(now.hour(), DEC);
  lcd.print(":");
  lcd.print((now.minute() < 10 ? "0" : ""));
  lcd.print(now.minute(), DEC);
  lcd.print(":");
  lcd.print((now.second() < 10 ? "0" : ""));
  lcd.print(now.second(), DEC);

  hourSetting = now.hour();
  minuteSetting = now.minute();
}

void dimDisplay() {
  if (dimmedDisplay == false) {
    lcd.noBacklight();
  } else {
    lcd.backlight();
  }
  delay(400);
  dimmedDisplay = !dimmedDisplay;
  lcd.clear();
}

void printAlarmOn() {
  lcd.setCursor(0, 0);
  lcd.print("Alarm: ");
  lcd.print((alarmHours < 10 ? "0" : ""));
  lcd.print(alarmHours, DEC);
  lcd.print(":");
  lcd.print((alarmMinutes < 10 ? "0" : ""));
  lcd.print(alarmMinutes, DEC);
}

void printAlarmOff() {
  lcd.setCursor(0, 0);
  lcd.print("Alarm: Off");
}

void handleAlarm() {
  if (digitalRead(BUTTON_ALARM_SW) == LOW) {
    alarmState++;
    delay(200);
    lcd.clear();
  }
  if (alarmState == 0) {
    printAlarmOff();
    noTone(BUZZER_PIN);
    digitalWrite(LED_PIN, LOW);
  }
  if (alarmState == 1) {
    printAlarmOn();
    DateTime now = RTC.now();
    if (now.hour() == alarmHours && now.minute() == alarmMinutes) {
      lcd.backlight();
      digitalWrite(LED_PIN, HIGH);
      playAlarm();
    } else {
      noTone(BUZZER_PIN);
      digitalWrite(LED_PIN, LOW);
    }
  }
  if (alarmState == 2) {
    alarmState = 0;
  }
}

void displaySetHour() {
  if (digitalRead(BUTTON_PLUS) == LOW) {
    lcd.clear();
    hourSetting = (hourSetting == 23) ? 0 : hourSetting + 1;
  }
  if (digitalRead(BUTTON_MINUS) == LOW) {
    lcd.clear();
    hourSetting = (hourSetting == 0) ? 23 : hourSetting - 1;
  }

  lcd.setCursor(0, 0);
  lcd.print("Set hour:");
  lcd.setCursor(0, 1);
  lcd.print(hourSetting, DEC);
  delay(200);
}

void displaySetMinute() {
  if (digitalRead(BUTTON_PLUS) == LOW) {
    lcd.clear();
    minuteSetting = (minuteSetting == 59) ? 0 : minuteSetting + 1;
  }
  if (digitalRead(BUTTON_MINUS) == LOW) {
    lcd.clear();
    minuteSetting = (minuteSetting == 0) ? 59 : minuteSetting - 1;
  }

  lcd.setCursor(0, 0);
  lcd.print("Set Minutes:");
  lcd.setCursor(0, 1);
  lcd.print(minuteSetting, DEC);
  delay(200);
}

void displayAlarmHour() {
  if (digitalRead(BUTTON_PLUS) == LOW) {
    lcd.clear();
    alarmHours = (alarmHours == 23) ? 0 : alarmHours + 1;
  }
  if (digitalRead(BUTTON_MINUS) == LOW) {
    lcd.clear();
    alarmHours = (alarmHours == 0) ? 23 : alarmHours - 1;
  }
  lcd.setCursor(0, 0);
  lcd.print("Set HOUR Alarm:");
  lcd.setCursor(0, 1);
  lcd.print(alarmHours, DEC);
  delay(200);
}

void displayAlarmMinute() {
  if (digitalRead(BUTTON_PLUS) == LOW) {
    lcd.clear();
    alarmMinutes = (alarmMinutes == 59) ? 0 : alarmMinutes + 1;
  }
  if (digitalRead(BUTTON_MINUS) == LOW) {
    lcd.clear();
    alarmMinutes = (alarmMinutes == 0) ? 59 : alarmMinutes - 1;
  }
  lcd.setCursor(0, 0);
  lcd.print("Set MIN. Alarm:");
  lcd.setCursor(0, 1);
  lcd.print(alarmMinutes, DEC);
  delay(200);
}

void storeSettings() {
  lcd.setCursor(0, 0);
  lcd.print("SAVING IN");
  lcd.setCursor(0, 1);
  lcd.print("PROGRESS");
  RTC.adjust(DateTime(-1, -1, -1, hourSetting, minuteSetting, 0));
  delay(200);
  lcd.clear();
}

struct Note {
  int frequency;
  unsigned long duration;
};

// Define your alarm melody as an array of notes
Note alarmMelody[] = {
  { 466, 500 },
  { 587, 375 },
  { 698, 125 },
  { 0, 375 },
  { 587, 125 },
  { 466, 375 },
  { 587, 125 },
  { 622, 500 },
  { 784, 375 },
  { 622, 125 },
  { 932, 375 },
  { 784, 125 },
  { 0, 500 },
  { 932, 500 },
  { 0, 3000 }
};
const int melodyLength = sizeof(alarmMelody) / sizeof(alarmMelody[0]);
void playAlarm() {
  while (alarmState == 1) {
    for (int i = 0; i < melodyLength; i++) {
      if (alarmMelody[i].frequency > 0) {
        tone(BUZZER_PIN, alarmMelody[i].frequency);
      } else {
        noTone(BUZZER_PIN);
      }
      delay(alarmMelody[i].duration);
      if (digitalRead(BUTTON_ALARM_SW) == LOW) {
        noTone(BUZZER_PIN);
        digitalWrite(LED_PIN, LOW);
        lcd.clear();
        printAlarmOff();
        alarmState = 0;
      }
    }
  }
}