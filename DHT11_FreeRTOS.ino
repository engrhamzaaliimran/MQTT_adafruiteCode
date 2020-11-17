#include <PubSubClient.h>

/****************************************
 * Include Libraries
 ****************************************/
 
#include <DHT.h>

/****************************************
 * Define Constants
 ****************************************/
namespace {
  const int DHTPIN = 5; // Pin where is connected the DHT11
  const int DHTTYPE = DHT11; // Type of DHT
  const int trigPin = 16; // Triger pin of the HC-SR04
  const int echoPin = 17; // Echo pin of the HC-SR04  
}

// define two tasks 
void TaskDHTRead( void *pvParameters );
void TaskSonarRead( void *pvParameters );
/* Sensor's declarations */
long duration;
float distance;

/****************************************
 * Auxiliar Functions
 ****************************************/

DHT dht(DHTPIN, DHTTYPE);
/****************************************
 * Sensor Functions
 ****************************************/
float readDistance() {
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = (pulseIn(echoPin, HIGH)); 
  distance = float(duration/29/2);
  return distance; 
}

/****************************************
 * Main Functions
 ****************************************/
void setup() 
{
  Serial.begin(115200);
// Now set up two tasks to run independently.
  xTaskCreatePinnedToCore(
    TaskDHT
    ,  "TaskDHTRead"   // A name just for humans
    ,  1024  // This stack size can be checked & adjusted by reading the Stack Highwater
    ,  NULL
    ,  2  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
    ,  NULL 
    ,  0);//assigned core 0

xTaskCreatePinnedToCore(
    TaskSonar
    ,  "TaskSonarRead"
    ,  1024  // Stack size
    ,  NULL
    ,  1  // Priority
    ,  NULL 
    ,  1);//core 1 used


  
  // Now the task scheduler, which takes over control of scheduling individual tasks, is automatically started.

 }
  

void loop() 
{
//Control never comes here
//this is left empty just to be consistant with Arduino IDE programming Style
}

void TaskDHT(void *pvParameters)  // This is a task.
{
  (void) pvParameters;

 /* Initializing the DHT11 */
  dht.begin(); //according to FreeRTOS style of programming hardware configurations should be done in task code
  
  for (;;) // A Task shall never return or exit.
  {
  /* Reading temperature and humidity */
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();  
  Serial.print("Humidity = ");
  Serial.println(humidity);
  Serial.print("Temperature = ");
  Serial.println(temperature);
  vTaskDelay(500);  // one tick delay (500ms) in between reads for stability
  }
}

void TaskSonar(void *pvParameters)  // This is a task.
{
  (void) pvParameters;
  /* Assign the PINS as INPUT/OUTPUT */
  pinMode(trigPin, OUTPUT); //according to FreeRTOS style of programming hardware configurations should be done in task code
  pinMode(echoPin, INPUT);
  for (;;)
  {
  /* call the funtion readDistance() */
  distance = readDistance();
  /* Print the sensor reading to the Serial Monitor */
  Serial.println("Values getting form sensors");
  Serial.print("Distance = ");
  Serial.println(distance);
  vTaskDelay(500);  // one tick delay (500ms) in between reads for stability
  }
}
