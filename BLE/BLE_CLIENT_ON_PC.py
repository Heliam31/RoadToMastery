#!/usr/bin/python3
# -*- coding: utf8 -*-
# Copyleft https://tutoduino.fr/
import argparse
import asyncio
from bleak import BleakScanner
from bleak import BleakClient
UUID = '19b10000-e8f2-537e-4f6c-d104768a1214'
async def main():
    print("Searching Arduino Nano ESP32 'LED' device, please wait...")
    # Scan BLE devices for timeout seconds and return discovered devices with advertising data
    devices = await BleakScanner.discover(timeout=30,
                                          return_adv=True)
    for ble_device, adv_data in devices.values():
        if ble_device.name == 'LED':
            print("Device found")
            # Connect to Arduino Nano ESP 32 device
            async with BleakClient(ble_device.address) as client:
                print("Connected to device")
                while True:
                    value=10111
                    val = await client.write_gatt_char(UUID, value)
                    await asyncio.sleep(1.0)
                    data=await client.read_gatt_char(UUID)
                    print("Received: {0}".format("".join(map(chr, data))))
                    await asyncio.sleep(1.0)
if __name__ == "__main__":
    asyncio.run(main())
