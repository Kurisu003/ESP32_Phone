from PIL import Image
import sys

def image_to_rgb_array(image_name):
    img = Image.open(image_name).convert("RGB")
    width, height = img.size
    pixels = img.load()

    rgb_array = [[[0, 0, 0] for _ in range(width)] for _ in range(height)]

    for y in range(height):
        for x in range(width):
            r, g, b = pixels[x, y]
            rgb_array[y][x] = [r, g, b]

    return rgb_array


if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: python image_to_rgb.py <image_filename>")
        sys.exit(1)

    image_name = sys.argv[1]
    rgb_array = image_to_rgb_array(image_name)

    print(f"Loaded image with shape: {len(rgb_array)}x{len(rgb_array[0])}x3")
    print(rgb_array)
