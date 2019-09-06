#ifndef RECOGNITION_HPP
#define RECOGNITION_HPP

#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>

namespace Recognition
{
static char* recognize(const char* fileName, const char* selectedLanguage)
{
    char *outText;

    tesseract::TessBaseAPI *api = new tesseract::TessBaseAPI();

    // Initialize tesseract-ocr with English, without specifying tessdata path
    if (api->Init(NULL, selectedLanguage))
    {
        fprintf(stderr, "Could not initialize tesseract.\n");
        exit(1);
    }

    // Open input image with leptonica library
    Pix *imagePix = pixRead(fileName);
    api->SetImage(imagePix);

    // Get OCR result
    outText = api->GetUTF8Text();

    // Destroy used object and release memory
    api->End();
    pixDestroy(&imagePix);

    return outText;
}
} // namespace Recognition
#endif
