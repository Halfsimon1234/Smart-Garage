import re
from collections import Counter

# Erweitertes Muster — akzeptiert auch wenn Zahlen in der Mitte sind
AT_PLATE_PATTERN = re.compile(r'^[A-Z]{2,6}[0-9]{2,4}[A-Z]?$')

def normalize(text: str) -> str:
    cleaned = "".join(c.upper() for c in text if c.isalnum())
    cleaned = re.sub(r'([A-Z])I([A-Z])', r'\1\2', cleaned)
    cleaned = re.sub(r'([A-Z]{2,})I([0-9])', r'\1\2', cleaned)
    return cleaned

def sort_plate_fragments(texts: list[str]) -> str:
    """Alle Buchstaben aus allen Fragmenten vor alle Zahlen stellen."""
    letters = ""
    numbers = ""
    for t in texts:
        cleaned = normalize(t)
        for c in cleaned:
            if c.isalpha():
                letters += c
            elif c.isdigit():
                numbers += c
    result = letters + numbers
    return result if result else ""


def is_valid_plate(text: str) -> bool:
    normalized = normalize(text)
    if len(normalized) < 5 or len(normalized) > 8:
        return False
    return bool(AT_PLATE_PATTERN.match(normalized))

def extract_best_plate(all_detections: list[str]) -> str | None:
    valid = [normalize(d) for d in all_detections if is_valid_plate(d)]
    if not valid:
        return None
    most_common, count = Counter(valid).most_common(1)[0]
    if count < 2:
        return None
    return most_common