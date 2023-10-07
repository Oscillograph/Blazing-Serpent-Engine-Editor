#include <BSE_Client.h>
#include <EntryPoint.h>

#include "hacks.h"
#include "./src/CameraScript.h"
#include "./src/ClientData.h"
#include "./src/SceneViewer.h"

// Panels
#include "./src/Panels/SceneHierarchyPanel.h"
#include "./src/Panels/ContentBrowserPanel.h"

using VoidFn = std::function<void(BSE::Entity& entity)>;
using EmptyFn = std::function<void()>;
void DrawVec3Control(	const std::string& label, 
						glm::vec3& values, 
						EmptyFn func = [](){}, 
						float resetValue = 0.0f, 
						float columnWidth = 100.0f
	){
	ImGuiContext* GImGui = ImGui::GetCurrentContext();
	char buffer[256]; 
	
	ImGui::Columns(2);
	ImGui::SetColumnWidth(0, columnWidth);
	ImGui::Text(label.c_str());
	ImGui::NextColumn();
	
	ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{0, 0});
	
	float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
	ImVec2 buttonSize = {lineHeight + 3.0f, lineHeight};
	
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.8f, 0.3f, 0.15f, 1.0f});
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{0.8f, 0.6f, 0.4f, 1.0f});
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{1.0f, 0.7f, 0.7f, 1.0f});
	sprintf(buffer, "X##x%s", label.c_str());
	if (ImGui::Button(buffer, buttonSize)){
		values.x = resetValue;
		func();
	}
	ImGui::SameLine();
	sprintf(buffer, "##x%s", label.c_str());
	if (ImGui::DragFloat(buffer, &values.x, 0.1f)){
		func();
	}
	ImGui::PopItemWidth();
	ImGui::PopStyleColor(3);
	ImGui::SameLine();
	
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.3f, 0.8f, 0.15f, 1.0f});
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{0.6f, 0.8f, 0.4f, 1.0f});
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{0.7f, 1.0f, 0.7f, 1.0f});
	sprintf(buffer, "Y##y%s", label.c_str());
	if (ImGui::Button(buffer, buttonSize)){
		values.y = resetValue;
		func();
	}
	ImGui::SameLine();
	sprintf(buffer, "##y%s", label.c_str());
	if (ImGui::DragFloat(buffer, &values.y, 0.1f)){
		func();
	}
	ImGui::PopItemWidth();
	ImGui::PopStyleColor(3);
	ImGui::SameLine();
	
	
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.15f, 0.3f, 0.8f, 1.0f});
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{0.4f, 0.6f, 0.8f, 1.0f});
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{0.7f, 0.7f, 1.0f, 1.0f});
	sprintf(buffer, "Z##z%s", label.c_str());
	if (ImGui::Button(buffer, buttonSize)){
		values.z = resetValue;
		func();
	}
	ImGui::SameLine();
	sprintf(buffer, "##z%s", label.c_str());
	if (ImGui::DragFloat(buffer, &values.z, 0.1f)){
		func();
	}
	ImGui::PopItemWidth();
	// ImGui::SameLine();
	ImGui::PopStyleColor(3);
	ImGui::PopStyleVar();
	ImGui::Columns(1);
}

class EditorGuiLayer : public BSE::ImGuiLayer {
public:
	EditorGuiLayer() {
		broTexture = BSE::Texture2D::Create("./assets/img/broscillograph.png");
		broTexture->Bind();
		
		// Panels setup
		m_Panel = new BSE::SceneHierarchyPanel();
		m_ContentBrowserPanel = new BSE::ContentBrowserPanel();
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
		
				{
					auto cameraController = BSE::GameData::m_CameraController;
					
					const char* projectionTypeStrings[] = {
						"Перспектива",
						"Ортография"
					};
					const char* currentProjectionTypeString = projectionTypeStrings[(int)cameraController->GetProjectionType()];
					if (ImGui::BeginCombo("Тип проекции", currentProjectionTypeString)){
						for (int i = 0; i < 2; i++){
							bool isSelected = currentProjectionTypeString == projectionTypeStrings[i];
							if (ImGui::Selectable(projectionTypeStrings[i], isSelected)){
								currentProjectionTypeString = projectionTypeStrings[i];
								cameraController->SetProjectionType((BSE::CameraProjectionType)i);
								cameraController->RefreshCamera();
							}
							
							if (isSelected){
								ImGui::SetItemDefaultFocus();
							}
						}
						ImGui::EndCombo();
					}
					
					glm::vec3 rotation = cameraController->GetRotation();
					DrawVec3Control("Поворот", rotation, [&rotation, &cameraController](){
						cameraController->Rotate(rotation);
						cameraController->UpdateView();
					});
					
					glm::vec3 position = cameraController->GetCameraPosition();
					DrawVec3Control("Положение", position, [&position, &cameraController](){
						cameraController->SetCameraPosition(position);
						cameraController->UpdateView();
					});
					
					float fov = cameraController->GetPerspectiveFOV();
					if (ImGui::DragFloat("FOV", &fov, 0.1f)){
						cameraController->SetPerspectiveFOV(fov);
						cameraController->UpdateProjection();
					}
					
					float zNear = cameraController->GetPerspectiveNear();
					if (ImGui::DragFloat("Ближняя зона", &zNear, 0.1f)){
						cameraController->SetPerspectiveNear(zNear);
						cameraController->UpdateProjection();
					}
					
					float zFar = cameraController->GetPerspectiveFar();
					if (ImGui::DragFloat("Дальняя зона", &zFar, 0.1f)){
						cameraController->SetPerspectiveFar(zFar);
						cameraController->UpdateProjection();
					}
				}
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
		
				// capture mouse coordinates on scene texture
				ImVec2 viewportOffset = ImGui::GetCursorPos();
				ImVec2 windowSize = ImGui::GetWindowSize();
				
				// ImVec2 viewportMinRegion = ImGui::GetWindowContentRegionMin();
				// ImVec2 viewportMaxRegion = ImGui::GetWindowContentRegionMax();
				ImVec2 minBound = ImGui::GetWindowPos();
				// minBound.x += viewportOffset.x;
				// minBound.y += viewportOffset.y;
		
				ImVec2 maxBound = {minBound.x + windowSize.x, minBound.y + windowSize.y};
				// m_ViewportBounds[0] = {viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y};
				// m_ViewportBounds[1] = {viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y};
				m_ViewportBounds[0] = {minBound.x, minBound.y};
				m_ViewportBounds[1] = {maxBound.x, maxBound.y};
		
				auto [mx, my] = ImGui::GetMousePos();
				mx -= m_ViewportBounds[0].x;
				my -= m_ViewportBounds[0].y;
				glm::vec2 viewportSize = m_ViewportBounds[1] - m_ViewportBounds[0];
				my = viewportSize.y - my; // flip coordinates to match OpenGL system
				
				int mouseX = (int)mx;
				int mouseY = (int)my;
				if ((mouseX > 0) && (mouseY > 0) && (mouseX < (int)viewportSize.x) && (mouseY < (int)viewportSize.y)) {
					// ClientData::m_FrameBuffer->Bind();
					int pixelData = ClientData::m_FrameBuffer->ReadPixel(1, mouseX, mouseY);
					if (BSE::Input::IsMouseButtonPressed((int)BSE::KeyCode::MouseButtonLeft)){
						// BSE_CORE_WARN("Mouse: {0}, {1} - {2}", mx, my, pixelData);
						if (pixelData >= 0){
							BSE::Entity tempEntity((entt::entity)pixelData, ClientData::m_ActiveScene);
							m_Panel->SelectEntity(tempEntity);
						} else {
							m_Panel->DeSelectEntity();
						}
					}
					// ClientData::m_FrameBuffer->Unbind();
				}
		
				// import from content browser panel
				if (ImGui::BeginDragDropTarget()){
					const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Content_Browser_Item");
					if (payload != nullptr){
						std::filesystem::path path = std::string((char*)payload->Data);
						std::string filepath = (ClientData::g_AssetsDirectory / path).string();
						OpenScene(filepath);
						ImGui::EndDragDropTarget();
					}
				}  
		
				// ============================================
				// Gizmos
				// m_GizmoType = ImGuizmo::OPERATION::TRANSLATE;
				// TODO: Fix gizmo mismatch an object
				if (m_Panel->IsSelectedEntity()){
					if (m_GizmoType == -1)
						m_GizmoType = ImGuizmo::OPERATION::TRANSLATE;
					BSE::Entity selectedEntity = m_Panel->GetSelectedEntity();
					ImGuizmo::SetOrthographic(false);
					ImGuizmo::SetDrawlist();
					
					//ImGuizmo::SetRect(
					//	ImGui::GetWindowPos().x, 
					//	ImGui::GetWindowPos().y, 
					//	(float)ImGui::GetWindowWidth(), 
					//	(float)ImGui::GetWindowHeight()
					//	);
					
					ImGuizmo::SetRect(
						m_ViewportBounds[0].x, 
						m_ViewportBounds[0].y, 
						m_ViewportBounds[1].x - m_ViewportBounds[0].x, 
						m_ViewportBounds[1].y - m_ViewportBounds[0].y
						);
					
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
						// case ImGuizmo::OPERATION::ROTATE: snapValue = 0.3925f; break;
						case ImGuizmo::OPERATION::ROTATE: snapValue = 45.0f; break;
						case ImGuizmo::OPERATION::SCALE: snapValue = 0.5f; break;
					}
					float snapValues[3] = { snapValue, snapValue, snapValue };
					// ----------------------
					
					ImGuizmo::Manipulate(
						glm::value_ptr(cameraView), 
						glm::value_ptr(cameraProjection),
						// glm::value_ptr(transform),
						(ImGuizmo::OPERATION)m_GizmoType, 
						ImGuizmo::LOCAL,
						glm::value_ptr(transform),
						nullptr,
						snap ? snapValues : nullptr
						);
					
					if (ImGuizmo::IsUsing) {
						glm::vec3 scale;
						glm::quat rotation;
						glm::vec3 translation;
						glm::vec3 skew;
						glm::vec4 perspective;
						glm::decompose(transform, scale, rotation, translation, skew, perspective);
						
						transformComponent.Translation = translation;
						transformComponent.Rotation = -glm::eulerAngles(glm::conjugate(rotation));;
						transformComponent.Scale = scale;
					}
				} else {
					m_GizmoType = -1;
				}
				// ============================================
				
			ImGui::End();
			ImGui::PopStyleVar();
		
			m_Panel->SetContext(ClientData::m_ActiveScene);
			m_Panel->OnImGuiRender();
		
			m_ContentBrowserPanel->SetContext(ClientData::m_ActiveScene);
			m_ContentBrowserPanel->OnImGuiRender();
		
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
	
	void OpenScene(const std::string& filepath){
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
	glm::vec2 m_ViewportBounds[2];
	BSE::Texture2D* broTexture;
	
	// Panels
	BSE::SceneHierarchyPanel* m_Panel = nullptr;
	BSE::ContentBrowserPanel* m_ContentBrowserPanel = nullptr;
	
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
