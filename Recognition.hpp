#ifndef RECOGNITION_HPP
#define RECOGNITION_HPP

#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>

#include <QImage>

namespace Recognition
{
static std::string recognize(QImage& image)
{
    if(image.width() < 1 || image.height() < 1){
        return "Errore nella lettura";
    }
    char *outText;

    tesseract::TessBaseAPI *api = new tesseract::TessBaseAPI();

    // Initialize tesseract-ocr with English, without specifying tessdata path
    if (api->Init(NULL, "eng"))
    {
        fprintf(stderr, "Could not initialize tesseract.\n");
        exit(1);
    }

    // Open input image with leptonica library
    Pix *imagePix = pixRead("./prova.jpeg");
    api->SetImage(imagePix);
    // api->SetImage(image.bits(), image.width(), image.height(), 3,image.bytesPerLine());

    // Get OCR result
    outText = api->GetUTF8Text();

    // Destroy used object and release memory
    api->End();
    pixDestroy(&imagePix);

    return outText;
}
} // namespace Recognition
#endif