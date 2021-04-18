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

#include <iostream>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <glm/gtc/matrix_transform.hpp>

#include <vk_pipelineUi.h>
#include <vk_sceneNodeUi.h>
#include <vk_ui_button.h>
#include <vk_ui_window.h>
#include <vk_pipelineUboGroup.h>
#include <vk_app.h>

namespace VK::UI {

	using namespace std;
	using namespace glm;

	Window::Window(const VulkanAppPtr& app)
		: _app(app)
		, _window(app->getWindow())
	{
		glfwSetWindowUserPointer(_window, this);
		init();
	}

	void Window::updateUniformBuffer(size_t swapChainImageIndex) {
		_pipelines->iterate([&](const PipelinePtr& pipeline) {
			pipeline->updateUniformBuffers(swapChainImageIndex);
		});
	}

	inline dvec2 Window::scaleCursorPos(GLFWwindow* window, double xPos, double yPos) {
		dvec2 result;
		int width, height;
		glfwGetWindowSize(window, &width, &height);
		result[0] = (2 * xPos / (double)width) - 1;
		result[1] = (2 * yPos / (double)height) - 1;
		return result;
	}

	ivec2 Window::toUi(const dvec2& pt) const {
		vec2 p(static_cast<float> (pt.x), static_cast<float> (pt.y));
		const auto& ubo = _pipelines->getUbo();
		/*
	vec2 p = inPosition;
	p = p * ubo.scale;
	p = p + ubo.offset;
		*/
		p -= ubo._offset;
		vec2 scale(1.0f / ubo._scale.x, 1.0f / ubo._scale.y);
		p *= scale;

		return ivec2(static_cast<int>(p.x + 0.5), static_cast<int>(p.y + 0.5));
	}

	ButtonPtr Window::addButton(const glm::vec4& bkgColor, const std::string& label, const Rect& frame) {
		auto pipeline = dynamic_pointer_cast<PipelineUi>(_pipelines->getPipeline(0));
		ButtonPtr btnPtr = make_shared<Button>(_app, bkgColor, label, frame);
		btnPtr->createBuffers();
		_buttons.push_back(btnPtr);

		pipeline->addSceneNode(btnPtr);
		return btnPtr;
	}

	int Window::getPixelDPI() const {
		return _app->getWindowDpi();
	}

	const int leftBtn = 0;
	const int rightBtn = 1;
	const int middleBtn = 2;

	bool Window::handleButtonClick(const glm::ivec2& pt, int btnNum, bool pressed, int modifiers) {
		for (size_t idx = 0; idx < _buttons.size(); idx++) {
			const auto& btn = _buttons[idx];
			if (btn->isPointInside(pt)) {
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

	bool Window::handleButtonMove(const glm::ivec2& pt, int btnNum) {
		size_t nextBtnIdx = stm1;

		for (size_t idx = 0; idx < _buttons.size(); idx++) {
			const auto& btn = _buttons[idx];
			if (btn->isPointInside(pt)) {
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

	bool Window::mouseButtonUi(const glm::ivec2& pt, int btnNum, bool pressed, int modifiers) {
		if (handleButtonClick(pt, btnNum, pressed, modifiers))
			return true;

		return false;
	}

	bool Window::mouseButton3D(const glm::dvec2& pt, int btnNum, bool pressed, int modifiers) {
		_mouseStartPos[btnNum] = pt;

		if (btnNum == rightBtn) {
			if (pressed)
				startMouseRotate();
			else
				endMouseRotate();
			return true;
		}

		return false;
	}

	bool Window::mouseMovedUi(const glm::ivec2& pos, int btnNum) {
		if (handleButtonMove(pos, btnNum)) {
			return true;
		}

		return false;
	}

	bool Window::mouseMoved3D(const glm::dvec2& pos, int btnNum) {
		if (btnNum == rightBtn)
			doMouseRotate(_mouseStartPos[rightBtn], pos);

		return false;
	}

	void Window::startMouseRotate() {
		_initialMatrix3D = _app->getModelToWorldTransform();
	}

	void Window::endMouseRotate() {

	}

	void Window::doMouseRotate(const dvec2& startPos, const dvec2& curPos) {
		auto delta = curPos - startPos;
		auto rot = mat3(_initialMatrix3D);
		auto invRot = inverse(rot);
		vec3 xAxis = invRot * vec3(0, 0, 1);
		vec3 yAxis = invRot * vec3(0, 1, 0);
		float theta = (float)(EIGEN_PI * delta[0]);
		float phi = (float)(EIGEN_PI * -delta[1]);
		auto xform = _initialMatrix3D;
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
		double xPos, yPos;
		glfwGetCursorPos(window, &xPos, &yPos);
		dvec2 pt = scaleCursorPos(window, xPos, yPos);

		Window* self = static_cast<Window*>(glfwGetWindowUserPointer(window));

		auto ptUi = self->toUi(pt);
		if (self->mouseButtonUi(ptUi, btnNum, pressed == GLFW_PRESS, modifiers))
			return;

		self->mouseButton3D(pt, btnNum, pressed == GLFW_PRESS, modifiers);
	}

	void Window::cursorPosCB(GLFWwindow* window, double xPos, double yPos) {
		dvec2 pt = scaleCursorPos(window, xPos, yPos);

		int btnNum = -1;
		if (glfwGetMouseButton(window, leftBtn) == GLFW_PRESS)
			btnNum = leftBtn;
		else if (glfwGetMouseButton(window, middleBtn) == GLFW_PRESS)
			btnNum = middleBtn;
		else if (glfwGetMouseButton(window, rightBtn) == GLFW_PRESS)
			btnNum = rightBtn;

		Window* self = static_cast<Window*>(glfwGetWindowUserPointer(window));

		auto ptUi = self->toUi(pt);
		if (self->mouseMovedUi(ptUi, btnNum))
			return;
		self->mouseMoved3D(pt, btnNum);
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

		size_t numSwapChains = _app->getSwapChain()._images.size();
		_pipelines = make_shared<PipelineUboGroup<UboType>>(_app, numSwapChains);
		_pipelines->add(make_shared<PipelineUi>(_pipelines));

		updateUbo();

		glfwSetMouseButtonCallback(win, mouseButtonCB);
		glfwSetCursorPosCallback(win, cursorPosCB);
		glfwSetScrollCallback(win, scrollCB);
	}

	void Window::updateUbo() {
		int widthPx, heightPx;

		auto win = _app->getWindow();
		glfwGetWindowSize(win, &widthPx, &heightPx);

		int dpi = _app->getWindowDpi();
		float widthIn = widthPx / (float)dpi;
		float heightIn = heightPx / (float)dpi;
		float widthPts = widthIn * 72;
		float heightPts = heightIn * 72;

		auto ubo = _pipelines->getUbo();
		ubo._scale = glm::vec2(1.0f / widthPts, 1.0f / heightPts);
		ubo._offset = glm::vec2(-1.0f, -1.0f);
		ubo._color = glm::vec4(1, 0, 0, 1);
		_pipelines->setUbo(ubo, 0);

	}

}