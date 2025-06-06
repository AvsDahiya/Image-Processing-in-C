This coursework involves writing a simple RGB bitmap image processing program in C, called process. process is run from the command line with two arguments, an input image file (which must already exist) and an output image file (which will be created or overwritten).

# F28HS Coursework 1
Aditya Vikram Singh Dahiya - H00444458 

Compiling the program using Clang:
----------------------------------
1. To compile the process file, execute:
    clang process.c -o process

2. To process a median filter and compare an image, execute:
    ./process inputimg.hs8 input-processed.hs8 inputimg2.hs8

        For example:
        ./process coffee.hs8 coffee-processed.hs8 bars.hs8

Viewing the formatted images with hsconvert:
--------------------------------------------
To view the hs8 formatted image, use the hsconvert program.
1. Ensure hsconvert is within the same directory as the process.c program
2. Ensure you have a ppm viewer program installed on your machine or VSCode 
3. execute the following command:
    ./hsconvert -f PPM inputimg.hs8 outputimg.ppm

        For example:
        ./hsconvert -f PPM bars.hs8 bars.ppm

