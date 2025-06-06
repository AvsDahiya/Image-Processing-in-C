/* This coursework specification, and the example code provided during the
 * course, is Copyright 2024 Heriot-Watt University.
 * Distributing this coursework specification or your solution to it outside
 * the university is academic misconduct and a violation of copyright law. */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdint.h>

/* The RGB values of a pixel. */
struct Pixel {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
};

/* An image loaded from a file. */
struct Image {
    /* TODO: Question 1 */
    int width;
    int height;
    struct Pixel *pixels; // pointer to dynamically-allocated array of pixels
    char *filename_out; //Output filename
};

/* Free a struct Image */
void free_image(struct Image *img) {
    /* TODO: Question 2a */
    if (img != NULL) {
        // free the array of pixels
        free(img->pixels);
        // Free allocated filename
        free(img->filename_out);
        // free the struct image itself
        free(img);
    }
}

/* Opens and reads an image file, returning a pointer to a new struct Image.
 * On error, prints an error message and returns NULL. */
struct Image *load_image(const char *filename, const char *filename_out) {
    /* open the file for reading */
    FILE *f = fopen(filename, "rb");
    if (f == NULL) {
        fprintf(stderr, "File %s could not be opened.\n", filename);
        return NULL;
    }

    /* Allocate the Image object, and read the image from the file */
    /* TODO: Question 2b */

    /* Read the header to get width and height */
    char magic[4];
    int width, height;
    if(fscanf(f, "%3s %d %d ", magic, &width, &height) != 3 || strcmp(magic, "HS8") !=0) {
        fprintf(stderr, "Invalid header in file %s.\n", filename);
        fclose(f);
        return NULL;
    }
    /* Allocate memory for the image object */
    struct Image *img = malloc(sizeof(struct Image));
    if (img == NULL) {
        fprintf(stderr, "Memory allocation failed.\n");
        fclose(f);
        return NULL;
    }
    /* Initialize width and height */
    img->width = width;
    img->height = height;
    /* Allocate memory for the pixel array */
    img->pixels = malloc(width * height * sizeof(struct Pixel));
    if (img->pixels == NULL) {
        fprintf(stderr, "Memory allocation failed.\n");
        fclose(f);
        free_image(img);
        return NULL;
    }
    /* Read pixel data */
    size_t pixels_read = fread(img->pixels, sizeof(struct Pixel), width * height, f);
    if (pixels_read != width * height) {
        fprintf(stderr, "Failed to read pixel data from file %s.\n", filename);
        fclose(f);
        free_image(img);
        return NULL;
    }
    /* Close the file */
    fclose(f);
    /* Copy the output filename */
    img->filename_out = strdup(filename_out);
    if (img->filename_out == NULL) {
        fprintf(stderr, "Memory allocation failed.\n");
        free_image(img);
        return NULL;
    }
    return img;
}

/* Write img to file filename. Return true on success, false on error. */
bool save_image(const struct Image *img, const char *filename) {
    /* TODO: Question 2c */

    /* Open the file for writting */
    FILE *f = fopen(filename, "wb");
    if (f == NULL) {
        fprintf(stderr, "File %s could not be opened fr writting.\n", filename);
        return false;
    }
    /* Write the header */
    fprintf(f, "HS8 %d %d\n", img->width, img->height);
    /* Write pixel data */
    size_t pixels_written = fwrite(img->pixels, sizeof(struct Pixel), img->width * img->height,f);
    if (pixels_written != img->width * img->height) {
        fprintf(stderr, "Failed to write pixel data to file %s.\n", filename);
        fclose(f);
        return false;
    }
    /* Close the file */
    fclose(f);
    return true;
}

/* Allocate a new struct Image and copy an existing struct Image's contents
 * into it. On error, returns NULL. */
struct Image *copy_image(const struct Image *source) {
    /* TODO: Question 2d */
    if(source == NULL) {
        return NULL; // return NULL if the source is NULL
    }
    /* Allocate memory for the new image object */
    struct Image *new_img = malloc(sizeof(struct Image));
    if (new_img == NULL) {
        fprintf(stderr, "Memory allocation failed.\n");
        return NULL;
    }
    /* Copy width and height */
    new_img->width = source->width;
    new_img->height = source->height;
    /* Allocate memory for the pixel array */
    new_img->pixels = malloc(source->width * source->height * sizeof(struct Pixel));
    if (new_img->pixels == NULL) {
        fprintf(stderr, "Memory allocation failed.\n");
        free(new_img); // Free allocated Image object
        return NULL;
    }
    /* Copy pixel data */
    memcpy(new_img->pixels, source->pixels, source->width * source->height * sizeof(struct Pixel));
    /* Copy the output filename */
    new_img->filename_out = strdup(source->filename_out);
    if(new_img->filename_out == NULL) {
        fprintf(stderr, "Memory allocation failed.\n");
        free_image(new_img);
        return NULL;
    }
    return new_img;
}

/* Perform your first task.
 * (TODO: Write a better comment here, and rename the function.
 * You may need to add or change arguments depending on the task.)
 * Returns a new struct Image containing the result, or NULL on error. */
/* TODO: Question 3 */
// Comparator function for qsort
int compare_uint8(const void *a, const void *b) {
    return (*(const uint8_t *)a - *(const uint8_t *)b);
}
// Helper function to get the pixel value safely
struct Pixel get_pixel(const struct Image *img, int x, int y) {
    // Clamp x and y to stay within image bounds
    x = (x < 0) ? 0 : (x >= img->width) ? img->width - 1 : x;
    y = (y < 0) ? 0 : (y >= img->height) ? img->height - 1 : y;
    // Calculate index
    int index = y * img->width + x;
    return img->pixels[index];
}
// Function to apply median filter to an image
struct Image *apply_MEDIAN(const struct Image *source) {
    // Allocate memory for the output image
    struct Image *output = malloc(sizeof(struct Image));
    if (output == NULL) {
        fprintf(stderr, "Memory allocation failed.\n");
        return NULL;
    }
    // Copy width and height
    output->width = source->width;
    output->height = source->height;
    // Allocate memory for the pixel array
    output->pixels = malloc(source->width * source->height * sizeof(struct Pixel));
    if (output->pixels == NULL) {
        fprintf(stderr, "Memory allocation failed.\n");
        free(output);
        return NULL;
    }
    // Iterate over each pixel in the image
    for (int y = 0; y < source->height; y++) {
        for (int x = 0; x < source->width; x++) {
            // Array to hold red, green, and blue values of surrounding pixels
            uint8_t red_values[9];
            uint8_t green_values[9];
            uint8_t blue_values[9];
            int index = 0;
            // Iterate over surrounding pixels
            for (int dy = -1; dy <= 1; dy++) {
                for (int dx = -1; dx <= 1; dx++) {
                    // Get pixel value
                    struct Pixel pixel = get_pixel(source, x + dx, y + dy);
                    // Store red, green, and blue values
                    red_values[index] = pixel.red;
                    green_values[index] = pixel.green;
                    blue_values[index] = pixel.blue;
                    index++;
                }
            }
            // Sort the values
            qsort(red_values, 9, sizeof(uint8_t), compare_uint8);
            qsort(green_values, 9, sizeof(uint8_t), compare_uint8);
            qsort(blue_values, 9, sizeof(uint8_t), compare_uint8);
            // Print ordered list of red, green, and blue values
            printf("Red values: ");
            for (int i = 0; i < 9; i++) {
                printf("%u ", red_values[i]);
            }
            printf("\n");
            fflush(stdout);
            printf("Green values: ");
            for (int i = 0; i < 9; i++) {
                printf("%u ", green_values[i]);
            }
            printf("\n");
            fflush(stdout);
            printf("Blue values: ");
            for (int i = 0; i < 9; i++) {
                printf("%u ", blue_values[i]);
            }
            printf("\n");
            fflush(stdout);
            // Get median values
            uint8_t red_median = red_values[4];
            uint8_t green_median = green_values[4];
            uint8_t blue_median = blue_values[4];
            // Print median values
            printf("Median Red: %u, Green: %u, Blue: %u\n", red_median, green_median, blue_median);
            fflush(stdout);
            // Update pixel in the output image
            int out_index = y * source->width + x;
            output->pixels[out_index].red = red_median;
            output->pixels[out_index].green = green_median;
            output->pixels[out_index].blue = blue_median;
        }
    }
    return output;
}

/* Perform your second task.
 * (TODO: Write a better comment here, and rename the function.
 * You may need to add or change arguments depending on the task.)
 * Returns true on success, or false on error. */
/* TODO: Question 4 */
// Function to count different pixels between two images
void apply_COMP(const struct Image *image1, const struct Image *image2) {
    int identical_pixels = 0;
    int different_pixels = 0;
    // check if both images have the same dimensions
    if (image1->width != image2->width || image1->height != image2->height) {
        printf("Images have different dimensions.\n");
        return;
    }
    //iterate over each pixel in the images
    for(int y = 0; y < image1->height; y++) {
        for(int x = 0; x < image1->width; x++) {
            // get pixels from both images
            struct Pixel pixel1 = get_pixel(image1, x, y);
            struct Pixel pixel2 = get_pixel(image2, x, y);
            // comapre RGB values of the pixels
            if (pixel1.red == pixel2.red && pixel1.green == pixel2.green && pixel1.blue == pixel2.blue) {
                identical_pixels++;
            } else {
                different_pixels++;
            }
        }
    }
    // print the results
    printf("Identical pixels: %d\n", identical_pixels);
    printf("Different pixels: %d\n", different_pixels);
}

int main(int argc, char *argv[]) {
    /* Initialise the random number generator, using the time as the seed */
    srand(time(NULL));
    /* Check command-line arguments */
    if (argc < 3) {
        fprintf(stderr, "Usage: process INPUTFILE OUTPUTFILE\n");
        return 1;
    };

    char *output_file = argv[2];
    
    struct Image *in_img = load_image(argv[1], output_file);
    struct Image *second_img = load_image(argv[3], output_file);

    /* Process all images */
        struct Image *processed_img = apply_MEDIAN(in_img);
        if (processed_img == NULL) {
            fprintf(stderr, "Failed to apply median filter to the input image.\n");
            // Clean up loaded and processed images on error
            free_image(in_img);
            return 1;
        }
        // Save the processed image
        if (!save_image(processed_img, output_file)) {
            fprintf(stderr, "Failed to save the processed image.\n");
            // Clean up the loaded and processed images on error
            free_image(processed_img);
            return 1;
        }
        free_image(processed_img);

    /* Compare images if there are at least two images loaded */
    apply_COMP(in_img, second_img);

    /* Free loaded images */
    return 0;

}


