# SimpleTesseract-GUI

A simple and light-weight Qt front-end for tesseract-ocr library [https://github.com/tesseract-ocr/tesseract]

![alt text](https://i.imgur.com/VIx3qI8.png)

## Building

### Requirements

- Basic Qt5 installation
- libtesseract-dev (Debian)
- libleptonica-dev (Automatically installed with libtesseract)

### Compiling

Clone the repository
```
git clone https://github.com/luco5826/simple-tesseract-gui 
```
Run `make` command
```
cd simple-tesseract-gui && make
```
Before the execution, you need to launch the following command too
```
export LC_ALL=C
./tesseract
```



