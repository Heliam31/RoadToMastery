#!/usr/bin/python3
# -*- coding: utf8 -*-
# Copyleft https://tutoduino.fr/
import argparse
import asyncio
from bleak import BleakScanner
from bleak import BleakClient
UUIDRx = '2ce4b984-2934-4e3a-abe5-1034ac6ca3f1'
UUIDTx = '2ce4b985-2934-4e3a-abe5-1034ac6ca3f1'

findRed = 0
findGreen = 0
findBlue = 0

def couleur(robot):
    global findRed
    global findGreen
    global findBlue
    if (robot[:2]=="00"):
        match robot[-3:] : #robot n°1
            case "100" : 
                if(findRed==0) :
                    findRed=1
                    resp=b'000'#au robot n°1 : stop
                else :
                    findRed=0
                    resp=b'011'#au robot n°2 : bouger
                print("rouge trouvé")
            case "010" : 
                if(findGreen==0) :
                    findGreen=1
                    resp=b'000'#au robot n°1 : stop
                else :
                    findGreen=0
                    resp=b'011'#au robot n°2 : bouger
                print("vert trouvé")
            case "001" :
                if(findBlue==0) :
                    findBlue=1
                    resp=b'000'#au robot n°1 : stop
                else :
                    findBlue=0
                    resp=b'011'#au robot n°2 : bouger
                print("bleu trouvé")
    else :
        match robot[-3:] : #robot n°2
            case "100" : 
                if(findRed==0) :
                    findRed=1
                    resp=b'010'#au robot n°2 : stop
                else :
                    findRed=0
                    resp=b'001'#au robot n°1 : bouger
                print("rouge trouvé")
            case "010" : 
                if(findGreen==0) :
                    findGreen=1
                    resp=b'010'#au robot n°2 : stop
                else :
                    findGreen=0
                    resp=b'001'#au robot n°1 : bouger
                print("vert trouvé")
            case "001" :
                if(findBlue==0) :
                    findBlue=1
                    resp=b'010'#au robot n°2 : stop
                else :
                    findBlue=0
                    resp=b'001'#au robot n°1 : bouger
                print("bleu trouvé")
    print(resp)
    print(findRed,findGreen,findBlue)
    return resp


def response(data):
    # trois premiers : print(data[:3])
    # trois derniers : data[-3:]
    #au milieu : data[2:5]

    resp=b'10000'
    match data[:2] :#check quel robot
        case "00" :#Si robot n°1
            match data[2:5]:
                case "001" : #croisement
                    resp=b'1111' #A VOIR
                case "010" :#obstacle
                    resp=b'008'#demi-tour
                case "011" : #couleur
                    resp=couleur(data)

        case "01" :#Si robot n°2
            match data[2:5]:
                case "001" : #croisement
                    resp=b'111'
                case "010" :#obstacle
                    resp=b'018'#demi-tour
                case "011" : #couleur
                    resp=couleur(data)

    return resp

async def main():
    print("Searching Arduino Nano ESP32 'LED' device, please wait...")
    # Scan BLE devices for timeout seconds and return discovered devices with advertising data
    devices = await BleakScanner.discover(timeout=20,
                                          return_adv=True)
    for ble_device, adv_data in devices.values():
        print(ble_device)
        if ble_device.name == 'LED':
            print("Device found")

            # Connect to Arduino Nano ESP 32 device
            async with BleakClient(ble_device.address) as client:
                
                print("Connected to device")
                while True:
                    try :
                        print("oui")
                        data=await client.read_gatt_char(UUIDRx)
                        print("Received: {0}".format("".join(map(chr, data))))                    
                        print(format("".join(map(chr, data))))
                        write_value=response(format("".join(map(chr, data))))
                        print(write_value)
                    except Exception as e:
                        print(f"Error:{e}")
                        await asyncio.sleep(1.0)
                        #write_value=b'0000'
                    #write_value = b"0110"
                    await client.write_gatt_char(UUIDTx, write_value)
                    #await asyncio.sleep(1.0)
                    data=await client.read_gatt_char(UUIDTx)
                    print("Received2: {0}".format("".join(map(chr, data))))
                    await asyncio.sleep(1.0)
                       
if __name__ == "__main__":
    asyncio.run(main())
