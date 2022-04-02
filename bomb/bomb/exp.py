from pwn import *

p = process("./bomb")

def leak(address):
    data = p.read(address, 4)
    #log.debug("%#x => %s" % (address, (data or '').encode('hex')))
    return data

def phase_1():
    # phase_1_cmp_addr = 0x402400
    phase_1_key = "Border relations with Canada have never been better."
    p.sendline(phase_1_key)

#def phase_2():

input("#")

phase_1()
#phase_2()
p.interactive()
