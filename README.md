# RC CAR using ESP_IDF

This is supposed to be a upgrade or an complete restruct of this project of an [Arduino based RC "Car"](https://github.com/VAlexandersson/FreeRTOS_RC_CAR) but it's starting to lean towards becoming something different. 


Initializing Wifi and getting time from a SNTP server:
```
I (00:00:02.632) WiFi: event_handler:35 IP_EVENT received
I (00:00:02.636) WiFi: ip_event_handler:77 Event ID 0
I (00:00:02.641) WiFi: ip_event_handler:81 Got IP, waiting for state_mutex
I (00:00:02.649) WiFi: ip_event_handler:84 Got IP, CONNECTED
I (01:00:03.560) MAIN: LED state: 1
I (01:00:04.558) MAIN: LED state: 0
I (01:00:05.558) MAIN: LED state: 1
I (15:13:24.285) Sntp: Time is Tue Sep 10 15:13:24 2024
I (15:13:24.818) MAIN: LED state: 0
I (15:13:25.818) MAIN: LED state: 1
````



TODO:
- WIFI MODULE 
- BLUETHOOTH MODULE 
- GENERIC GPIO INTERFACE