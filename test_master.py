import canopen
import canopen.nmt
import time
import os

NODE_ID = 2
EDS_FILE = 'slave.dcf'
INTERFACE = 'socketcan'
CHANNEL = 'can0'

# Bitmask untuk Statusword CiA 402
SW_TARGET_REACHED = (1 << 10)

def wait_for_move_complete(node, timeout=10):
    """Polls the Statusword until the 'Target reached' bit is set."""
    print("  -> Menunggu gerakan selesai (polling Statusword bit 10)...")
    start_time = time.time()
    while True:
        try:
            sw = node.sdo['Status word'].raw
            if sw & SW_TARGET_REACHED:
                print("  -> 'Target reached' terdeteksi!")
                return True
        except canopen.sdo.SdoCommunicationError:
            # Abaikan timeout SDO sesekali, coba lagi
            pass
        
        if time.time() - start_time > timeout:
            print("  -> GAGAL: Timeout menunggu gerakan selesai.")
            return False
        
        time.sleep(0.1) # Jeda polling

def main():
    network = canopen.Network()
    network.connect(bustype=INTERFACE, channel=CHANNEL)
    print("Terhubung ke bus CAN.")

    try:
        network.nmt.send_command(0x81)
        print("Mengirim NMT Reset, menunggu node untuk boot...")
        node = network.add_node(NODE_ID, EDS_FILE)
        node.nmt.wait_for_bootup(5)
        print(f"Node {NODE_ID} berhasil boot.")
    except Exception as e:
        print(f"Gagal menemukan node. Error: {e}")
        network.disconnect()
        return

    try:
        print("\n--- Memulai Tes Gerakan dengan State Machine ---")

        # 1. Lakukan urutan enable
        print("Mengaktifkan drive (Enable Sequence)...")
        node.sdo['Control word'].raw = 6   # Shutdown
        time.sleep(0.1)
        node.sdo['Control word'].raw = 7   # Switch On
        time.sleep(0.1)
        node.sdo['Control word'].raw = 15  # Enable Operation
        time.sleep(0.1)
        sw = node.sdo['Status word'].raw
        if (sw & 0x006F) != 0x0027:
            raise Exception(f"Gagal mencapai state Operation Enabled. Statusword: 0x{sw:04X}")
        print("  -> Drive sekarang 'Operation Enabled'.")

        # 2. Perintahkan gerakan
        target_pos = 51200
        print(f"\nMemerintahkan motor bergerak ke posisi {target_pos}...")
        node.sdo['Profile target position'].raw = target_pos
        
        # 3. Tunggu gerakan selesai menggunakan polling Statusword
        if not wait_for_move_complete(node):
            raise Exception("Motor tidak menyelesaikan gerakan dalam waktu yang ditentukan.")

        final_pos = node.sdo['Actual motor position'].raw
        print(f"Gerakan selesai. Posisi akhir terbaca: {final_pos}")
        assert abs(final_pos - target_pos) < 256

        print("\n--- Tes 'wait_move_done' Berhasil! ---")

    except Exception as e:
        print(f"\n--- GAGAL Tes: {e} ---")
    finally:
        print("\nMenonaktifkan drive dan menutup koneksi...")
        try:
            node.sdo['Control word'].raw = 6 # Shutdown
        except:
            pass
        network.disconnect()

if __name__ == '__main__':
    main()