import struct
from threading import Thread
import time
import serial
import serial.tools.list_ports
from colored_terminal import *
import os
import subprocess
import platform

DELAY_CONSOLE_LOOP = 0.001


class INTERFACE:
    """for a terminal based controll over the server"""

    def __init__(self):
        self.isRunning = True
        self.Port = None
        self.Serial = None
        self.counter = 0
        self.start()

    def start(self):
        print_yellow("\n<<Compartion>>\n/? for help\n")
        self.autoconnect()

        # Start input thread
        Thread(target=self.command_loop, daemon=True).start()

        while self.isRunning:
            time.sleep(DELAY_CONSOLE_LOOP)  # delay to conserve performance
            try:
                if self.Serial is not None:
                    self.receive_data()

                # Auto reconnection
                if self.Serial is None:
                    
                    # reconnection animation
                    if not self.counter % 20:  # reduce prints to every 2 seconds
                        print("\033\033[1A", end="") #moves cursor up a line
                        print("\033[2K\033[G", end="")  # Clears the current line
                        print_cyan(
                            "waiting for a connection["
                            + int(self.counter / 20) * "."
                            + "]",
                            indent=1,
                        )
                        print("\n", end="", flush=True) #moves cursor down a line

                    if self.counter > 80:
                        self.counter = 0
                    else:
                        self.counter += 1

                    # reconnect
                    self.autoconnect(reconnection=True)
                    time.sleep(0.1)

            except serial.SerialException as e:
                if self.Serial is None:
                    # print_red(f'start() error:"{e}"\n')
                    self.Port = None
                else:
                    self.Serial = None
                    self.Port = None
                    print_red(f"Disconnected\n")

            except KeyboardInterrupt:
                print_yellow(
                    "blocked keyboard interupt. Use /q to savely shutdown server\n",
                    indent=1,
                )

            except Exception as e:
                print(e)
                break

    def receive_data(self):
        if self.Serial != None:
            try:
                if self.Serial.in_waiting > 0:
                    received = self.Serial.readline().decode("utf-8")
                    print_magenta(f"{received}")

            except serial.SerialException as e:
                if self.Serial is None:
                    print_red(f'recive data error:"{e}"\n')
                    self.Port = None
                else:
                    self.Serial = None
                    self.Port = None
                    print_red(f"Disconnected\n")

            except Exception as e:
                print_red(f"Error Serial Recive:{e}\n", indent=1)

    def command_loop(self):
        """This handles user input. It's in an extra Thread to not block incomming data"""
        while self.isRunning:
            try:
                cmd = input("~")
                # print("\033[2K", end="\r") #resets cursor
                self.input_command(cmd)
            except EOFError:
                break
            except Exception as e:
                print_red(f"Command error: {e}\n", indent=1)

    def autoconnect(self, reconnection=False):
        ports = serial.tools.list_ports.comports()
        # test for stm32
        for port in ports:
            if "STM" in str(port.manufacturer) or "Arduino" in str(port.manufacturer):
                self.Port = port
                self.Serial = serial.Serial(self.Port.device, baudrate=9600, timeout=1)
                break

        if self.Port is None:  # costmetics for leading animation
            if not reconnection:
                print_red("Autoconnect failed\n", indent=1)
        else:
            if reconnection:  # costmetics for leading animation
                print("\033[2K\033[G", end="", flush=True)  # Clears the current line

            print_green("Autoconnect")
            print_white(f":{self.Port.device}|{self.Port.manufacturer}|9600\n")

    def input_command(self, command: str):
        if len(command) > 1 and command[0] == "/":
            match command[1:].split()[
                0
            ]:  # filters for just the command regardless the length.

                case "?":
                    print_yellow("<Help>")
                    print_yellow(
                        """
/l List serial ports
/s [number] [baudrate=9600] select serial port
/t [string] to uplink a raw string
/c [command] [parameter] to uplink a command. 
   to see avaliable commands use /c [?]
/dfu | list dfu devices
/cmkae | runns Cmkae
/flash [Firmware.bin==./build/Debug/ESC.bin] flashes firmware
/q | quit terminal

"""
                    )

                case "l":
                    ports = serial.tools.list_ports.comports()
                    if not ports:
                        print_yellow("No serial ports found.\n")
                        return

                    print_yellow("COM ports:\n")
                    print("nr|name|manufacturer|description")
                    a = 0
                    for port in ports:
                        if port.device != "":
                            if port == self.Port:  # replaxe none with stared port
                                print_green(
                                    f">>{a}|{port.name}|{port.manufacturer}|{port.description}\n"
                                )
                            else:
                                print(
                                    f"{a}|{port.name}|{port.manufacturer}|{port.description}"
                                )  # - {port.description}- {port.product} -- {port.name}

                            a += 1

                case "select" | "s":
                    # select com port
                    comand_buf = command[3:].split()
                    ports = serial.tools.list_ports.comports()
                    number = int(comand_buf[0])

                    if number > len(ports):
                        print_red("values to high. try /l\n")
                        return

                    # select baundrate
                    baudrate = 9600
                    if len(comand_buf) > 1:
                        baudrate = int(comand_buf[1])

                    # select com port
                    if len(ports) >= number:
                        self.Port = ports[number]
                        self.Serial = serial.Serial(
                            self.Port.device, baudrate, timeout=1
                        )

                    print_yellow(
                        f"Selected:{self.Port.device}-{self.Port.manufacturer}\n"
                    )

                    pass

                # sends string
                case "t":
                    if self.Serial is None:
                        print_red("No device connected\n")
                    else:
                        try:
                            # print_cyan(f"{command[2:]}\n")
                            self.Serial.write((command[2:] + "\n").encode())
                        except Exception as e:
                            print_red(f"Error Command Console:{e}", indent=1)
                    pass

                # sends a command
                case "c":
                    # if self.Serial.port_active is None:
                    #     print_red("No device connected\n")
                    # else:
                    #     try:
                    #         command_buf = (command[2:] + " 0 " * 6).split()[
                    #             :2
                    #         ]  # filling missed spaces with 0
                    #         print_yellow(f"Sending: {command_buf[0:2]} | ")

                    #         comm = command_buf[0]
                    #         param1 = float(command_buf[1])

                    #         uploadbuffer = comm.encode("utf-8") * 4 + struct.pack(
                    #             "fff", param1, param1, param1
                    #         )  # com x4 because of struct patting

                    #         # print_yellow(f'sending command: {command_buf}|{param1}|\nin bytes: {uploadbuffer}\n',indent=1)
                    #         self.Serial.BUFFER_UPLINK.append(uploadbuffer)
                    #     except Exception as e:
                    #         print_red(f"Error Command Console:{e}", indent=1)
                    pass

                case "dfu":
                    print_yellow("List dfu devices\n")

                    # # Get the absolute path to dfu-util
                    base_path = os.path.dirname(os.path.abspath(__file__))

                    if platform.system() == "Windows":
                        path = os.path.join(base_path, "dfu-util-static.exe")
                    else:
                        path = os.path.join(base_path, "dfu-util")

                    # print(base_path)
                    # # Run it
                    result = subprocess.run(
                        [path, "-l"], capture_output=True, text=True, check=True
                    )

                    output = result.stdout + result.stderr

                    # # Show the output
                    print(output)

                case "cmake" | "cm":
                    print_yellow("Build project\n")
                    # cmake --build ./build/Debug --config Debug
                    base_path = os.path.dirname(os.path.abspath(__file__))
                    base_path = os.path.dirname(base_path)
                    # print(f"Base path: {base_path}") 

                    build_path = os.path.join(base_path, "build", "Debug")

                    try:
                        # Run the cmake build command
                        result = subprocess.run(
                            ["cmake", "--build", build_path, "--config", "Debug"],
                            capture_output=True,
                            text=True,
                            check=True,
                        )
                        # # Show the output
                        print(result.stdout + result.stderr)
                    except subprocess.CalledProcessError as e:
                        print(f"Error occurred during build: {e}")
                        print(f"Output: {e.output}")
                        print(f"Return code: {e.returncode}")

                    # # Get the absolute path to dfu-util
                    base_path = os.path.dirname(os.path.abspath(__file__))

                case "flash":
                    DFU_COMMAND = "dfu\n"
                    BIN_LOCATION = "build", "Debug" ,"ESC.bin"
                    print_yellow("<Flash dfu device>\n")
                    
                    if self.Serial is not None:
                        print_yellow(f'Sending dfu command {repr(DFU_COMMAND)}\n')
                        self.Serial.write((DFU_COMMAND).encode())
                    time.sleep(0.5)
                    
                    # COMMAND ../../dfu-util/dfu-util -a 0 -i 0 -s 0x08000000:leave -D ${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}.bin

                    # # Get the absolute path to dfu-util
                    base_path = os.path.dirname(os.path.abspath(__file__))

                    if platform.system() == "Windows":
                        path = os.path.join(base_path, "dfu-util-static.exe")
                        path_bin = os.path.join(
                            os.path.dirname(base_path), *BIN_LOCATION
                        )
                    else:
                        path = os.path.join(base_path, "dfu-util")

                    # print(f'command: {path + " -a 0 -i 0 -s 0x08000000:leave" + " -D " + path_bin}') 
                    
                    # # Run it
                    result = subprocess.run(
                        [path, "-a","0","-i","0","-s","0x08000000:leave", "-D",path_bin], capture_output=True, text=True, check=True
                    )

                    output = result.stdout + result.stderr

                    # # Show the output
                    print(output)

                case "quit" | "q":
                    print_yellow("<shuting down Interface>\n")
                    # self.Serial.IsRunning = 0
                    self.isRunning = 0
                    print_yellow("<all down>\n", indent=1)

                case _:
                    print_red(f'Unknown Command "{command[1:]}"\n')
        else:
            print_yellow("use /? for help\n")


if __name__ == "__main__":
    interface = INTERFACE()
