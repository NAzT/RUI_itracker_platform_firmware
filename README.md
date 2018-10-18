RAK_itracker_firmware_based_on_nRF5SDK15.0_freertos_baseline


1.Overview

The project is a baseline of RAK itracker series, which includes series mcu of nRF52 and many kinds of peripheral devices, such as accelerometer, magnetometer...

It is developed based RAK8212 and other products in existence will ba added in this project gradually. We have Refactored the code and increased the expansibility as a platform to accommodate as many as more products, to decrease difficulty of maintenance. Besides, we provide a series of common api for user to handle the different sensors, and user don't need to care about the concrete realization of every sensor.

2.Structure

![image](https://github.com/RAKWireless/RAK_itracker_firmware_based_on_nRF5SDK15.0_freertos_baseline/blob/master/structure.png)
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
