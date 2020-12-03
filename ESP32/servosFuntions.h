#include <Servo.h>
static const int POS_UNO = 136;                               // Container 1
static const int POS_DOS = 26;                                // Container 2
static const int POS_CENTRAL = 74;                            // Central Position, where pill is droped

static const int servo1Pin = 18;                              // Servo pin

void pill(int i, Servo servo1) {

  int pos = 0;
  servo1.attach(servo1Pin);

  if (i == 1)
  {
    for (pos = POS_CENTRAL; pos <= POS_UNO; pos++) {          // goes from 0 degrees to 180 degrees
      servo1.write(pos);                                      // tell servo to go to position in variable 'pos'
      delay(5);                                               // waits 15ms for the servo to reach the position
    }
    
    wait(1000);

    for (pos = POS_UNO; pos >= POS_CENTRAL; pos--) {
      servo1.write(pos);
      delay(15);
    }
  }

  else if (i == 2)
  {
    for (pos = POS_CENTRAL; pos >= POS_DOS; pos--) {          // goes from 0 degrees to 180 degrees
      servo1.write(pos);                                      // tell servo to go to position in variable 'pos'
      delay(5);                                               // waits 15ms for the servo to reach the position
    }
    
    wait(1000);

    for (pos = POS_DOS; pos <= 85; pos++) {
      servo1.write(pos);
      delay(15);
    }
  }

}
