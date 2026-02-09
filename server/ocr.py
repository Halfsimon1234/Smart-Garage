import cv2
import easyocr
import re
from collections import Counter

VIDEO_PATH = "assets/Kennzeichen_Test.mp4"

reader = easyocr.Reader(['de', 'en'])
cap = cv2.VideoCapture(VIDEO_PATH)

frame_id = 0
detected_plates = []

def clean(text):
    return "".join([c.upper() for c in text if c.isalnum()])

def extract_plate(fragments):
    letters = ""
    numbers = ""    

    for f in fragments:
        # nur Buchstaben
        if f.isalpha():
            letters += f
        # nur Zahlen
        elif f.isdigit():
            numbers += f
        else:
            # gemischt → trennen
            for c in f:
                if c.isalpha():
                    letters += c
                elif c.isdigit():
                    numbers += c

    # Plate = LETTERS + NUMBERS
    plate = letters + numbers

    # Mindestanforderung: mind. 1 Buchstabe + 2 Zahlen
    if re.match(r"^[A-Z]+[0-9]{2,}$", plate):
        return plate
    return None

while cap.isOpened():
    ret, frame = cap.read()
    if not ret:
        break

    frame_id += 1
    if frame_id % 10 != 0:
        continue

    results = reader.readtext(frame)

    fragments = []
    for box, text, conf in results:
        cleaned = clean(text)
        if len(cleaned) >= 2:
            fragments.append(cleaned)

    plate = extract_plate(fragments)
    if plate:
        detected_plates.append(plate)

cap.release()

# ---------- FINAL RESULT ----------
if detected_plates:
    final_plate = Counter(detected_plates).most_common(1)[0][0]
    print("\n============================")
    print("   FINAL KENNZEICHEN:")
    print("   ➡", final_plate)
    print("============================\n")
else:
    print("❌ Kein Kennzeichen erkannt")


Whitelist = [
    "BRJN9120"
    "BRJN910"  
]

if final_plate == Whitelist:
    print("asdf")



 