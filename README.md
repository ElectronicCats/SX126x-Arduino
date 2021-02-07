# SX126x-Arduino 
![LibraryBuild](https://github.com/ElectronicCats/SX126x-Arduino/workflows/LibraryBuild/badge.svg)

Arduino library for LoRa communication with Semtech SX126x chips. It is based on Semtech's SX126x libraries and adapted to the Arduino framework for ESP32, ESP8266 and nRF52832. It will not work with other uC's like AVR.    

## General info
I stumbled over the [SX126x LoRa family](https://www.semtech.com/products/wireless-rf/lora-transceivers) in a customer project. Most of the existing Arduino libraries for Semtech's SX127x family are unfortunately not working with this new generation LoRa chip. I found a usefull base library from Insight SIP which is based on the original Semtech SX126x library and changed it to work with the ESP32.   
For now the library is tested with an [eByte E22-900M22S](http://www.ebyte.com/en/product-view-news.aspx?id=437) module connected to an ESP32 and an [Insight SIP ISP4520](https://www.insightsip.com/products/combo-smart-modules/isp4520) which combines a Nordic nRF52832 and a Semtech SX1262 in one module. It is as well tested with an [RAKwireless WisCore RAK4630](https://store.rakwireless.com/products) module    


### Based on    
- Semtech open source code for SX126x chips [SX126xLib](https://os.mbed.com/teams/Semtech/code/SX126xLib/)    
- Insight SIP open source code for ISP4520 module [LIBRARY - Source Code Examples](https://www.insightsip.com/fichiers_insightsip/pdf/ble/ISP4520/ISP4520_Source_Code.zip)    


## Features
  - Support SX1261, SX1262 and SX1268 chips    
  - Support of LoRa protocol and FSK protocol (theoretical, I did not test FSK at all)    
  - Flexible setup for different modules (antenna control, TXCO control)    
  - Support LoRaWan node class A, B and C tested with single channel LoRaWan gateway    

  ### Licenses    
Library published under MIT license    

Semtech revised BSD license for codeparts used from Semtech S.A.   
```
--- Revised BSD License ---
Copyright (c) 2013, SEMTECH S.A.
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the Semtech corporation nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL SEMTECH S.A. BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
```