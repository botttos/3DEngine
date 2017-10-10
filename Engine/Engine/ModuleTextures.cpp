#include "ModuleTextures.h"
#include "Glew/include/glew.h"

#include "Devil/include/il.h"
#include "Devil/include/ilu.h"
#include "Devil/include/ilut.h"

#pragma comment (lib, "Engine/Devil/libx86/DevIL.lib")
#pragma comment (lib, "Engine/Devil/libx86/ILU.lib")
#pragma comment (lib, "Engine/Devil/libx86/ILUT.lib")


// Constructors =================================
ModuleTextures::ModuleTextures(const char * _name, MODULE_ID _id, bool _config_menu, bool _enabled) :Module(_name, _id, _enabled)
{

}

// Destructors ==================================
ModuleTextures::~ModuleTextures()
{

}

bool ModuleTextures::Awake(const JSON_Object * data_root)
{
	//  ----- Initialize DevIL -----
	ilutRenderer(ILUT_OPENGL);
	ilInit();
	iluInit();
	ilutInit();
	ilutRenderer(ILUT_OPENGL);
	LOG("DevIL lib initialized!");

	return true;
}

// Game Loop ====================================
bool ModuleTextures::Start()
{
	for (int i = 0; i < CHECKERS_HEIGHT; i++) 
	{
		for (int j = 0; j < CHECKERS_WIDTH; j++) 
		{
			int c = ((((i & 0x8) == 0) ^ (((j & 0x8)) == 0))) * 255;
			checkImage[i][j][0] = (GLubyte)c;
			checkImage[i][j][1] = (GLubyte)c;
			checkImage[i][j][2] = (GLubyte)c;
			checkImage[i][j][3] = (GLubyte)255;
		}
	}

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &check_image);
	glBindTexture(GL_TEXTURE_2D, check_image);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, CHECKERS_WIDTH, CHECKERS_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, checkImage);
	glEnable(GL_TEXTURE_2D);

	//Load lenna image
	lenna_porn = LoadTexture("texturacaja.jpg");

	return true;
}

// Functionality ================================
uint ModuleTextures::LoadTexture(const char * str)
{
	std::string filename = std::string(str);
	filename = "Assets/" + filename;

	//Uint where the texture id will be saved
	ILuint textureID = 0;
	
	//Generate gl texture
	glGenTextures(1, &textureID);
	
	//Focus the generated texture
	glBindTexture(GL_TEXTURE_2D, textureID);

	//Load the image
	ILboolean success = ilLoadImage(filename.c_str());
	
	//If the image is correctly loaded
	if (success)
	{
		/*ILinfo ImageInfo;
		iluGetImageInfo(&ImageInfo);
		
		if (ImageInfo.Origin == IL_ORIGIN_UPPER_LEFT)
		{
			iluFlipImage();
		}*/

		success = ilConvertImage(ilGetInteger(IL_IMAGE_FORMAT), IL_UNSIGNED_BYTE);

		// Quit out if we failed the conversion
		if (!success)
		{
			ILenum error = ilGetError();
			LOG("[error] Loading texture from: %s", filename.c_str());
			LOG("[error] %s", iluErrorString(error));
			ilDeleteImages(1, &textureID);
			return NULL;
		}

		//Set the texture parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		
		//Generate the image with the loaded data
		glTexImage2D(GL_TEXTURE_2D, 		// Type of texture
			0,								// Pyramid level (for mip-mapping) - 0 is the top level
			ilGetInteger(IL_IMAGE_FORMAT),	// Internal pixel format to use. Can be a generic type like GL_RGB or GL_RGBA, or a sized type
			ilGetInteger(IL_IMAGE_WIDTH),	// Image width
			ilGetInteger(IL_IMAGE_HEIGHT),	// Image height
			0,								// Border width in pixels (can either be 1 or 0)
			ilGetInteger(IL_IMAGE_FORMAT),	// Format of image pixel data
			GL_UNSIGNED_BYTE,				// Image data type
			ilGetData());					// The actual image data itself

		glGenerateMipmap(GL_TEXTURE_2D);


		LOG("Texture: %s correctly loaded!", filename.c_str());
	}
	else
	{
		ILenum error = ilGetError();
		LOG("[error] Loading texture from: %s", filename.c_str());
		LOG("[error] %s", iluErrorString(error));
		ilDeleteImages(1, &textureID);
		return NULL;
	}
	
	//Delete the image 
	ilDeleteImages(1, &textureID);

	return textureID;
}