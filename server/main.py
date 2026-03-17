import cv2
import sys
from ocr import run_ocr
from detect_plate import extract_best_plate, normalize, sort_plate_fragments
from whitelist import WHITELIST

VIDEO_PATH = "../assets/Kennzeichen_Test.mp4"
FRAME_SKIP = 10
MIN_DETECTIONS = 2

def run_video_test(video_path: str):
    cap = cv2.VideoCapture(video_path)
    if not cap.isOpened():
        print(f"Fehler: Video nicht gefunden: {video_path}")
        sys.exit(1)

    all_detections = []
    frame_id = 0

    print(f"Verarbeite Video: {video_path}")
    print(f"Jeden {FRAME_SKIP}. Frame wird analysiert...")

    while cap.isOpened():
        ret, frame = cap.read()
        if not ret:
            break

        frame_id += 1
        if frame_id % FRAME_SKIP != 0:
            continue

        texts = run_ocr(frame)
        print(f"Frame {frame_id}: {texts}")

        # Fragmente sortieren und zusammensetzen (Buchstaben vor Zahlen)
        combined = sort_plate_fragments(texts)
        if combined:
            all_detections.append(combined)
        all_detections.extend(texts)

    cap.release()

    final_plate = extract_best_plate(all_detections)

    print("\n" + "=" * 40)
    if final_plate:
        print(f"  ERKANNTES KENNZEICHEN: {final_plate}")
        if normalize(final_plate) in WHITELIST:
            print("  ZUGANG: ERLAUBT")
        else:
            print("  ZUGANG: VERWEIGERT")
    else:
        print("  Kein Kennzeichen sicher erkannt")
        print(f"  Alle Erkennungen: {all_detections}")
    print("=" * 40 + "\n")

if __name__ == "__main__":
    path = sys.argv[1] if len(sys.argv) > 1 else VIDEO_PATH
    run_video_test(path)