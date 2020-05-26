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

#include <defines.h>

#include <memory>
#include <functional>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <vk_ui_button.h>

struct GLFWwindow;

namespace VK {

	class VulkanApp;
	using VulkanAppPtr = std::shared_ptr<class VulkanApp>;

	class PipelineUi;
	using PipelineUiPtr = std::shared_ptr<PipelineUi>;

	namespace UI {

		class Window;
		using WindowPtr = std::shared_ptr<Window>;

		class Window {
		public:
			Window(const VulkanAppPtr& app);

			void setDefaultButtonSize(float width, float height);
			void addButton(const Button& btn);

			size_t getChangeNumber() const;

			int getPixelDPI() const;

		private:

			void init();

			static glm::dvec2 scaleCursorPos(GLFWwindow* window, double xPos, double yPos);

			static void mouseButtonCB(GLFWwindow* window, int btnNum, int pressed, int modifiers);
			static void scrollCB(GLFWwindow* window, double dx, double dy);
			static void cursorPosCB(GLFWwindow* window, double xPos, double yPos);

			void mouseButton(GLFWwindow* window, int btnNum, bool pressed, int modifiers);

			void mouseMoved(GLFWwindow* window, const glm::dvec2& pos);
			void doMouseRotate(const glm::dvec2& startPos, const glm::dvec2& curPos);
			void startMouseRotate();
			void endMouseRotate();

			bool handleButtonClick(double xPos, double yPos, int btnNum, bool pressed, int modifiers);
			bool handleButtonMove(double xPos, double yPos);

			void scroll(GLFWwindow* window, const glm::dvec2& pos);

			size_t _changeNumber = 0;
			glm::dvec2 _mouseStartPos[4];
			glm::mat4 _initialMatrix;
			VulkanAppPtr _app;
			GLFWwindow* _window;

			size_t _lastButtonDownIdx = stm1;
			size_t _hilightButtonIdx = stm1;
			std::vector<ButtonPtr> _buttons;
		};

		inline size_t Window::getChangeNumber() const {
			return _changeNumber;
		}

	}
}

