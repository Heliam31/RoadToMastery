#!/usr/bin/python3
# -*- coding: utf8 -*-
# Copyleft https://tutoduino.fr/
import argparse
import asyncio
import csv
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
                    resp=b'000'#au robot n°1 : stop and wait
                else :
                    findRed=0
                    resp=b'011'#au robot n°2 : bouger
                print("rouge trouvé")
            case "010" : 
                if(findGreen==0) :
                    findGreen=1
                    resp=b'000'#au robot n°1 : stop and wait
                else :
                    findGreen=0
                    resp=b'011'#au robot n°2 : bouger
                print("vert trouvé")
            case "001" :
                if(findBlue==0) :
                    findBlue=1
                    resp=b'000'#au robot n°1 : stop and wait
                else :
                    findBlue=0
                    resp=b'011'#au robot n°2 : bouger
                print("bleu trouvé")
    else :
        match robot[-3:] : #robot n°2
            case "100" : 
                if(findRed==0) :
                    findRed=1
                    resp=b'010'#au robot n°2 : stop and wait
                else :
                    findRed=0
                    resp=b'001'#au robot n°1 : bouger
                print("rouge trouvé")
            case "010" : 
                if(findGreen==0) :
                    findGreen=1
                    resp=b'010'#au robot n°2 : stop and wait
                else :
                    findGreen=0
                    resp=b'001'#au robot n°1 : bouger
                print("vert trouvé")
            case "001" :
                if(findBlue==0) :
                    findBlue=1
                    resp=b'010'#au robot n°2 : stop and wait
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

    resp=b'0000'
    match data[:2] :#check quel robot
        case "00" :#Si robot n°1
            match data[2:5]:
                case "001" : #croisement ( mettre dans CSV la data récupérée, process par carto.py, prise de la réponse dans un csv)
                    fichier = open('data.csv','w')
                    #cw = csv.writer(fichier)
                    #cw.writerow("kiki")
                    fichier.write(data)
                    fichier.close()
                    f = open('data.csv','r')
                    for lines in f : 
                        print("la ligne: ",lines)
                    f.close()
                    while(True) :
                        with open('answer0.csv') as csvfile:
                            spamreader = csv.reader(csvfile)
                            for row in spamreader:
                                if (row != ""):
                                    #print("ligne: ", row)
                                    row = row[2] + row[3] + row[4] + row[5]
                                    resp=bytearray(row,encoding='utf-8')
                                    print("next position:",resp)
                                    break
                        break

                    ans = open("answer0.csv",'w')
                    cw = csv.writer(ans)
                    cw.writerow("")
                    ans.close()

                case "010" :#obstacle
                    resp=b'001000'#demi-tour
                case "011" : #couleur
                    resp=couleur(data)

        case "01" :#Si robot n°2
            match data[2:5]:
                case "001" : #croisement
                    fichier = open("data.csv",'a')
                    cw = csv.writer(fichier)
                    cw.writerow(data)
                    fichier.close()
                    while(True) :
                        with open("answer1.csv", "r") as f:
                            last_line = f.readlines()[-1]
                            if (last_line != ""):
                                resp=bytearray(last_line,encoding='utf-8')
                                break
                    ans = open("answer1.csv",'w')
                    cw = csv.writer(ans)
                    cw.writerow("")
                    ans.close()
                case "010" :#obstacle
                    resp=b'011000'#demi-tour
                case "011" : #couleur
                    resp=couleur(data)

    return resp

async def main():
    print("Searching Arduino Nano ESP32 'LED' device, please wait...")
    # Scan BLE devices for timeout seconds and return discovered devices with advertising data
    devices = await BleakScanner.discover(timeout=10,
                                          return_adv=True)
    for ble_device, adv_data in devices.values():
        #print(ble_device)
        if ble_device.name == 'LED':
            print("Device found")

            # Connect to Arduino Nano ESP 32 device
            async with BleakClient(ble_device.address) as client:
                
                print("Connected to device")
                while True:
                    try :
                        data=await client.read_gatt_char(UUIDRx)
                        print("Received: {0}".format("".join(map(chr, data))))                    
                        print(format("".join(map(chr, data))))
                        write_value=response(format("".join(map(chr, data))))
                        print(write_value)
                    except Exception as e:
                        print(f"Error:{e}")
                        await asyncio.sleep(1.0)
                        write_value=b'0'
                    #write_value = b"0110"
                    await client.write_gatt_char(UUIDTx, write_value)
                    #await asyncio.sleep(1.0)
                    data=await client.read_gatt_char(UUIDTx)
                    print("Response : {0}".format("".join(map(chr, data))))
                    await asyncio.sleep(1.0)
                       
if __name__ == "__main__":
    asyncio.run(main())
