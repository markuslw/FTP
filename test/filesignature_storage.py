import sys
import sysv_ipc

# define the key and size
SHM_KEY = 1234
SHM_SIZE = 1024

with open('shared.bin', 'rb') as file:
    data = file.read()
print(f"[Python] Data read from shared file: {data}")

filetype = None
match data[:2]:
    # Image formats
    case b'\xff\xd8':
        filetype = ".jpg"
    case b'\x89\x50':
        filetype = ".png"
    case b'\x47\x49':
        filetype = ".gif"
    case b'\x42\x4d':
        filetype = ".bmp"  # Bitmap image
    case b'\x49\x49' | b'\x4d\x4d':
        filetype = ".tiff"  # TIFF image (Little-endian or Big-endian)

    # Video formats
    case b'\x00\x00':
        filetype = ".mp4"
    case b'\x1a\x45':
        filetype = ".mkv"  # Matroska video file
    case b'\x66\x74':
        filetype = ".mov"  # MOV (QuickTime movie)
    case b'\x52\x49':
        filetype = ".avi"  # AVI video file
    case b'\x46\x4c':
        filetype = ".flv"  # Flash Video file

    # Audio formats
    case b'\xff\xfb':
        filetype = ".mp3"
    case b'\x49\x44':
        filetype = ".mp3"  # Alternative MP3 signature

    # Document formats
    case b'\x25\x50':
        filetype = ".pdf"
    case b'\xD0\xCF':
        filetype = ".doc"  # Older Microsoft Office documents (OLE2 Compound File, covers .doc, .xls, .ppt)
    case b'\x52\x54':
        filetype = ".rtf"  # Rich Text Format document

    # Compressed formats
    case b'\x50\x4b':
        filetype = ".zip"
    case b'\x1f\x8b':
        filetype = ".gz"  # GZIP compressed file
    case b'\x37\x7a':
        filetype = ".7z"  # 7-Zip compressed file
    case b'\xfd\x37':
        filetype = ".xz"  # XZ compressed file
print(f"\n[Python] Data is identified as: {filetype}\n")


with open('shared.bin', 'w') as file:
    file.write(filetype)
print(f"[Python] Data written to shared file: {filetype}")