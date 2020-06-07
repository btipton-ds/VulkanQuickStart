# VulkanQuickStart

VulkanQuickStart is intended as a tutorial, example and foundation for building applications using the Vulkan graphics engine.

If you have been looking for the next generation of cross platform graphics engines, Vulkan looks like one of the better choices. Possibly the best or only choice.

The core of Vulkan offers tremendous advantages in cross platform development, speed, flexibility and customizability. It is under development by the Khronos group (https://www.khronos.org/vulkan/), 
a consortium of the top graphics companies in the world.

It achieves this at the cost of ease of starting a project. This has been a nearly universal comment by the graphics developers I work with. 
I'm sure there is a large cadre of low level, graphics pipeline gurus who can pick up Vulkan in a few days. 
But for myself and those I work with, "You have to write 2,000 lines of code to draw a triangle" has been the most common comment.

If you've been wanting to learn or apply Vulkan in your projects, VulkanQuickStart is the answer.

Unlike other examples and tutorials, it can create multiple objects of various types - like real applications do.

[YouTube video of VulkanQuickStart](https://youtu.be/TF_OYqCb9qA)

**It is by no means an out of the box solution!** It is not intended to be one. It doesn't make best use of Vulkan's high performance optimizations, such as multiple uniform buffers etc.

**It is** a quick way to get several thousand lines of Vulkan code up and running so you can modify it.

I plan to grow it into a light weight development platform which can be used to start projects.

1 Skeleton obj reader - material file support is limited.  
2 stl reader  
3 Piplelines and scene nodes are templated based on uniform buffer and vertex type. It's difficult to mismatch shaders etc.  
4 230+ fps on a generic HP Envy laptop  
5 Makefile based in VS 2019 so it's written on windows in a unix fashion.  
6 Compiles under gcc.  
7 No target application, file format type. Add what you need.  
8 Button clicks are handled by c++11 lambda functions. This makes event handling look like Javascript annoymous arrow functions.  
9 Modifiable default pipelines. Use the default pipelines to get up and going fast. Modify them or use them as guides for writing your own.

The source code is available under GPLv3. I don't plan to, or have the resources to, pursue violators. It's yours to use. 
However, if you use it for commercial purposes I expect fare compensation for the value you received.

I am also accepting donations through [GoFundMe.com ElectroFish](https://www.gofundme.com/f/electrofish)

The project was developed as the foundation for some cross platform CAD/CAE tools I need to complete some work on the ElectroFish project. I'm strapped enough for resources, and locked down in the Philippines 
quarantine. It's actually faster to develop my own toolset.

## Getting Started

The project was developed under MS Visual Studio 2019 as a Make File Project. It was also built with gcc under WSL. That environment should work best.  

If someone with a unix box wants to contribute by getting the linux environment working, I'm interested in talking.

### Prerequisites

1 A Vulkan API. It is configured to be build with [Lunar G's Vulkan API](https://vulkan.lunarg.com/)  
2 The prequisites required by the API.  
3 FreeType (https://www.freetype.org/)  
4 Eigen Matrix (http://eigen.tuxfamily.org/index.php?title=Main_Page) - Eigen is optional. It's a high performance package, but IMO too heavy for easy debugging.  
5 Dark Sky Innovative Solution's [triMesh library] (https://github.com/btipton-ds/triMesh)  
6 CMake
7 A C++-17 compliant compiler. Some of the third party code incorporated required C++17.

### Installing

VulkanQuickStart is being developed using Visual Studio 2019 as a MakeFile project with Unix/Linux compatibility being tested using WSL. The foundation libraries are currently building on WSL, but not the main app.

1 Create a project root.  
2 Create a Thirdparty directory there.  
3 Place dependencies in this directory. Except triMesh.  
4 Setup FreeType - this was challenging. I ended up building it from source in the Thirdparty directory.  

5 Clone triMesh to the project root  
6 Build triMesh  

7 Clone the VulkanQuickStart to the project root.  
8 Build it also.  

9 Launch VS 2019 (2017 shuould work also) - get the community edition if you don't have a copy  
10 Skip the entry screen using the 'without code' option  
11 Open the project  
12 Choose makefile project and select the root CMakeLists.txt  

A fully working linux version is in the future, but for logistal reasons I don't have a linux box to develop on. Virtual Box doesn't do graphics which makes graphics work _difficult_. WSL is the current workaround.

## Running the tests

Vulkan Quick Start has no automatic tests at this time.

## Deployment

It's currently built as a fully linked app. No shared libraries are required.

## Built with Visual Studio 2019 (https://visualstudio.microsoft.com/downloads/)

## Contributing

Please read [CONTRIBUTING.md](https://gist.github.com/PurpleBooth/b24679402957c63ec426) for details on our code of conduct, and the process for submitting pull requests to us.

## Authors

**Robert R (Bob) Tipton** - btipton <-> darkskyinnovation.com
founder [Dark Sky Innovative Solutions](http://darkskyinnovation.com/index.html)


## License

This project is licensed under the GPLv3 License - see <https://www.gnu.org/licenses/> file for details

