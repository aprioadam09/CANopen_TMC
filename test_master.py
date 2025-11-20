import canopen
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

    # --- Tes State Machine CiA 402 ---
    try:
        print("\n--- Memulai Tes State Machine CiA 402 ---")

        # 1. Baca Statusword awal (Harusnya Switch On Disabled)
        sw = node.sdo['Status word'].raw
        print(f"Statusword awal: 0x{sw:04X}")
        assert (sw & 0x004F) == 0x0040

        # 2. Transisi 2: Shutdown (dari Switch On Disabled -> Ready to Switch On)
        print("Mengirim perintah SHUTDOWN (6)...")
        node.sdo['Control word'].raw = 6
        time.sleep(0.1)
        sw = node.sdo['Status word'].raw
        print(f"  -> Statusword baru: 0x{sw:04X}")
        assert (sw & 0x006F) == 0x0021

        # 3. Transisi 3: Switch On (dari Ready to Switch On -> Switched On)
        print("Mengirim perintah SWITCH ON (7)...")
        node.sdo['Control word'].raw = 7
        time.sleep(0.1)
        sw = node.sdo['Status word'].raw
        print(f"  -> Statusword baru: 0x{sw:04X}")
        assert (sw & 0x006F) == 0x0023

        # 4. Transisi 4: Enable Operation (dari Switched On -> Operation Enabled)
        print("Mengirim perintah ENABLE OPERATION (15)...")
        node.sdo['Control word'].raw = 15
        time.sleep(0.1)
        sw = node.sdo['Status word'].raw
        print(f"  -> Statusword baru: 0x{sw:04X}")
        assert (sw & 0x006F) == 0x0027
        
        print("\n--- State Machine Berhasil Mencapai 'Operation Enabled' ---")

    except Exception as e:
        print(f"\n--- GAGAL Tes State Machine: {e} ---")
    finally:
        print("\nMenutup koneksi...")
        network.disconnect()

if __name__ == '__main__':
    main()