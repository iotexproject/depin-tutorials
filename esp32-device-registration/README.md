# Device Registration Example

![ESP32](https://img.shields.io/badge/ESP32-Tested-brightgreen)  ![ESP32-S3](https://img.shields.io/badge/ESP32--S3-Tested-brightgreen)  ![ESP32-C3](https://img.shields.io/badge/ESP32--C3-Tested-brightgreen) 

## Configuration

```sh
idf.py menuconfig
```
- In `Example Configuration` Set the wifi credentials 
- In `Component Config->ESP HTTPS Server` Enable **Esp HTTPS Server** component 
- In `Component Config->ESP-TLS` Enable **Allow Potentially insecure Options** and **Skip server certificate verification**
- In `Component Config → ESP System Settings` Set ensure **Main Task Stack Size** is high enough, e.g. `8192`    

Please see the tutorial at: https://docs.iotex.io/builders/depin/ioid-step-by-step-tutorial/a-fully-decentralized-approach
