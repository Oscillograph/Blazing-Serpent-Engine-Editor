#include <BSE_Client.h>
#include <EntryPoint.h>

#include "hacks.h"
#include "./src/CameraScript.h"
#include "./src/ClientData.h"
#include "./src/SceneViewer.h"

// Panels
#include "./src/Panels/SceneHierarchyPanel.h"

class EditorGuiLayer : public BSE::ImGuiLayer {
public:
	EditorGuiLayer() {
		broTexture = BSE::Texture2D::Create("./assets/img/broscillograph.png");
		broTexture->Bind();
		
		// Panels setup
		m_Panel = new BSE::SceneHierarchyPanel();
	}
	
	~EditorGuiLayer() {
		if (broTexture != nullptr)
			delete broTexture;
	}
	
	void OnImGuiRender(float time) {
		FixImGuiContext(m_ImGuiContext);

		// a big part of this code is taken from ShowDockSpaceExample or sort of that function name in imgui_demo.cpp
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
			// m_FontDefault->Scale = 2.0f;
			ImGui::PushFont(m_FontDefault);
		
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
				if (ImGui::BeginMenu("Файл"))
				{	
					if (ImGui::MenuItem("Новая сцена", "Ctrl-N")){
						NewScene();
					}
					if (ImGui::MenuItem("Загрузить...", "Ctrl-O")){
						OpenScene();
					}
					if (ImGui::MenuItem("Сохранить, как...", "Ctrl-S")){
						SaveSceneAs();
					}
					ImGui::MenuItem("- - - - -");
					if (ImGui::MenuItem("Выход"))
						BSE::Application::Get()->Close();
					ImGui::EndMenu();
				}
				ImGui::EndMenuBar();
			}
		
			ImGui::Begin("Разные настройки");
				ImGui::ColorPicker4("Цвет квадратов", glm::value_ptr(BSE::GameData::CustomColor));
		
				/*
				if (ImGui::Button("Сохранить...")){
					
				}
				if (ImGui::Button("Загрузить...")){
					
				}
				*/
		
				if (m_GizmoType != -1){
					ImGui::Text("3D-Манипулятор");
					std::string buttonText = "";
					switch (m_GizmoType) {
						case ImGuizmo::OPERATION::TRANSLATE: buttonText = "Перемещение"; break; // no Gizmo
						case ImGuizmo::OPERATION::ROTATE: buttonText = "Поворот"; break;
						case ImGuizmo::OPERATION::SCALE: buttonText = "Размер"; break;
					default: buttonText = "???"; break;
					}
					if (ImGui::Button(buttonText.c_str())) {
						m_GizmoType++;
						if (m_GizmoType > 2)
							m_GizmoType = 0;
					}
				}
			ImGui::End();
		
			ImGui::Begin("Квадратный фон");
				ImGui::Text("По горизонтали:");
				ImGui::DragInt("##maxX", &(ClientData::quadsMaxX), 1);
				ImGui::Text("По вертикали:");
				ImGui::DragInt("##maxY", &(ClientData::quadsMaxY), 1);
			ImGui::End();
		
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
			ImGui::Begin("Сцена");
				// TODO: AssetManager, GameData storage
				// ImGui::Image((void*)(BSE::AssetManager->GetTexture(GameData->Textures[0])->GetID()), {1.0f, 1.0f});
				ClientData::ViewPortFocused = ImGui::IsWindowFocused();
				ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
				if ((m_ViewportSize.x != viewportPanelSize.x) || (m_ViewportSize.y != viewportPanelSize.y)){
					m_ViewportSize = {viewportPanelSize.x, viewportPanelSize.y};
					BSE::GameData::m_FrameBuffer->Resize(m_ViewportSize);
					if (ClientData::m_ActiveScene->GetCameraController() != nullptr)
						ClientData::m_ActiveScene->GetCameraController()->OnResize(viewportPanelSize.x, viewportPanelSize.y);
					// ClientData::m_ActiveScene->GetCameraController()->RefreshCamera();
					// ClientData::m_ActiveScene->GetCameraController()->OnUpdate(time);
				}
				// BSE_INFO("Viewport size is: {0}, {1}", viewportPanelSize.x, viewportPanelSize.y);
				uint32_t sceneTexture;
				if ((0 < viewportPanelSize.x) && (0 < viewportPanelSize.y)){
					sceneTexture = BSE::GameData::m_FrameBuffer->GetColorAttachmentID();
				} else {
					sceneTexture = broTexture->GetID();
				}
					
				ImGui::Image(
					(void*)sceneTexture, 
					{m_ViewportSize.x, m_ViewportSize.y},
					{0, 1},
					{1, 0}
					);
		
				// ============================================
				// Gizmos
				// m_GizmoType = ImGuizmo::OPERATION::TRANSLATE;
				if (m_Panel->IsSelectedEntity()){
					if (m_GizmoType == -1)
						m_GizmoType = ImGuizmo::OPERATION::TRANSLATE;
					BSE::Entity selectedEntity = m_Panel->GetSelectedEntity();
					ImGuizmo::SetOrthographic(false);
					ImGuizmo::SetDrawlist();
					float windowWidth = (float)ImGui::GetWindowWidth();
					float windowHeight = (float)ImGui::GetWindowHeight();
					ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, windowWidth, windowHeight);
					
					// Camera controller and camera itself 
					auto cameraController = ClientData::m_ActiveScene->GetCameraController();
					auto camera = ClientData::m_ActiveScene->GetCameraController()->GetCamera();
					
					glm::mat4 cameraView = camera->GetViewMatrix();
					glm::mat4 cameraProjection = camera->GetProjectionMatrix();
					
					// Entity transform
					auto& transformComponent = selectedEntity.GetComponent<BSE::TransformComponent>();
					glm::mat4 transform = transformComponent.GetTransform();
					
					// ----------------------
					// Snap to grid section
					bool snap = BSE::Input::IsKeyPressed(BSE::KeyCode::LeftControl); // TODO: allow to customize the key
					float snapValue;
					switch (m_GizmoType) {
						case ImGuizmo::OPERATION::TRANSLATE: snapValue = 0.5f; break; // no Gizmo
						case ImGuizmo::OPERATION::ROTATE: snapValue = 0.3925f; break;
						case ImGuizmo::OPERATION::SCALE: snapValue = 0.5f; break;
					}
					float snapValues[3] = { snapValue, snapValue, snapValue };
					// ----------------------
					
					ImGuizmo::Manipulate(
						glm::value_ptr(cameraView), 
						glm::value_ptr(cameraProjection), 
						(ImGuizmo::OPERATION)m_GizmoType, 
						ImGuizmo::LOCAL, 
						glm::value_ptr(transform),
						nullptr,
						snap ? snapValues : nullptr
						);
					
					if (ImGuizmo::IsUsing) {
						/*
						glm::vec3 translation, scale;
						glm::quat rotation;
						translation = transform[3];

						for(int i = 0; i < 3; i++)
							scale[i] = glm::length(glm::vec3(transform[i]));
						const glm::mat3 rotMtx(
							glm::vec3(transform[0]) / scale[0],
							glm::vec3(transform[1]) / scale[1],
							glm::vec3(transform[2]) / scale[2]);
						rotation = glm::quat_cast(rotMtx);
						
						transformComponent.Translation = translation; 
						transformComponent.Rotation = glm::eulerAngles(rotation);
						transformComponent.Scale = scale;
						*/
						
						
						glm::vec3 scale;
						glm::quat rotation;
						glm::vec3 translation;
						glm::vec3 skew;
						glm::vec4 perspective;
						glm::decompose(transform, scale, rotation, translation, skew, perspective);
						
						transformComponent.Translation = translation;
						// rotation = glm::eulerAngles(glm::conjugate(rotation));
						// glm::vec3 rot = glm::eulerAngles(glm::conjugate(rotation));
						glm::vec3 rot = -glm::eulerAngles(glm::conjugate(rotation));
						glm::vec3 deltaRotation = rot - transformComponent.Rotation;
						BSE_CORE_INFO("PX: {0}, PY: {1}, PZ: {2}", 
							transformComponent.Rotation[0], transformComponent.Rotation[1], transformComponent.Rotation[2]);
						BSE_CORE_INFO("CX: {0}, CY: {1}, CZ: {2}", 
							rot[0], rot[1], rot[2]);
						// rounding up to the fourth digit after the dot looks nice
						for (int i = 0; i < 3; i++){
							deltaRotation[i] = roundf(10000 * deltaRotation[i])/10000;
						}
						BSE_CORE_INFO("DX: {0}, DY: {1}, DZ: {2}", 
							deltaRotation[0], deltaRotation[1], deltaRotation[2]);
						transformComponent.Rotation += deltaRotation;
						transformComponent.Scale = scale;
						
						
						/*
						glm::vec3 translation, rotation, scale;
						BSE::Math::Mat4Decompose(transform, translation, rotation, scale);
						transformComponent.Translation = translation;
						
						// MAGIC? Aimed to fix gimble lock problem, according to The Cherno
						glm::vec3 deltaRotation = glm::conjugate(rotation) - transformComponent.Rotation; 
						transformComponent.Rotation += deltaRotation;
						
						transformComponent.Scale = scale;
						*/
					}
				} else {
					m_GizmoType = -1;
				}
				// ============================================
				
			ImGui::End();
			ImGui::PopStyleVar();
		
			m_Panel->SetContext(ClientData::m_ActiveScene);
			m_Panel->OnImGuiRender();
		
			ImGui::PopFont();
		ImGui::End();
	}
	
	void OnEvent(BSE::Event& e){
		BSE::EventDispatcher dispatcher(e);
		dispatcher.Dispatch<BSE::KeyPressedEvent>([this](BSE::KeyPressedEvent& event){
			return OnKeyPressed(event);
		});
	}
	
	bool OnKeyPressed(BSE::KeyPressedEvent& event){
		if (event.GetRepeatCount() > 0)
			return false;
		
		bool control = BSE::Input::IsKeyPressed(BSE::KeyCode::LeftControl) || BSE::Input::IsKeyPressed(BSE::KeyCode::RightControl);
		bool shift = BSE::Input::IsKeyPressed(BSE::KeyCode::LeftShift) || BSE::Input::IsKeyPressed(BSE::KeyCode::RightShift);
		
		if (event.GetKeyCode() == (int)BSE::KeyCode::S){
			if (control){
				SaveSceneAs();
				return true;
			}
		}
		if (event.GetKeyCode() == (int)BSE::KeyCode::O){
			if (control){
				OpenScene();
				return true;
			}
		}
		if (event.GetKeyCode() == (int)BSE::KeyCode::N){
			if (control){
				NewScene();
				return true;
			}
		}
		
		return false;
	}
	
	void SaveSceneAs(){
		std::string filepath = BSE::FileDialogs::SaveFile("Blazing Serpent Engine Scene (*.BSEScene)\0*.BSEScene\0");
		if (filepath.size() > 0){
			BSE::SceneSerializer serializer(ClientData::m_ActiveScene);
			serializer.SerializeToFile(filepath);
		}
	}
	
	void OpenScene(){
		std::string filepath = BSE::FileDialogs::OpenFile("Blazing Serpent Engine Scene (*.BSEScene)\0*.BSEScene\0");
		if (filepath.size() > 0){
			NewScene();
			
			BSE::SceneSerializer serializer(ClientData::m_ActiveScene);
			serializer.DeserializeFromFile(filepath);
		}
	}
	
	void NewScene(){
		ClientData::NewScene();
	}
	
	void CloseScene(){}
	
private:
	float layerTime = 0.0f;
	glm::vec2 m_ViewportSize = {600.0f, 400.0f};
	BSE::Texture2D* broTexture;
	
	// Panels
	BSE::SceneHierarchyPanel* m_Panel = nullptr;
	
	// tools
	int m_GizmoType = -1;
};

class Editor : public BSE::Application {
public:
	Editor(){
		// PushLayer(new ExampleLayer());
		PushLayer(new SceneViewer());
		
		SetImGuiLayer(new EditorGuiLayer());
		PushOverlay(GetImGuiLayer());
		
		m_ImGuiLayerEnabled = true;
		// TODO: find out why m_WinTitle is overridden by the engine library
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
