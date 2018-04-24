/*
 * serial_echo.pde
 * ----------------- 
 * Echoes what is sent back through the serial port.
 *
 * http://spacetinkerer.blogspot.com
 */

char incomingByte[30];    // for incoming serial data

void setup() {
    for (int i = 0; i < 30; i++) {
      incomingByte[i] = 0;
    }
  
    Serial.begin(115200);    // opens serial port, sets data rate to 9600 bps
}

void loop() {  
  if (Serial.available()) {
      int i = 0;
    // send data only when you receive data:
    while (Serial.available() > 0) {
      // read the incoming byte:
      incomingByte[i] = Serial.read();
      i++;
    }
    // say what you got:
    for (int j = 0; j < i; j++) {
      Serial.print(incomingByte[j]);
    }
  } 
}
