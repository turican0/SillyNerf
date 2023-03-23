// SillyNerf.cpp : Tento soubor obsahuje funkci main. Provádění programu se tam zahajuje a ukončuje.
//

#include <iostream>
#include <string>
#include <vector>
#include <filesystem>
#include <fstream>
#include <png.h>
#include "SillyNerf.h"

std::string inputDir = "c:/prenos/SillyNerf/images/"; // zadejte cestu k adresáři, kde jsou PNG soubory

png_byte color_type;
png_byte bit_depth;
png_bytep* row_pointers = NULL;
int width;
int height;

typedef struct {
	unsigned char r;
	unsigned char g;
	unsigned char b;
	unsigned char a;
}
TColor;

typedef struct {
	int width;
	int height;
	TColor* buffer;
}
imgInfo;

typedef struct {
	TColor color;
}
Voxel;

void read_png_file(char* filename, imgInfo* img) {
	FILE* fp;
	fopen_s(&fp, filename, "rb");

	png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (!png) abort();

	png_infop info = png_create_info_struct(png);
	if (!info) abort();

	if (setjmp(png_jmpbuf(png))) abort();

	png_init_io(png, fp);

	png_read_info(png, info);

	width = png_get_image_width(png, info);
	height = png_get_image_height(png, info);
	color_type = png_get_color_type(png, info);
	bit_depth = png_get_bit_depth(png, info);

	// Read any color_type into 8bit depth, RGBA format.
	// See http://www.libpng.org/pub/png/libpng-manual.txt

	if (bit_depth == 16)
		png_set_strip_16(png);

	if (color_type == PNG_COLOR_TYPE_PALETTE)
		png_set_palette_to_rgb(png);

	// PNG_COLOR_TYPE_GRAY_ALPHA is always 8 or 16bit depth.
	if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
		png_set_expand_gray_1_2_4_to_8(png);

	if (png_get_valid(png, info, PNG_INFO_tRNS))
		png_set_tRNS_to_alpha(png);

	// These color_type don't have an alpha channel then fill it with 0xff.
	if (color_type == PNG_COLOR_TYPE_RGB ||
		color_type == PNG_COLOR_TYPE_GRAY ||
		color_type == PNG_COLOR_TYPE_PALETTE)
		png_set_filler(png, 0xFF, PNG_FILLER_AFTER);

	if (color_type == PNG_COLOR_TYPE_GRAY ||
		color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
		png_set_gray_to_rgb(png);

	png_read_update_info(png, info);

	//if (row_pointers) abort();

	row_pointers = (png_bytep*)malloc(sizeof(png_bytep) * height);
	for (int y = 0; y < height; y++) {
		row_pointers[y] = (png_byte*)malloc(png_get_rowbytes(png, info));
	}

	png_read_image(png, row_pointers);

	img->width = width;
	img->height = height;
	img->buffer = (TColor*)malloc(width * height * sizeof(TColor));
	int index = 0;
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++)
		{
			TColor colTemp;
			colTemp.r = row_pointers[y][x * 4 + 0];
			colTemp.g = row_pointers[y][x * 4 + 1];
			colTemp.b = row_pointers[y][x * 4 + 2];
			colTemp.a = row_pointers[y][x * 4 + 3];
			img->buffer[index] = colTemp;
			index++;
		}
	}

	for (int y = 0; y < height; y++) {
		free(row_pointers[y]);
	}
	free(row_pointers);

	fclose(fp);

	png_destroy_read_struct(&png, &info, NULL);
}

void freeBuffers(std::vector<imgInfo> images) {
	for (int i = 0; i < images.size(); i++) {
		free(images.at(0).buffer);
	}
};

int main()
{
    std::vector<imgInfo> images;

    for (const auto& file : std::filesystem::directory_iterator(inputDir)) {
        if (file.path().extension() == ".png") {
			char filechar[2000];
			wcstombs_s(nullptr, filechar, file.path().c_str(), 2000);
			/*size_t i;
			wcstombs_s(&i, pMBBuffer, (size_t)BUFFER_SIZE,
				pWCBuffer, (size_t)BUFFER_SIZE - 1); // -1 so the appended NULL doesn't fall outside the allocated buffer
			*/
			imgInfo actimg;
			read_png_file(filechar,&actimg);
			images.push_back(actimg);
        }
    }

	int maxWidth = 0;
	int maxHeight = 0;
	for (int i = 0; i < images.size(); i++) {
		if (images.at(i).width > maxWidth)
			maxWidth = images.at(i).width;
		if (images.at(i).height > maxHeight)
			maxHeight = images.at(i).height;
	}

	Voxel* vArray = (Voxel*)malloc(sizeof(Voxel)* maxWidth* maxWidth* maxHeight);

	freeBuffers(images);

    return 0;
}

// Spuštění programu: Ctrl+F5 nebo nabídka Ladit > Spustit bez ladění
// Ladění programu: F5 nebo nabídka Ladit > Spustit ladění

// Tipy pro zahájení práce:
//   1. K přidání nebo správě souborů použijte okno Průzkumník řešení.
//   2. Pro připojení ke správě zdrojového kódu použijte okno Team Explorer.
//   3. K zobrazení výstupu sestavení a dalších zpráv použijte okno Výstup.
//   4. K zobrazení chyb použijte okno Seznam chyb.
//   5. Pokud chcete vytvořit nové soubory kódu, přejděte na Projekt > Přidat novou položku. Pokud chcete přidat do projektu existující soubory kódu, přejděte na Projekt > Přidat existující položku.
//   6. Pokud budete chtít v budoucnu znovu otevřít tento projekt, přejděte na Soubor > Otevřít > Projekt a vyberte příslušný soubor .sln.
