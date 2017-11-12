#include "ResourceMaterial.h"
#include "Serializer.h"

// Constructors =================================
ResourceMaterial::ResourceMaterial()
{

}

// Destructors ==================================
ResourceMaterial::~ResourceMaterial()
{

}

// Set Methods ==================================
void ResourceMaterial::SetWidth(uint val)
{
	width = val;
}

void ResourceMaterial::SetHeight(uint val)
{
	height = val;
}

void ResourceMaterial::SetDepth(uint val)
{
	depth = val;
}

void ResourceMaterial::SetBytesPerPixel(uint val)
{
	bytes_per_pixel = val;
}

void ResourceMaterial::SetNumMipMaps(uint val)
{
	num_mip_maps = val;
}

void ResourceMaterial::SetNumLayers(uint val)
{
	num_layers = val;
}

void ResourceMaterial::SetBytes(uint val)
{
	bytes = val;
}

void ResourceMaterial::SetMaterialID(uint val)
{
	mat_id = val;
}

void ResourceMaterial::SetColorFormat(COLOR_FORMAT fmt)
{
	color_format = fmt;
}

// Functionality ================================
bool ResourceMaterial::Save(Serializer & file_root) const
{
	//Save all the material resource data
	file_root.InsertInt("width", width);
	file_root.InsertInt("height", height);
	file_root.InsertInt("depth", depth);
	file_root.InsertInt("bytes_per_pixel", bytes_per_pixel);
	file_root.InsertInt("num_mip_maps", num_mip_maps);
	file_root.InsertInt("num_layers", num_layers);
	file_root.InsertInt("bytes", bytes);
	file_root.InsertInt("mat_id", mat_id);
	file_root.InsertInt("height", height);
	file_root.InsertString("color_format", ColorFormatToStr(color_format));

	return true;
}

const char * ColorFormatToStr(COLOR_FORMAT fmt)
{
	switch (fmt)
	{
	case INDEX_COLOR:		return "index_color";
	case RGB_COLOR:			return "rgb_color";
	case RGBA_COLOR:		return "rgba_color";
	case BGR_COLOR:			return "bgr_color";
	case BGRA_COLOR:		return "bgra_color";
	case LUMINANCE_COLOR:	return "luminance_color";
	}
	return "invalid_color_format";
}

COLOR_FORMAT StrToColorFormat(const char * str)
{
	if (strcmp(str, "index_color") == 0)	return INDEX_COLOR;
	if (strcmp(str, "rgb_color") == 0)		return RGB_COLOR;
	if (strcmp(str, "rgba_color") == 0)		return RGBA_COLOR;
	if (strcmp(str, "bgr_color") == 0)		return BGR_COLOR;
	if (strcmp(str, "bgra_color") == 0)		return BGRA_COLOR;
	if (strcmp(str, "luminance_color") == 0)return LUMINANCE_COLOR;
	return COLOR_FORMAT::UNKNOWN_COLOR;
}