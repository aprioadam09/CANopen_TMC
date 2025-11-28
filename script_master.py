#!/usr/bin/env python3
"""
Interactive CANopen CLI for TMC5160 Motor Control
Mendukung SDO dan PDO operations dengan command-line interaktif
"""

import canopen
import time
import cmd
import sys
from colorama import init, Fore, Style

# Initialize colorama untuk cross-platform colored output
init(autoreset=True)

# Konfigurasi
NODE_ID = 2
EDS_FILE = 'slave.dcf'
INTERFACE = 'socketcan'
CHANNEL = 'can0'

# Bitmask untuk Statusword
SW_TARGET_REACHED = (1 << 10)
SW_HOMING_ATTAINED = (1 << 12)

class CANopenCLI(cmd.Cmd):
    """Interactive CLI untuk kontrol motor TMC5160 via CANopen"""
    
    intro = f"""
{Fore.CYAN}╔══════════════════════════════════════════════════════════════╗
║  CANopen TMC5160 Interactive CLI v1.0                        ║
║  Ketik 'help' untuk melihat daftar command                   ║
║  Ketik 'help <command>' untuk detail command tertentu        ║
╚══════════════════════════════════════════════════════════════╝{Style.RESET_ALL}
"""
    prompt = f'{Fore.GREEN}canopen> {Style.RESET_ALL}'
    
    def __init__(self):
        super().__init__()
        self.network = None
        self.node = None
        self.is_connected = False
        self.is_enabled = False
        self.current_mode = None
        self.use_pdo = False  # Toggle antara SDO dan PDO
        
    # ==================== SETUP & CONNECTION ====================
    
    def do_connect(self, arg):
        """Koneksi ke CAN bus dan inisialisasi node
        Usage: connect"""
        if self.is_connected:
            print(f"{Fore.YELLOW}⚠ Sudah terkoneksi!{Style.RESET_ALL}")
            return
        
        try:
            print(f"{Fore.CYAN}🔌 Menghubungkan ke CAN bus...{Style.RESET_ALL}")
            self.network = canopen.Network()
            self.network.connect(bustype=INTERFACE, channel=CHANNEL)
            
            print(f"{Fore.CYAN}🔄 Mengirim NMT Reset...{Style.RESET_ALL}")
            self.network.nmt.send_command(0x81)
            
            print(f"{Fore.CYAN}⏳ Menunggu bootup node {NODE_ID}...{Style.RESET_ALL}")
            self.node = self.network.add_node(NODE_ID, EDS_FILE)
            self.node.nmt.wait_for_bootup(5)
            
            # Setup PDO
            self._configure_pdos()
            self._setup_pdo_callbacks()
            
            # Switch to OPERATIONAL
            print(f"{Fore.CYAN}🚀 Switching to OPERATIONAL...{Style.RESET_ALL}")
            self.node.nmt.state = 'OPERATIONAL'
            
            self.is_connected = True
            print(f"{Fore.GREEN}✅ Node {NODE_ID} berhasil terhubung!{Style.RESET_ALL}\n")
            
        except Exception as e:
            print(f"{Fore.RED}❌ Koneksi gagal: {e}{Style.RESET_ALL}")
            if self.network:
                self.network.disconnect()
                self.network = None
    
    def do_disconnect(self, arg):
        """Putuskan koneksi dari CAN bus
        Usage: disconnect"""
        if not self.is_connected:
            print(f"{Fore.YELLOW}⚠ Belum terkoneksi!{Style.RESET_ALL}")
            return
        
        try:
            if self.is_enabled:
                print(f"{Fore.CYAN}⚙ Menonaktifkan drive dulu...{Style.RESET_ALL}")
                self.do_disable("")
            
            print(f"{Fore.CYAN}🔌 Memutuskan koneksi...{Style.RESET_ALL}")
            self.network.nmt.send_command(0x80)  # PRE-OPERATIONAL
            time.sleep(0.2)
            self.network.disconnect()
            
            self.is_connected = False
            self.network = None
            self.node = None
            print(f"{Fore.GREEN}✅ Koneksi diputus dengan aman{Style.RESET_ALL}")
            
        except Exception as e:
            print(f"{Fore.RED}❌ Error saat disconnect: {e}{Style.RESET_ALL}")
    
    def _configure_pdos(self):
        """Konfigurasi internal untuk PDO"""
        # Disable PDO untuk konfigurasi
        for i in range(1, 4):
            cob_id = self.node.sdo[0x1400 + i - 1][1].raw
            self.node.sdo[0x1400 + i - 1][1].raw = cob_id | 0x80000000
        
        for i in range(1, 3):
            cob_id = self.node.sdo[0x1800 + i - 1][1].raw
            self.node.sdo[0x1800 + i - 1][1].raw = cob_id | 0x80000000
        
        # Set transmission type ke event-driven
        self.node.sdo[0x1800][2].raw = 254
        self.node.sdo[0x1801][2].raw = 254
        
        # Enable kembali
        for i in range(1, 4):
            cob_id = self.node.sdo[0x1400 + i - 1][1].raw
            self.node.sdo[0x1400 + i - 1][1].raw = cob_id & 0x7FFFFFFF
        
        for i in range(1, 3):
            cob_id = self.node.sdo[0x1800 + i - 1][1].raw
            self.node.sdo[0x1800 + i - 1][1].raw = cob_id & 0x7FFFFFFF
        
        self.node.pdo.read()
    
    def _setup_pdo_callbacks(self):
        """Setup callbacks untuk monitoring TPDO (silent mode)"""
        # Callback ini tidak print agar tidak mengganggu CLI
        # Tapi bisa diakses untuk monitoring
        self.last_statusword = None
        self.last_position = None
        
        def on_tpdo1(message):
            self.last_statusword = int.from_bytes(message.data[0:2], byteorder='little')
        
        def on_tpdo2(message):
            self.last_statusword = int.from_bytes(message.data[0:2], byteorder='little')
            self.last_position = int.from_bytes(message.data[2:6], byteorder='little', signed=True)
        
        self.node.tpdo[1].add_callback(on_tpdo1)
        self.node.tpdo[2].add_callback(on_tpdo2)
    
    # ==================== STATE MACHINE ====================
    
    def do_enable(self, arg):
        """Aktifkan drive (State Machine: Shutdown → Switch On → Enable)
        Usage: enable"""
        if not self._check_connected():
            return
        
        if self.is_enabled:
            print(f"{Fore.YELLOW}⚠ Drive sudah enabled!{Style.RESET_ALL}")
            return
        
        try:
            print(f"{Fore.CYAN}⚙ Mengaktifkan drive...{Style.RESET_ALL}")
            
            if self.use_pdo:
                # Gunakan RPDO1
                self.node.rpdo[1]['Control word'].raw = 0x06
                self.node.rpdo[1].transmit()
                time.sleep(0.3)
                
                self.node.rpdo[1]['Control word'].raw = 0x07
                self.node.rpdo[1].transmit()
                time.sleep(0.3)
                
                self.node.rpdo[1]['Control word'].raw = 0x0F
                self.node.rpdo[1].transmit()
                time.sleep(0.5)
            else:
                # Gunakan SDO
                self.node.sdo['Control word'].raw = 0x06
                time.sleep(0.3)
                self.node.sdo['Control word'].raw = 0x07
                time.sleep(0.3)
                self.node.sdo['Control word'].raw = 0x0F
                time.sleep(0.5)
            
            self.is_enabled = True
            sw = self.node.sdo['Status word'].raw
            print(f"{Fore.GREEN}✅ Drive enabled! Statusword: 0x{sw:04X}{Style.RESET_ALL}")
            
        except Exception as e:
            print(f"{Fore.RED}❌ Enable gagal: {e}{Style.RESET_ALL}")
    
    def do_disable(self, arg):
        """Nonaktifkan drive (kembali ke Shutdown state)
        Usage: disable"""
        if not self._check_connected():
            return
        
        try:
            print(f"{Fore.CYAN}⚙ Menonaktifkan drive...{Style.RESET_ALL}")
            
            if self.use_pdo:
                self.node.rpdo[1]['Control word'].raw = 0x06
                self.node.rpdo[1].transmit()
            else:
                self.node.sdo['Control word'].raw = 0x06
            
            time.sleep(0.3)
            self.is_enabled = False
            print(f"{Fore.GREEN}✅ Drive disabled{Style.RESET_ALL}")
            
        except Exception as e:
            print(f"{Fore.RED}❌ Disable gagal: {e}{Style.RESET_ALL}")
    
    # ==================== HOMING ====================
    
    def do_home(self, arg):
        """Lakukan homing (Method 35: Current Position)
        Usage: home"""
        if not self._check_enabled():
            return
        
        try:
            print(f"{Fore.CYAN}🏠 Memulai homing...{Style.RESET_ALL}")
            
            if self.use_pdo:
                # Set mode ke Homing (6)
                self.node.rpdo[2]['Control word'].raw = 0x0F
                self.node.rpdo[2]['Modes of operation'].raw = 6
                self.node.rpdo[2].transmit()
                time.sleep(0.2)
                
                # Start homing (bit 4 = 1)
                self.node.rpdo[2]['Control word'].raw = 0x1F
                self.node.rpdo[2]['Modes of operation'].raw = 6
                self.node.rpdo[2].transmit()
                time.sleep(0.5)
            else:
                # Gunakan SDO
                self.node.sdo['Modes of operation'].raw = 6
                time.sleep(0.1)
                self.node.sdo['Control word'].raw = 0x1F
                time.sleep(0.5)
            
            # Verifikasi
            sw = self.node.sdo['Status word'].raw
            homing_ok = (sw >> 12) & 1
            target_ok = (sw >> 10) & 1
            
            if homing_ok and target_ok:
                print(f"{Fore.GREEN}✅ Homing SUKSES! Posisi di-nol-kan.{Style.RESET_ALL}")
            else:
                print(f"{Fore.YELLOW}⚠ Homing mungkin gagal. SW: 0x{sw:04X}{Style.RESET_ALL}")
            
            # Kembali ke Profile Position Mode
            if self.use_pdo:
                self.node.rpdo[2]['Control word'].raw = 0x0F
                self.node.rpdo[2]['Modes of operation'].raw = 1
                self.node.rpdo[2].transmit()
            else:
                self.node.sdo['Control word'].raw = 0x0F
                self.node.sdo['Modes of operation'].raw = 1
            
            self.current_mode = 1
            time.sleep(0.2)
            
        except Exception as e:
            print(f"{Fore.RED}❌ Homing gagal: {e}{Style.RESET_ALL}")
    
    # ==================== MOTION CONTROL ====================
    
    def do_move(self, arg):
        """Gerakkan motor ke posisi absolut (CiA 402 Compliant)
        Usage: move <target_position>
        Contoh: move 50000"""
        if not self._check_enabled():
            return
        
        try:
            target = int(arg)
        except ValueError:
            print(f"{Fore.RED}❌ Format salah! Gunakan: move <angka>{Style.RESET_ALL}")
            return
        
        try:
            print(f"{Fore.CYAN}🎯 Menggerakkan ke posisi {target}...{Style.RESET_ALL}")
            
            if self.use_pdo:
                # CiA 402 Compliant: 3-step process
                # Step 1: Write target (motor belum gerak)
                self.node.rpdo[3]['Profile target position'].raw = target
                self.node.rpdo[3]['Control word'].raw = 0x0F
                self.node.rpdo[3].transmit()
                time.sleep(0.05)
                
                # Step 2: Clear bit 4
                self.node.rpdo[3]['Control word'].raw = 0x0F
                self.node.rpdo[3].transmit()
                time.sleep(0.05)
                
                # Step 3: Set bit 4 (RISING EDGE → TRIGGER!)
                self.node.rpdo[3]['Control word'].raw = 0x1F
                self.node.rpdo[3].transmit()
            else:
                # Gunakan SDO
                self.node.sdo['Profile target position'].raw = target
                time.sleep(0.05)
                self.node.sdo['Control word'].raw = 0x0F
                time.sleep(0.05)
                self.node.sdo['Control word'].raw = 0x1F
            
            print(f"{Fore.GREEN}✅ Perintah gerakan dikirim! Gunakan 'wait' untuk tunggu selesai.{Style.RESET_ALL}")
            
        except Exception as e:
            print(f"{Fore.RED}❌ Move gagal: {e}{Style.RESET_ALL}")
    
    def do_wait(self, arg):
        """Tunggu hingga gerakan selesai (polling Target Reached)
        Usage: wait [timeout]
        Default timeout: 10 detik"""
        if not self._check_enabled():
            return
        
        try:
            timeout = int(arg) if arg else 10
        except ValueError:
            timeout = 10
        
        print(f"{Fore.CYAN}⏳ Menunggu gerakan selesai...{Style.RESET_ALL}")
        start = time.time()
        
        try:
            while True:
                sw = self.node.sdo['Status word'].raw
                if sw & SW_TARGET_REACHED:
                    pos = self.node.sdo['Actual motor position'].raw
                    print(f"{Fore.GREEN}✅ Target tercapai! Posisi: {pos}{Style.RESET_ALL}")
                    return
                
                if time.time() - start > timeout:
                    print(f"{Fore.YELLOW}⚠ Timeout! Gerakan mungkin belum selesai.{Style.RESET_ALL}")
                    return
                
                time.sleep(0.1)
                
        except Exception as e:
            print(f"{Fore.RED}❌ Error saat wait: {e}{Style.RESET_ALL}")
    
    # ==================== STATUS & MONITORING ====================
    
    def do_status(self, arg):
        """Tampilkan status motor (Statusword, Position, Mode)
        Usage: status"""
        if not self._check_connected():
            return
        
        try:
            sw = self.node.sdo['Status word'].raw
            pos = self.node.sdo['Actual motor position'].raw
            mode = self.node.sdo['Modes of operation'].raw
            
            print(f"\n{Fore.CYAN}═══ Status Motor ═══{Style.RESET_ALL}")
            print(f"Statusword      : 0x{sw:04X}")
            print(f"  - Ready       : {'✅' if sw & 0x01 else '❌'}")
            print(f"  - Switched On : {'✅' if sw & 0x02 else '❌'}")
            print(f"  - Enabled     : {'✅' if sw & 0x04 else '❌'}")
            print(f"  - Target OK   : {'✅' if sw & SW_TARGET_REACHED else '❌'}")
            print(f"Position        : {pos}")
            print(f"Mode            : {mode} ({self._mode_name(mode)})")
            print(f"PDO Mode        : {'Enabled' if self.use_pdo else 'Disabled (SDO)'}")
            print()
            
        except Exception as e:
            print(f"{Fore.RED}❌ Gagal baca status: {e}{Style.RESET_ALL}")
    
    def do_getpos(self, arg):
        """Baca posisi motor saat ini
        Usage: getpos"""
        if not self._check_connected():
            return
        
        try:
            pos = self.node.sdo['Actual motor position'].raw
            print(f"{Fore.GREEN}📍 Posisi: {pos}{Style.RESET_ALL}")
        except Exception as e:
            print(f"{Fore.RED}❌ Gagal baca posisi: {e}{Style.RESET_ALL}")
    
    # ==================== PARAMETER SETTING (SDO) ====================
    
    def do_setvel(self, arg):
        """Set Profile Velocity (0x6081)
        Usage: setvel <value>
        Contoh: setvel 100000"""
        if not self._check_connected():
            return
        
        try:
            vel = int(arg)
            self.node.sdo['Profile target velocity'].raw = vel
            print(f"{Fore.GREEN}✅ Velocity set ke {vel}{Style.RESET_ALL}")
        except ValueError:
            print(f"{Fore.RED}❌ Format salah! Gunakan: setvel <angka>{Style.RESET_ALL}")
        except Exception as e:
            print(f"{Fore.RED}❌ Gagal set velocity: {e}{Style.RESET_ALL}")
    
    def do_setaccel(self, arg):
        """Set Profile Acceleration (0x6083)
        Usage: setaccel <value>
        Contoh: setaccel 5000"""
        if not self._check_connected():
            return
        
        try:
            accel = int(arg)
            self.node.sdo['Profile target acceleration'].raw = accel
            print(f"{Fore.GREEN}✅ Acceleration set ke {accel}{Style.RESET_ALL}")
        except ValueError:
            print(f"{Fore.RED}❌ Format salah! Gunakan: setaccel <angka>{Style.RESET_ALL}")
        except Exception as e:
            print(f"{Fore.RED}❌ Gagal set acceleration: {e}{Style.RESET_ALL}")
    
    def do_setdecel(self, arg):
        """Set Profile Deceleration (0x6084)
        Usage: setdecel <value>
        Contoh: setdecel 5000"""
        if not self._check_connected():
            return
        
        try:
            decel = int(arg)
            self.node.sdo['Profile target deceleration'].raw = decel
            print(f"{Fore.GREEN}✅ Deceleration set ke {decel}{Style.RESET_ALL}")
        except ValueError:
            print(f"{Fore.RED}❌ Format salah! Gunakan: setdecel <angka>{Style.RESET_ALL}")
        except Exception as e:
            print(f"{Fore.RED}❌ Gagal set deceleration: {e}{Style.RESET_ALL}")
    
    def do_getparams(self, arg):
        """Tampilkan parameter motion profile saat ini
        Usage: getparams"""
        if not self._check_connected():
            return
        
        try:
            vel = self.node.sdo['Profile target velocity'].raw
            accel = self.node.sdo['Profile target acceleration'].raw
            decel = self.node.sdo['Profile target deceleration'].raw
            
            print(f"\n{Fore.CYAN}═══ Motion Profile Parameters ═══{Style.RESET_ALL}")
            print(f"Velocity     : {vel}")
            print(f"Acceleration : {accel}")
            print(f"Deceleration : {decel}")
            print()
            
        except Exception as e:
            print(f"{Fore.RED}❌ Gagal baca parameters: {e}{Style.RESET_ALL}")
    
    # ==================== MODE SWITCHING ====================
    
    def do_usepdo(self, arg):
        """Switch ke mode PDO untuk motion commands
        Usage: usepdo"""
        self.use_pdo = True
        print(f"{Fore.GREEN}✅ Mode PDO diaktifkan. Command akan menggunakan RPDO.{Style.RESET_ALL}")
    
    def do_usesdo(self, arg):
        """Switch ke mode SDO untuk motion commands
        Usage: usesdo"""
        self.use_pdo = False
        print(f"{Fore.GREEN}✅ Mode SDO diaktifkan. Command akan menggunakan SDO.{Style.RESET_ALL}")
    
    # ==================== HELPER FUNCTIONS ====================
    
    def _check_connected(self):
        """Helper untuk cek koneksi"""
        if not self.is_connected:
            print(f"{Fore.RED}❌ Belum terkoneksi! Gunakan 'connect' dulu.{Style.RESET_ALL}")
            return False
        return True
    
    def _check_enabled(self):
        """Helper untuk cek drive enabled"""
        if not self._check_connected():
            return False
        if not self.is_enabled:
            print(f"{Fore.RED}❌ Drive belum enabled! Gunakan 'enable' dulu.{Style.RESET_ALL}")
            return False
        return True
    
    def _mode_name(self, mode):
        """Convert mode number ke nama"""
        modes = {1: "Profile Position", 6: "Homing", 3: "Profile Velocity"}
        return modes.get(mode, "Unknown")
    
    # ==================== CMD OVERRIDES ====================
    
    def do_exit(self, arg):
        """Keluar dari CLI
        Usage: exit"""
        if self.is_connected:
            print(f"{Fore.YELLOW}⚠ Koneksi masih aktif. Disconnect dulu? (y/n){Style.RESET_ALL}")
            choice = input().lower()
            if choice == 'y':
                self.do_disconnect("")
        
        print(f"{Fore.CYAN}👋 Terima kasih! Sampai jumpa.{Style.RESET_ALL}")
        return True
    
    def do_quit(self, arg):
        """Alias untuk exit"""
        return self.do_exit(arg)
    
    def emptyline(self):
        """Override agar tidak repeat command terakhir"""
        pass
    
    def default(self, line):
        """Handle unknown command"""
        print(f"{Fore.RED}❌ Command tidak dikenal: '{line}'{Style.RESET_ALL}")
        print(f"    Ketik 'help' untuk daftar command yang tersedia.")

# ==================== MAIN ENTRY POINT ====================

def main():
    try:
        cli = CANopenCLI()
        cli.cmdloop()
    except KeyboardInterrupt:
        print(f"\n{Fore.YELLOW}⚠ Interrupted by user{Style.RESET_ALL}")
        if cli.is_connected:
            cli.do_disconnect("")
    except Exception as e:
        print(f"\n{Fore.RED}❌ Fatal error: {e}{Style.RESET_ALL}")
        import traceback
        traceback.print_exc()

if __name__ == '__main__':
    main()