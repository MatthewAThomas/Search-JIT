import random
import argparse

def cmdline():
    parser = argparse.ArgumentParser()
    parser.add_argument("n", help="number of elements to insert into tree", type=int)
    args = parser.parse_args()
    return args.n

def generate(n):
    min_int32 = -2147483648
    max_int32 = 2147483647
    data = [random.randint(min_int32, max_int32) for _ in range(n)]

    f = open("data.c", 'w')

    txt = "#include <stdint.h>\n#include \"data.h\"\n\n"
    txt += f"int NUM_INSERTIONS = {n};\n\n"
    txt += "int32_t vals[] = {\n"    

    line_width = 10
    for i in range((n + line_width - 1) // line_width):
        txt += "    "
        for j in range(min(line_width, n - i * line_width)):
            txt += f"{data[i + j]}, "
        txt += "\n"
    txt = txt[:-3] #get rid of the extra ", " at the end of the array
    
    txt += "\n};"
    f.write(txt)
    f.close()

if __name__ == "__main__":
    n = cmdline()
    assert(n > 0)
    generate(n)