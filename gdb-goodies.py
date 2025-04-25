import gdb

class SkipOverInstruction(gdb.Command):
    def __init__(self):
        super(SkipOverInstruction, self).__init__("soi", gdb.COMMAND_USER)

    def invoke(self, _arg, _from_tty):
        # https://stackoverflow.com/a/16411080
        addr = int(gdb.parse_and_eval("$pc"))
        arch = gdb.selected_frame().architecture()
        length = int(arch.disassemble(addr)[0]['length'])

        next_addr = addr + length
        gdb.execute(f"set $pc = {next_addr}")
        print(f"pc is now at {hex(next_addr)}")

        gdb.execute(f"disassemble")

SkipOverInstruction()
