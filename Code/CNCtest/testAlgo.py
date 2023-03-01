
import math

def compute(current, rat):
    return math.ceil(current * rat)

def main():
    ratio = max_steps / (req_steps + 1)
    count = 1
    msg = ""
    
    for i in range(1, max_steps + 1):
        num = compute(count, ratio)
        if(num == i and count <= req_steps):
            count += 1
            msg += '1'
        else:
            msg += '0'

    print(msg)
    print(msg[::-1])
    print(msg == msg[::-1])
    print(len(msg))
    print(len(msg.replace('0', '')))

if __name__ == "__main__":
    max_steps = int(input("Max: "))
    req_steps = int(input("Req: "))
    main()