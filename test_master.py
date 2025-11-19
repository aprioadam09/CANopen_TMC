import canopen
import canopen.nmt
import time
import os

# --- Konfigurasi ---
NODE_ID = 2
EDS_FILE = 'slave.dcf' 
INTERFACE = 'socketcan'
CHANNEL = 'can0'

def main():
    print(f"Menggunakan file EDS/DCF: {os.path.abspath(EDS_FILE)}")
    if not os.path.exists(EDS_FILE):
        print(f"Error: File '{EDS_FILE}' tidak ditemukan.")
        return

    network = canopen.Network()
    network.connect(bustype=INTERFACE, channel=CHANNEL)
    print("Terhubung ke bus CAN.")

    # --- Logika Boot-up ---
    try:
        # [FIX] Perintah NMT Reset Node adalah broadcast dan hanya butuh 1 argumen
        network.nmt.send_command(0x81) 
        print("Mengirim NMT Reset, menunggu node untuk boot...")
        node = network.add_node(NODE_ID, EDS_FILE)
        node.nmt.wait_for_bootup(5)
        print(f"Node {NODE_ID} berhasil boot. Status: {node.nmt.state}")
    except Exception as e:
        print(f"Gagal menemukan node setelah reset. Error: {e}")
        network.disconnect()
        return

    # --- Urutan Tes ---
    
    # 1. Tes Baca Posisi Awal
    print("\n--- Tes 1: Membaca Posisi Awal (0x6064)...")
    try:
        initial_pos = node.sdo['Actual motor position'].raw
        print(f"  -> Berhasil: Posisi awal = {initial_pos}")
        assert initial_pos == 0
    except Exception as e:
        print(f"  -> Gagal: {e}")
        network.disconnect()
        return

    # 2. Tes Tulis Posisi Target
    target_pos = 51200
    print(f"\n--- Tes 2: Menulis Target Posisi (0x607A) = {target_pos}...")
    try:
        node.sdo['Profile target position'].raw = target_pos
        print("  -> Berhasil: Perintah tulis terkirim.")
        print("  ==> PERIKSA MOTOR, SEHARUSNYA BERGERAK SATU PUTARAN! <==")
        print("  Menunggu 5 detik...")
        time.sleep(5)
    except Exception as e:
        print(f"  -> Gagal: {e}")
        network.disconnect()
        return

    # 3. Tes Baca Posisi Akhir
    print("\n--- Tes 3: Membaca Posisi Akhir (0x6064)...")
    try:
        final_pos = node.sdo['Actual motor position'].raw
        print(f"  -> Berhasil: Posisi akhir = {final_pos}")
        if abs(final_pos - target_pos) < 256: 
             print("  -> Verifikasi Berhasil: Posisi akhir mendekati target.")
        else:
             print(f"  -> Verifikasi Gagal: Posisi akhir ({final_pos}) jauh dari target ({target_pos}).")
    except Exception as e:
        print(f"  -> Gagal: {e}")
        network.disconnect()
        return
        
    print("\n--- Semua Tes Selesai ---")
    
    network.disconnect()
    print("Koneksi ditutup.")

if __name__ == '__main__':
    main()