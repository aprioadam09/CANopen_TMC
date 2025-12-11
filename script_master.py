#!/usr/bin/env python3
"""
Interactive CANopen CLI for TMC5160 Motor Control
Supports SDO and PDO operations with interactive command-line interface
"""

import canopen
import time
import cmd
import sys
from colorama import init, Fore, Style

# Initialize colorama for cross-platform colored output
init(autoreset=True)

# Configuration
NODE_ID = 2
EDS_FILE = 'slave.dcf'
INTERFACE = 'socketcan'
CHANNEL = 'can0'

# Bitmask for Statusword
SW_TARGET_REACHED = (1 << 10)
SW_HOMING_ATTAINED = (1 << 12)

class CANopenCLI(cmd.Cmd):
    """Interactive CLI for TMC5160 motor control via CANopen"""
    
    intro = f"""
{Fore.CYAN}================================================================
  CANopen TMC5160 Interactive CLI v1.0
  Type 'help' to see list of commands
  Type 'help <command>' for detailed command information
================================================================{Style.RESET_ALL}
"""
    prompt = f'{Fore.GREEN}canopen> {Style.RESET_ALL}'
    
    def __init__(self):
        super().__init__()
        self.network = None
        self.node = None
        self.is_connected = False
        self.is_enabled = False
        self.current_mode = None
        self.use_pdo = False  # Toggle between SDO and PDO
        
    # ==================== SETUP & CONNECTION ====================
    
    def do_connect(self, arg):
        """Connect to CAN bus and initialize node
        Usage: connect"""
        if self.is_connected:
            print(f"{Fore.YELLOW}Already connected!{Style.RESET_ALL}")
            return
        
        try:
            print(f"{Fore.CYAN}Connecting to CAN bus...{Style.RESET_ALL}")
            self.network = canopen.Network()
            self.network.connect(bustype=INTERFACE, channel=CHANNEL)
            
            print(f"{Fore.CYAN}Sending NMT Reset...{Style.RESET_ALL}")
            self.network.nmt.send_command(0x81)
            
            print(f"{Fore.CYAN}Waiting for node {NODE_ID} bootup...{Style.RESET_ALL}")
            self.node = self.network.add_node(NODE_ID, EDS_FILE)
            self.node.nmt.wait_for_bootup(5)
            
            # Setup PDO
            self._configure_pdos()
            self._setup_pdo_callbacks()
            
            # Switch to OPERATIONAL
            print(f"{Fore.CYAN}Switching to OPERATIONAL state...{Style.RESET_ALL}")
            self.node.nmt.state = 'OPERATIONAL'
            
            self.is_connected = True
            print(f"{Fore.GREEN}Node {NODE_ID} connected successfully{Style.RESET_ALL}\n")
            
        except Exception as e:
            print(f"{Fore.RED}Connection failed: {e}{Style.RESET_ALL}")
            if self.network:
                self.network.disconnect()
                self.network = None
    
    def do_disconnect(self, arg):
        """Disconnect from CAN bus
        Usage: disconnect"""
        if not self.is_connected:
            print(f"{Fore.YELLOW}Not connected!{Style.RESET_ALL}")
            return
        
        try:
            if self.is_enabled:
                print(f"{Fore.CYAN}Disabling drive first...{Style.RESET_ALL}")
                self.do_disable("")
            
            print(f"{Fore.CYAN}Disconnecting...{Style.RESET_ALL}")
            self.network.nmt.send_command(0x80)  # PRE-OPERATIONAL
            time.sleep(0.2)
            self.network.disconnect()
            
            self.is_connected = False
            self.network = None
            self.node = None
            print(f"{Fore.GREEN}Disconnected safely{Style.RESET_ALL}")
            
        except Exception as e:
            print(f"{Fore.RED}Disconnect error: {e}{Style.RESET_ALL}")
    
    def _configure_pdos(self):
        """Internal PDO configuration"""
        # Disable PDO for configuration
        for i in range(1, 4):
            cob_id = self.node.sdo[0x1400 + i - 1][1].raw
            self.node.sdo[0x1400 + i - 1][1].raw = cob_id | 0x80000000
        
        for i in range(1, 3):
            cob_id = self.node.sdo[0x1800 + i - 1][1].raw
            self.node.sdo[0x1800 + i - 1][1].raw = cob_id | 0x80000000
        
        # Set transmission type to event-driven
        self.node.sdo[0x1800][2].raw = 254
        self.node.sdo[0x1801][2].raw = 254
        
        # Re-enable
        for i in range(1, 4):
            cob_id = self.node.sdo[0x1400 + i - 1][1].raw
            self.node.sdo[0x1400 + i - 1][1].raw = cob_id & 0x7FFFFFFF
        
        for i in range(1, 3):
            cob_id = self.node.sdo[0x1800 + i - 1][1].raw
            self.node.sdo[0x1800 + i - 1][1].raw = cob_id & 0x7FFFFFFF
        
        self.node.pdo.read()
    
    def _setup_pdo_callbacks(self):
        """Setup callbacks for TPDO monitoring (silent mode)"""
        # These callbacks don't print to avoid cluttering CLI
        # But can be accessed for monitoring
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
        """Enable drive (State Machine: Shutdown -> Switch On -> Enable)
        Usage: enable"""
        if not self._check_connected():
            return
        
        if self.is_enabled:
            print(f"{Fore.YELLOW}Drive already enabled!{Style.RESET_ALL}")
            return
        
        try:
            print(f"{Fore.CYAN}Enabling drive...{Style.RESET_ALL}")
            
            if self.use_pdo:
                # Use RPDO1
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
                # Use SDO
                self.node.sdo['Control word'].raw = 0x06
                time.sleep(0.3)
                self.node.sdo['Control word'].raw = 0x07
                time.sleep(0.3)
                self.node.sdo['Control word'].raw = 0x0F
                time.sleep(0.5)
            
            self.is_enabled = True
            sw = self.node.sdo['Status word'].raw
            print(f"{Fore.GREEN}Drive enabled! Statusword: 0x{sw:04X}{Style.RESET_ALL}")
            
        except Exception as e:
            print(f"{Fore.RED}Enable failed: {e}{Style.RESET_ALL}")
    
    def do_disable(self, arg):
        """Disable drive (return to Shutdown state)
        Usage: disable"""
        if not self._check_connected():
            return
        
        try:
            print(f"{Fore.CYAN}Disabling drive...{Style.RESET_ALL}")
            
            if self.use_pdo:
                self.node.rpdo[1]['Control word'].raw = 0x06
                self.node.rpdo[1].transmit()
            else:
                self.node.sdo['Control word'].raw = 0x06
            
            time.sleep(0.3)
            self.is_enabled = False
            print(f"{Fore.GREEN}Drive disabled{Style.RESET_ALL}")
            
        except Exception as e:
            print(f"{Fore.RED}Disable failed: {e}{Style.RESET_ALL}")
    
    # ==================== HOMING ====================
    
    def do_home(self, arg):
        """Perform homing operation (Method 35: Current Position)
        Usage: home"""
        if not self._check_enabled():
            return
        
        try:
            print(f"{Fore.CYAN}Starting homing operation...{Style.RESET_ALL}")
            
            if self.use_pdo:
                # Set mode to Homing (6)
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
                # Use SDO
                self.node.sdo['Modes of operation'].raw = 6
                time.sleep(0.1)
                self.node.sdo['Control word'].raw = 0x1F
                time.sleep(0.5)
            
            # Verify
            sw = self.node.sdo['Status word'].raw
            homing_ok = (sw >> 12) & 1
            target_ok = (sw >> 10) & 1
            
            if homing_ok and target_ok:
                print(f"{Fore.GREEN}Homing completed successfully! Position zeroed.{Style.RESET_ALL}")
            else:
                print(f"{Fore.YELLOW}Homing may have failed. Statusword: 0x{sw:04X}{Style.RESET_ALL}")
            
            # Return to Profile Position Mode
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
            print(f"{Fore.RED}Homing failed: {e}{Style.RESET_ALL}")
    
    # ==================== MOTION CONTROL ====================
    
    def do_move(self, arg):
        """Move motor to absolute position (CiA 402 Compliant)
        Usage: move <target_position> [wait]
        
        Parameters:
            target_position : Target position in encoder counts
            wait            : 'wait' to block until motion complete (optional)
        
        Example: move 50000
                 move 50000 wait"""
        if not self._check_enabled():
            return
        
        args = arg.split()
        if len(args) < 1:
            print(f"{Fore.RED}Error: Position required{Style.RESET_ALL}")
            print("Usage: move <position> [wait]")
            return
        
        try:
            target = int(args[0])
            wait = len(args) > 1 and args[1].lower() == 'wait'
        except ValueError:
            print(f"{Fore.RED}Error: Invalid position value{Style.RESET_ALL}")
            return
        
        try:
            print(f"{Fore.CYAN}Moving to position {target}...{Style.RESET_ALL}")
            
            if self.use_pdo:
                # CiA 402 Compliant: 3-step process
                # Step 1: Write target (motor not moving yet)
                self.node.rpdo[3]['Profile target position'].raw = target
                self.node.rpdo[3]['Control word'].raw = 0x0F
                self.node.rpdo[3].transmit()
                time.sleep(0.05)
                
                # Step 2: Clear bit 4
                self.node.rpdo[3]['Control word'].raw = 0x0F
                self.node.rpdo[3].transmit()
                time.sleep(0.05)
                
                # Step 3: Set bit 4 (RISING EDGE -> TRIGGER!)
                self.node.rpdo[3]['Control word'].raw = 0x1F
                self.node.rpdo[3].transmit()
            else:
                # Use SDO
                self.node.sdo['Profile target position'].raw = target
                time.sleep(0.05)
                self.node.sdo['Control word'].raw = 0x0F
                time.sleep(0.05)
                self.node.sdo['Control word'].raw = 0x1F
            
            print(f"{Fore.GREEN}Motion command sent!{Style.RESET_ALL}")
            
            if wait:
                self._wait_motion_complete()
            
        except Exception as e:
            print(f"{Fore.RED}Move failed: {e}{Style.RESET_ALL}")
    
    def do_wait(self, arg):
        """Wait until motion completes (polling Target Reached bit)
        Usage: wait [timeout]
        
        Parameters:
            timeout : Maximum time to wait in seconds (default: 10)
        
        Example: wait
                 wait 20"""
        if not self._check_enabled():
            return
        
        try:
            timeout = float(arg) if arg else 10.0
        except ValueError:
            print(f"{Fore.RED}Invalid timeout value! Using default: 10 seconds{Style.RESET_ALL}")
            timeout = 10.0
        
        self._wait_motion_complete(timeout)
    
    def _wait_motion_complete(self, timeout=10.0):
        """Internal method to wait for motion completion"""
        print(f"{Fore.CYAN}Waiting for motion to complete...{Style.RESET_ALL}")
        start_time = time.time()
        
        try:
            while True:
                sw = self.node.sdo['Status word'].raw
                if sw & SW_TARGET_REACHED:
                    pos = self.node.sdo['Actual motor position'].raw
                    elapsed = time.time() - start_time
                    print(f"{Fore.GREEN}Target reached ({elapsed:.2f}s){Style.RESET_ALL}")
                    print(f"{Fore.GREEN}Actual position: {pos}{Style.RESET_ALL}")
                    return True
                
                if time.time() - start_time > timeout:
                    print(f"{Fore.YELLOW}Motion timeout after {timeout}s{Style.RESET_ALL}")
                    return False
                
                time.sleep(0.1)
                
        except Exception as e:
            print(f"{Fore.RED}Wait error: {e}{Style.RESET_ALL}")
            return False
    
    # ==================== STATUS & MONITORING ====================
    
    def do_status(self, arg):
        """Display motor status (Statusword, Position, Mode)
        Usage: status"""
        if not self._check_connected():
            return
        
        try:
            sw = self.node.sdo['Status word'].raw
            pos = self.node.sdo['Actual motor position'].raw
            mode = self.node.sdo['Modes of operation'].raw
            
            print(f"\n{Fore.CYAN}=== Motor Status ==={Style.RESET_ALL}")
            print(f"Statusword      : 0x{sw:04X}")
            print(f"  - Ready       : {'Yes' if sw & 0x01 else 'No'}")
            print(f"  - Switched On : {'Yes' if sw & 0x02 else 'No'}")
            print(f"  - Enabled     : {'Yes' if sw & 0x04 else 'No'}")
            print(f"  - Target OK   : {'Yes' if sw & SW_TARGET_REACHED else 'No'}")
            print(f"Position        : {pos}")
            print(f"Mode            : {mode} ({self._mode_name(mode)})")
            print(f"PDO Mode        : {'Enabled' if self.use_pdo else 'Disabled (SDO)'}")
            print()
            
        except Exception as e:
            print(f"{Fore.RED}Failed to read status: {e}{Style.RESET_ALL}")
    
    def do_getpos(self, arg):
        """Read current motor position
        Usage: getpos"""
        if not self._check_connected():
            return
        
        try:
            pos = self.node.sdo['Actual motor position'].raw
            print(f"{Fore.GREEN}Current position: {pos}{Style.RESET_ALL}")
        except Exception as e:
            print(f"{Fore.RED}Failed to read position: {e}{Style.RESET_ALL}")
    
    # ==================== PARAMETER SETTING (SDO) ====================
    
    def do_setvel(self, arg):
        """Set Profile Velocity (0x6081)
        Usage: setvel <value>
        Example: setvel 100000"""
        if not self._check_connected():
            return
        
        try:
            vel = int(arg)
            self.node.sdo['Profile target velocity'].raw = vel
            print(f"{Fore.GREEN}Velocity set to {vel}{Style.RESET_ALL}")
        except ValueError:
            print(f"{Fore.RED}Invalid format! Use: setvel <number>{Style.RESET_ALL}")
        except Exception as e:
            print(f"{Fore.RED}Failed to set velocity: {e}{Style.RESET_ALL}")
    
    def do_setaccel(self, arg):
        """Set Profile Acceleration (0x6083)
        Usage: setaccel <value>
        Example: setaccel 5000"""
        if not self._check_connected():
            return
        
        try:
            accel = int(arg)
            self.node.sdo['Profile target acceleration'].raw = accel
            print(f"{Fore.GREEN}Acceleration set to {accel}{Style.RESET_ALL}")
        except ValueError:
            print(f"{Fore.RED}Invalid format! Use: setaccel <number>{Style.RESET_ALL}")
        except Exception as e:
            print(f"{Fore.RED}Failed to set acceleration: {e}{Style.RESET_ALL}")
    
    def do_setdecel(self, arg):
        """Set Profile Deceleration (0x6084)
        Usage: setdecel <value>
        Example: setdecel 5000"""
        if not self._check_connected():
            return
        
        try:
            decel = int(arg)
            self.node.sdo['Profile target deceleration'].raw = decel
            print(f"{Fore.GREEN}Deceleration set to {decel}{Style.RESET_ALL}")
        except ValueError:
            print(f"{Fore.RED}Invalid format! Use: setdecel <number>{Style.RESET_ALL}")
        except Exception as e:
            print(f"{Fore.RED}Failed to set deceleration: {e}{Style.RESET_ALL}")
    
    def do_getparams(self, arg):
        """Display current motion profile parameters
        Usage: getparams"""
        if not self._check_connected():
            return
        
        try:
            vel = self.node.sdo['Profile target velocity'].raw
            accel = self.node.sdo['Profile target acceleration'].raw
            decel = self.node.sdo['Profile target deceleration'].raw
            
            print(f"\n{Fore.CYAN}=== Motion Profile Parameters ==={Style.RESET_ALL}")
            print(f"Velocity     : {vel}")
            print(f"Acceleration : {accel}")
            print(f"Deceleration : {decel}")
            print()
            
        except Exception as e:
            print(f"{Fore.RED}Failed to read parameters: {e}{Style.RESET_ALL}")
    
    # ==================== MODE SWITCHING ====================
    
    def do_usepdo(self, arg):
        """Switch to PDO mode for motion commands
        Usage: usepdo"""
        self.use_pdo = True
        print(f"{Fore.GREEN}PDO mode enabled. Commands will use RPDO.{Style.RESET_ALL}")
    
    def do_usesdo(self, arg):
        """Switch to SDO mode for motion commands
        Usage: usesdo"""
        self.use_pdo = False
        print(f"{Fore.GREEN}SDO mode enabled. Commands will use SDO.{Style.RESET_ALL}")
    
    # ==================== HELPER FUNCTIONS ====================
    
    def _check_connected(self):
        """Helper to check connection"""
        if not self.is_connected:
            print(f"{Fore.RED}Not connected! Use 'connect' first.{Style.RESET_ALL}")
            return False
        return True
    
    def _check_enabled(self):
        """Helper to check if drive is enabled"""
        if not self._check_connected():
            return False
        if not self.is_enabled:
            print(f"{Fore.RED}Drive not enabled! Use 'enable' first.{Style.RESET_ALL}")
            return False
        return True
    
    def _mode_name(self, mode):
        """Convert mode number to name"""
        modes = {1: "Profile Position", 6: "Homing", 3: "Profile Velocity"}
        return modes.get(mode, "Unknown")
    
    # ==================== CMD OVERRIDES ====================
    
    def do_exit(self, arg):
        """Exit from CLI
        Usage: exit"""
        if self.is_connected:
            print(f"{Fore.YELLOW}Connection still active. Disconnect first? (y/n){Style.RESET_ALL}")
            choice = input().lower()
            if choice == 'y':
                self.do_disconnect("")
        
        print(f"{Fore.CYAN}Thank you! Goodbye.{Style.RESET_ALL}")
        return True
    
    def do_quit(self, arg):
        """Alias for exit"""
        return self.do_exit(arg)
    
    def emptyline(self):
        """Override to prevent repeating last command"""
        pass
    
    def default(self, line):
        """Handle unknown command"""
        print(f"{Fore.RED}Unknown command: '{line}'{Style.RESET_ALL}")
        print(f"    Type 'help' for list of available commands.")

# ==================== MAIN ENTRY POINT ====================

def main():
    try:
        cli = CANopenCLI()
        cli.cmdloop()
    except KeyboardInterrupt:
        print(f"\n{Fore.YELLOW}Interrupted by user{Style.RESET_ALL}")
        if cli.is_connected:
            cli.do_disconnect("")
    except Exception as e:
        print(f"\n{Fore.RED}Fatal error: {e}{Style.RESET_ALL}")
        import traceback
        traceback.print_exc()

if __name__ == '__main__':
    main()