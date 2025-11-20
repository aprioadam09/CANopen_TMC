# File: test_master.py (versi baru untuk tes 5.4.3)

import canopen
import canopen.nmt
import time
import os

NODE_ID = 2
EDS_FILE = 'slave.dcf'
INTERFACE = 'socketcan'
CHANNEL = 'can0'

def main():
    network = canopen.Network()
    network.connect(bustype=INTERFACE, channel=CHANNEL)
    print("Terhubung ke bus CAN.")

    try:
        network.nmt.send_command(0x81)
        print("Mengirim NMT Reset, menunggu node untuk boot...")
        node = network.add_node(NODE_ID, EDS_FILE)
        node.nmt.wait_for_bootup(5)
        print(f"Node {NODE_ID} berhasil boot. Status awal NMT: {node.nmt.state}")
    except Exception as e:
        print(f"Gagal menemukan node. Error: {e}")
        network.disconnect()
        return

    try:
        print("\n--- Memulai Tes Keamanan State Machine ---")

        # 1. Pastikan kita di state awal (Switch On Disabled)
        sw = node.sdo['Status word'].raw
        print(f"Statusword awal: 0x{sw:04X}")
        assert (sw & 0x004F) == 0x0040

        # 2. Tes Gagal: Coba gerakkan motor saat drive belum diaktifkan
        print("\n--- Tes 1: Mencoba menggerakkan motor (harus gagal)...")
        try:
            node.sdo['Profile target position'].raw = 10000
            print("  -> GAGAL: Seharusnya SDO Abort, tetapi perintah berhasil.")
        except canopen.sdo.SdoAbortedError as e:
            print(f"  -> SUKSES: Perintah ditolak seperti yang diharapkan. Kode Abort: {e.code:08X}")
            # 0x06090030 adalah "Value range of parameter exceeded" atau
            # 0x08000022 adalah "Data cannot be transferred... because of the device state"
            assert e.code in [0x08000022, 0x06090030]
        
        # 3. Lakukan urutan enable yang benar
        print("\n--- Tes 2: Urutan Enable CiA 402 ---")
        node.sdo['Control word'].raw = 6   # Shutdown
        time.sleep(0.1)
        node.sdo['Control word'].raw = 7   # Switch On
        time.sleep(0.1)
        node.sdo['Control word'].raw = 15  # Enable Operation
        time.sleep(0.1)
        sw = node.sdo['Status word'].raw
        print(f"  -> Statusword setelah Enable: 0x{sw:04X}")
        assert (sw & 0x006F) == 0x0027

        # 4. Tes Berhasil: Coba gerakkan motor saat drive sudah aktif
        print("\n--- Tes 3: Menggerakkan motor (harus berhasil)...")
        target_pos = 51200
        node.sdo['Profile target position'].raw = target_pos
        print(f"  -> Perintah tulis ke {target_pos} terkirim.")
        print("  ==> MOTOR SEHARUSNYA BERGERAK SEKARANG! <==")
        time.sleep(5) # Tunggu gerakan

    except Exception as e:
        print(f"\n--- GAGAL Tes: {e} ---")
    finally:
        print("\nMenonaktifkan drive dan menutup koneksi...")
        try:
            # Urutan shutdown yang aman
            node.sdo['Control word'].raw = 7 # Disable Operation
            time.sleep(0.1)
            node.sdo['Control word'].raw = 6 # Shutdown
        except:
            pass # Mungkin sudah terputus
        network.disconnect()

if __name__ == '__main__':
    main()