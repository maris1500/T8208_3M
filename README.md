# b80_cailun_WZ_9880

dual mode mouse

## V1.0.3

### Bug Fixes
* (mouse) Change the Bluetooth name of channel 1 to MS312-BT1-5.0, and the channel 2's name to MS312-BT1-5.0.
* (mouse) Update the flash lock judgment bug.
* (mouse) Fixed a bug where OTA forgot to feed 32k Watchdog.
* (mouse) Start different OTA startup addresses based on different flash sizes.
* (mouse) Fix the risk of content overflow in adv_scan_data.
* (mouse) Change the alarm voltage back to 1.65V, and the full voltage is 2.1V (for wz_9880).
* (dongle) Update the flash lock judgment bug.
* (dongle) Open the flash lock.
* (dongle) Don't upload device ID and version number to PC when pairing successful.

### Bug Fixes
* (mouse) 蓝牙名称1通道改为MS312_BT1_5.0，二通道改为MS312_BT1_5.0.
* (mouse) 更新flash锁判断条件.
* (mouse) 修复ota 忘喂32k Watchdog 的bug.
* (mouse) 根据不同flash大小启动不同的ota 启动地址.
* (mouse) 修复adv_scan_data内容溢出风险.
* (mouse) 报警电压改回1.65v, 满值电压为2.1v.(for wz_9880)
* (dongle) 更新flash锁判断条件.
* (dongle) 默认打开flash 锁.
* (dongle) 关闭配对成功上传device id和版本号给pc.