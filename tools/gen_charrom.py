from PIL import Image

im = Image.open("../res/charmap.png")

char_width = 5
char_height = 6
char_byte = 4

chars_x = 16
chars_y = 16

char_idx = 0


def padded_bin_str(num, padding):
    """
    Returns a correct length 0-padded binary number as string
    """
    b = bin(num)[2:]
    zs = padding - len(b)
    if zs > 0:
        b = ('0' * zs) + b
    return b

rom = []

for cy in range(0, chars_y):
    for cx in range(0, chars_x):
        char_idx = (cy * chars_y) + cx
        char_bytes = [0] * char_byte
        for y in range(0, char_height):
            for x in range(0, char_width):
                px_idx = (y * char_width) + x
                px_byte = int(px_idx / 8)
                px_bit = px_idx % 8
                px = (im.getpixel(((cx * char_width) + x, (cy * char_height) + y)) == (255, 255, 255)) * 1
                char_bytes[px_byte] |= px << px_bit
        print(char_idx, padded_bin_str(char_bytes[0], 8)[::-1] + padded_bin_str(char_bytes[1], 8)[::-1] + padded_bin_str(char_bytes[2], 8)[::-1] + padded_bin_str(char_bytes[3], 8)[::-1])
        rom.extend(char_bytes)

with open("charrom.bin", 'wb+') as target:
    for e in rom:
        target.write(b"%c" % (e))