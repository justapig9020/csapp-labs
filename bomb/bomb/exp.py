from pwn import *

p = process("./bomb")

sol = open("./psol.txt", "w+")

def recv_msg():
    msg = p.recv().decode("ascii")
    print(msg)

def str_to_bytes(s):
    return bytes(s, "ascii")

def phase_1():
    # phase_1_cmp_addr = 0x402400
    payload = "Border relations with Canada have never been better."
    p.sendline(str_to_bytes(payload))
    sol.write(payload)
    sol.write('\n')

def phase_2():
    args = []
    val = 1
    for i in range(6):
        args.append(val)
        val *= 2
    payload = ' '.join(str(n) for n in args)
    p.sendline(str_to_bytes(payload))
    sol.write(payload)
    sol.write('\n')

def phase_3():
    """
    0x402470:   0x0000000000400f7c  0x0000000000400fb9
    0x402480:   0x0000000000400f83  0x0000000000400f8a
    0x402490:   0x0000000000400f91  0x0000000000400f98
    0x4024a0:   0x0000000000400f9f  0x0000000000400fa6
    """
    a = 1
    b = 0x137
    payload = str(a) + ' ' + str(b)
    p.sendline(str_to_bytes(payload))
    sol.write(payload)
    sol.write('\n')

def phase_4():
    a = 7
    b = 0
    payload = str(a) + ' ' + str(b)
    p.sendline(str_to_bytes(payload))
    sol.write(payload)
    sol.write('\n')
    
def phase_5():
    """
    6d616475696572736e666f747662796c
    """
    data = "maduiersnfotvbyl"
    key = "flyers"
    args = []
    for i in key:
        n = data.find(i)
        args.append(n)
    payload = ''.join(chr(n + ord('@')) for n in args)
    print(payload)
    p.sendline(str_to_bytes(payload))
    sol.write(payload)
    sol.write('\n')


def phase_6():
    """
0x6032d0 <node1>:   0x000000010000014c  0x00000000006032e0
0x6032e0 <node2>:   0x00000002000000a8  0x00000000006032f0
0x6032f0 <node3>:   0x000000030000039c  0x0000000000603300
0x603300 <node4>:   0x00000004000002b3  0x0000000000603310
0x603310 <node5>:   0x00000005000001dd  0x0000000000603320
    """
    values = [0x0000014c, 0x000000a8, 0x0000039c, 0x000002b3, 0x000001dd, 0x000001bb]
    tuples = []
    for i in range(len(values)):
        tuples.append((i + 1, values[i]))
    tuples.sort(key = lambda tup: tup[1])

    args = []
    for tup in tuples:
        args.append(7 - tup[0])
    args.reverse()
    
    payload = ' '.join(str(n) for n in args)
    p.sendline(str_to_bytes(payload))
    sol.write(payload)
    sol.write('\n')

recv_msg()
phase_1()

recv_msg()
phase_2()

recv_msg()
phase_3()

recv_msg()
phase_4()

recv_msg()
phase_5()

recv_msg()
phase_6()
recv_msg()

sol.close()
