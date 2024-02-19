import multiprocessing
import queue
import time

import csv
from datetime import datetime

import dbus

from demo.ble_process import BLEProcess

from demo.util import byte_arr_to_str

dbus.mainloop.glib.DBusGMainLoop(set_as_default=True)

def main():
    output_queue = multiprocessing.Queue()

    ble_process = BLEProcess(output_queue)
    ble_process.start()
    with open('file.csv', 'w', newline='') as file:
        writer = csv.writer(file)
        writer.writerow(['hour', 'x','y'])


    while True:
        try:
            now = datetime.now()
            current_time = now.strftime("%H:%M:%S")
            #print("Current Time =", current_time)
            curr_value = output_queue.get(timeout=1)
            #for i in range(3,1) :
            #    output_queue.put({"uuid":'f76ce015-952b-c6a8-e17c-c2c19aac7b1b', "value": byte_arr_to_str(i)})
            #print(curr_value)
            print(f"Value written to Characteristic with UUID {curr_value['uuid']}: {curr_value['value']} at {current_time}")
            #with open('file.csv', 'w', newline='') as file:
            #    writer = csv.writer(file)
            #    writer.writerow([current_time, curr_value['value']])
            time.sleep(1)
        except queue.Empty:
            time.sleep(1)


if __name__ == "__main__":
    main()
