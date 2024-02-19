import multiprocessing
import queue
import time


import csv
from datetime import datetime

import dbus
from demo.core_ble.constants import DBUS_PROP_IFACE, GATT_CHRC_IFACE
from demo.ble_process import BLEProcess

from demo.util import byte_arr_to_str

dbus.mainloop.glib.DBusGMainLoop(set_as_default=True)


def main():
    output_queue = multiprocessing.Queue()
    ble_process = BLEProcess(output_queue)
    ble_process.start()
    
    while True:
        try:
            now = datetime.now()
            current_time = now.strftime("%H:%M:%S")
            curr_value = output_queue.get(timeout=1)
            print(curr_value['value'])
            print(f"Value written to Characteristic with UUID {curr_value['uuid']}: {curr_value['value']} at {current_time}")
            

            #IF HEADER 001 CROISEMENT :
            if (curr_value['value'][0-2]=="001") :
                with open('cartographie.csv', 'r') as fichier:
                    data = csv.DictReader(fichier)
                    donnees=list(data)

                donnees.append({"date": current_time, "value":curr_value['value']})

                with open('cartographie.csv', 'w', newline='') as file:
                    champs = list({"date": current_time, "value":curr_value['value']}.keys())
                    ecrivain_csv = csv.DictWriter(file, fieldnames=champs)
                    if not file.readable():
                        ecrivain_csv.writeheader()

                    ecrivain_csv.writerows(donnees)

            #IF HEADER 010 OBSTACLE :
            if (curr_value['value'][0-2]=="010") :
                print(f"Obstable détecté à : {current_time}")

                
            #IF HEADER 011 COULEUR :
            if (curr_value['value'][0-2]=="011") : 
                print(f"Couleur détectée : {curr_value['value']} à {current_time}")
                #Envoie message à autre robot pour lui dire

                
                
            #time.sleep(1)
        except queue.Empty:
            dataToSend={'uuid': 'f76ce015-952b-c6a8-e17c-c2c19aac7b1b', 'value': 'changed'}
            #print("oui")
            output_queue.put(dataToSend)
            #curr_value = output_queue.get(timeout=1)
            #print(f"Value written to Characteristic with UUID {curr_value['uuid']}: {curr_value['value']} at {current_time}")
            #ble_process.PropertiesChanged(GATT_CHRC_IFACE, {"Value": "ouiiii"}, [])
            time.sleep(1)
    

if __name__ == "__main__":
    main()
