import requests
import json
from datetime import datetime
import matplotlib.pyplot as plt

ae = "Spoodergang"
cnt = "MotionDetected"

headers = {
    'X-M2M-Origin' : 'admin:admin',
    'Content-type' : 'application/json'
}

response = requests.get(f'http://127.0.0.1:8080/~/in-cse/in-name/{ae}/{cnt}/?rcn=4', headers=headers)

res = json.loads(response.text)

values = []
times = []

for item in res['m2m:cnt']['m2m:cin']:
    time = datetime.strptime(item['ct'], '%Y%m%dT%H%M%S')
    if (datetime.now()-time).total_seconds() < 3600.0:
        values.append(int(item['con']))
        times.append(time)

plt.plot(times, values)
plt.xlabel('Time')
plt.ylabel('Value')
plt.show()
