#!/usr/bin/python3
# -*- coding: utf8 -*-
# Copyleft https://tutoduino.fr/
import argparse
import asyncio
from bleak import BleakScanner
from bleak import BleakClient
UUIDRx = '2ce4b984-2934-4e3a-abe5-1034ac6ca3f1'
UUIDTx = '2ce4b985-2934-4e3a-abe5-1034ac6ca3f1'
async def main():
    print("Searching Arduino Nano ESP32 'LED' device, please wait...")
    # Scan BLE devices for timeout seconds and return discovered devices with advertising data
    devices = await BleakScanner.discover(timeout=10,
                                          return_adv=True)
    for ble_device, adv_data in devices.values():
        if ble_device.name == 'LED':
            print("Device found")

            # Connect to Arduino Nano ESP 32 device
            async with BleakClient(ble_device.address) as client:
                
                print("Connected to device")
                while True:
                    try :
                        data=await client.read_gatt_char(UUIDRx)
                        print("Received: {0}".format("".join(map(chr, data))))                    
                        print("try")
                    except Exception as e:
                        print(f"Error:{e}")
                        await asyncio.sleep(1.0)
                    write_value = b"0110"
                    await client.write_gatt_char(UUIDTx, write_value)
                    #await asyncio.sleep(1.0)
                    data=await client.read_gatt_char(UUIDTx)
                    print("Received2: {0}".format("".join(map(chr, data))))
                    await asyncio.sleep(1.0)
                       
if __name__ == "__main__":
    asyncio.run(main())
