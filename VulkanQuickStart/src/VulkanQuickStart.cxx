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

// Vulkan 01.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <vk_defines.h>

#include <fstream>

#include "vk_app.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "vk_model.h"
#include "vk_modelObj.h"
#include <vk_pipelineUi.h>
#include <vk_ui_button.h>
#include <vk_ui_window.h>
#include <vk_pipelineGroup.h>

#include <triMesh.h>
#include <readStl.h>

using namespace VK;
using namespace std;

#define TEST_OBJ 1
#define TEST_STL 1
#define TEST_GUI 1
#define ORBIT 0 // Test the user defined world transform option


const std::string modelPath = "../../../../resources/models/";

#if TEST_OBJ
const std::string pottedPlantPath = modelPath + "IndoorPotPlant/";
const std::string pottedPlantFilename = "indoor_plant_02.obj";

const std::string dnaPath = modelPath + "DNA/";
const std::string dnaFilename = "DNA.obj";

const std::string gliderPath = modelPath + "glider/";
const std::string gliderFilename = "FFGLOBJ.obj";

const std::string apricotPath = modelPath + "apricot/";
const std::string apricotFilename = "Apricot_02_hi_poly.obj";

ModelObjPtr plant;
ModelObjPtr dna;
ModelObjPtr apricot;

#endif

#if TEST_STL
const std::string stlFilenameCourse = "test_part_course.stl";
const std::string stlFilenameFine = "test_part_fine.stl";
#endif

PipelineGroup<Pipeline3DPtr> pipeline3DShaded;
PipelineGroup<Pipeline3DPtr> pipeline3DWireframe;

PipelineGroup<Pipeline3DWSamplerPtr> pipeline3DWSampler;

VulkanAppPtr gApp;
OffscreenPassPtr offscreen;
size_t offscreenIdx = stm1;

ModelPtr vase, part;


#if TEST_GUI
void buildUi(UI::WindowPtr& gui) {
	glm::vec4 bkgColor(0.875f, 0.875f, 0.875f, 1);
	uint32_t w = 160;
	uint32_t h = 22;
	uint32_t row = 0;

	gui->addButton(bkgColor, "Reset View", UI::Rect(row, 0, row + h, w))->
		setAction(UI::Button::ActionType::ACT_CLICK, [&](int btnNum, int modifiers) {
		if (btnNum == 0) {
			glm::mat4 xform = glm::mat4(1.0f);
			gApp->setModelToWorldTransform(xform);
		}
	});

	row += h;
	gui->addButton(bkgColor, "Toggle Wireframe", UI::Rect(row, 0, row + h, w))->
		setAction(UI::Button::ActionType::ACT_CLICK, [&](int btnNum, int modifiers) {
		if (btnNum == 0) {
			pipeline3DShaded.toggleVisiblity();
			pipeline3DWireframe.toggleVisiblity();
		}
	});

#if TEST_OBJ
	row += h;
	gui->addButton(bkgColor, "Show/Hide plant", UI::Rect(row, 0, row + h, w))->
		setAction(UI::Button::ActionType::ACT_CLICK, [&](int btnNum, int modifiers) {
		if (btnNum == 0) {
			if (plant) {
				plant->toggleVisibility();
			}
		}
	});
#endif

	row += h;
	gui->addButton(bkgColor, "Show/Hide part", UI::Rect(row, 0, row + h, w))->
		setAction(UI::Button::ActionType::ACT_CLICK, [&](int btnNum, int modifiers) {
		if (btnNum == 0) {
			if (part)
				part->toggleVisibility();
		}
	});

	row += h;
	gui->addButton(bkgColor, "Show/Hide vase", UI::Rect(row, 0, row + h, w))->
		setAction(UI::Button::ActionType::ACT_CLICK, [&](int btnNum, int modifiers) {
		if (btnNum == 0) {
			if (vase) 
				vase->toggleVisibility();
		}
	});

	row += h;
	gui->addButton(bkgColor, "Screenshot", UI::Rect(row, 0, row + h, w))->
		setAction(UI::Button::ActionType::ACT_CLICK, [&](int btnNum, int modifiers) {
		string path = "/Users/Bob/Documents/Projects/ElectroFish/HexMeshTests/";
		if (btnNum == 0) {
			ImagePtr image = gApp->getOffscreenImage(offscreenIdx);
			if (!image) {
				const auto& swapChain = gApp->getSwapChain();
				const auto& images = swapChain._images;
				image = images[gApp->getSwapChainIndex()];
			}
			image->saveImage(path + "screenshot.jpg");
			image->saveImage(path + "screenshot.bmp");
			image->saveImage(path + "screenshot.png");
		}
	});

	row += h;
	gui->addButton(bkgColor, "Button 2", UI::Rect(row, 0, row + h, w));
}
#endif

void addObj() {
#if TEST_OBJ
	glm::mat4 xform;

	plant = ModelObj::create(gApp, pottedPlantPath, pottedPlantFilename);
	pipeline3DWSampler.addSceneNode(plant);

	xform = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	plant->setModelTransform(xform);
	plant->setModelTransformFunc([&](const glm::mat4& src)->glm::mat4 {
		double revs = gApp->getRuntimeMillis() / 1000.0 * (15.0 / 60.0);
		while (revs > 1)
			revs -= 1;

		glm::mat4 xform(src);
		xform *= glm::rotate(glm::mat4(1.0f), (float)(2 * EIGEN_PI * revs), glm::vec3(0.0f, 1.0f, 0.0f));

		return xform;
	});

	dna = ModelObj::create(gApp, dnaPath, dnaFilename);
	pipeline3DWSampler.addSceneNode(dna);
	xform = glm::translate(glm::mat4(1.0f), glm::vec3(0, 10, 0));
	dna->setModelTransform(xform);
	dna->setModelTransformFunc([&](const glm::mat4& src)->glm::mat4 {
		double revs = gApp->getRuntimeMillis() / 1000.0 * (15.0 / 120.0);
		while (revs > 1)
			revs -= 1;

		glm::mat4 xform(src);
		xform *= glm::rotate(glm::mat4(1.0f), (float)(2 * EIGEN_PI * revs), glm::vec3(0.0f, 0.0f, 1.0f));

		return xform;
	});

	apricot = ModelObj::create(gApp, apricotPath, apricotFilename);
	pipeline3DWSampler.addSceneNode(apricot);
	xform = glm::translate(glm::mat4(1.0f), glm::vec3(10, 10, 0));
	xform *= glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	apricot->setModelTransform(xform);
#endif
}

int readStl(const string& filename, ModelPtr& model) {
	TriMesh::CMeshPtr meshPtr = std::make_shared<TriMesh::CMesh>();
	CReadSTL readStl(meshPtr);
	if (!readStl.read(modelPath, filename))
		return 1;


	model = Model::create(gApp, meshPtr);
	pipeline3DShaded.addSceneNode(model);

	return 0;
}

int addStl() {
#if TEST_STL
	glm::mat4 xform;

	bool fine = false;
	std::string filename = fine ? stlFilenameFine : stlFilenameCourse;
	readStl(filename, part);
	xform = glm::scale(glm::mat4(1.0f), glm::vec3(.05f, .05f, .05f));
	part->setModelTransform(xform);

	readStl("Vase.stl", vase);
	xform = glm::translate(glm::mat4(1.0f), glm::vec3(-5, -5, 0));
	xform *= glm::scale(glm::mat4(1.0f), glm::vec3(.25f, .25f, .25f));
	vase->setModelTransform(xform);

	auto xformFunc = [&](const glm::mat4& src)->glm::mat4 {
		double revs = gApp->getRuntimeMillis() / 1000.0 * (15.0 / 30.0);
		while (revs > 1)
			revs -= 1;

		glm::mat4 xform(src);
		xform *= glm::rotate(glm::mat4(1.0f), (float)(2 * EIGEN_PI * revs), glm::vec3(0.0f, 0.0f, 1.0f));
		return xform;
	};

	vase->setModelTransformFunc(xformFunc);

#endif
	return 0;
}

void createPipelines() {
	vector<string> sampler3DFilenames = { "shaders/shader_depth_vert.spv", "shaders/shader_depth_frag.spv" };

	pipeline3DWSampler.add(gApp->addPipelineWithSource<Pipeline3DWSampler>("obj_shader", sampler3DFilenames));
	pipeline3DWSampler.add(offscreen->addPipelineWithSource<Pipeline3DWSampler>("obj_shader", sampler3DFilenames));

	vector<string> shaded3DFilenames = { "shaders/shader_vert.spv", "shaders/shader_frag.spv" };
	pipeline3DShaded.add(gApp->addPipelineWithSource<Pipeline3D>("stl_shaded", shaded3DFilenames));
	pipeline3DShaded.add(offscreen->addPipelineWithSource<Pipeline3D>("stl_shaded", shaded3DFilenames));

	vector<string> wf3DFilenames = { "shaders/shader_vert.spv", "shaders/shader_wireframe_frag.spv" };
	pipeline3DWireframe.add(gApp->addPipelineWithSource<Pipeline3D>("stl_wireframe", wf3DFilenames));
	pipeline3DWireframe.add(offscreen->addPipelineWithSource<Pipeline3D>("stl_wireframe", wf3DFilenames));
	//pipeline3DWireframe.toggleVisiblity();
	pipeline3DWireframe.setPolygonMode(VK_POLYGON_MODE_LINE);

	gApp->changed();
}

auto orbitFunc = [](uint32_t width, uint32_t height)->VulkanApp::UboType {
	VulkanApp::UboType ubo;

	ubo = {};
	ubo.ambient = 0.10f;
	ubo.numLights = 2;
	ubo.lightDir[0] = glm::normalize(glm::vec3(1, -0.5, 1));
	ubo.lightDir[1] = glm::normalize(glm::vec3(-1, -0.5, 3));

	float w = (float)width;
	float h = (float)height;
	float maxDim = std::max(w, h);
	float minDim = std::min(w, h);
	w /= maxDim;
	h /= maxDim;


	double revs = gApp->getRuntimeMillis() / 1000.0 * (-5.0 / 60.0);
	while (revs > 1)
		revs -= 1;

	float scale = 0.075f;
	float theta = (float)(15.0 * EIGEN_PI / 180.0);
	glm::vec3 ctr(0, 0, 0);
	glm::mat4 model = glm::scale(glm::mat4(1.0f), glm::vec3(scale, scale, scale));
	model *= glm::translate(glm::mat4(1.0f), ctr);
	model *= glm::rotate(glm::mat4(1.0f), theta, glm::vec3(0.0f, 1.0f, 0.0f));
	model *= glm::rotate(glm::mat4(1.0f), (float)(2 * EIGEN_PI * revs), glm::vec3(0.0f, 0.0f, 1.0f));

	glm::mat4 view = glm::lookAt(glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));

	ubo.modelView = view * model;

	ubo.proj = glm::ortho(-w, w, -h, h, -10.0f, 10.0f);
	ubo.proj[1][1] *= -1;

	return ubo;
};

int main(int numArgs, char** args) {
	VkRect2D frame;
	frame.offset = { 0,0, };
	frame.extent.width = 1500;
	frame.extent.height = 900;
	gApp = VulkanApp::create(frame);

	gApp->setAntiAliasSamples(VK_SAMPLE_COUNT_4_BIT);

	VkExtent2D offscreenExtent = { 2048, 2048 };
	offscreen = make_shared<OffscreenPass>(gApp, VK_FORMAT_R8G8B8A8_UNORM);
	offscreen->setAntiAliasSamples(VK_SAMPLE_COUNT_1_BIT);
	offscreen->init(offscreenExtent);
	offscreenIdx = gApp->addOffscreen(offscreen);


#if TEST_GUI
	UI::WindowPtr gui = make_shared<UI::Window>(gApp);
	gApp->setUiWindow(gui);
	buildUi(gui);
#else
	UI::Window gui(gApp);
#endif

	glfwSetWindowTitle(gApp->getWindow(), "Vulkan Quick Start");

	createPipelines();

	addObj();
	addStl();

#if ORBIT
	gApp->setUboUpdateFunction(orbitFunc);
#endif

	try {
		gApp->run();
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
