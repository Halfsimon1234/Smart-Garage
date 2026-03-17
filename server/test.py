import requests

IMAGE_PATH = "../assets/Kennzeichen_test.jpg"
SERVER_URL  = "http://localhost:5000/check"

with open(IMAGE_PATH, "rb") as f:
    response = requests.post(SERVER_URL, files={"image": f})

print("Status Code:", response.status_code)
print("Rohe Antwort:", response.text)