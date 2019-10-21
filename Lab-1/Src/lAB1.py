import urllib.request
import json
import time
from pprint import pprint

# import Adafruit IO REST client
from Adafruit_IO import Client

ADAFRUIT_IO_USERNAME = 'ArduinoLab'
ADAFRUIT_IO_KEY = '2ab0daef666e49c5b98c0fce4901e647'

# Create an instance of the REST client
aio = Client(ADAFRUIT_IO_USERNAME, ADAFRUIT_IO_KEY)

# set up Adafruit IO feeds
temperature = aio.feeds('temperature')
humidity = aio.feeds('humidity')

while True:
  TS = urllib.request.urlopen("https://api.thingspeak.com/channels/859453/feeds.json?results=2")

  response = TS.read()
  data=json.loads(response)
  pprint(data)
  #b = data['channel']['field1']
  temperature_data = data['feeds'][1]['field1']
  #e = data['feeds'][1]['field1']
  print ("Temperature: " + temperature_data)
  aio.send(temperature.key, temperature_data)

  humidity_data = data['feeds'][0]['field2']
  print ("Humidity:" + humidity_data)
  aio.send(humidity.key, humidity_data)


  time.sleep(10)
  TS.close()

