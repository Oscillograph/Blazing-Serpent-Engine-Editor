#include <BSE_Client.h>
#include <EntryPoint.h>

#include "hacks.h"
#include "./src/SceneViewer.h"

class EditorGuiLayer : public BSE::ImGuiLayer {
public:
	EditorGuiLayer() {
		broTexture = BSE::Texture2D::Create("./assets/img/broscillograph.png");
		broTexture->Bind();
	}
	
	~EditorGuiLayer() {
		if (broTexture != nullptr)
			delete broTexture;
	}
	
	void OnImGuiRender(float time) {
		FixImGuiContext(m_ImGuiContext);

		// If you strip some features of, this demo is pretty much equivalent to calling DockSpaceOverViewport()!
		// In most cases you should be able to just call DockSpaceOverViewport() and ignore all the code below!
		// In this specific demo, we are not using DockSpaceOverViewport() because:
		// - we allow the host window to be floating/moveable instead of filling the viewport (when opt_fullscreen == false)
		// - we allow the host window to have padding (when opt_padding == true)
		// - we have a local menu bar in the host window (vs. you could use BeginMainMenuBar() + DockSpaceOverViewport() in your code!)
		// TL;DR; this demo is more complicated than what you would normally use.
		// If we removed all the options we are showcasing, this demo would become:
		//     void ShowExampleAppDockSpace()
		//     {
		//         ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
		//     }
		
		static bool dockspace_open = true;
		static bool opt_fullscreen = true;
		static bool opt_padding = false;
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;
		
		// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
		// because it would be confusing to have two docking targets within each others.
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		if (opt_fullscreen)
		{
			const ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->WorkPos);
			ImGui::SetNextWindowSize(viewport->WorkSize);
			ImGui::SetNextWindowViewport(viewport->ID);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
			window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		}
		else
		{
			dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
		}
		
		// When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
		// and handle the pass-thru hole, so we ask Begin() to not render a background.
		if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
			window_flags |= ImGuiWindowFlags_NoBackground;
		
		// Important: note that we proceed even if Begin() returns false (aka window is collapsed).
		// This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
		// all active windows docked into it will lose their parent and become undocked.
		// We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
		// any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
		if (!opt_padding)
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		
		ImGui::Begin("DockSpace Demo", &dockspace_open, window_flags);
			if (!opt_padding)
				ImGui::PopStyleVar();
			
			if (opt_fullscreen)
				ImGui::PopStyleVar(2);
			
			// Submit the DockSpace
			// ImGuiIO& io = ImGui::GetIO();
			if (m_io->ConfigFlags & ImGuiConfigFlags_DockingEnable)
			{
				ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
				ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
			}
			
			if (ImGui::BeginMenuBar())
			{
				if (ImGui::BeginMenu(u8"Файл"))
				{						
					if (ImGui::MenuItem(u8"Выход"))
						BSE::Application::Get()->Close();
					ImGui::EndMenu();
				}
				ImGui::EndMenuBar();
			}
		
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
			ImGui::Begin(u8"Сцена");
				// TODO: AssetManager, GameData storage
				// ImGui::Image((void*)(BSE::AssetManager->GetTexture(GameData->Textures[0])->GetID()), {1.0f, 1.0f});
				ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
				if ((m_ViewportSize.x != viewportPanelSize.x) || (m_ViewportSize.y != viewportPanelSize.y)){
					m_ViewportSize = {viewportPanelSize.x, viewportPanelSize.y};
					BSE::GameData::m_FrameBuffer->Resize(m_ViewportSize);
					BSE::GameData::m_CameraController->OnResize(viewportPanelSize.x, viewportPanelSize.y);
				}
				// BSE_INFO("Viewport size is: {0}, {1}", viewportPanelSize.x, viewportPanelSize.y);
				uint32_t sceneTexture = BSE::GameData::m_FrameBuffer->GetColorAttachmentID();
				ImGui::Image(
					(void*)sceneTexture, 
					{m_ViewportSize.x, m_ViewportSize.y},
					{0, 1},
					{1, 0}
					);
			ImGui::End();
			ImGui::PopStyleVar();
		
			ImGui::Begin(u8"Разные настройки");
				ImGui::ColorPicker4(u8"Цвет квадратов", glm::value_ptr(BSE::GameData::CustomColor));
			ImGui::End();
		
		ImGui::End();
	}
	
private:
	float layerTime = 0.0f;
	glm::vec2 m_ViewportSize = {600.0f, 400.0f};
	BSE::Texture2D* broTexture;
};

class Editor : public BSE::Application {
public:
	Editor(){
		// PushLayer(new ExampleLayer());
		PushLayer(new SceneViewer());
		
		SetImGuiLayer(new EditorGuiLayer());
		PushOverlay(GetImGuiLayer());
		
		m_ImGuiLayerEnabled = true;
		m_WinTitle = "BSE Editor";
		m_Window->SetTitle(m_WinTitle);
		m_Window->SetVSync(false);
		// m_Window->SetVSync(true);
		
		BSE::GameData::FPS_deltaTime = BSE_FPS60;
	}
	
	~Editor(){
		BSE_INFO("Exit Editor");
	}
	
private:
	std::string m_WinTitle;
};

BSE::Application* BSE::CreateApplication() {
	return new Editor();
}
