import serial
import serial.tools.list_ports
ports = serial.tools.list_ports.comports()
com_list = []
for p in ports:
    com_list.append(p.device)
    if(p.description.find("Arduino")!=-1):
        com = p.name
ser = serial.Serial(com, 9600)
file = open("data.csv",'w')
print("Starting to Export from Serial Window.")
file.write("Displacement,Pressure\n")
while True:
    try:
        res =ser.readline()
        file.write(res.decode("ascii").replace("\r",""))
    except:
        print("Exception Found-Finish Exporting.")
        ser.close()
        file.close()
        exit()