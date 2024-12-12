#include "ImageImporter.h"
#include <filesystem>
#include <cassert>
#include <IL/il.h>
#include <IL/ilu.h>
using namespace std;
namespace fs = std::filesystem;

static void initDevIL() {
	static bool ilInitialized = false;
	if (!ilInitialized) {
		ilInit();
		iluInit();
		ilInitialized = true;
	}
}

std::shared_ptr<Image> ImageImporter::loadFromFile(const std::string& path) {
	initDevIL();

	auto img = ilGenImage();
	ilBindImage(img);

	auto ps = fs::absolute(path);
	assert(fs::exists(ps));
	bool image_loaded = ilLoadImage((const wchar_t*)(ps.string().c_str()));
	assert(image_loaded);

	const auto origin = ilGetInteger(IL_IMAGE_ORIGIN);
	if (origin != IL_ORIGIN_UPPER_LEFT) iluFlipImage();

	const auto width = ilGetInteger(IL_IMAGE_WIDTH);
	const auto height = ilGetInteger(IL_IMAGE_HEIGHT);
	const auto channels = ilGetInteger(IL_IMAGE_CHANNELS);

	if (channels == 1) ilConvertImage(IL_LUMINANCE, IL_UNSIGNED_BYTE);
	else if (channels == 2) ilConvertImage(IL_LUMINANCE_ALPHA, IL_UNSIGNED_BYTE);
	else if (channels == 3) ilConvertImage(IL_RGB, IL_UNSIGNED_BYTE);
	else if (channels == 4)ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);
	const auto* data = ilGetData();

	auto image = make_shared<Image>();
	image->load(width, height, channels, data);

	ilDeleteImage(img);

	return image;
}


