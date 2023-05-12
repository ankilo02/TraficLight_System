import serial.tools.list_ports
import random
import time
import  sys
from  Adafruit_IO import  MQTTClient

AIO_FEED_IDS = ["traffic-light", "tkll-traffic-light.traffic-counter"]
AIO_USERNAME = "sonnguyen19"
AIO_KEY = "aio_Stww11sSjB0YE86rM8OjR7j4F3jH"

def  connected(client):
    print("Ket noi thanh cong...")
    for feed in AIO_FEED_IDS:
        client.subscribe(feed)

def  subscribe(client , userdata , mid , granted_qos):
    print("Subcribe thanh cong...")

def  disconnected(client):
    print("Ngat ket noi...")
    sys.exit (1)

def  message(client , feed_id , payload):
    print("Nhan du lieu: " + payload)
    if isMicrobitConnected:
        if payload == "*" or payload == "4" or payload == "5" or payload == "#":
            ser.write((str(payload)).encode())

client = MQTTClient(AIO_USERNAME , AIO_KEY)
client.on_connect = connected
client.on_disconnect = disconnected
client.on_message = message
client.on_subscribe = subscribe
client.connect()
client.loop_background()

#"USB Serial Device"
#"com0com - serial port emulator"
#Standard Serial over Bluetooth link
def getPort():
    ports = serial.tools.list_ports.comports()
    N = len(ports)
    commPort = "None"
    for i in range(0, N):
        port = ports[i]
        strPort = str(port)
        if "USB-SERIAL CH340" in strPort:
            splitPort = strPort.split(" ")
            commPort = (splitPort[0])
    print(commPort)
    return commPort

isMicrobitConnected = False
if getPort() != "None":
    ser = serial.Serial( port=getPort(), baudrate=9600)
    isMicrobitConnected = True


def processData(data):
    data = data.replace("!", "")
    data = data.replace("#", "")
    splitData = data.split(":")
    print(splitData)
    try:
        if splitData[0] == "LIGHT":
            client.publish("traffic-light", splitData[1]+splitData[2])
            #client.publish("tkll-traffic-light.traffic-counter", splitData[1]+splitData[2])
        if splitData[0] == "COUNTER":
            client.publish("tkll-traffic-light.traffic-counter", splitData[1])
            #client.publish("traffic-counter", splitData[2])
    except:
        pass
    
mess = ""
def readSerial():
    bytesToRead = ser.inWaiting()
    if (bytesToRead > 0):
        global mess
        mess = mess + ser.read(bytesToRead).decode("UTF-8")
        #print(mess)
        while ("#" in mess) and ("!" in mess):
            start = mess.find("!")
            end = mess.find("#")
            processData(mess[start:end + 1])
            if (end == len(mess)):
                mess = ""
            else:
                mess = mess[end+1:]

while True:
    if isMicrobitConnected:
        readSerial()

    time.sleep(1)