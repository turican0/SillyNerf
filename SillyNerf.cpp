// SillyNerf.cpp : Tento soubor obsahuje funkci main. Provádění programu se tam zahajuje a ukončuje.
//

#include <iostream>
#include <string>
#include <vector>
#include <filesystem>
#include <fstream>

#include <iomanip>
#include <sstream>

#include <png.h>

#define OGT_VOX_IMPLEMENTATION
#include <ogt_vox.h>

using namespace std;

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
int zoom = 1;

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

void SetTransparent(Voxel* vArray) {
	for (int x = 0; x < maxWidthZ; x++)
		for (int y = 0; y < maxWidthZ; y++)
			for (int z = 0; z < maxHeightZ; z++)
			{
				Voxel tempvox;
				tempvox.color.a = 0;
				tempvox.color.r = 0;
				tempvox.color.g = 0;
				tempvox.color.b = 0;

				vArray[x * maxWHZ + y * maxHeightZ + z] = tempvox;
			}
};


//NN

void AddTranspColor(vector<TColor>** vArray) {
	for (int x = 0; x < maxWidthZ; x++)
		for (int y = 0; y < maxWidthZ; y++)
			for (int z = 0; z < maxHeightZ; z++)
			{
				TColor color;
				color.r = 0;
				color.g = 0;
				color.b = 0;
				color.a = 0;

				bool isIncluded = false;
				for (int i = 0; i < vArray[x * maxWHZ + y * maxHeightZ + z]->size(); i++)
					if ((color.r == (*vArray[x * maxWHZ + y * maxHeightZ + z])[i].r) &&
						(color.g == (*vArray[x * maxWHZ + y * maxHeightZ + z])[i].g) &&
						(color.b == (*vArray[x * maxWHZ + y * maxHeightZ + z])[i].b) &&
						(color.a == (*vArray[x * maxWHZ + y * maxHeightZ + z])[i].a))
						isIncluded = true;
				if (!isIncluded)vArray[x * maxWHZ + y * maxHeightZ + z]->push_back(color);
			}
};

void AddCanColor(vector<TColor>** vArray, imgInfo actImage, int rotation, bool invert) {
	//int imgWidth = maxWidth;
	//int imgHeight = maxHeight;
	//Bit8u* imgBbuffer = (Bit8u*)malloc(maxWidthZ * maxHeightZ * 4);

	float s = sin(rotation * 0.0174533);
	float c = cos(rotation * 0.0174533);

	int minX = 0;
	int maxX = maxWidthZ;
	int addX = 1;
	int minY = 0;
	int maxY = maxWidthZ;
	int addY = 1;

	float tempAddCenter = ((maxWidthZ - 1) * 0.5 * zoom);

	for (int x = minX; x != maxX; x += addX)
		for (int y = minY; y != maxY; y += addY)
		{
			int newX = ((x - tempAddCenter) * c - (y - tempAddCenter) * s) + tempAddCenter + 0.01;
			int newY = ((x - tempAddCenter) * s + (y - tempAddCenter) * c) + tempAddCenter + 0.01;
			if ((newX < 0) || (newY < 0))
				continue;
			int tempIndex = newX * maxWHZ + newY * maxHeightZ;
			for (int z = 0; z < maxHeightZ; z++)
			{
				TColor color;
				color.r = actImage.buffer[x + z * maxWidthZ].r;
				color.g = actImage.buffer[x + z * maxWidthZ].g;
				color.b = actImage.buffer[x + z * maxWidthZ].b;
				color.a = actImage.buffer[x + z * maxWidthZ].a;
				if ((color.r == 0x80) && (color.g == 0x80) && (color.b == 0x80))
				{
					color.r = 0;
					color.g = 0;
					color.b = 0;
					color.a = 0;
					vArray[tempIndex + z]->clear();
					vArray[tempIndex + z]->push_back(color);
				}
				else
				{
					bool isIncluded = false;
					for (int i = 0; i < vArray[tempIndex + z]->size(); i++)
						if ((color.r == (*vArray[tempIndex + z])[i].r) &&
							(color.g == (*vArray[tempIndex + z])[i].g) &&
							(color.b == (*vArray[tempIndex + z])[i].b) &&
							(color.a == (*vArray[tempIndex + z])[i].a))
							isIncluded = true;
					if (!isIncluded)vArray[tempIndex + z]->push_back(color);
				}
			}
		}
};

long Compare(Voxel* vArray, imgInfo actImage, int rotation, bool invert) {
	char* filename = (char*)"test.png";
	char* title = (char*)"test png";
	int imgWidth = maxWidth;
	int imgHeight = maxHeight;
	Bit8u* imgBbuffer = (Bit8u*)malloc(maxWidthZ * maxHeightZ * 4);

	for (int x = 0; x < maxWidthZ; x++)
		for (int z = 0; z < maxHeightZ; z++)
		{
			imgBbuffer[(x + z * maxWidthZ) * 4 + 0] = 0;
			imgBbuffer[(x + z * maxWidthZ) * 4 + 1] = 0;
			imgBbuffer[(x + z * maxWidthZ) * 4 + 2] = 0;
			imgBbuffer[(x + z * maxWidthZ) * 4 + 3] = 0;
		}

	float s = sin(rotation * 0.0174533);
	float c = cos(rotation * 0.0174533);

	int minX = 0;
	int maxX = maxWidthZ;
	int addX = 1;
	int minY = 0;
	int maxY = maxWidthZ;
	int addY = 1;

	float tempAddCenter = ((maxWidthZ - 1) * 0.5 * zoom);

	for (int x = minX; x != maxX; x += addX)
		for (int y = minY; y != maxY; y += addY)
		{
			int newX = ((x - tempAddCenter) * c - (y - tempAddCenter) * s) + tempAddCenter + 0.01;
			int newY = ((x - tempAddCenter) * s + (y - tempAddCenter) * c) + tempAddCenter + 0.01;
			if ((newX < 0) || (newY < 0))
				continue;
			int tempIndex = newX * maxWHZ + newY * maxHeightZ;
			for (int z = 0; z < maxHeightZ; z++)
			{
				if (vArray[tempIndex + z].color.a == 255)
				{
					imgBbuffer[(x + z * maxWidthZ) * 4 + 0] = vArray[tempIndex + z].color.r;
					imgBbuffer[(x + z * maxWidthZ) * 4 + 1] = vArray[tempIndex + z].color.g;
					imgBbuffer[(x + z * maxWidthZ) * 4 + 2] = vArray[tempIndex + z].color.b;
					imgBbuffer[(x + z * maxWidthZ) * 4 + 3] = vArray[tempIndex + z].color.a;
				}
				/*
				imgBbuffer[(x + z * maxWidthZ) * 4 + 0] = vArray[x * maxWHZ + y * maxHeightZ + z].color.r;
				imgBbuffer[(x + z * maxWidthZ) * 4 + 1] = vArray[x * maxWHZ + y * maxHeightZ + z].color.g;
				imgBbuffer[(x + z * maxWidthZ) * 4 + 2] = vArray[x * maxWHZ + y * maxHeightZ + z].color.b;
				imgBbuffer[(x + z * maxWidthZ) * 4 + 3] = vArray[x * maxWHZ + y * maxHeightZ + z].color.a;
				*/
			}
		}
	//createPng((char*)"test2.png", 10, 10);
	//writeImagePNG(filename, maxWidthZ, maxHeightZ, imgBbuffer, title);

	int diffX = maxWidth - actImage.width;
	int diffY = maxHeight - actImage.height;

	long diff=0;
	for (int x = 0; x < maxWidthZ; x++)
		for (int z=0; z< maxHeightZ; z++)
	{
			int imgX = (int)((x - diffX) / zoom);
			int imgZ = (int)((z - diffY) / zoom);
			if ((imgX < actImage.width) && (imgZ < actImage.height)&&(imgX>=0) && (imgZ >= 0))
			{
				diff += abs(actImage.buffer[imgX + imgZ * actImage.width].r - imgBbuffer[(x + z * maxWidthZ) * 4 + 0]);
				diff += abs(actImage.buffer[imgX + imgZ * actImage.width].g - imgBbuffer[(x + z * maxWidthZ) * 4 + 1]);
				diff += abs(actImage.buffer[imgX + imgZ * actImage.width].b - imgBbuffer[(x + z * maxWidthZ) * 4 + 2]);
				diff += 5*abs(actImage.buffer[imgX + imgZ * actImage.width].a - imgBbuffer[(x + z * maxWidthZ) * 4 + 3]);

				//imgBbuffer[(x + z * maxWidthZ) * 4 + 0] = actImage.buffer[imgX + imgZ * actImage.width].r;
				//imgBbuffer[(x + z * maxWidthZ) * 4 + 1] = actImage.buffer[imgX + imgZ * actImage.width].g;
				//imgBbuffer[(x + z * maxWidthZ) * 4 + 2] = actImage.buffer[imgX + imgZ * actImage.width].b;
				//imgBbuffer[(x + z * maxWidthZ) * 4 + 3] = 255;
			}
			else
			{
				diff += abs(0 - imgBbuffer[(x + z * maxWidthZ) * 4 + 0]);
				diff += abs(0 - imgBbuffer[(x + z * maxWidthZ) * 4 + 1]);
				diff += abs(0 - imgBbuffer[(x + z * maxWidthZ) * 4 + 2]);
				diff += 5*abs(0 - imgBbuffer[(x + z * maxWidthZ) * 4 + 3]);
			}
	}
	//writeImagePNG((char*)"test2.png", maxWidthZ, maxHeightZ, imgBbuffer, title);
	free(imgBbuffer);
	return diff;
};

void SetImage(Voxel* vArray, imgInfo actImage, int rotation) {
	int diffX = (maxWidth - actImage.width)/2;
	int diffY = (maxHeight - actImage.height)/2;

	for (int x = diffX; x < maxWidthZ; x++)
		for (int y = 0; y < maxWidthZ; y++)
			for (int z = diffY; z < maxHeightZ; z++)
			{
				int imgX = (int)((x- diffX) / zoom);
				int imgZ = (int)((z- diffY) / zoom);
				if ((imgX < actImage.width) && (imgZ < actImage.height))
				{
					Voxel tempvox;
					tempvox.color.a = actImage.buffer[imgX + imgZ * actImage.width].a;
					tempvox.color.r = actImage.buffer[imgX + imgZ * actImage.width].r;
					tempvox.color.g = actImage.buffer[imgX + imgZ * actImage.width].g;
					tempvox.color.b = actImage.buffer[imgX + imgZ * actImage.width].b;
					/*if ((tempvox.color.r == 0x80) && (tempvox.color.g == 0x80) && (tempvox.color.b == 0x80))
					{
						tempvox.color.r = 0;
						tempvox.color.g = 0;
						tempvox.color.b = 0;
						tempvox.color.a = 0;
					}*/

					vArray[x * maxWHZ + y * maxHeightZ + z] = tempvox;
				}
			}
};

void SaveImage(Voxel* vArray, int rotation, char* filename) {
	//char* filename = (char*)"test.png";
	char* title = (char*)"test png";
	int imgWidth = maxWidth;
	int imgHeight = maxHeight;
	Bit8u* imgBbuffer = (Bit8u*)malloc(maxWidthZ * maxHeightZ * 4);

	for (int x = 0; x != maxWidthZ; x++)
		for (int y = 0; y != maxHeightZ; y++)
		{
			imgBbuffer[(x + y * maxWidthZ) * 4 + 0] = 0;
			imgBbuffer[(x + y * maxWidthZ) * 4 + 1] = 0;
			imgBbuffer[(x + y * maxWidthZ) * 4 + 2] = 0;
			imgBbuffer[(x + y * maxWidthZ) * 4 + 3] = 0;
		}

	float s = sin(rotation * 0.0174533);
	float c = cos(rotation * 0.0174533);
	//createPng((char*)"test2.png", 10, 10);

	/*
	int minX = 0;
	int maxX = maxWidthZ;
	int addX = 1;
	int minY = 0;
	int maxY = maxWidthZ;
	int addY = 1;
	*/
	int minX = 0;
	int maxX = maxWidthZ;
	int addX = 1;
	int minY = 0;
	int maxY = maxWidthZ;
	int addY = 1;

	float tempAddCenter = ((maxWidthZ-1) * 0.5 * zoom);

	for (int x = minX; x != maxX; x+= addX)
		for (int y = minY; y != maxY; y += addY)
		{
			int newX = ((x - tempAddCenter) * c - (y - tempAddCenter) * s) + tempAddCenter+0.01;
			int newY = ((x - tempAddCenter) * s + (y - tempAddCenter) * c) + tempAddCenter+0.01;
			if ((newX < 0) || (newY < 0))
				continue;
			for (int z = 0; z < maxHeightZ; z++)
			{
				if (vArray[newX * maxWHZ + newY * maxHeightZ + z].color.a == 255)
				{
					imgBbuffer[(x + z * maxWidthZ) * 4 + 0] = vArray[newX * maxWHZ + newY * maxHeightZ + z].color.r;
					imgBbuffer[(x + z * maxWidthZ) * 4 + 1] = vArray[newX * maxWHZ + newY * maxHeightZ + z].color.g;
					imgBbuffer[(x + z * maxWidthZ) * 4 + 2] = vArray[newX * maxWHZ + newY * maxHeightZ + z].color.b;
					imgBbuffer[(x + z * maxWidthZ) * 4 + 3] = vArray[newX * maxWHZ + newY * maxHeightZ + z].color.a;
				}
				/*
				imgBbuffer[(x + z * maxWidthZ) * 4 + 0] = vArray[x * maxWHZ + y * maxHeightZ + z].color.r;
				imgBbuffer[(x + z * maxWidthZ) * 4 + 1] = vArray[x * maxWHZ + y * maxHeightZ + z].color.g;
				imgBbuffer[(x + z * maxWidthZ) * 4 + 2] = vArray[x * maxWHZ + y * maxHeightZ + z].color.b;
				imgBbuffer[(x + z * maxWidthZ) * 4 + 3] = vArray[x * maxWHZ + y * maxHeightZ + z].color.a;
				*/
			}
		}
	//createPng((char*)"test2.png", 10, 10);
	writeImagePNG(filename, maxWidthZ, maxHeightZ, imgBbuffer, title);
	free(imgBbuffer);
};

void RandomMutage(Voxel* vArray, vector<TColor>** canColorArray) {
	{
		int x = fast_rand() % maxWidthZ;
		int y = fast_rand() % maxWidthZ;
		int z = fast_rand() % maxHeightZ;

		int color = fast_rand() % canColorArray[x * maxWHZ + y * maxHeightZ + z]->size();
		TColor randcolor = (*canColorArray[x * maxWHZ + y * maxHeightZ + z])[color];
		vArray[x * maxWHZ + y * maxHeightZ + z].color = randcolor;
	}
	/*else
	{
		int xMin = fast_rand() % ((int)(maxWidthZ / size));
		int yMin = fast_rand() % ((int)(maxWidthZ / size));
		int zMin = fast_rand() % ((int)(maxHeightZ / size));
		int color = fast_rand() % 4;
		int step = (fast_rand() % 2) ? 1 : -1;
		for (int x = xMin* size; x < (xMin+1) * size; x++)
			for (int y = yMin * size; y < (yMin + 1) * size; y++)
				for (int z = zMin * size; z < (zMin + 1) * size; z++)
				{					
					TColor oldColor = vArray[x * maxWHZ + y * maxHeightZ + z].color;
					switch (color)
					{
					case 0:
						oldColor.r += step;
						break;
					case 1:
						oldColor.g += step;
						break;
					case 2:
						oldColor.b += step;
						break;
					case 3:
						oldColor.a += step;
						break;
					}
					vArray[x * maxWHZ + y * maxHeightZ + z].color = oldColor;
				}
	}*/
};

void CopyArray(Voxel* vArray, Voxel* vArray2) {
	memcpy(vArray2, vArray, sizeof(Voxel) * maxWidthZ * maxWidthZ * maxHeightZ);
		/*for (int x = 0; x < maxWidthZ; x++)
			for (int y = 0; y < maxWidthZ; y++)
				for (int z = 0; z < maxHeightZ; z++)
				{
					vArray2[x * maxWHZ + y * maxHeightZ + z] = vArray[x * maxWHZ + y * maxHeightZ + z];
				}*/
}

struct Voxel2 {
	uint8_t r, g, b, a;
};

void save_vox_scene(const char* pcFilename, const ogt_vox_scene* scene)
{
	// save the scene back out. 
	uint32_t buffersize = 0;
	uint8_t* buffer = ogt_vox_write_scene(scene, &buffersize);
	if (!buffer)
		return;

	// open the file for write
#if defined(_MSC_VER) && _MSC_VER >= 1400
	FILE* fp;
	if (0 != fopen_s(&fp, pcFilename, "wb"))
		fp = 0;
#else
	FILE* fp = fopen(pcFilename, "wb");
#endif
	if (!fp) {
		ogt_vox_free(buffer);
		return;
	}

	fwrite(buffer, buffersize, 1, fp);
	fclose(fp);
	ogt_vox_free(buffer);
}

/*
#include <cstdint>
#include <fstream>
#include <vector>

#pragma pack(push, 1)
struct VoxHeader {
	char id[4];
	int32_t version;
};

struct ChunkHeader {
	char id[4];
	int32_t contentSize;
	int32_t childrenSize;
};

struct SizeChunk {
	int32_t x;
	int32_t y;
	int32_t z;
};

struct Voxel {
	uint8_t x;
	uint8_t y;
	uint8_t z;
	uint8_t i;
};
#pragma pack(pop)

int main() {
	const int32_t size = 3;
	std::vector<Voxel> voxels;

	for (uint8_t x = 0; x < size; ++x) {
		for (uint8_t y = 0; y < size; ++y) {
			for (uint8_t z = 0; z < size; ++z) {
				voxels.push_back({x, y, z, 1});
			}
		}
	}

	std::vector<uint32_t> palette = {0x00FFFFFF, 0xFF0000FF, 0x00FF00FF, 0x0000FFFF,
									  0xFFFF00FF, 0xFF00FFFF, 0x00FFFFFF, 0xFFFFFFFF};

	VoxHeader header = {'V', 'O', 'X', ' ', 150};
	ChunkHeader mainChunk = {'M', 'A', 'I', 'N', 0, 40 + (int32_t)voxels.size() * 4};
	ChunkHeader sizeChunk = {'S', 'I', 'Z', 'E', 12, 0};
	SizeChunk dimensions = {size, size, size};
	ChunkHeader xyzChunk = {'X', 'Y', 'Z', 'I', (int32_t)voxels.size() * 4, 0};
	ChunkHeader rgbaChunk = {'R', 'G', 'B', 'A', 256 * 4, 0};

	std::ofstream file("cube.vox", std::ios::binary);

	file.write(reinterpret_cast<const char*>(&header), sizeof(header));
	file.write(reinterpret_cast<const char*>(&mainChunk), sizeof(mainChunk));
	file.write(reinterpret_cast<const char*>(&sizeChunk), sizeof(sizeChunk));
	file.write(reinterpret_cast<const char*>(&dimensions), sizeof(dimensions));
	file.write(reinterpret_cast<const char*>(&xyzChunk), sizeof(xyzChunk));
	file.write(reinterpret_cast<const char*>(voxels.data()), voxels.size() * sizeof(Voxel));
	file.write(reinterpret_cast<const char*>(&rgbaChunk), sizeof(rgbaChunk));
	file.write(reinterpret_cast<const char*>(palette.data()), palette.size() * sizeof(uint32_t));

	file.close();

	return 0;
}
*/

#pragma pack(push, 1)
struct VoxHeader {
	char id[4];
	int32_t version;
};

struct ChunkHeader {
	char id[4];
	int32_t contentSize;
	int32_t childrenSize;
};

struct SizeChunk {
	int32_t x;
	int32_t y;
	int32_t z;
};

struct Voxel3 {
	uint8_t x;
	uint8_t y;
	uint8_t z;
	uint8_t i;
};

struct Voxel4 {
	uint8_t r;
	uint8_t g;
	uint8_t b;
};
#pragma pack(pop)

const ogt_vox_scene* load_vox_scene(const char* filename, uint32_t scene_read_flags = 0)
{
	// open the file
#if defined(_MSC_VER) && _MSC_VER >= 1400
	FILE* fp;
	if (0 != fopen_s(&fp, filename, "rb"))
		fp = 0;
#else
	FILE* fp = fopen(filename, "rb");
#endif
	if (!fp)
		return NULL;

	// get the buffer size which matches the size of the file
	fseek(fp, 0, SEEK_END);
	uint32_t buffer_size = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	// load the file into a memory buffer
	uint8_t* buffer = new uint8_t[buffer_size];
	fread(buffer, buffer_size, 1, fp);
	fclose(fp);

	// construct the scene from the buffer
	const ogt_vox_scene* scene = ogt_vox_read_scene_with_flags(buffer, buffer_size, scene_read_flags);

	// the buffer can be safely deleted once the scene is instantiated.
	delete[] buffer;

	return scene;
}

void  toUpperCase(std::string& str)
{
	std::transform(str.begin(), str.end(), str.begin(), ::toupper);
}

std::string rgbToHexString(int r, int g, int b)
{
	std::stringstream stream;
	stream << std::setfill('0') << std::setw(2) << std::hex << r
		<< std::setw(2) << std::hex << g << std::setw(2) << std::hex << b;
	string output = stream.str();
	toUpperCase(output);
	return output;
}

std::string rgbaToHexString(int r, int g, int b, int a)
{
	std::stringstream stream;
	stream << "#" << std::setfill('0') << std::setw(2) << std::hex << r
		<< std::setw(2) << std::hex << g << std::setw(2) << std::hex << b
		<< std::setw(2) << std::hex << a;
	string output = stream.str();
	toUpperCase(output);
	return output;
}

void SaveVox6(Voxel* vArray)
{
/*
# Goxel 0.10.8
# One line per voxel
# X Y Z RRGGBB
-16 -16 0 ffffff
*/
	std::ofstream file("cube.text", std::ios::binary);

	file << "# Goxel 0.10.8" << endl;
	file << "# One line per voxel" << endl;
	file << "# X Y Z RRGGBB" << endl;

	for (int x = 0; x < maxWidthZ; x++)
		for (int y = 0; y < maxWidthZ; y++)
			for (int z = 0; z < maxHeightZ; z++)
			{
				if (vArray[x * maxWHZ + y * maxHeightZ + z].color.a > 64)
				{
					file << x << " " << y << " " << z << " ";
					file << rgbToHexString(vArray[x * maxWHZ + y * maxHeightZ + z].color.r,
						vArray[x * maxWHZ + y * maxHeightZ + z].color.g,
						vArray[x * maxWHZ + y * maxHeightZ + z].color.b) << endl;
				}
			}

	file.close();
}

void SaveVox5(Voxel* vArray)
{
	const int32_t sizex = maxWidthZ;
	const int32_t sizey = maxHeightZ;
	const int32_t sizez = maxWidthZ;
	std::vector<Voxel4> voxels;

	for (uint8_t x = 0; x < maxWidthZ; ++x) {
		for (uint8_t z = 0; z < maxHeightZ; ++z) {
			for (uint8_t y = 0; y < maxWidthZ; ++y) {
				uint8_t r = vArray[x * maxWHZ + y * maxHeightZ + z].color.r;
				uint8_t g = vArray[x * maxWHZ + y * maxHeightZ + z].color.g;
				uint8_t b = vArray[x * maxWHZ + y * maxHeightZ + z].color.b;
				uint8_t a = vArray[x * maxWHZ + y * maxHeightZ + z].color.a;
				if ((r == 0) && (g == 0) && (b = 0))
				{
					r = 1;
					g = 1;
					b = 1;
				}
				if (a == 0)
				{
					r = 0;
					g = 0;
					b = 0;
				}
				voxels.push_back({ r, g, b});
			}
		}
	}

	SizeChunk dimensions = { sizex, sizey, sizez };

	std::ofstream file("cube.cub", std::ios::binary);

	file.write(reinterpret_cast<const char*>(&dimensions), sizeof(dimensions));
	file.write(reinterpret_cast<const char*>(voxels.data()), voxels.size() * sizeof(Voxel4));

	file.close();
}

void SaveVox4(Voxel* vArray)
{
	std::ofstream file("cube.csv", std::ios::binary);

	file << maxWidthZ;
	file << ",";
	file << maxWidthZ;
	file << ",";
	file << maxHeightZ;
	file << endl;

	//file.write(reinterpret_cast<const char*>(&maxWidthZ), sizeof(header));

	for (int y = 0; y < maxWidthZ; y++)
	{
		for (int z = 0; z < maxHeightZ; z++)
		{
		    for (int x = 0; x < maxWidthZ; x++)
			{
				file << rgbaToHexString(
					vArray[x * maxWHZ + y * maxHeightZ + z].color.r,
					vArray[x * maxWHZ + y * maxHeightZ + z].color.g,
					vArray[x * maxWHZ + y * maxHeightZ + z].color.b,
					vArray[x * maxWHZ + y * maxHeightZ + z].color.a);
				if (x < maxWidthZ - 1)
					file << ",";
			}
			file << endl;
		}
		file << endl;
	}

	file.close();
}

void SaveVox3(Voxel* vArray)
{
	const int32_t sizex = maxWidthZ;
	const int32_t sizey = maxHeightZ;
	const int32_t sizez = maxWidthZ;
	std::vector<Voxel3> voxels;

	for (uint8_t x = 0; x < maxWidthZ; ++x) {
		for (uint8_t z = 0; z < maxHeightZ; ++z) {
			for (uint8_t y = 0; y < maxWidthZ; ++y) {
				voxels.push_back({ x, y, z, 1/*vArray[x * maxWHZ + y * maxHeightZ + z].color.r*/});
			}
		}
	}

	std::vector<uint32_t> palette = { 0x00FFFFFF, 0xFF0000FF, 0x00FF00FF, 0x0000FFFF,
									  0xFFFF00FF, 0xFF00FFFF, 0x00FFFFFF, 0xFFFFFFFF };

	VoxHeader header = { 'V', 'O', 'X', ' ', 150 };
	ChunkHeader mainChunk = { 'M', 'A', 'I', 'N', 0, 0x58a/* 40 + (int32_t)voxels.size() * 4*/ };
	ChunkHeader sizeChunk = { 'S', 'I', 'Z', 'E', 12, 0 };
	SizeChunk dimensions = { sizex, sizey, sizez };
	ChunkHeader xyzChunk = { 'X', 'Y', 'Z', 'I', (int32_t)voxels.size() * 4, 0 };
	int32_t nextSize = (int32_t)(voxels.size() - 1);
	ChunkHeader rgbaChunk = { 'R', 'G', 'B', 'A', 256 * 4, 0 };

	std::ofstream file("cube.vox", std::ios::binary);

	file.write(reinterpret_cast<const char*>(&header), sizeof(header));
	file.write(reinterpret_cast<const char*>(&mainChunk), sizeof(mainChunk));
	file.write(reinterpret_cast<const char*>(&sizeChunk), sizeof(sizeChunk));
	file.write(reinterpret_cast<const char*>(&dimensions), sizeof(dimensions));
	file.write(reinterpret_cast<const char*>(&xyzChunk), sizeof(xyzChunk));
	file.write(reinterpret_cast<const char*>(&nextSize), sizeof(int32_t));
	file.write(reinterpret_cast<const char*>(voxels.data()), voxels.size() * sizeof(Voxel3));


	file.write(reinterpret_cast<const char*>(&rgbaChunk), sizeof(rgbaChunk));
	file.write(reinterpret_cast<const char*>(palette.data()), palette.size() * sizeof(uint32_t));

	file.close();

	uint32_t scene_read_flags = 0;
	const ogt_vox_scene* testsc = load_vox_scene("cube.vox", scene_read_flags);
	save_vox_scene("cube2.vox", testsc);
}

void SaveVox2(Voxel* vArray)
{
	ogt_vox_scene vox_scene;
	ogt_vox_palette palette;
	ogt_vox_model scmodel;
	ogt_vox_model* scmodelar[1];
	//vox_scene->palette = palette;
	scmodel.size_x = 5;
	scmodel.size_y = 5;
	scmodel.size_z = 5;
	scmodel.voxel_data = (uint8_t*)malloc(5*5*5*4);
	scmodelar[0] = &scmodel;
	vox_scene.models = (const ogt_vox_model**) & scmodelar[0];
	vox_scene.num_groups = 1;
	save_vox_scene("merged.vox", &vox_scene);
	ogt_vox_destroy_scene(&vox_scene);
}

void SaveVox(Voxel* vArray) {
		using namespace std;

		Voxel2* voxels =(Voxel2*)malloc(sizeof(Voxel2)*maxWidthZ * maxWidthZ * maxHeightZ);
		int index = 0;
		for (int x = 0; x < maxWidthZ; x++)
			for (int y = 0; y < maxWidthZ; y++)
				for (int z = 0; z < maxHeightZ; z++)
		{
			voxels[index].r = vArray[x * maxWHZ + y * maxHeightZ + z].color.r;  // nastavení barvy voxelu na červenou
			voxels[index].g = vArray[x * maxWHZ + y * maxHeightZ + z].color.g;
			voxels[index].b = vArray[x * maxWHZ + y * maxHeightZ + z].color.b;
			voxels[index].a = vArray[x * maxWHZ + y * maxHeightZ + z].color.a;
			index++;
		}

		// Otevření souboru pro zápis binárních dat
		ofstream outfile("cube.vox", ios::binary);

		// Hlavička souboru
		outfile.write("VOX ", 4);  // magic number
		uint32_t version = 0x58e4;  // verze formátu VOX
		outfile.write(reinterpret_cast<char*>(&version), 4);

		// Hlavička modelu
		outfile.write("MAIN", 4);
		uint32_t main_chunk_size = 0;// 4 + 4 + 4 + 4 + 4;  // velikost hlavičky modelu
		outfile.write(reinterpret_cast<char*>(&main_chunk_size), 4);
		outfile.write(reinterpret_cast<char*>(&version), 4);  // verze formátu VOX
		outfile.write("SIZE", 4);
		//outfile.write(reinterpret_cast<char*>(&version), 4);  // verze formátu VOX
		uint64_t num_size = 12;
		outfile.write(reinterpret_cast<char*>(&num_size), 8);
		uint32_t x = maxWidthZ;
		outfile.write(reinterpret_cast<char*>(&x), 4);
		uint32_t y = maxWidthZ;
		outfile.write(reinterpret_cast<char*>(&y), 4);
		uint32_t z = maxHeightZ;
		outfile.write(reinterpret_cast<char*>(&z), 4);
		outfile.write("XYZI", 4);
		uint64_t num_voxelss = maxWidthZ * maxWidthZ * maxHeightZ *4 ;  // počet voxelů v modelu
		outfile.write(reinterpret_cast<char*>(&num_voxelss), 8);
		uint32_t num_voxels = (maxWidthZ * maxWidthZ * maxHeightZ)-1;  // počet voxelů v modelu
		outfile.write(reinterpret_cast<char*>(&num_voxels), 4);

		int indexc = 0;
		for (uint32_t x = 0; x < maxWidthZ; x++)
			for (uint32_t y = 0; y < maxWidthZ; y++)
				for (uint32_t z = 0; z < maxHeightZ; z++)
				{
					outfile.write(reinterpret_cast<char*>(&x), 1);
					outfile.write(reinterpret_cast<char*>(&y), 1);
					outfile.write(reinterpret_cast<char*>(&z), 1);
					outfile.write(reinterpret_cast<char*>(&voxels[indexc].r), 1);
					//outfile.write(reinterpret_cast<char*>(&voxels[index].g), 1);
					//outfile.write(reinterpret_cast<char*>(&voxels[index].b), 1);
					//outfile.write(reinterpret_cast<char*>(&voxels[i].a), 1);
					indexc++;
				}
		/*for (int i = 0; i < num_voxels; i++) {
			//uint8_t x = maxWidthZ;
			//uint8_t y = maxWidthZ;
			//uint8_t z = maxHeightZ;
			outfile.write(reinterpret_cast<char*>(&voxels[i].r), 1);
			outfile.write(reinterpret_cast<char*>(&voxels[i].g), 1);
			outfile.write(reinterpret_cast<char*>(&voxels[i].b), 1);
			outfile.write(reinterpret_cast<char*>(&voxels[i].a), 1);
		}*/
		/*
		// Chunk s daty voxelů
		outfile.write("RGBA", 4);
		uint32_t rgba_chunk_size = 4 + 4 + 4 * num_voxels;  // velikost chunku s daty voxelů
		outfile.write(reinterpret_cast<char*>(&rgba_chunk_size), 4);
		uint32_t num_voxels_32 = static_cast<uint32_t>(num_voxels);
		outfile.write(reinterpret_cast<char*>(&num_voxels_32), 4);  // počet voxelů v chunku
		for (int i = 0; i < num_voxels; i++) {
			//uint8_t x = maxWidthZ;
			//uint8_t y = maxWidthZ;
			//uint8_t z = maxHeightZ;
			outfile.write(reinterpret_cast<char*>(&voxels[i].r), 1);
			outfile.write(reinterpret_cast<char*>(&voxels[i].g), 1);
			outfile.write(reinterpret_cast<char*>(&voxels[i].b), 1);
			outfile.write(reinterpret_cast<char*>(&voxels[i].a), 1);
		}*/

		free(voxels);
		// Uzavření souboru
		outfile.close();
}

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
			for (uint32_t x = 0; x < actimg.width; x++)
				for (uint32_t y = 0; y < actimg.height; y++)
				{
					if ((actimg.buffer[y * actimg.width + x].r == 0x80) &&
						(actimg.buffer[y * actimg.width + x].g == 0x80) &&
						(actimg.buffer[y * actimg.width + x].b == 0x80))
					{
						actimg.buffer[y * actimg.width + x].r = 0;
						actimg.buffer[y * actimg.width + x].g = 0;
						actimg.buffer[y * actimg.width + x].b = 0;
						actimg.buffer[y * actimg.width + x].a = 0;
					}
				}
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
	Voxel* tempArray = (Voxel*)malloc(sizeof(Voxel) * maxWidthZ * maxWidthZ * maxHeightZ);

	//vector<TColor>* canColorArray = (vector<TColor>*)malloc(sizeof(TColor) * maxWidthZ * maxWidthZ * maxHeightZ);
	vector<TColor>** canColorArray = new vector<TColor>*[sizeof(TColor) * maxWidthZ * maxWidthZ * maxHeightZ];
	const int maxColors = 100;
	for (uint32_t x = 0; x < maxWidthZ; x++)
		for (uint32_t y = 0; y < maxWidthZ; y++)
			for (uint32_t z = 0; z < maxHeightZ; z++)
				canColorArray[x * maxWHZ + y * maxHeightZ + z] = new vector<TColor>[maxColors];
				//canColorArray[x * maxWHZ + y * maxHeightZ + z] = new vector<TColor>[10];

	AddCanColor(canColorArray, images[0], 0, true);
	AddCanColor(canColorArray, images[1], -90, true);
	AddTranspColor(canColorArray);

	//SetRandom(vArray);
	SetTransparent(vArray);
	//SetImage(vArray, images[0],0);
	SaveImage(vArray, 0,(char*)"test00.png");


	long diff=0;
	long bestdiff = 10000000000000000;

	int addStep = 30;
	//int addStep2 = 20;
	//int oldStep = 100;
	//long oldScoreDiff = 0;
	long oldScore = 0;
	long actDiff = 0;
	int countSame = 0;

	for (long step = 0; step < 1000000; step++)
	{
		diff = 0;
		CopyArray(vArray, tempArray);
		for (long step2 = 0; step2 < addStep; step2++)
			RandomMutage(vArray, canColorArray);
		diff += Compare(vArray, images[0], 0, true);
		diff += Compare(vArray, images[1], -90, true);
		if (diff <= bestdiff)
		{
			bestdiff = diff;
		}
		else
		{
			CopyArray(tempArray, vArray);
		}

		actDiff = oldScore - bestdiff;
		oldScore = bestdiff;
		if (actDiff==0)countSame++;
		if (countSame > 2000)
		{
			countSame = 0;
			if (addStep > 1)addStep--;
		}
		if (step % 1000 == 0)
		{
			/*if (oldScoreDiff > actDiff)
				dir *= -1;
			if ((oldScoreDiff == actDiff)&&(dir==1))
				dir = -1;
			oldScoreDiff = actDiff;
			actDiff= oldScore - bestdiff;
			oldScore = bestdiff;
			addStep += dir;
			if (addStep < 1)addStep = 1;*/
			SaveImage(vArray, 0, (char*)"test00.png");
			SaveImage(vArray, -90, (char*)"test90.png");
			//SaveVox6(tempArray);
		}
		if (step % 5000 == 0)
			SaveVox6(tempArray);
		if (step % 100 == 0)
			cout << bestdiff << " " << actDiff << " " << addStep << endl;
	}

	//clearing
	for (uint32_t x = 0; x < maxWidthZ; x++)
		for (uint32_t y = 0; y < maxWidthZ; y++)
			for (uint32_t z = 0; z < maxHeightZ; z++)
				delete(canColorArray[x * maxWHZ + y * maxHeightZ + z]);
	//free(canColorArray);
	free(vArray);
	free(tempArray);
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
