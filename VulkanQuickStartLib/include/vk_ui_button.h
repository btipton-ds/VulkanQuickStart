#pragma once

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

#include <memory>
#include <functional>
#include <map>
#include <vector>

#include <vk_forwardDeclarations.h>
#include <vk_buffer.h>
#include <vk_sceneNodeUi.h>
#include <vk_pipelineUi.h>

#pragma warning (push)
#pragma warning( disable : 4251 )

namespace VK {
	class VulkanApp;
	using VulkanAppPtr = std::shared_ptr<VulkanApp>;

	namespace UI {

		struct Rect {
			Rect() = default;
			Rect(const Rect& src) = default;
			Rect(uint32_t bottom, uint32_t left, uint32_t top, uint32_t right);
			int _bottom = 0, _left = 0, _top = 0, _right = 0;
		};

		struct Accel {
			Accel() = default;
			char _keyCode = 0;
			int _modifers = 0;
		};

		class EXPORT_VQS Button : public SceneNodeUi {
		public:
			using VertexType = PipelineUi::VertexType;
			enum ActionType {
				ACT_ENTER,
				ACT_EXIT,
				ACT_CLICK,
				ACT_DOUBLE_CLICK,
				ACT_UNKNOWN // Must be the last one
			};

			// Currently using one, over featured, generic callback
			using FuncMouse = std::function<void(int btnNum, int modifiers)>;

			Button(const VulkanAppPtr& app);
			Button(const VulkanAppPtr& app, const glm::vec4& backgroundColor, const std::string& label, const Rect& rect, const Accel& accel = Accel());

			const glm::vec4& getFontColor() const;
			const glm::vec4& getBackgroundColor() const;

			void createBuffers();
			void addCommands(VkCommandBuffer cmdBuff) const override;

			void buildImageInfoList(std::vector<VkDescriptorImageInfo>& imageInfoList) const override;

			void setAction(ActionType action, const FuncMouse& f);

			bool isPointInside(const glm::ivec2& pt) const;
			void handleMouseClick(int btnNum, int modifiers);
			void handleMouseEnter();
			void handleMouseExit();

		private:
			void init();
			void createGeometryBuffers();
			void createTexture();
			void createImage(size_t& width, size_t& height, std::vector<uint32_t>& image);
			void createImageBackGround(size_t& width, size_t& height, int& frameWidth, std::vector<uint32_t>& image);
			void renderLabel(size_t width, size_t height, int frameWidth, std::vector<uint32_t>& image);

			Rect _rect;
			float _fontSizePoints = 14;
			std::string _label;
			std::string _fontPath;
			std::string _fontName;
			Accel _accel;
			glm::vec4 _backgroundColor = glm::vec4(0.75f, 0.75f, 0.75f, 1);
			glm::vec4 _fontColor = glm::vec4(0, 0, 0.25f, 1);
			std::map<ActionType, FuncMouse> _mouseFuncs;

			TextureImagePtr _texture;
			Buffer _vertexBuffer, _indexBuffer;
		};

		using ButtonPtr = std::shared_ptr<Button>;

		inline Rect::Rect(uint32_t bottom, uint32_t left, uint32_t top, uint32_t right) 
			: _bottom(bottom)
			, _left(left)
			, _top(top)
			, _right(right)
		{}

		inline const glm::vec4& Button::getFontColor() const {
			return _fontColor;
		}

		inline const glm::vec4& Button::getBackgroundColor() const {
			return _backgroundColor;
		}

	}
}

#pragma warning (pop)
