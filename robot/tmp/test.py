import csv
from bitstring import BitArray

# with open('data.csv') as csvfile:
#     spamreader = csv.reader(csvfile)
#     for row in spamreader:
#         print(row[0][3:5])
availinter = [0,0,0,0]
with open('data.csv') as csvfile:
    spamreader = csv.reader(csvfile)
    for row in spamreader:
        if (BitArray(bin=row[0][0:2]).int == 1):
            print("ident ",BitArray(bin=row[0][0:2]))
            if(BitArray(bin=row[0][2:5]).int == 1):
                print("commande: ",BitArray(bin=row[0][2:5]))
                for i in range(4):
                    availinter[i] = int(row[0][i+5])
                        
print(availinter)
oui = str(bin(1)[2:])
direc = 3
while(len(oui) < 2):
    oui = "0" + oui

if direc == 0:
    oui = oui + "0010"
if direc == 1:
    oui = oui + "0001"
if direc == 2:
    oui = oui + "1000"
if direc == 3:
    oui = oui + "0100"
print(oui)