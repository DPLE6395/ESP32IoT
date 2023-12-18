/*
    >> xTaskCreate(TaskFunction,TaskName,StackDepth,(void*)PassParameters,TaskPriority,TaskHandle)
        TaskFunction : ชื่อฟังก์ชันที่จะให้ทำงานเป็น Task ซึ่งในฟังก์ชันนั้นควรจะเขียนโปรแกรมแบบทำงานวนลูป( infinite loop )
        TaskName : ชื่อของ Task ( ข้อมูลตรงส่วนนี้จะใช้ตอน Debug ไม่มีผลต่อการทำงานของโปรแกรมโดยตรง )
        StackDepth : ขนาด Stack ของ Task (เพื่อจอง memory) การกำหนดขนาดของ Task ถ้ากำหนดไว้น้อยเกินไปจะทำให้ ESP32 Restart ตัวเองตลอดเวลา แต่ถ้ากำหนดไว้มากไปก็ทำให้เสีย memory ทิ้งเปล่าๆ การกำหนดค่านี้แบบคร่าวๆ คือ ลอง complie ดูขนาดของ function ที่ผูกกับ Task นี้ ลองดูจำนวน Byte ที่ใช้ไปครับ แล้วเอามาใส่ในตัวแปรนี้
        PassParameter : ชื่อตัวแปรที่จะส่งค่าเข้ามาทำงานต่อใน Task ( ดูวิธีการทำงานใน code ตัวอย่างเพิ่มเติมครับ )
        TaskPriority : กำหนดเลข Priority ให้ Task ซึ่งค่า 0 คือ Priority ที่ต่ำที่สุด
        TaskHandle : ชื่อตัวแปรของ Task ที่จะนำไปใช้ในการ Handle ทำงานอื่นๆต่อ ( ดูวิธีการทำงานใน code ตัวอย่างเพิ่มเติมครับ )
*/
/* xTaskCreatePinnedToCore(
      Task1code, /* Function to implement the task */
     // "Task1", /* Name of the task */
     // 10000,  /* Stack size in words */
     // NULL,  /* Task input parameter */
     // 0,  /* Priority of the task */
     // &Task1,  /* Task handle. */
     //  0); /* Core where the task should run */


#include <Arduino.h>
// สร้างตัวแปรเก็บค่า Tick ที่แปลงจากเวลาที่เป็น millisecond
const TickType_t xDelay2000ms = pdMS_TO_TICKS(2000);   
// สร้างตัวแปร TaskHandle สำหรับแต่ละ Task
TaskHandle_t Task1 = NULL;    
TaskHandle_t Task2 = NULL;
TaskHandle_t Task3 = NULL;
// สร้างตัวแปรเก็บค่าเป็น integer เพื่อเอาไปใช้ใน Task
int passValue = 0;

void setup() {
  // เริ่มต้น Serial port เพื่อดูค่าที่แต่ละ Task ปริ้นออกมา
  Serial.begin(115200);  
  delay(1000);  
  // สร้าง Task1 โดยใช้ฟังก์ชัน func1_Task() พร้อมทั้งส่งผ่านค่าตัวแปร PassVale เข้าไปคำนวนใน Task 
  xTaskCreatePinnedToCore(func1_Task,"Task1",1000,(void*) passValue,1,&Task1,0);
  // สร้าง Task2 โดยใช้ฟังก์ชัน func2_Task()
  xTaskCreatePinnedToCore(func2_Task,"Task2",1000,NULL,1,&Task2,0);
  // สร้าง Task3 โดยใช้ฟังก์ชัน func3_Task()
  xTaskCreatePinnedToCore(func3_Task,"Task3",1000,NULL,1,&Task3,0);
  
}

void loop() {

}

// funtion ที่ทำงานใน Task1
void func1_Task(void *pvvalue){
  int f1param = (int)pvvalue ;  //แปลงค่าตัวแปรที่ส่งผ่านมาเป็น Integer
  while(1){
      Serial.println(String("hello from Task1 : count >> ") + f1param );
      f1param++;
      vTaskDelay(xDelay2000ms);   // Delay Task นี้ 2 วินาที
    }    
  }
// funtion ที่ทำงานใน Task2
void func2_Task(void *pvParam){
    while(1){      
      Serial.println(String("hello from Task2"));       
      vTaskDelay(xDelay2000ms);   // Delay Task นี้ 2 วินาที
    }    
  }
// funtion ที่ทำงานใน Task3
void func3_Task(void *pvParam){
  while(1){
      Serial.println(String("hello from Task3"));
      vTaskDelay(xDelay2000ms);   // Delay Task นี้ 2 วินาที
    }    
  }  

void loop() {

}

//==============================================

/*********
  Rui Santos
  Complete project details at https://randomnerdtutorials.com  
*********/
// https://randomnerdtutorials.com/esp32-dual-core-arduino-ide/
TaskHandle_t Task1;
TaskHandle_t Task2;

// LED pins
const int led1 = 2;
const int led2 = 4;

void setup() {
  Serial.begin(115200); 
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);

  //create a task that will be executed in the Task1code() function, with priority 1 and executed on core 0
  xTaskCreatePinnedToCore(
                    Task1code,   /* Task function. */
                    "Task1",     /* name of task. */
                    10000,       /* Stack size of task */
                    NULL,        /* parameter of the task */
                    1,           /* priority of the task */
                    &Task1,      /* Task handle to keep track of created task */
                    0);          /* pin task to core 0 */                  
  delay(500); 

  //create a task that will be executed in the Task2code() function, with priority 1 and executed on core 1
  xTaskCreatePinnedToCore(
                    Task2code,   /* Task function. */
                    "Task2",     /* name of task. */
                    10000,       /* Stack size of task */
                    NULL,        /* parameter of the task */
                    1,           /* priority of the task */
                    &Task2,      /* Task handle to keep track of created task */
                    1);          /* pin task to core 1 */
    delay(500); 
}

//Task1code: blinks an LED every 1000 ms
void Task1code( void * pvParameters ){
  Serial.print("Task1 running on core ");
  Serial.println(xPortGetCoreID());

  for(;;){
    digitalWrite(led1, HIGH);
    delay(1000);
    digitalWrite(led1, LOW);
    delay(1000);
  } 
}

//Task2code: blinks an LED every 700 ms
void Task2code( void * pvParameters ){
  Serial.print("Task2 running on core ");
  Serial.println(xPortGetCoreID());

  for(;;){
    digitalWrite(led2, HIGH);
    delay(700);
    digitalWrite(led2, LOW);
    delay(700);
  }
}

void loop() {
  
}