#ifndef GET_DISTANCE_H
#define GET_DISTANCE_H



float getDistance(int sigPin) {
  long duration;
  float distance_cm;

  // Set pin to OUTPUT to send trigger
  pinMode(sigPin, OUTPUT);
  digitalWrite(sigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(sigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(sigPin, LOW);

  // Set pin to INPUT to read echo
  pinMode(sigPin, INPUT);
  duration = pulseIn(sigPin, HIGH);

  // Calculate distance (in cm)
  distance_cm = duration * 0.0343 / 2;

  return distance_cm;
}

#endif