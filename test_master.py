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

def test_homing(node):
    print("\n--- Memulai Tes Homing Method 35 ---")
    
    # 1. Ganti Mode Operasi ke Homing (Mode 6)
    print("1. Setting Mode to Homing (6)...")
    node.sdo['Modes of operation'].raw = 6
    time.sleep(0.1)
    
    # 2. Set Metode Homing ke 35 (Current Position)
    # Kita coba tulis, kalau error (karena object belum ada di .dcf) kita abaikan dulu
    # karena default firmware Anda nanti akan melakukan logika reset posisi
    try:
        node.sdo[0x6098].raw = 35 
        print("2. Homing Method set to 35")
    except:
        print("2. Skip setting Homing Method (gunakan default logic)")

    # 3. Kirim Perintah 'Start Homing' (Bit 4 di Controlword)
    # Enable Operation (0xF) + Start Homing (0x10) = 0x1F
    print("3. Mengirim Start Homing (Controlword = 0x1F)...")
    node.sdo['Control word'].raw = 0x1F 
    time.sleep(0.5) # Beri waktu STM32 memproses

    # 4. Cek Statusword untuk konfirmasi sukses
    sw = node.sdo['Status word'].raw
    
    # Bit 12 = Homing Attained (Sukses), Bit 10 = Target Reached (Selesai)
    homing_sukses = (sw >> 12) & 1
    target_selesai = (sw >> 10) & 1
    
    print(f"   -> Statusword terbaca: 0x{sw:04X}")
    
    if homing_sukses and target_selesai:
        print("   -> HASIL: SUKSES! Motor berhasil di-nol-kan.")
    else:
        print("   -> HASIL: GAGAL. Statusword tidak sesuai.")

    # 5. Matikan bit 'Start Homing' dan kembali ke Mode Profile Position (1)
    print("4. Kembali ke Mode Profile Position (1)...")
    node.sdo['Control word'].raw = 0x0F # Matikan bit 4
    node.sdo['Modes of operation'].raw = 1
    time.sleep(0.1)

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
        # 1. Lakukan Enable dulu (seperti kode Anda sebelumnya)
        print("Mengaktifkan drive...")
        node.sdo['Control word'].raw = 6   # Shutdown
        node.sdo['Control word'].raw = 7   # Switch On
        node.sdo['Control word'].raw = 15  # Enable Operation
        time.sleep(0.5)

        # 2. Panggil tes Homing yang baru kita buat
        test_homing(node)

        # 3. (Opsional) Lanjut tes gerakan absolut untuk membuktikan posisi sudah 0
        print("\n--- Tes Gerakan setelah Homing ---")
        # Gerak ke 50000 (karena posisi sekarang sudah dianggap 0)
        node.sdo['Profile target position'].raw = 50000
        node.sdo['Control word'].raw = 0x1F # New Setpoint (Bit 4)
        
        wait_for_move_complete(node) # Fungsi yang sudah ada di kode lama Anda
        
        posisi_akhir = node.sdo['Actual motor position'].raw
        print(f"Posisi Akhir: {posisi_akhir}")

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