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

#include <iostream>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <glm/gtc/matrix_transform.hpp>

#include <vk_pipelineUi.h>
#include <vk_sceneNodeUi.h>
#include <vk_ui_button.h>
#include <vk_ui_window.h>
#include <vk_app.h>

namespace VK::UI {

	using namespace std;
	using namespace glm;

	Window::Window(const VulkanAppPtr& app)
		: _app(app)
		, _window(app->getWindow())
	{
		_app->setUiWindow(this);
		glfwSetWindowUserPointer(_window, this);
		init();
	}

	void Window::addButton(const Button& btn) {
		int width, height;
		glfwGetWindowSize(_app->getWindow(), &width, &height);

		ButtonPtr btnPtr = make_shared<Button>(btn);
		btnPtr->createBuffers(_app);
		_buttons.push_back(btnPtr);
		PipelineUiPtr pipeline = _app->addPipeline(createPipeline<PipelineUi>(_app.get()));

		SceneNode2DPtr p = btnPtr;
		pipeline->addSceneNode(p);
	}

	int Window::getPixelDPI() const {
		return _app->getWindowDpi();
	}

	const int leftBtn = 0;
	const int rightBtn = 1;

	bool Window::handleButtonClick(double xPos, double yPos, int btnNum, bool pressed, int modifiers) {
		for (size_t idx = 0; idx < _buttons.size(); idx++) {
			const auto& btn = _buttons[idx];
			if (btn->isPointInside(xPos, yPos)) {
				if (pressed) {
					_lastButtonDownIdx = idx;
				}
				else if (idx == _lastButtonDownIdx) {
					btn->handleMouseClick(btnNum, modifiers);
					_lastButtonDownIdx = stm1;
				}
				return true;
			}
		}
		return false;
	}

	bool Window::handleButtonMove(double xPos, double yPos) {
		size_t nextBtnIdx = stm1;

		for (size_t idx = 0; idx < _buttons.size(); idx++) {
			const auto& btn = _buttons[idx];
			if (btn->isPointInside(xPos, yPos)) {
				nextBtnIdx = idx;
				break;
			}
		}
		if (nextBtnIdx != stm1 && nextBtnIdx == _hilightButtonIdx)
			return true;
		
		if (_hilightButtonIdx != stm1) {
			_buttons[_hilightButtonIdx]->handleMouseExit();
			_hilightButtonIdx = stm1;
		}

		if (nextBtnIdx != stm1) {
			_hilightButtonIdx = nextBtnIdx;
			_buttons[_hilightButtonIdx]->handleMouseEnter();
		}

		return nextBtnIdx != stm1;
	}

	void Window::mouseButton(GLFWwindow* window, int btnNum, bool pressed, int modifiers) {
		double xPos, yPos;
		glfwGetCursorPos(window, &xPos, &yPos);
		if (handleButtonClick(xPos, yPos, btnNum, pressed, modifiers))
			return;
		_mouseStartPos[btnNum] = scaleCursorPos(window, xPos, yPos);


		if (btnNum == rightBtn) {
			if (pressed)
				startMouseRotate();
			else
				endMouseRotate();
		}
	}

	void Window::mouseMoved(GLFWwindow* window, const dvec2& pos) {
		double xPos, yPos;
		glfwGetCursorPos(window, &xPos, &yPos);
		if (handleButtonMove(xPos, yPos)) {
			return;
		}
		if (glfwGetMouseButton(window, rightBtn) == GLFW_PRESS)
			doMouseRotate(_mouseStartPos[rightBtn], pos);
	}

	void Window::startMouseRotate() {
		_initialMatrix = _app->getModelToWorldTransform();
	}

	void Window::endMouseRotate() {

	}

	void Window::doMouseRotate(const dvec2& startPos, const dvec2& curPos) {
		auto delta = curPos - startPos;
		auto rot = mat3(_initialMatrix);
		auto invRot = inverse(rot);
		vec3 xAxis = invRot * vec3(0, 0, 1);
		vec3 yAxis = invRot * vec3(0, 1, 0);
		float theta = (float)(EIGEN_PI * delta[0]);
		float phi = (float)(EIGEN_PI * delta[1]);
		auto xform = _initialMatrix;
		xform *= rotate(mat4(1.0f), theta, xAxis);
		xform *= rotate(mat4(1.0f), -phi, yAxis);
		_app->setModelToWorldTransform(xform);
	}

	void Window::scroll(GLFWwindow* window, const dvec2& pos) {
		double scaleF = 1.0 - 20 * pos[1];
		double modelScale = _app->getModelScale();
		modelScale *= scaleF;

		_app->setModelScale(modelScale);
	}

	void Window::mouseButtonCB(GLFWwindow* window, int btnNum, int pressed, int modifiers) {
		Window* self = static_cast<Window*>(glfwGetWindowUserPointer(window));
		self->mouseButton(window, btnNum, pressed == GLFW_PRESS, modifiers);
	}

	inline dvec2 Window::scaleCursorPos(GLFWwindow* window, double xPos, double yPos) {
		dvec2 result;
		int width, height;
		glfwGetWindowSize(window, &width, &height);
		result[0] = xPos / (double)width;
		result[1] = 1.0 - yPos / (double)height;
		return result;
	}
	void Window::cursorPosCB(GLFWwindow* window, double xPos, double yPos) {

		Window* self = static_cast<Window*>(glfwGetWindowUserPointer(window));
		self->mouseMoved(window, scaleCursorPos(window, xPos, yPos));
	}

	void Window::scrollCB(GLFWwindow* window, double dx, double dy) {
		int width, height;
		glfwGetWindowSize(window, &width, &height);

		Window* self = static_cast<Window*>(glfwGetWindowUserPointer(window));
		dvec2 delta;
		delta[0] = dx / width;
		delta[1] = dy / height;
		self->scroll(window, delta);
	}

	void Window::init() {
		auto win = _app->getWindow();

		glfwSetMouseButtonCallback(win, mouseButtonCB);
		glfwSetCursorPosCallback(win, cursorPosCB);
		glfwSetScrollCallback(win, scrollCB);
	}

}