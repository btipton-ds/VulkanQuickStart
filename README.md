# Project Title

VulkanQuickStart is intended as a tutorial, example and foundation for building applications using the Vulkan graphics engine.

If you have been looking for the next generation of cross platform graphics engines, Vulkan looks like one of the better choices. Possibly the best or only choice.

The core of Vulkan offers tremendous advantages in cross platform development, speed, flexibility and customizability. It is under development by the Khronos group (https://www.khronos.org/vulkan/), a consortium of the top graphics companies in the world.

It achieves this at the cost of ease of starting a project. This has been a nearly universal comment by the graphics developers I work with. I'm sure there is a large cadre of lowlevel, graphics pipeline gurus who can pick up Vulkan in a few days. But for myself and those I work with the "You have to write 2,000 lines of code to draw a triangle" has been the most common comment.

If you've been wanting to learn or apply Vulkan in your projects, VulkanQuickStart is the answer.

Unlike other examples and tutorials, it creates multiple objects of various types. Currently it reads textured obj and stl files and has support for a simple 2D button layer on top of the 3D layer.

![Image of VulkanQuickStart](https://youtu.be/PCE05P8i4VE)

It is by no means an out of the box solution! It is not intended to be one. It doesn't make best use of Vulkan's high performance optimizations, such as multiple uniform buffers etc.

It is a quick way to get several thousand lines of Vulkan code up an running so you can modify it.

It is planned to grow into a light weight development platform which can be used to start projects.

The source code is available under GPLv3. I don't plan to, or have the resources, to pursue violators. It's yours to use. However, if you use it for commercial purposes I expect fare compensation for the value you received.

I am also accepting donations through ![GoFundMe.com ElectroFish](https://www.gofundme.com/f/electrofish)

## Getting Started

The project was developed under MS Visual Studio 2019 as a Make File Project. It was also built with gcc under WSL. That environment will work best.

### Prerequisites

* A Vulkan API. It is configured to be build with ![Lunar G's API](https://vulkan.lunarg.com/)
* The prequisites required by the API.
* FreeType (https://www.freetype.org/)
* Eigen Matrix (http://eigen.tuxfamily.org/index.php?title=Main_Page) - Eigen is optional. It's a high performance package, but IMO too heavy for easy debugging.
* Dark Sky Innovative Solution's triMesh library ();
* CMake
* A C++-17 complient compiler. Some of the third party code incorporated required C++17.

### Installing

VulkanQuickStart is being developed using Visual Studio 2019 as a MakeFile project with Unix/Linux compatibility being tested using WSL. The foundation libraries are currently building on WSL, but not the main app.

Create a project root
Create a Thirdparty directory there.
Place dependencies in this directory. Except triMesh.
Setup FreeType - this was challenging. I ended up building it from source in the Thirdparty directory.

Clone triMesh to the project root
Build triMesh

Clone the VulkanQuickStart to the project root.
Build it also.

Launch VS 2019 (2017 shuould work also) - get the community edition if you don't have a copy
Skip the entry screen using the 'without code' option
Open the project
Choose makefile project and select the root CMakeLists.txt

A fully working linux version is in the future, but for logistal reasons I don't have a linux box to develop on. Virtual Box doesn't do graphics which makes graphics work _difficult_. WSL is the current workaround.

## Running the tests

Vulkan Quick Start has no automatic tests at this time.

## Deployment

It's currently build as a fully linked app. No shared libraries are required.

## Built With

* Built with Visual Studio 2019 (https://visualstudio.microsoft.com/downloads/)

## Contributing

Please read [CONTRIBUTING.md](https://gist.github.com/PurpleBooth/b24679402957c63ec426) for details on our code of conduct, and the process for submitting pull requests to us.


## Authors

**Robert R (Bob) Tipton** - btipton - darkskyinnovation.com

## License

This project is licensed under the GPLv3 License - see <https://www.gnu.org/licenses/> file for details

