from PIL import Image
import sys

def rgb_to_565(r, g, b):
    return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3)

def encode_rle_c_array(image_name, out_name="image_rle.c"):
    img = Image.open(image_name).convert("RGB")
    width, height = img.size
    pixels = list(img.getdata())

    rle = []
    prev_color = None
    run_length = 0

    for r, g, b in pixels:
        color = rgb_to_565(r, g, b)
        if color == prev_color:
            run_length += 1
        else:
            if prev_color is not None:
                rle.append((run_length, prev_color))
            prev_color = color
            run_length = 1

    rle.append((run_length, prev_color))

    # Write C array
    with open(out_name, "w") as f:
        f.write("typedef struct { uint16_t count; uint16_t color; } RLE_Pixel;\n\n")
        f.write(f"const RLE_Pixel image_data[] = {{\n")
        for count, color in rle:
            f.write(f"    {{ {count}, 0x{color:04X} }},\n")
        f.write("};\n\n")
        f.write(f"const uint32_t image_data_len = sizeof(image_data) / sizeof(image_data[0]);\n")

    print(f"Wrote {len(rle)} runs to {out_name}")

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: python encode_rle_c.py <image>")
        sys.exit(1)

    encode_rle_c_array(sys.argv[1])
