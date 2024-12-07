# Device Registration Example

![ESP32](https://img.shields.io/badge/ESP32-Tested-brightgreen)  ![ESP32-S3](https://img.shields.io/badge/ESP32--S3-Tested-brightgreen)  ![ESP32-C3](https://img.shields.io/badge/ESP32--C3-Tested-brightgreen) 

## Configuration

```sh
idf.py menuconfig
```
- Set the wifi credentials in `Example Configuration` 
- Enable **Esp HTTPS Server** component in `Component Config->ESP HTTPS Server`
- Enable **Allow Potentialli insecure Options** in `Component Config->ESP-TLS`
  
Please see the tutorial at: https://docs.iotex.io/builders/depin/ioid-step-by-step-tutorial/a-fully-decentralized-approach
