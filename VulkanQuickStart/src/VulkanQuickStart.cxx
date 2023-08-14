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

#include <vector>
#include <locale>
#include <codecvt>
#include <vk_app.h>
#include <vk_main.h>

// Start of wxWidgets "Hello World" Program

#ifdef WIN32
#pragma warning(push)
#pragma warning(disable: 4996)
#include <wx/wx.h>
#pragma warning(pop)
#else
#include <wx/wx.h>
#endif // WIN32

#if 0
// Old Vulkan main
int main(int numArgs, char** args) {
    return VK::mainRunTest(numArgs, args);
}
#endif

using namespace std;
using namespace VK;

class MyApp : public wxApp
{
public:
    bool OnInit() override;
    virtual bool Initialize(int& argc, wxChar** argv) wxOVERRIDE;
private:
    VK::VulkanAppPtr m_vkApp;
    string m_shaderDir;
};
#if 0
wxIMPLEMENT_APP(MyApp);
#else
extern "C" int __stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, wxCmdLineArgType lpCmdLine, int nCmdShow) {
    ; ; 
    return wxEntry(hInstance, hPrevInstance, lpCmdLine, nCmdShow);
} 

MyApp& wxGetApp() {
    return *static_cast<MyApp*>(wxApp::GetInstance());
} 

wxAppConsole* wxCreateApp() {
    wxAppConsole::CheckBuildOptions("3" "." "2" " (" "wchar_t" ",Visual C++ " "1900" ",wx containers"  ",compatible with 3.0" ")", "your program"); 
    return new MyApp;
} 

wxAppInitializer wxTheAppInitializer((wxAppInitializerFunction)wxCreateApp);
#endif

class MyFrame : public wxFrame
{
public:
    MyFrame();

    void addMenus();
    void addStatusBar();

private:
    void OnHello(wxCommandEvent& event);
    void OnExit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
};

enum
{
    ID_Hello = 1
};

bool MyApp::OnInit()
{
    wxApp::OnInit();

    MyFrame *frame = new MyFrame();
    frame->Show(true);
    wxRect rect = frame->GetRect();
    auto pWindow = GetMainTopWindow();
    WXHWND hWind = pWindow->GetHWND();
    VkRect2D vkRect;
    vkRect.extent.height = rect.height;
    vkRect.extent.width = rect.width;

    m_vkApp = VK::VulkanApp::create(vkRect);
    m_vkApp->setShaderDir(m_shaderDir);

    VK::runWindow(m_vkApp);
    
    return true;
}

bool MyApp::Initialize(int& argc, wxChar** argv)
{
    wxApp::Initialize(argc, argv);

    wstring cmdStr(argv[0]);
    auto pos = cmdStr.find(L"\\");
    while (pos != wstring::npos) {
        cmdStr.replace(pos, 1, L"/");
        pos = cmdStr.find(L"\\");
    }
    pos = cmdStr.rfind(L"/");
    cmdStr = cmdStr.substr(0, pos);
    bool dirFound = wxDirExists(cmdStr + L"/shaders_spv");
    while (!dirFound && cmdStr.find(L"/") != 0) {
        pos = cmdStr.rfind(L"/");
        cmdStr = cmdStr.substr(0, pos);
        dirFound = wxDirExists(cmdStr + L"/shaders_spv");
    }

    if (dirFound) {
        setlocale(LC_CTYPE, "");

        string s;
        for (auto c : cmdStr) {
            s += char(c);
        }
        m_shaderDir += s + "/shaders_spv";
    }
    return true;
}

MyFrame::MyFrame()
    : wxFrame(nullptr, wxID_ANY, "Vulkan Quick Start")
{
    addMenus();
    addStatusBar();
}

void MyFrame::addMenus()
{
    wxMenu* menuFile = new wxMenu;
    menuFile->Append(ID_Hello, "&Hello...\tCtrl-H",
        "Help string shown in status bar for this menu item");
    menuFile->AppendSeparator();
    menuFile->Append(wxID_EXIT);

    wxMenu* menuHelp = new wxMenu;
    menuHelp->Append(wxID_ABOUT);

    wxMenuBar* menuBar = new wxMenuBar;
    menuBar->Append(menuFile, "&File");
    menuBar->Append(menuHelp, "&Help");

    SetMenuBar(menuBar);

    Bind(wxEVT_MENU, &MyFrame::OnHello, this, ID_Hello);
    Bind(wxEVT_MENU, &MyFrame::OnAbout, this, wxID_ABOUT);
    Bind(wxEVT_MENU, &MyFrame::OnExit, this, wxID_EXIT);
}

void MyFrame::addStatusBar()
{
    CreateStatusBar();
    SetStatusText("Welcome to VulkanQuickStart!");
}

void MyFrame::OnExit(wxCommandEvent& event)
{
    Close(true);
}

void MyFrame::OnAbout(wxCommandEvent& event)
{
    wxMessageBox("This is a wxWidgets Hello World example",
                 "About Hello World", wxOK | wxICON_INFORMATION);
}

void MyFrame::OnHello(wxCommandEvent& event)
{
    wxLogMessage("Hello world from wxWidgets!");
}
