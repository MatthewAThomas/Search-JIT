import random
import argparse

def generate(n):
    f = open("data.c", 'w')

    txt = "#include <stdint.h>\n#include \"data.h\"\n\n"
    txt += "int32_t vals[] = {\n"

    txt += "    "
    txt += "1, 2, 3, 4\n"
    
    txt += "};"
    f.write(txt)
    f.close()

if __name__ == "__main__":
    generate(1)