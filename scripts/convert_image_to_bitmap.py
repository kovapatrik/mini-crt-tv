import sys
from typing import Literal

import cv2
import numpy as np


def convert_to_rgb332(image):
    r = (image[:, :, 2] >> 5).astype(np.uint8)
    g = (image[:, :, 1] >> 5).astype(np.uint8)
    b = (image[:, :, 0] >> 6).astype(np.uint8)
    return (r << 5) | (g << 2) | b


def convert_to_rgb565(image):
    r = (image[:, :, 2] >> 3).astype(np.uint16)
    g = (image[:, :, 1] >> 2).astype(np.uint16)
    b = (image[:, :, 0] >> 3).astype(np.uint16)
    return (r << 11) | (g << 5) | b


def convert(image_path, output_name, rgb: Literal["332", "565"]):
    # Load the image using OpenCV
    image = cv2.imread(image_path)

    if rgb == "332":
        convert_function = convert_to_rgb332
    elif rgb == "565":
        convert_function = convert_to_rgb565

    output_image = convert_function(image)

    # Save the resized image for reference
    cv2.imwrite(f"{output_name}.png", output_image)

    # Flatten the image to create a single C array
    return output_image.flatten()


if __name__ == "__main__":
    file = sys.argv[1]
    rgb = "565" if sys.argv[2] == "16" else "332"
    output_name = file.split(".")[0]
    c_array = convert(file, output_name, rgb)
    with open(f"{output_name}.h", "w") as f:
        if rgb == "332":
            f.write(f"const unsigned char {output_name}_bitmap[] = {{\n")
        elif rgb == "565":
            f.write(f"const unsigned uint16_t {output_name}_bitmap[] = {{\n")

        f.write("    ")
        for i, value in enumerate(c_array):
            if rgb == "332":
                f.write(f"0x{value:02x}, ")
            elif rgb == "565":
                f.write(f"0x{value:04x}, ")
            if (i + 1) % 16 == 0:
                f.write("\n    ")
        f.write("\n};\n")
