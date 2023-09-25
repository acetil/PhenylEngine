import re
import sys

def make_define (in_file: str) -> str:
    filename = re.search(r"[^/]*$", in_file)[0]

    return "EMBED_" + re.sub(r"[^a-zA-z0-9_]", "_", filename).upper()

def gen(in_file: str, out_file: str):
    print(f"Generating {out_file} from {in_file}")
    with open(in_file, "rb") as f:
        with open(out_file, "w") as f1:
            f1.write("#pragma once\n")
            f1.write(f"#define {make_define(in_file)} ")
            f1.write("{")
            while b := f.read(1):
                f1.write("0x")
                f1.write(b.hex())
                f1.write(",")
            f1.write("0x0")
            f1.write("}")


if __name__ == "__main__":
    if len(sys.argv) != 3:
        sys.stderr.write(f"Usage: {sys.argv[0]} <input_file> <output_file>\n")
        sys.stderr.write(f"{sys.argv}\n")
        exit(1)

    gen(sys.argv[1], sys.argv[2])
