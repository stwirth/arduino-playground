
#include <EEPROM.h>

// initialization
bool has_message = false;
bool is_box_open = false;
bool last_is_box_open = false;
bool is_playing = false;
bool is_recording = false;

const int OPEN_THRESHOLD = 100;
const int CLOSED_THRESHOLD = 50; // a little bit of histeresis

const int LED_OPEN_PIN = 13;
const int RECORD_PIN = 2;
const int PLAY_PIN = 3;
const int HAS_MESSAGE_PIN = 4;

void read_has_message_from_eeprom() {
  int address = 0;
  byte value = EEPROM.read(address);
  has_message = value;
}

void setup() {
  // initialize the LED as an output:
  pinMode(LED_OPEN_PIN, OUTPUT);
  pinMode(RECORD_PIN, OUTPUT);
  pinMode(PLAY_PIN, OUTPUT);
  pinMode(HAS_MESSAGE_PIN, OUTPUT);
  // initialize serial communication:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  read_has_message_from_eeprom();
}

void start_play_message() {
  if (!is_playing) {
    Serial.println("starting playback");
    digitalWrite(PLAY_PIN, HIGH);
  }
  is_playing = true;
}

void stop_play_message() {
  if (is_playing) {
    Serial.println("stopping playback");
    digitalWrite(PLAY_PIN, LOW);
  }
  is_playing = false;
}

void start_record_message() {
  if (!is_recording) {
    Serial.println("starting record");
    digitalWrite(RECORD_PIN, HIGH);
  }
  is_recording = true; 
}

void stop_record_message() {
  if (is_recording) {
    Serial.println("stopping record");
    digitalWrite(RECORD_PIN, LOW);
  }
  is_recording = false;
}

void loop() {
  // monitor light sensor
  int sensor_value = analogRead(A0);
  if (sensor_value > OPEN_THRESHOLD) {
    is_box_open = true;
    digitalWrite(LED_OPEN_PIN, HIGH);
  } else if (sensor_value < CLOSED_THRESHOLD) {
    is_box_open = false;
    digitalWrite(LED_OPEN_PIN, LOW);
  }
  if (is_box_open && !last_is_box_open) {
    // box just opened, do the stuff
    Serial.println("box opened");
    if (has_message) {
      Serial.println("box has message");
      start_play_message();
      has_message = false;
    } else {
      Serial.println("box does not have message");
      start_record_message();
      has_message = true;
    }
  } else if (last_is_box_open && !is_box_open) {
    // box has been closed, stop doing the stuff
    Serial.println("box closed");
    if (is_recording) {
      stop_record_message();
    } else if (is_playing) {
      stop_play_message();
    }
  }
  if (has_message) {
    digitalWrite(HAS_MESSAGE_PIN, HIGH);
  } else {
    digitalWrite(HAS_MESSAGE_PIN, LOW);
  }
  last_is_box_open = is_box_open;
}
