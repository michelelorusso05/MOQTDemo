# preprocess.py - converte file .ply in formato binario
# I file vengono processati al fine di occupare minor spazio possibile prima
# di essere indirizzati sulla rete.
# Il formato è il seguente: XXYYZZRGB, dove ogni lettera rappresenta un byte.
# X, Y, Z: coordinate 3D (2 byte ciascuna), R, G, B: componenti colore del punto (1 byte ciascuna)

import struct
import glob

def ply_to_bin(ply_path, bin_path):
    with open(ply_path, 'r') as f:
        lines = f.readlines()

    header_end = 0
    for i, line in enumerate(lines):
        if "end_header" in line:
            header_end = i + 1
            break

    points = []
    for line in lines[header_end:]:
        if not line.strip(): continue
        
        parts = line.split()
        if len(parts) >= 6:
            x, y, z = int(parts[0]), int(parts[1]), int(parts[2])
            r, g, b = int(parts[3]), int(parts[4]), int(parts[5])
            
            packed = struct.pack('<HHHBBB', x, y, z, r, g, b)
            points.append(packed)

    with open(bin_path, 'wb') as f:
        f.write(b''.join(points))
        
    print(f"Salvato {bin_path} - Punti: {len(points)}")

if __name__ == "__main__":
    for f in glob.glob("*.ply"):
        ply_to_bin(f, f[:-4] + ".bin")