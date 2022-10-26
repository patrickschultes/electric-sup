#include <Servo.h>

#define RECEIVER_PIN 2
#define MOTOR1_PIN 3
#define MOTOR2_PIN 4
#define BATTERY_PIN A0
#define FILTER_TIME 12
#define BATTERY_LOW 235

// Comment out debug to suppress Uart output
#define DEBUG

Servo Motor1;
Servo Motor2;

uint16_t battery_array[FILTER_TIME] = {0};
uint8_t battery_index = 0;

void setup(){
    Serial.begin(115200);
    pinMode(RECEIVER_PIN, INPUT);

    Motor1.attach(MOTOR1_PIN,1000,2000);
    Motor2.attach(MOTOR2_PIN,1000,2000);

    for(uint8_t i = 0; i < FILTER_TIME; i++)
    {
        battery_array[i] = BATTERY_LOW;
    }
}

void loop(){
    int static request = 0;
    int input = pulseIn(RECEIVER_PIN, HIGH);

    int mapped = map(input, 1080, 2130, -100 , 100);
    if (mapped < -100)
    {
        mapped = -100;
    }
    if (mapped > 100)
    {
        mapped = 100;
    }

    if (mapped > 20 && mapped < 95)
    {
        request = request + 5;
    }
    if (mapped > 95)
    {
        request = request + 20;
    }
    if (mapped < -20 && mapped > -95)
    {
        request = request - 5;
    }
    if(mapped < -95)
    {
        request = request - 20;
    }

    if (request < 0)
    {
        request = 0;
    }
    if (request > 100)
    {
        request = 100;
    }

    int out = map(request, 0, 100, 0 , 2130);


    // Filter section to get a senseful battery voltage
    uint32_t battery_sum = 0;
    battery_array[battery_index] = analogRead(BATTERY_PIN);
    battery_index++;
    if (battery_index == FILTER_TIME)
    {
        battery_index = 0;
    }

    for(uint8_t i = 0; i < FILTER_TIME; i++)
    {
        battery_sum = battery_sum + battery_array[i];
    }
    uint16_t battery = battery_sum / FILTER_TIME;


#ifdef DEBUG
    Serial.print(input);
    Serial.print(";");
    Serial.print(mapped);
    Serial.print(";");
    Serial.print(out);
    Serial.print(";");
    Serial.println(battery);
#endif
    
    if (BATTERY_LOW < battery)
    {
        Motor1.write(out);
        Motor2.write(out);
    }
    else 
    {
        Motor1.write(1000);
        Motor2.write(1000);

        delay(900);
    }


    delay(100);
}