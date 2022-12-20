
// ---------------------------------------------------------------- //
// Arduino 
// ---------------------------------------------------------------- //

#define echoPin 11 // attach pin D2 Arduino to pin Echo of HC-SR04
#define trigPin 12 //attach pin D3 Arduino to pin Trig of HC-SR04
#define echoPin2 9 
#define trigPin2 10
#define buzzPin 6
#define buttonPin 2 // button pin for calibration button
#define ledPin 3

// Set time for stretch reminder here in seconds
unsigned long time_stretch = 1200; // every 1200sec / 20min stretch alert!


// defines variables
//top sensor
long duration; // variable for the duration of sound wave travel
int distance; // variable for the distance measurement

// bottom sensor
long duration2; // variable for the duration of sound wave travel
int distance2; // variable for the distance measurement

bool start = true; 
bool seated = false;
bool calibrating = false;

int slouch_distance_top = 0;
int slouch_distance_bottom = 0;
int button_state = 0;

unsigned long time_seated;
unsigned long time_cur;
unsigned long time_start;
unsigned long time_stretch_milis;


// at the start when sitting detected buzz the piezo for calibration 
// after 5 buzzes, it is calibrated to your spine location
// if the user slouches too much, it buzzes 
// if the user is seated for too long, it buzzes
// press button for recalibration once turned on

void setup() {
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an OUTPUT
  pinMode(echoPin, INPUT); // Sets the echoPin as an INPUT

  pinMode(trigPin2, OUTPUT); // Sets the trigPin as an OUTPUT
  pinMode(echoPin2, INPUT); // Sets the echoPin as an INPUT

  time_stretch_milis = time_stretch * 1000;

  pinMode(buzzPin, OUTPUT);
  pinMode(buttonPin, INPUT);
  Serial.begin(9600); // // Serial Communication is starting with 9600 of baudrate speed
  Serial.println("Ultrasonic Sensor HC-SR04 Test"); // print some text in Serial Monitor
  Serial.print("Time stretch: ");
  Serial.println(time_stretch);
  Serial.print("Time stretch millis: ");
  Serial.println(time_stretch_milis);
}


void loop() {
  button_state = digitalRead(buttonPin);
  if (button_state == HIGH) {
    calibrate_user();
  }


  //Clears the trigPin condition
  digitalWrite(trigPin, LOW);
  digitalWrite(trigPin2, LOW);

  delayMicroseconds(2);
  // Sets the trigPin HIGH (ACTIVE) for 10 microseconds
  digitalWrite(trigPin, HIGH);

  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  //Clears the trigPin condition
  digitalWrite(trigPin, LOW);

  digitalWrite(trigPin2, LOW);
  delayMicroseconds(2);
  // Sets the trigPin HIGH (ACTIVE) for 10 microseconds
  digitalWrite(trigPin2, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin2, LOW);
  duration2 = pulseIn(echoPin2, HIGH);

  // Calculating the distance in cm from sensor
  distance = duration * 0.034 / 2; // top sensor
  distance2 = duration2 * 0.034 / 2; // bottom sensor

  if (start) {
    time_start = millis(); // get start time after start
  }

  time_cur = millis(); // get current time

  // if user really close to bottom sensor aka seated
  if(distance2 <= 50 && distance2 > 0) {
    if (!seated) {
      // wasnt seated but seated now
      // take time for calculating seated time
      time_seated = millis();
      seated = true;
    }

    if (calibrating && distance < 400) {
      slouch_distance_top = distance * 1.3;
      slouch_distance_bottom = distance2 * 2;

      calibrating = false;
    }

    if ((time_cur - time_seated) > (time_seated + time_stretch_milis)) {
      stretch_reminder();
    } 

    // if top sensor reading is further than slouch distance then warn user!
    if ((slouch_distance_top > 0) && (slouch_distance_bottom > 0) && (distance < 400) && (distance2 < 400)) {
      if ((distance > slouch_distance_top) || (distance2 > slouch_distance_bottom)) {
        
      tone(buzzPin, 1000); // Send 1KHz sound signal...
      delay(100);        // ...for 1 sec
      noTone(buzzPin);     // Stop sound...
      delay(10);
      }
    }

  } else {
    if (distance2 < 400) {
      seated = false;
    }
  }

  // Displays the distance on the Serial Monitor
  Serial.print("Distance 2 (TOP): ");
  Serial.print(distance2);
  Serial.println(" cm");

  Serial.print("Distance 1 (BOTTOM): ");
  Serial.print(distance);
  Serial.println(" cm");

  Serial.print("Slouch Distance (TOP): ");
  Serial.print(slouch_distance_top);
  Serial.println(" cm");
  Serial.print("Slouch Distance (BOT): ");
  Serial.print(slouch_distance_bottom);
  Serial.println(" cm");
  Serial.print("time seated: ");
  Serial.println(time_seated);

  Serial.print("SEATED:   ");
  Serial.println(seated);

  // delay(1000);
}

void calibrate_user() {
  Serial.println("*****CALIBRATING USER*****");
  calibrating = true;
  for (int i = 0; i < 10; i++) {
    tone(buzzPin, 2000); // Send 1KHz sound signal...
    delay(500);        // ...for 1 sec
    noTone(buzzPin);     // Stop sound...
    delay(500);
  }
  
}


void stretch_reminder() {
  for (int i = 0; i < 5; i++) {
    tone(buzzPin, 1000); // Send 1KHz sound signal...
    delay(200);        // ...for 1 sec
    noTone(buzzPin);     // Stop sound...
    delay(200);
  }
}