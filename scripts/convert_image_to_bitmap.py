import sys

import cv2
import numpy as np

SCREEN_RESOLUTION = (256, 240)


def convert_to_rgb332(image):
    """
    Convert an image to RGB332 format with proper value scaling.
    RGB332 uses 3 bits for red, 3 bits for green, and 2 bits for blue.
    """
    # First separate the channels
    b, g, r = cv2.split(image)

    # Scale each channel to their respective bit depths
    r_scaled = np.right_shift(r, 5)  # 3 bits for red (0-7)
    g_scaled = np.right_shift(g, 5)  # 3 bits for green (0-7)
    b_scaled = np.right_shift(b, 6)  # 2 bits for blue (0-3)

    # Scale back up to 8-bit range for preview
    r_preview = np.left_shift(r_scaled, 5)
    g_preview = np.left_shift(g_scaled, 5)
    b_preview = np.left_shift(b_scaled, 6)

    # Combine channels into single 8-bit value for LCD
    rgb332 = (r_scaled << 5) | (g_scaled << 2) | b_scaled

    # Create preview image
    preview = cv2.merge([b_preview, g_preview, r_preview])

    return rgb332, preview


def apply_ntsc_correction(image):
    """
    Apply color correction for NTSC display.
    Adjusts for typical NTSC color space and signal characteristics.
    """
    # Convert to YUV color space (similar to what NTSC uses)
    yuv = cv2.cvtColor(image, cv2.COLOR_BGR2YUV)

    # Adjust color saturation and brightness
    yuv[:, :, 1] = cv2.multiply(yuv[:, :, 1], 1.1)  # U channel (blue-yellow)
    yuv[:, :, 2] = cv2.multiply(yuv[:, :, 2], 1.2)  # V channel (red-cyan)

    # Convert back to BGR
    corrected = cv2.cvtColor(yuv, cv2.COLOR_YUV2BGR)

    return corrected


def convert(image_path, output_name):
    # Load the image using OpenCV
    image = cv2.imread(image_path)

    if image.shape[0] > SCREEN_RESOLUTION[1] or image.shape[1] > SCREEN_RESOLUTION[0]:
        raise ValueError("The image is too big for the screen resolution")

    image = apply_ntsc_correction(image)
    output_image, preview = convert_to_rgb332(image)

    # Save the resized image for reference
    cv2.imwrite(f"{output_name}_converted.png", preview)

    # Flatten the image to create a single C array
    return output_image.flatten()


if __name__ == "__main__":
    file = sys.argv[1]
    output_name = file.split(".")[0]
    c_array = convert(file, output_name)
    with open(f"{output_name}.h", "w") as f:
        f.write(f"const unsigned char {output_name}_bitmap[] = {{\n")
        f.write("    ")
        for i, value in enumerate(c_array):
            f.write(f"0x{value:02x}, ")
            if (i + 1) % 16 == 0:
                f.write("\n    ")
        f.write("\n};\n")
