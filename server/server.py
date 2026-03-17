from flask import Flask, request, jsonify
import cv2
import numpy as np
import logging

from ocr import run_ocr
from detect_plate import extract_best_plate, normalize
from whitelist import WHITELIST

app = Flask(__name__)
logging.basicConfig(level=logging.INFO, format="%(asctime)s %(levelname)s %(message)s")

@app.route("/check", methods=["POST"])
def check_plate():
    if "image" not in request.files:
        return jsonify({"error": "Kein Bild übermittelt"}), 400

    img_bytes = request.files["image"].read()
    img_array = np.frombuffer(img_bytes, np.uint8)
    img = cv2.imdecode(img_array, cv2.IMREAD_COLOR)

    if img is None:
        return jsonify({"error": "Bild konnte nicht gelesen werden"}), 400

    # OCR ausführen
    detected_texts = run_ocr(img)
    logging.info(f"OCR Ergebnisse: {detected_texts}")

    # Bestes Kennzeichen aus diesem einzelnen Bild extrahieren
    # (Mehrfach-Frame-Voting passiert beim Video-Test in main.py)
    from detect_plate import is_valid_plate
    valid_plates = [t for t in detected_texts if is_valid_plate(t)]

    if not valid_plates:
        logging.info("Kein gültiges Kennzeichen erkannt")
        return jsonify({"open": False, "reason": "Kein Kennzeichen erkannt"})

    # Whitelist prüfen — FIX: 'in' statt '==' !!
    for plate_raw in valid_plates:
        plate = normalize(plate_raw)
        if plate in WHITELIST:
            logging.info(f"ZUGANG ERLAUBT: {plate}")
            return jsonify({"open": True, "plate": plate})

    logging.info(f"ZUGANG VERWEIGERT. Erkannt: {valid_plates}")
    return jsonify({"open": False, "detected": valid_plates})

if __name__ == "__main__":
    app.run(host="0.0.0.0", port=5000, debug=True)