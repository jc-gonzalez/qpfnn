import requests

# Get request
r = requests.get('http://localhost:8000/euc.png')
print r.status_code
print r.headers

filename='e4.png'
with open(filename, 'wb') as fd:
    for chunk in r.iter_content(chunk_size=128):
        fd.write(chunk)

# Post request
payload = {'key1': 'value1', 'key2': 'value2'}

r = requests.put("http://localhost:8000/post", data=payload)
print(r.text)
