# CppNDCapstone
Capstone for C++ Nanodegree

# Steps to Build

    mkdir build
    cd build
    mkdir data
    Download MNIST dataset into data directory
    cmake ..
    make
    ./mnist


# LeNet

Because my interest in C++ stems from my interests in machine learning and artificial intelligence, I opted to do my capstone project in this area.  I recreated [LeNet 5](http://yann.lecun.com/exdb/publis/pdf/lecun-01a.pdf).  This was chosen because it is now a simple enough neural net to be understood and recreated with my still rudimentary understanding of C++, despite being a groundbreaking method 20 years ago.

## File Structure

|-CMakeLists.txt
|-dataloaders.tpp
|-mnist.cpp
|-net.h
|-net.cpp
|-README.md
|-build
|--data
|--t10k-images-idx1-ubyte
|--t10k-labels-idx1-ubyte
|--t10k-images-idx3-ubyte
|--t10k-labels-idx3-ubyte

## Class Structure
As I chose to use libtorch library in this project, I opted to keep in line with its coding style.  Libtorch uses structs and templates more than classes so I have also use structs instead of classes.  I have separated the net definition into its own file so there is a `net.h` and a `net.cpp`.  I have also placed the dataloaders in their own file as `dataloaders.tpp`.  

## Expected Output



## Rubric

For the completion of the capstone project, I must satisfy four mandatory criteria as well as an additional five of my choosing.  The [rubric](./UdacityCapstoneRubric.pdf) can be found in the repository as well.  Below I will discuss each of the criteria.

### A README with instructions is included with project

 - [ ] README is included with the project and has instructions for building/running the project
 - [ ] If any additional libraries are needed to run the project, these are indicated with cross-platform installation instructions.
 - [x] Markdown or pdf: Markdown is chosen

### The README indicates which project is chosen

 - [x] The README describes the project you have built.
 - [x] The README also indicates the file and class structure, along with the expected behavior or output of the program

### The README includes information about each rubric point addressed

 - [ ] The README indicates which rubric points are addressed.  The README also indicates where in the code (i.e. files and line numbers) that the rubric points are addressed.

### The submission must compile and run

 - [x] The project code must compile and run without errors
 - [x] Code must compile on any reviewer platform

### The project reads data from a file and process the data
 - [x] The project reads data from an external file or writes data to a file as part of the necessary operation of the program

### The project accepts user input and processes the input
 - [x] The porject accepts input from a user as part of the necessary operation of the program

### The project demonstrates an understanding of C++ functions and control structures
 - [x] A variety of control structures are used in the project
 - [x] the project is clearly organized into functions

### Templates generalize functions in the project
 - [x] One function is declared with a template that allows it to accept a generic parameter

### The project uses move semantics to move data, instead of copying it, where possible
 - [x] For classes with move constructors, the project returns objects of that class by value, and relies on the move constructor, instead of copying the object
