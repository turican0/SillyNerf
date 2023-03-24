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

typedef unsigned char Bit8u;

inline void setRGBA(png_byte* ptr, Bit8u* val)
{
	ptr[0] = val[0];
	ptr[1] = val[1];
	ptr[2] = val[2];
	ptr[3] = val[3];
}

void writeImagePNG(char* filename, int width, int height, Bit8u* buffer, char* title)
{
	int code = 0;
	FILE* fp = NULL;
	png_structp png_ptr = NULL;
	png_infop info_ptr = NULL;
	png_bytep row = NULL;

	// Open file for writing (binary mode)
	fopen_s(&fp, filename, "wb");
	if (fp == NULL) {
		fprintf(stderr, "Could not open file %s for writing\n", filename);
		code = 1;
		goto finalise;
	}

	// Initialize write structure
	png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (png_ptr == NULL) {
		fprintf(stderr, "Could not allocate write struct\n");
		code = 1;
		goto finalise;
	}

	// Initialize info structure
	info_ptr = png_create_info_struct(png_ptr);
	if (info_ptr == NULL) {
		fprintf(stderr, "Could not allocate info struct\n");
		code = 1;
		goto finalise;
	}

	// Setup Exception handling
	if (setjmp(png_jmpbuf(png_ptr))) {
		fprintf(stderr, "Error during png creation\n");
		code = 1;
		goto finalise;
	}

	png_init_io(png_ptr, fp);

	// Write header (8 bit colour depth)
	png_set_IHDR(png_ptr, info_ptr, width, height,
		8, PNG_COLOR_TYPE_RGBA, PNG_INTERLACE_NONE,
		PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

	// Set title
	if (title != NULL) {
		png_text title_text;
		title_text.compression = PNG_TEXT_COMPRESSION_NONE;
		title_text.key = (png_charp)"Title";
		title_text.text = title;
		png_set_text(png_ptr, info_ptr, &title_text, 1);
	}

	png_write_info(png_ptr, info_ptr);

	// Allocate memory for one row (3 bytes per pixel - RGB)
	row = (png_bytep)malloc(4 * width * sizeof(png_byte));

	// Write image data
	int x, y;
	for (y = 0; y < height; y++) {
		for (x = 0; x < width; x++) {
			setRGBA(&(row[x * 4]), buffer + (y * width + x) * 4);
		}
		png_write_row(png_ptr, row);
	}

	// End write
	png_write_end(png_ptr, NULL);

finalise:
	if (fp != NULL) fclose(fp);
	if (info_ptr != NULL) png_free_data(png_ptr, info_ptr, PNG_FREE_ALL, -1);
	if (png_ptr != NULL) png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
	if (row != NULL) free(row);

	//return code;
}

void freeBuffers(std::vector<imgInfo> images) {
	for (int i = 0; i < images.size(); i++) {
		free(images[i].buffer);
	}
};

int maxWidth = 0;
int maxHeight = 0;
int maxWidthZ = 0;
int maxWHZ = 0;
int maxHeightZ = 0;
int zoom = 2;

static unsigned int g_seed;

// Used to seed the generator.           
inline void fast_srand(int seed) {
	g_seed = seed;
}

// Compute a pseudorandom integer.
// Output value in range [0, 32767]
inline int fast_rand(void) {
	g_seed = (214013 * g_seed + 2531011);
	return (g_seed >> 16) & 0x7FFF;
}


void SetRandom(Voxel *vArray) {
	for(int x=0; x < maxWidthZ; x++)
		for (int y = 0; y < maxWidthZ; y++)
			for (int z = 0; z < maxHeightZ; z++)
			{
				Voxel tempvox;
				tempvox.color.a = (fast_rand() % 2) * 255;
				tempvox.color.r = fast_rand() % 256;
				tempvox.color.g = fast_rand() % 256;
				tempvox.color.b = fast_rand() % 256;

				vArray[x * maxWHZ + y * maxHeightZ + z] = tempvox;
			}
};


//NN

long Compare(imgInfo actImage, int rotation) {
	return 0;
};

void SetImage(Voxel* vArray, imgInfo actImage, int rotation) {
	for (int x = 0; x < maxWidthZ; x++)
		for (int y = 0; y < maxWidthZ; y++)
			for (int z = 0; z < maxHeightZ; z++)
			{
				Voxel tempvox;
				tempvox.color.a = 255;// (actImage.buffer[x + z * actImage.width].a) ? 0 : 255;
				tempvox.color.r = actImage.buffer[x + z * actImage.width].r;
				tempvox.color.g = actImage.buffer[x + z * actImage.width].g;
				tempvox.color.b = actImage.buffer[x + z * actImage.width].b;

				vArray[x * maxWHZ + y * maxHeightZ + z] = tempvox;
			}
};

void SaveImage(Voxel* vArray, int rotation) {
	char* filename = (char*)"test.png";
	char* title = (char*)"test png";
	int imgWidth = maxWidth;
	int imgHeight = maxHeight;
	Bit8u* imgBbuffer = (Bit8u*)malloc(maxWidthZ * maxHeightZ * 4);
	//createPng((char*)"test2.png", 10, 10);
	for (int x = 0; x < maxWidthZ; x++)
		for (int y = 0; y < 1; y++)
			for (int z = 0; z < maxHeightZ; z++)
			{
				imgBbuffer[(x + z * imgWidth) * 4 + 0] = vArray[x * maxWHZ + y * maxHeightZ + z].color.r;
				imgBbuffer[(x + z * imgWidth) * 4 + 1] = vArray[x * maxWHZ + y * maxHeightZ + z].color.g;
				imgBbuffer[(x + z * imgWidth) * 4 + 2] = vArray[x * maxWHZ + y * maxHeightZ + z].color.b;
				imgBbuffer[(x + z * imgWidth) * 4 + 3] = vArray[x * maxWHZ + y * maxHeightZ + z].color.a;
			}
	//createPng((char*)"test2.png", 10, 10);
	writeImagePNG(filename, imgWidth, imgHeight, imgBbuffer, title);
	free(imgBbuffer);
};

int main()
{
	fast_srand(time(NULL));

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

	for (int i = 0; i < images.size(); i++) {
		if (images[i].width > maxWidth)
			maxWidth = images[i].width;
		if (images[i].height > maxHeight)
			maxHeight = images[i].height;
	}

	maxWidthZ = maxWidth * zoom;
	maxHeightZ = maxHeight * zoom;
	maxWHZ = maxWidthZ * maxHeight;

	Voxel* vArray = (Voxel*)malloc(sizeof(Voxel) * maxWidthZ * maxWidthZ * maxHeightZ);

	SetRandom(vArray);
	SetImage(vArray, images[0],0);
	SaveImage(vArray, 0);
	long diff = Compare(images[0], 0);

	free(vArray);
	freeBuffers(images);

    return 0;
}

//x* maxWHZ + y * maxHeightZ + z 0 red
//x * maxWHZ + y * maxHeightZ + z 1 red
//x * maxWHZ + y * maxHeightZ + z 14820 red
//x * maxWHZ + y * maxHeightZ + z 14821 gray




// Spuštění programu: Ctrl+F5 nebo nabídka Ladit > Spustit bez ladění
// Ladění programu: F5 nebo nabídka Ladit > Spustit ladění

// Tipy pro zahájení práce:
//   1. K přidání nebo správě souborů použijte okno Průzkumník řešení.
//   2. Pro připojení ke správě zdrojového kódu použijte okno Team Explorer.
//   3. K zobrazení výstupu sestavení a dalších zpráv použijte okno Výstup.
//   4. K zobrazení chyb použijte okno Seznam chyb.
//   5. Pokud chcete vytvořit nové soubory kódu, přejděte na Projekt > Přidat novou položku. Pokud chcete přidat do projektu existující soubory kódu, přejděte na Projekt > Přidat existující položku.
//   6. Pokud budete chtít v budoucnu znovu otevřít tento projekt, přejděte na Soubor > Otevřít > Projekt a vyberte příslušný soubor .sln.
