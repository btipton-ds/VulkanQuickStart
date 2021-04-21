/*

This file is part of the VulkanQuickStart Project.

	The VulkanQuickStart Project is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	The VulkanQuickStart Project is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	This link provides the exact terms of the GPL license <https://www.gnu.org/licenses/>.

	The author's interpretation of GPL 3 is that if you receive money for the use or distribution of the TriMesh Library or a derivative product, GPL 3 no longer applies.

	Under those circumstances, the author expects and may legally pursue a reasoble share of the income. To avoid the complexity of agreements and negotiation, the author makes
	no specific demands in this regard. Compensation of roughly 1% of net or $5 per user license seems appropriate, but is not legally binding.

	In lay terms, if you make a profit by using the VulkanQuickStart Project (violating the spirit of Open Source Software), I expect a reasonable share for my efforts.

	Robert R Tipton - Author

	Dark Sky Innovative Solutions http://darkskyinnovation.com/

*/

#include <vk_defines.h>

#include <array>
#include <vector>

#include <ft2build.h>
#include FT_FREETYPE_H

#include <vk_logger.h>
#include <vk_ui_button.h>
#include <vk_ui_window.h>
#include <vk_app.h>

namespace VK::UI {

	using namespace std;

	Button::Button(const VulkanAppPtr& app)
	: SceneNodeUi(app)
	, _vertexBuffer(app->getDeviceContext())
	, _indexBuffer(app->getDeviceContext())
	{
		init();
	}

	Button::Button(const VulkanAppPtr& app, const glm::vec4& backgroundColor, const std::string& label, const Rect& rect, const Accel& accel)
		: SceneNodeUi(app)
		, _rect(rect)
		, _label(label)
		, _accel(accel)
		, _backgroundColor(backgroundColor)
		, _vertexBuffer(app->getDeviceContext())
		, _indexBuffer(app->getDeviceContext())
	{
		init();
	}

	namespace {
		template<typename T>
		T powerOf2(T val) {
			T max = 1024;
			while (max > 8) {
				if (val == max)
					return max;
				if (val > max)
					return 2 * max;
				max /= 2;
			}
		}
	}

	namespace {
		uint32_t fromVec4(const glm::vec4& color) {
			unsigned char r = ((unsigned char)(color[0] * 255 + 0.5));
			unsigned char g = ((unsigned char)(color[1] * 255 + 0.5));
			unsigned char b = ((unsigned char)(color[2] * 255 + 0.5));
			unsigned char a = ((unsigned char)(color[3] * 255 + 0.5));
			uint32_t result = a << 24 | b << 16 | g << 8 | r;
			return result;
		}

		glm::vec4 toVec4(uint32_t val) {
			glm::vec4 result;
			result[0] = (val & 0xff) / 255.0f;
			result[1] = ((val >> 8) & 0xff) / 255.0f;
			result[2] = ((val >> 16) & 0xff) / 255.0f;
			result[3] = ((val >> 24) & 0xff) / 255.0f;
			return result;
		}
	}
	void Button::createBuffers() {
		createGeometryBuffers();
		createTexture();
	}

	void Button::createGeometryBuffers() {
		using namespace glm;
		vector<VertexType> vertices;
		vector<uint32_t> indices;

		vertices.push_back(VertexType(vec2(_rect._left, _rect._bottom), vec2(0, 0)));
		vertices.push_back(VertexType(vec2(_rect._right, _rect._bottom), vec2(1, 0)));
		vertices.push_back(VertexType(vec2(_rect._right, _rect._top), vec2(1, 1)));
		vertices.push_back(VertexType(vec2(_rect._left, _rect._top), vec2(0, 1)));

		indices.push_back(0);
		indices.push_back(1);
		indices.push_back(2);

		indices.push_back(0);
		indices.push_back(2);
		indices.push_back(3);

		_vertexBuffer.create(vertices, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		_indexBuffer.create(indices, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	}

#define IMG_SCALE 4

	void Button::createTexture() {
		vector<uint32_t> image;
		size_t w, h;

		createImage(w, h, image);

		_texture = TextureImage::create(getApp()->getDeviceContext(), w, h, (unsigned char*)image.data());
	}

	void Button::createImage(size_t& width, size_t& height, vector<uint32_t>& image) {
		int frameWidth;
		createImageBackGround(width, height, frameWidth, image);
		renderLabel(width, height, frameWidth, image);
	}

	void Button::createImageBackGround(size_t& width, size_t& height, int& frameWidth, vector<uint32_t>& image) {
		width = _rect._right - _rect._left;
		height = _rect._top - _rect._bottom;

		width *= IMG_SCALE;
		height *= IMG_SCALE;

		glm::vec4 shadow(.25f, .25f, .25f, _backgroundColor[3]);
		glm::vec4 hiLight(1, 1, 1, _backgroundColor[3]);

		image.resize(width * height, fromVec4(_backgroundColor));
		uint32_t* pixels = image.data();
		frameWidth = 2 * IMG_SCALE;
		for (size_t i = 0; i < height; i++) {
			uint32_t c = fromVec4(_backgroundColor);
			if (i < frameWidth) {
				c = fromVec4(hiLight);
			}
			else if (i >= height - frameWidth) {
				c = fromVec4(shadow);
			}
			for (size_t j = 0; j < width; j++) {
				pixels[width * i + j] = c;
			}
		}
	}

	namespace {
		void drawGlyphBitmap(Button& btn, FT_Bitmap& bitMap, FT_UInt x0, FT_UInt y0,
			size_t imageWidth, size_t imageHeight, std::vector<uint32_t>& image) {

			auto fontColor = btn.getFontColor();
			for (unsigned int ly = 0; ly < bitMap.rows; ly++) {
				FT_UInt y = y0 + ly;
				for (unsigned int lx = 0; lx < bitMap.width; lx++) {
					FT_UInt x = x0 + lx;
					size_t imageIdx = y * imageWidth + x;
					if (imageIdx < image.size()) {
						float grayScale = bitMap.buffer[bitMap.width * ly + lx] / 255.0f;
						glm::vec4 current = toVec4(image[y * imageWidth + x]);
						glm::vec4 val = grayScale * fontColor + (1 - grayScale) * current;
						val[3] = current[3]; // use source alpha
						image[y * imageWidth + x] = fromVec4(val);
					}
				}
			}

		}
	}
	void Button::renderLabel( size_t width, size_t height, int frameWidth, std::vector<uint32_t>& image) {
		string fontFilename = _fontPath + _fontName;
		FT_Library ftLib;
		FT_Error err = FT_Init_FreeType(&ftLib);
		if (err)
			THROW("Failed to initialize FreeType.");

		FT_Face face;
		err = FT_New_Face(ftLib, fontFilename.c_str(), 0, &face);
		if (err == FT_Err_Unknown_File_Format)
			THROW("Unknown font format.")
		else if (err)
			THROW("Failed to read font file.");

		unsigned int dpi = getApp()->getUiWindow()->getPixelDPI();

		err = FT_Set_Char_Size(face, 0, (FT_F26Dot6)(_fontSizePoints * 64 * IMG_SCALE + 0.5), dpi, dpi);
		if (err)
			THROW("Failed to set char size for font.");

		FT_Int pen_x = 0;
		FT_Int pen_y = (FT_Int)height - frameWidth - (1 * IMG_SCALE) - (-face->descender >> 6);
		for (auto iter = _label.begin(); iter != _label.end(); iter++) {
			FT_UInt glyph_index = FT_Get_Char_Index(face, *iter);

			auto& slot = face->glyph;
			err = FT_Load_Glyph(face, glyph_index, FT_LOAD_DEFAULT);
			if (err)
				THROW("Failed load glyph.");
			err = FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);
			if (err)
				THROW("Failed render glyph.");

			drawGlyphBitmap(*this, slot->bitmap,
				pen_x + slot->bitmap_left,
				pen_y - slot->bitmap_top, width, height, image);

			/* increment pen position */
			pen_x += slot->advance.x >> 6;
			pen_y += slot->advance.y >> 6; /* not useful for now */
		}
	}

	void Button::init() {
#ifdef _WIN32
        _fontPath = "/Windows/Fonts/";
        _fontName = "arial.ttf";
#else
                _fontPath = "/usr/share/fonts/opentype/noto/"; //"/Windows/Fonts/";
                _fontName = "NotoSansCJK-Regular.ttc"; //"arial.ttf";
#endif

		// Make sure all button are set so we don't get segfault/nullptr exceptions.
		// TODO. This probably isn't required, according to something I read. Test and remove in possible.
		auto nullFunc = [](int btnNum, int modifiers) {};
		for (int i = 0; i < (int)ACT_UNKNOWN; i++) {
			_mouseFuncs[(ActionType) i] = nullFunc;
		}
	}

	void Button::addCommands(VkCommandBuffer cmdBuff) const {
		VkBuffer vertexBuffers[] = { _vertexBuffer };
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(cmdBuff, 0, 1, vertexBuffers, offsets);

		vkCmdBindIndexBuffer(cmdBuff, _indexBuffer, 0, VK_INDEX_TYPE_UINT32);

//		vkCmdBindDescriptorSets(cmdBuff, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &_descriptorSets[swapChainIndex], 0, nullptr);

		vkCmdDrawIndexed(cmdBuff, 6, 1, 0, 0, 0);
	}

	void Button::buildImageInfoList(std::vector<VkDescriptorImageInfo>& imageInfoList) const {
		imageInfoList.clear();
		VkDescriptorImageInfo imageInfo = {};
		imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		imageInfo.imageView = _texture->getImageView();
		imageInfo.sampler = _texture->getSampler();
		imageInfoList.push_back(imageInfo);
	}
	
	void Button::setAction(ActionType action, const FuncMouse& f) {
		_mouseFuncs[action] = f;
	}

	bool Button::isPointInside(const glm::ivec2& pt) const {
		if (pt.x < _rect._left || _rect._right < pt.x)
			return false;
		
		if (pt.y < _rect._bottom || _rect._top < pt.y)
			return false;

		return true;
	}

	void Button::handleMouseClick(int btnNum, int modifiers) {
		_mouseFuncs[ACT_CLICK](btnNum, modifiers);
	}

	void Button::handleMouseEnter() {
		_mouseFuncs[ACT_ENTER](-1, 0);
	}

	void Button::handleMouseExit() {
		_mouseFuncs[ACT_EXIT](-1, 0);
	}


}
