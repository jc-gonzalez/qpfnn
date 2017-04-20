import requests
import json

BaseServerURL = 'http://localhost:8080/'

# Get request
r = requests.get(BaseServerURL + 'tmp/Docker.dmg')
print r.status_code
print r.headers

filename='D.dmg'
with open(filename, 'wb') as fd:
    for chunk in r.iter_content(chunk_size=128):
        fd.write(chunk)

# Get session ID
url = BaseServerURL + 'session'
print url
r = requests.get(url)
print r.text

# Post request
files = {'file': ('QPFnn.tgz', open('/home/eucops/QPFnn.tgz', 'rb'))}

url = BaseServerURL + 'upload'
print url
r = requests.post(url, files=files)
print r.text
