// Define Trig and Echo pins
#define trigPin 3
#define echoPin 2

// Define variables
long duration;
int distance;

// Configurations
void setup() {
  // Define inputs and outputs
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  
  // Begin Serial communication at a baudrate of 9600:
  Serial.begin(9600);
}

// Main
void loop() {
  // Clear the trigPin by setting it LOW
  digitalWrite(trigPin, LOW);
  delayMicroseconds(5);
  
  // Trigger the sensor by setting the trigPin high for 10 microseconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  // Read the echoPin. pulseIn() returns the duration (length of the pulse) in microseconds
  duration = pulseIn(echoPin, HIGH);
  
  // Calculate the distance
  // Distance = pulse duration * sound speed / 2 (sending and receiving)
  distance = duration*0.034/2;
  
  // Print the distance on the Serial Monitor
  Serial.print("Distance = ");
  Serial.print(distance);
  Serial.println(" cm");

  // Loop delay
  delay(100);
}
