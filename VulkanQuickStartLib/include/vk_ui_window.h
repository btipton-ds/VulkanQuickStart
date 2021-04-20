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

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <vk_forwardDeclarations.h>
#include <vk_ui_button.h>
#include <vk_pipelineUboGroup.h>

struct GLFWwindow;

#pragma warning (push)
#pragma warning( disable : 4251 )

namespace VK {

	namespace UI {

		class EXPORT_VQS Window {
		public:
			using UboType = UniformBufferObjectUi;
			using PipelineGroupType = PipelineUboGroup<UboType>;
			using PipelineGroupTypePtr = PipelineUboGroupPtr<UboType>;
			using PipelinePtr = PipelineGroupType::PipelinePtr;

			Window(const VulkanAppPtr& app);

			void setDefaultButtonSize(float width, float height);
			ButtonPtr addButton(const glm::vec4& bkgColor, const std::string& label, const Rect& frame);

			size_t getChangeNumber() const;

			int getPixelDPI() const;
			const PipelineGroupTypePtr& getPipelines() const;

			void updateUniformBuffer(size_t swapChainImageIndex);

		private:

			void init();

			static glm::dvec2 scaleCursorPos(GLFWwindow* window, double xPos, double yPos);

			static void mouseButtonCB(GLFWwindow* window, int btnNum, int pressed, int modifiers);
			static void scrollCB(GLFWwindow* window, double dx, double dy);
			static void cursorPosCB(GLFWwindow* window, double xPos, double yPos);

			glm::ivec2 toUi(const glm::dvec2& pt) const;

			bool mouseButtonUi(const glm::ivec2& pt, int btnNum, bool pressed, int modifiers);
			bool mouseButton3D(const glm::dvec2& pt, int btnNum, bool pressed, int modifiers);

			bool mouseMovedUi(const glm::ivec2& pos, int btnNum);
			bool mouseMoved3D(const glm::dvec2& pos, int btnNum);

			void doMouseRotate(const glm::dvec2& startPos, const glm::dvec2& curPos);
			void startMouseRotate();
			void endMouseRotate();

			bool handleButtonClick(const glm::ivec2& pt, int btnNum, bool pressed, int modifiers);
			bool handleButtonMove(const glm::ivec2& pt, int btnNum);

			void scroll(GLFWwindow* window, const glm::dvec2& pos);

			void updateUbo();

			PipelineGroupTypePtr _pipelines;
			size_t _changeNumber = 0;
			glm::dvec2 _mouseStartPos[4];
			glm::mat4 _initialMatrix3D;
			VulkanAppPtr _app;
			GLFWwindow* _window;

			size_t _lastButtonDownIdx = stm1;
			size_t _hilightButtonIdx = stm1;
			std::vector<ButtonPtr> _buttons;
		};

		inline size_t Window::getChangeNumber() const {
			return _changeNumber;
		}

		inline const typename Window::PipelineGroupTypePtr& Window::getPipelines() const {
			return _pipelines;
		}
	}
}

#pragma warning (pop)
