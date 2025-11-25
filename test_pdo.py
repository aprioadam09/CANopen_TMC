import canopen
import time

NODE_ID = 2
EDS_FILE = 'slave.dcf'
INTERFACE = 'socketcan'
CHANNEL = 'can0'

# Bitmask untuk Statusword CiA 402
SW_TARGET_REACHED = (1 << 10)
SW_HOMING_ATTAINED = (1 << 12)

def configure_pdos(node):
    """Konfigurasi PDO untuk transmisi event-driven"""
    print("\n--- Konfigurasi PDO ---")
    
    # Disable semua PDO dulu untuk rekonfigurasi
    print("Menonaktifkan PDO untuk konfigurasi...")
    for i in range(1, 4):  # RPDO 1-3
        cob_id = node.sdo[0x1400 + i - 1][1].raw
        node.sdo[0x1400 + i - 1][1].raw = cob_id | 0x80000000  # Set bit 31 = disable
    
    for i in range(1, 3):  # TPDO 1-2
        cob_id = node.sdo[0x1800 + i - 1][1].raw
        node.sdo[0x1800 + i - 1][1].raw = cob_id | 0x80000000  # Set bit 31 = disable
    
    # Konfigurasi TPDO untuk transmission type 254 (event-driven)
    print("Setting TPDO transmission type ke event-driven...")
    node.sdo[0x1800][2].raw = 254  # TPDO1: event-driven
    node.sdo[0x1801][2].raw = 254  # TPDO2: event-driven
    
    # Enable kembali semua PDO
    print("Mengaktifkan kembali PDO...")
    for i in range(1, 4):  # RPDO 1-3
        cob_id = node.sdo[0x1400 + i - 1][1].raw
        node.sdo[0x1400 + i - 1][1].raw = cob_id & 0x7FFFFFFF  # Clear bit 31 = enable
    
    for i in range(1, 3):  # TPDO 1-2
        cob_id = node.sdo[0x1800 + i - 1][1].raw
        node.sdo[0x1800 + i - 1][1].raw = cob_id & 0x7FFFFFFF  # Clear bit 31 = enable

    print("Membaca kembali konfigurasi PDO dari slave...")
    node.pdo.read()
    
    print("Konfigurasi PDO selesai!")

def setup_pdo_callbacks(node):
    """Setup callbacks untuk menerima TPDO dari slave"""
    
    def on_tpdo1_received(message):
        """Callback saat TPDO1 (Statusword) diterima"""
        statusword = int.from_bytes(message.data[0:2], byteorder='little')
        print(f"  [TPDO1] Statusword: 0x{statusword:04X}")
    
    def on_tpdo2_received(message):
        """Callback saat TPDO2 (Statusword + Actual Position) diterima"""
        statusword = int.from_bytes(message.data[0:2], byteorder='little')
        position = int.from_bytes(message.data[2:6], byteorder='little', signed=True)
        print(f"  [TPDO2] Statusword: 0x{statusword:04X}, Position: {position}")
    
    # Subscribe ke TPDO messages
    node.tpdo[1].add_callback(on_tpdo1_received)
    node.tpdo[2].add_callback(on_tpdo2_received)
    
    print("TPDO callbacks terpasang")

def enable_drive_pdo(node):
    """Enable drive menggunakan RPDO1 (Controlword only)"""
    print("\n--- Enable Drive via PDO ---")
    
    # Gunakan RPDO1 untuk mengirim Controlword
    # RPDO1 mapping: 0x6040 (Controlword, 16-bit)
    
    print("1. Shutdown (CW = 0x06)")
    node.rpdo[1]['Control word'].raw = 0x06
    node.rpdo[1].transmit()
    time.sleep(0.3)
    
    print("2. Switch On (CW = 0x07)")
    node.rpdo[1]['Control word'].raw = 0x07
    node.rpdo[1].transmit()
    time.sleep(0.3)
    
    print("3. Enable Operation (CW = 0x0F)")
    node.rpdo[1]['Control word'].raw = 0x0F
    node.rpdo[1].transmit()
    time.sleep(0.5)
    
    print("Drive enabled!")

def test_homing_pdo(node):
    """Test homing menggunakan RPDO2 (Controlword + Mode)"""
    print("\n--- Test Homing via PDO ---")
    
    # Gunakan RPDO2: Controlword (0x6040) + Modes of Operation (0x6060)
    print("1. Set Mode to Homing (6) + Enable Operation")
    node.rpdo[2]['Control word'].raw = 0x0F
    node.rpdo[2]['Modes of operation'].raw = 6
    node.rpdo[2].transmit()
    time.sleep(0.2)
    
    print("2. Start Homing (CW = 0x1F)")
    node.rpdo[2]['Control word'].raw = 0x1F
    node.rpdo[2]['Modes of operation'].raw = 6
    node.rpdo[2].transmit()
    time.sleep(0.5)
    
    # Verifikasi via SDO (untuk debugging)
    sw = node.sdo['Status word'].raw
    homing_attained = (sw >> 12) & 1
    target_reached = (sw >> 10) & 1
    
    print(f"   Statusword: 0x{sw:04X}")
    if homing_attained and target_reached:
        print("   ✓ HOMING SUKSES!")
    else:
        print("   ✗ HOMING GAGAL")
    
    # Kembali ke Profile Position Mode
    print("3. Kembali ke Profile Position Mode (1)")
    node.rpdo[2]['Control word'].raw = 0x0F
    node.rpdo[2]['Modes of operation'].raw = 1
    node.rpdo[2].transmit()
    time.sleep(0.2)

def move_absolute_pdo(node, target_position):
    """Gerakkan motor ke posisi absolut menggunakan RPDO3"""
    print(f"\n--- Move Absolute via PDO ke {target_position} ---")
    
    # RPDO3: Controlword (0x6040) + Target Position (0x607A)
    node.rpdo[3]['Control word'].raw = 0x0F  # Pastikan masih enabled
    node.rpdo[3]['Profile target position'].raw = target_position
    node.rpdo[3].transmit()
    time.sleep(0.05)
    
    # Trigger new setpoint dengan set bit 4
    node.rpdo[3]['Control word'].raw = 0x1F  # Enable + New Setpoint
    node.rpdo[3]['Profile target position'].raw = target_position
    node.rpdo[3].transmit()
    
    print(f"  Perintah gerakan ke {target_position} dikirim via PDO")

def wait_move_done_pdo(node, timeout=10):
    """Tunggu gerakan selesai dengan polling Statusword"""
    print("  Menunggu gerakan selesai...")
    start_time = time.time()
    
    while True:
        # Baca via SDO untuk sinkronisasi
        # Dalam implementasi full, ini bisa diganti dengan monitoring TPDO
        sw = node.sdo['Status word'].raw
        if sw & SW_TARGET_REACHED:
            print("  ✓ Target reached!")
            return True
        
        if time.time() - start_time > timeout:
            print("  ✗ Timeout!")
            return False
        
        time.sleep(0.1)

def main():
    network = canopen.Network()
    network.connect(bustype=INTERFACE, channel=CHANNEL)
    print("Terhubung ke CAN bus")
    
    try:
        # Reset dan tunggu bootup
        network.nmt.send_command(0x81)
        print("NMT Reset dikirim, menunggu bootup...")
        
        node = network.add_node(NODE_ID, EDS_FILE)
        node.nmt.wait_for_bootup(5)
        print(f"Node {NODE_ID} bootup sukses")
        
        # Setup PDO
        configure_pdos(node)
        setup_pdo_callbacks(node)
        time.sleep(0.5)
        
        # Test sequence dengan PDO
        enable_drive_pdo(node)
        test_homing_pdo(node)
        
        # Test beberapa gerakan
        positions = [50000, -30000, 100000, 0]
        for pos in positions:
            move_absolute_pdo(node, pos)
            wait_move_done_pdo(node)
            
            actual = node.sdo['Actual motor position'].raw
            print(f"  Position actual: {actual}\n")
            time.sleep(0.5)
        
        print("\n=== SEMUA TEST PDO SELESAI ===")
        
    except Exception as e:
        print(f"\n!!! ERROR: {e}")
        import traceback
        traceback.print_exc()
    
    finally:
        print("\nShutdown drive...")
        try:
            node.rpdo[1]['Control word'].raw = 0x06
            node.rpdo[1].transmit()
        except:
            pass
        
        network.disconnect()
        print("Disconnected")

if __name__ == '__main__':
    main()