#include "font.h"

#define STB_RECT_PACK_IMPLEMENTATION
#define STB_TRUETYPE_IMPLEMENTATION
#include "../libraries/stb_rect_pack.h"
#include "../libraries/stb_truetype.h"

#include <stdio.h>

namespace sk {

///////////////////////////////////////////

font_t font_find(const char *id) {
	font_t result = (font_t)assets_find(id, asset_type_font);
	if (result != nullptr) {
		assets_addref(result->header);
		return result;
	}
	return nullptr;
}

///////////////////////////////////////////

font_t font_create(const char *file) {
	font_t result = font_find(file);
	if (result != nullptr)
		return result;
	result = (font_t)assets_allocate(asset_type_font);
	assets_set_id(result->header, file);

	FILE *fp;
	if (fopen_s(&fp, assets_file(file), "rb") != 0 || fp == nullptr)
		return nullptr;

	// Get length of file
	fseek(fp, 0L, SEEK_END);
	size_t length = ftell(fp);
	rewind(fp);

	// Read the data
	unsigned char *data = (unsigned char *)malloc(sizeof(unsigned char) *length);
	if (data == nullptr) { fclose(fp); return nullptr; }
	fread(data, 1, length, fp);
	fclose(fp);

	// Load and pack font data
	const int w = 512;
	const int h = 512;
	const float size = 64;
	const int start_char = 32;
	//stbtt_fontinfo font;
	uint8_t *bitmap;
	stbtt_pack_context pc;
	stbtt_packedchar chars[128];
	//stbtt_InitFont(&font, data, stbtt_GetFontOffsetForIndex(data,0));
	bitmap = (uint8_t*)malloc(sizeof(uint8_t) * w * h);
	stbtt_PackBegin(&pc, (unsigned char*)(bitmap), w, h, 0, 1, NULL);
	stbtt_PackFontRange(&pc, data, 0, size, start_char, 95, chars);
	stbtt_PackEnd(&pc);
	free(data);
	
	// convert characters
	float convert_w = 1.0f / w;
	float convert_h = 1.0f / h;
	result->character_height = 0;
	for (size_t i = 0; i < _countof(chars)-(start_char+1); i++) {
		result->characters[i+start_char] = {
			chars[i].xoff/size,  -chars[i].yoff/size,
			chars[i].xoff2/size, -chars[i].yoff2/size,
			chars[i].x0*convert_w, chars[i].y0*convert_h,
			chars[i].x1*convert_w, chars[i].y1*convert_h,
			chars[i].xadvance/size,
		};
	}
	result->character_height = fabsf(chars['T'].yoff/size);

	// Convert to color data
	color32 *colors = (color32*)malloc(w * h * sizeof(color32));
	for (size_t i = 0; i < w*h; i++) {
		colors[i] = color32{ bitmap[i], 0, 0, 0 };
	}
	result->font_tex = tex_create(tex_type_image);
	tex_set_colors(result->font_tex, w, h, colors);

	free(bitmap);
	free(colors);

	return result;
}

///////////////////////////////////////////

void font_set_id(font_t font, const char* id) {
	assets_set_id(font->header, id);
}

///////////////////////////////////////////

void font_release(font_t font) {
	if (font == nullptr)
		return;
	assets_releaseref(font->header);
}

///////////////////////////////////////////

void font_destroy(font_t font) {
	tex_release(font->font_tex);
	*font = {};
}

///////////////////////////////////////////

tex_t font_get_tex(font_t font) {
	return font->font_tex;
}

} // namespace sk