
***************************************************************************************************************************
2018.11.2 

1. Add support to RAK8212_M

2. How to use

    2.1 First enable the marco of product in ..\itracker\Source\include\board_platform.h like below:
    
    #define RAK8212
    
    //#define RAK8212_M
    
    //#define RAK8211_G
    
    //#define RAK8211_NB
    
    
    2.2 Then open the corresponding project in ..\itracker\ble_peripheral\itracker\pca10040\s132\arm5_no_packs\
   
3. Log display

    use J-link RTT Viewer to watch client running log like below:
   
   ![](https://github.com/RAKWireless/RAK8212_itracker_firmware_based_on_nRF5SDK15.0_freertos/blob/master/20180827122232.png)
 
 
   ![](https://github.com/RAKWireless/RAK8212_itracker_firmware_based_on_nRF5SDK15.0_freertos/blob/master/20180827122253.jpg)
   

***************************************************************************************************************************
2018.11.1

Update log:

1. Add support to RAK8211_NB


***************************************************************************************************************************

2018.10.29

**Important update log**:

We supply a project tool to help you have a quick start. You can choose the type of mcu and related sensors, and then make sure with
finish button. It will automatically config the project according to your choose and open the project. For example, choose like below:

*MCU: nRF52832

*Sensor: bme280,lis3dh,lis2mdl,opt3001,BG96

It means the product RAK_8212. Now it support RAK_8212 and RAK_8211_G, and we will add new product abidingly. And the config of each 
product, you can see it from our website below:

https://www.rakwireless.com/en/download

**Folder declare**:

RAK_CONFIG_TOOL: Tools source code, open with vs2015

head: product config head file

RUI_CONFIG_TOOL.exe: config tools

**Notice**:

When choose and finish, the keil project will open. If prompt message is the Syntax error, just affirm. 

**How to use**:

1. First download the zip file to your folder, then download the nRF52832 SDK 15.0.0

    https://github.com/RAKWireless/nRF5SDK15.0.0

2. Uncompress the zip and put itracker to the path like below 

..\src\nRF5_SDK_15.0.0_a53641a\itracker\

3. Open the RUI_CONFIG_TOOL.exe (based on 64 bit) below ..\itracker\tools. First choose mcu and then choose sensors. If the product is available ,the keil project will open automatically. Now it support RAK_8212 and RAK_8211_G, and we will add new product abidingly. And the config of each product, you can see it from our website below:

    https://www.rakwireless.com/en/download

***************************************************************************************************************************


2018.10.24

update log:

1. Support RAK8211-G and RAK8211 now

2. Find the product you will compile at \itracker\ble_peripheral\itracker\pca10040\s132\arm5_no_packs

3. Before compiling, choose the product in board_platform.h at itracker\Source\include. Just keep one like below:

//here define the type of itracker 

//#define RAK8212

#define RAK8211_G

4. GPS data is GPGGA format, detail is below:

$GPGGA,<1>,<2>,<3>,<4>,<5>,<6>,<7>,<8>,<9>,M,<10>,M,<11>,<12>*hh

<1> UTC time，hhmmss 

<2> Dimensionality

<3> Dimensions of the hemisphere, N or S

<4> Longitude

<5> Longitude of the hemisphereE, E or W

<6> GPS state：0=no location, 1=nondifferential positioning，2=differential positioning，6=estimating 

<7> satellite quantity（00~12）

<8> Horizontal dilution of precision（0.5~99.9） 

<9> Altitude（-9999.9~99999.9） 

<10> The height of the earth's ellipsoid relative to the geoid

<11> Differential time

<12> Differential station
***************************************************************************************************************************
RUI_itracker_platform_firmware
==

1.Overview

The project is a baseline of itracker with nRF5 SDK 15.0.0 and FreeRTOS, which includes series mcu of nRF52 and many kinds of peripheral devices, such as accelerometer, magnetometer... We are trying to provide a platform structure for itracker series.

It is developed based on 8212 and other products in existence will ba added in this project gradually. We have Refactored the code and increased the expansibility as a platform to accommodate as many as more products, to decrease difficulty of maintenance. Besides, we provide a series of common api for user to handle the different sensors, and user don't need to care about the concrete realization of every sensor.

2.Structure

![image](https://github.com/RAKWireless/RUI_itracker_platform_firmware/blob/master/structure.png)
The file includes two part：

2.1 bootloader

Bootloader is the major part of dfu feature of itracker. So if not need update feature, just ignore it.

2.2 itracker

Itracker includes serveral folders and details of each part shown below:

2.2.1 app

This folder is for user to develop their feature. Because Itracker has proveded two basic feature, ble and dfu, user could do their own jobs here. There is a readme.txt here to guide user to how to do. And a example here to show how to use our api.

2.2.2 board

Here include hardware parameters and api adapter for different product of RAK itracker. User just ignore.

2.2.3 driver

Here include many kinds of sensors driver of RAK itracker. User just ignore.

2.2.4 external

Here include external lib or third part code. User should put third part code or lib here, if need.

2.2.5 hal

Here is the hardware shield of nRF52 series, include kinds of bus, like I2C, SPI, UART....

2.2.6 include

Here include all head file

2.2.7 service

Here include service layer such as dfu and sensor init... User just ignore

3.Guide

This project is tailorable and we provide a series of common api for user to develop. And we provide a basic featuer: dfu. It is a background task. If no need, user could stop it in their own task. But if want to use it, user should stop other tasks in dfu_task(the line has been marked) when enter dfu process. Finally, user should pay attention to the ram size and use accurately, or it will overflow.
