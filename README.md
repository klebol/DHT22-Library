# DHT22-Library
Simple library for one or multiple DHT22 sensors.  
<img src="https://github.com/klebol/DHT22-Library/blob/main/Photos/example-platform.jpg" alt="platform" width="700" height="480">
## This library
* was written on STM32 platform with HAL Libraries, using BlackPill board with STM32F401CCU6
* is suitable for handling multiple DHT22 sensors
* supports checksum verification
* supports sensing sensor presence
* requires one hardware timer to handle us delay
## How to use it
1. Copy the .c and .h files to your project (you can find them in "DHT22 Library" folder
2. Set one hardware timer in CubeMX for tick per 1us, then write handler in dht22.h file
    ```
    #define DHT22_1US_TIMER htim11
    ```
4. Include library in file where you want to use it
    ```
    #include "dht22.h"
    ```
5. Define desired number of sensors as an object variable
    ```
   /* ----- Define your sensors ----  */
    DHT22_t Sensor_1;
    DHT22_t Sensor_2;
    ```
4. Start the 1us timer
    ```
    HAL_TIM_Base_Start(&htim11);
    ```
5. Init your sensors (pass the defined pin and ports)
    ```
    DHT22_Init(&Sensor_1, DHT22_1_GPIO_Port, DHT22_1_Pin);
    DHT22_Init(&Sensor_2, DHT22_2_GPIO_Port, DHT22_2_Pin);
    ```
6. Query the sensor in main loop every 1-2s
    ```
    DHT22_ReadData(&Sensor_1);
    DHT22_ReadData(&Sensor_2);
    HAL_Delay(2000);
    ```
7. You can get received temperature and humidity by using these functions:
   ```
   Temp = DHT22_GetTemperature(&Sensor_1);
   Humi = DHT22_GetHumidity(&Sensor_1);
   ```

   ## Example code
   You can check example CubeIDE project in "DHT22_Example" folder
   ## Porting
   Library can be ported to other platforms, look for functions marked as -- Functions to port --. You'll need to take care of HAL's Gpio types in DHT22_t type also.
