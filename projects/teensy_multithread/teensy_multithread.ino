#include <TeensyThreads.h>

volatile long int count1 = 0;
volatile long int count2 = 0;

void thread_func_1(int inc) {
  while (1) {
    count1 += inc;
    Serial.println(millis());
    threads.delay(1);
  }
}

void thread_func_2(int inc) {
  while (1) {
    count2 += inc;
    Serial.println(millis());
    threads.delay(1);
  }
}


void setup() {
  Serial.begin(9600);
  threads.addThread(thread_func_1, 1);
  threads.addThread(thread_func_2, 1);
}

void loop() {
  // threads.delay(1000);
}
