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

#include <defines.h>

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

#include <triMesh.h>
#include <readStl.h>

using namespace VK;
using namespace std;

#define TEST_OBJ 1
#define TEST_STL 1
#define TEST_GUI 1


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
#endif

#if TEST_STL
const std::string stlFilenameCourse = "test_part_course.stl";
const std::string stlFilenameFine = "test_part_fine.stl";
#endif

VulkanAppPtr gApp;

#if TEST_GUI
void buildUi(UI::Window& gui) {
	glm::vec4 bkgColor(0.875f, 0.875f, 0.875f, 1);
	uint32_t w = 80;
	uint32_t h = 22;
	uint32_t row = 0;

	auto btn1 = gui.addButton(UI::Button(bkgColor, "Button 0", UI::Rect(row, 0, row + h, w)));
	btn1->setAction(UI::Button::ActionType::ACT_CLICK, [&](int btnNum, int modifiers) {
		if (btnNum == 0) {
			glm::mat4 xform = glm::mat4(1.0f);
			gApp->setModelToWorldTransform(xform);
		}
	});

	row += h;
	gui.addButton(UI::Button(bkgColor, "Button 1", UI::Rect(row, 0, row + h, w)));

	row += h;
	gui.addButton(UI::Button(bkgColor, "Button 2", UI::Rect(row, 0, row + h, w)));
}
#endif

int main(int numArgs, char** args) {
	gApp = make_shared<VulkanApp>(1618, 1000);

	gApp->setAntiAliasSamples(VK_SAMPLE_COUNT_4_BIT);

#if TEST_GUI
	UI::Window gui(gApp);
	buildUi(gui);
#else
	UI::Window gui(gApp);
#endif

	glfwSetWindowTitle(gApp->getWindow(), "Vulkan Quick Start");

	glm::mat4 xform;
#if TEST_OBJ

	ModelObjPtr plant = std::dynamic_pointer_cast<ModelObj> (gApp->addSceneNode3D(pottedPlantPath, pottedPlantFilename));
	xform = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	plant->setModelTransform(xform);

	ModelObjPtr dna = std::dynamic_pointer_cast<ModelObj> (gApp->addSceneNode3D(dnaPath, dnaFilename));
	xform = glm::translate(glm::mat4(1.0f), glm::vec3(0, 10, 0));
	dna->setModelTransform(xform);

	ModelObjPtr apricot = std::dynamic_pointer_cast<ModelObj> (gApp->addSceneNode3D(apricotPath, apricotFilename));
	xform = glm::translate(glm::mat4(1.0f), glm::vec3(10, 10, 0));
	xform *= glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	apricot->setModelTransform(xform);
#endif

#if TEST_STL
	{
		bool fine = true;
		std::string filename = fine ? stlFilenameFine : stlFilenameCourse;

		TriMesh::CMeshPtr meshPtr = std::make_shared<TriMesh::CMesh>();
		CReadSTL readStl(meshPtr);
		if (!readStl.read(modelPath, filename))
			return 1;
		auto meshModel = gApp->addSceneNode3D(meshPtr);
		meshModel->setModelTransform(glm::scale(glm::mat4(1.0f), glm::vec3(.05f, .05f, .05f)));
	}

	{
		TriMesh::CMeshPtr meshPtr = std::make_shared<TriMesh::CMesh>();
		CReadSTL readStl(meshPtr);
		if (!readStl.read(modelPath, "Vase.stl"))
			return 1;
		auto meshModel = gApp->addSceneNode3D(meshPtr);
		xform = glm::translate(glm::mat4(1.0f), glm::vec3(-5, -5, 0));
		xform *= glm::scale(glm::mat4(1.0f), glm::vec3(.25f, .25f, .25f));
		meshModel->setModelTransform(xform);
	}
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
