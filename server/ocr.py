import cv2
import easyocr
import numpy as np

# Reader einmal global laden (dauert beim ersten Mal ~10 Sekunden)
_reader = None

def get_reader() -> easyocr.Reader:
    global _reader
    if _reader is None:
        print("Lade EasyOCR Modell...")
        _reader = easyocr.Reader(['de', 'en'], gpu=False)
        print("Modell geladen.")
    return _reader

def preprocess(img: np.ndarray) -> np.ndarray:
    """Bild für bessere Kennzeichenerkennung vorbereiten."""
    # Hochskalieren (Kennzeichen sind oft klein im Bild)
    scale = 2.0
    img = cv2.resize(img, None, fx=scale, fy=scale, interpolation=cv2.INTER_CUBIC)
    # Graustufen
    gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
    # Kontrast erhöhen
    gray = cv2.equalizeHist(gray)
    # Binärisierung (Otsu wählt automatisch den besten Schwellwert)
    _, binary = cv2.threshold(gray, 0, 255, cv2.THRESH_BINARY + cv2.THRESH_OTSU)
    return binary

def run_ocr(img: np.ndarray, confidence_threshold: float = 0.4) -> list[str]:
    """
    OCR auf einem Bild ausführen.
    Gibt nur Ergebnisse zurück die über dem Confidence-Schwellwert liegen.
    """
    processed = preprocess(img)
    reader = get_reader()
    results = reader.readtext(processed)

    texts = []
    for _box, text, confidence in results:
        if confidence >= confidence_threshold:
            texts.append(text)

    return texts