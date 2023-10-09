#include "./SceneHierarchyPanel.h"
// #include <BSE_Client.h> // just to make RedPanda C++ parse well the code below
// #include <BSE/vendor/imgui/imgui_internal.h>

namespace BSE {
	SceneHierarchyPanel::SceneHierarchyPanel(Scene* scene){
		SetContext(scene);
	}
	
	void SceneHierarchyPanel::SetContext(Scene* scene){
		m_Context = scene;
	}
	
	void SceneHierarchyPanel::OnImGuiRender(){
		if (!m_SelectedEntity)
			m_SelectionContext = {};
		
		ImGui::Begin("Сущности на сцене");
		Entity entity;
		m_Context->Registry().each([this, &entity](auto entityID){
			entity = Entity(entityID, m_Context);
			DrawEntityNode(entity);
		});
		
		if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered()){
		 	m_SelectionContext = {};
			m_SelectedEntity = false;
		}
		
		// Opens if RMB clicked on empty space
		if (ImGui::BeginPopupContextWindow(0, ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems)){
			if (ImGui::MenuItem("Создать сущность")){
				m_Context->CreateEntity("Новая сущность");
			}
			ImGui::EndPopup();
		}
		ImGui::End();
		
		ImGui::Begin("Компоненты");
		if (m_SelectedEntity){
			DrawComponents(m_SelectionContext);
		}
		ImGui::End();
	}
	
	void SceneHierarchyPanel::DrawEntityNode(Entity& entity){
		auto& name = m_Context->Registry().get<NameComponent>(entity.GetID()).Name;
		// ImGui::Text("%s", name.c_str());
		
		ImGuiTreeNodeFlags flags = ((m_SelectionContext == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
		flags |= ImGuiTreeNodeFlags_SpanAvailWidth;
		bool opened = ImGui::TreeNodeEx((void*)(uint32_t)entity.GetID(), flags, name.c_str());
		bool deleted = false;
		
		if (ImGui::IsItemClicked()){
			m_SelectedEntity = true;
			m_SelectionContext = entity;
			// m_CurrentlySelectedEntity = entity;
			BSE_INFO("Clicked Entity id: {0}", (uint32_t)entity.GetID());
		}
		
		// Opens on RMB clicked
		if (ImGui::BeginPopupContextItem(0, ImGuiPopupFlags_MouseButtonRight)){
			if (ImGui::MenuItem("Удалить сущность")){
				m_Context->DestroyEntity(entity);
				deleted = true;
				m_SelectionContext = {};
				m_SelectedEntity = false;
			}
			ImGui::EndPopup();
		}
		
		// for child entities to be implemented in future
		if (opened && !deleted){
			ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
			bool opened = ImGui::TreeNodeEx((void*)((uint32_t)entity.GetID() + 10000), flags, name.c_str());
			if (opened){
				ImGui::TreePop();
			}
			ImGui::TreePop();
		}
	}
	
	template <typename Component>
	void SceneHierarchyPanel::DrawComponent(const char* label, Entity& entity, VoidFn func, bool canBeDeleted){
		ImGuiTreeNodeFlags treeNodeFlags = 
			ImGuiTreeNodeFlags_DefaultOpen |
			ImGuiTreeNodeFlags_Framed |
			ImGuiTreeNodeFlags_AllowItemOverlap |
			ImGuiTreeNodeFlags_SpanAvailWidth;
		
		if (entity.HasComponent<Component>()){
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{4, 4});
			ImVec2 contentRegionAvailiable = ImGui::GetContentRegionAvail();
			float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
			ImGui::Separator();
			
			bool open = ImGui::TreeNodeEx((void*)typeid(Component).hash_code(), treeNodeFlags, label);
			
			bool removeComponent = false;
			if (canBeDeleted){
				ImGui::SameLine(contentRegionAvailiable.x - lineHeight * 0.5f - 25.0f);
				if (ImGui::Button("...", ImVec2(lineHeight, lineHeight))){
					ImGui::OpenPopup("Component settings");
				}
				
				if (ImGui::BeginPopup("Component settings")){
					if (ImGui::MenuItem("Удалить компонент"))
						removeComponent = true;
					
					ImGui::EndPopup();
				}
			}
			ImGui::PopStyleVar();
			
			if (open) {
				func(entity);
				
				if (removeComponent)
					entity.RemoveComponent<Component>();
				
				ImGui::TreePop();
			}
		}
	}
	
	void SceneHierarchyPanel::DrawVec3Control(const std::string& label, glm::vec3& values, EmptyFn func, float resetValue, float columnWidth){
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
	
	void SceneHierarchyPanel::DrawComponents(Entity& entity){
		// BSE_INFO("Inspecting Entity id: {0}", (uint32_t)entity.GetID());
		DrawComponent<NameComponent>("Name", entity, [this](Entity& entity){
			auto& name = m_Context->Registry().get<NameComponent>(entity.GetID()).Name;
			
			ImGui::Columns(2);
			ImGui::SetColumnWidth(0, 200.0f);
			
			char buffer[256]; 
			sprintf(buffer, "##Имя%d", (int)entity.GetID());
			ImGui::PushItemWidth(-1);
			ImGui::InputText(buffer, &name, ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_EnterReturnsTrue);
			ImGui::PopItemWidth();
			
			ImGui::NextColumn();
			
			// float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
			if (ImGui::Button("Новый компонент", ImVec2{150.0f, 20.0f})){
				ImGui::OpenPopup("Add component");
			}
			
			if (ImGui::BeginPopup("Add component")){
				if (ImGui::MenuItem("Камера")){
					if (!m_SelectionContext.HasComponent<CameraControllerComponent>())
						m_SelectionContext.AddComponent<CameraControllerComponent>(
							(float)GameData::m_Window->GetWidth()/GameData::m_Window->GetHeight(),
							1.0f, 
							true, 
							true);
					ImGui::CloseCurrentPopup();
				}
				if (ImGui::MenuItem("Спрайт")){
					if (!m_SelectionContext.HasComponent<SpriteComponent>())
						m_SelectionContext.AddComponent<SpriteComponent>();
					ImGui::CloseCurrentPopup();
				}
				ImGui::EndPopup();
			}
			
			ImGui::Columns(1);
		}, false);
		
		DrawComponent<TransformComponent>("Transform", entity, [this](Entity& entity){
			auto& transform = m_Context->Registry().get<TransformComponent>(entity.GetID());
			
			DrawVec3Control("Позиция", transform.Translation);
			glm::vec3 rotation = glm::degrees(transform.Rotation);
			DrawVec3Control("Поворот", rotation);
			transform.Rotation = glm::radians(rotation);
			DrawVec3Control("Размер", transform.Scale, [](){}, 1.0f);
		}, false);
		
		// DrawComponent<NativeScriptComponent>("Native Script", entity, [this](Entity& entity){ });

		
		DrawComponent<CameraControllerComponent>("Camera Controller", entity, [this](Entity& entity){
			CameraController* cameraController = m_Context->Registry().get<CameraControllerComponent>(entity.GetID()).cameraController;
			
			if (ClientData::m_ActiveScene->GetCameraController() == cameraController){
				if (ImGui::Button("Отвязать от сцены##cameraComponent")){
					ClientData::m_ActiveScene->SetCameraController(nullptr);
					ClientData::EditorCameraOn = false;
				}
			} else {
				if (ImGui::Button("Привязать к сцене##cameraComponent")){
					ClientData::m_ActiveScene->SetCameraController(cameraController);
				}
			}
			
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
						cameraController->SetProjectionType((CameraProjectionType)i);
						cameraController->RefreshCamera();
					}
					
					if (isSelected){
						ImGui::SetItemDefaultFocus();
					}
				}
				ImGui::EndCombo();
			}
			
			if (cameraController->GetProjectionType() == CameraProjectionType::Perspective){
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
			
			if (cameraController->GetProjectionType() == CameraProjectionType::Orthographic){
				float zoomlevel = cameraController->GetZoomLevel();
				if (ImGui::DragFloat("Размер", &zoomlevel, 0.1f)){
					cameraController->SetZoomLevel(zoomlevel);
					cameraController->UpdateProjection();
				}
				float zNear = cameraController->GetOrthographicZNear();
				if (ImGui::DragFloat("Ближняя зона", &zNear, 0.1f)){
					cameraController->SetOrthographicZNear(zNear);
					cameraController->UpdateProjection();
				}
				
				float zFar = cameraController->GetOrthographicZFar();
				if (ImGui::DragFloat("Дальняя зона", &zFar, 0.1f)){
					cameraController->SetOrthographicZFar(zFar);
					cameraController->UpdateProjection();
				}
			}
			
			ImGui::Text("Соотношение сторон:"); 
			std::string buttonText = cameraController->constantAspectRatio ? "Постоянное" : "Переменное";
			if (ImGui::Button(buttonText.c_str())) {
				cameraController->constantAspectRatio = !cameraController->constantAspectRatio;
			}
			float aspectRatio = cameraController->GetAspectRatio();
			if (ImGui::DragFloat("Соотношение:", &aspectRatio, 0.1f)){
				cameraController->SetAspectRatio(aspectRatio);
				cameraController->UpdateProjection();
			}
		});
		
		DrawComponent<SpriteComponent>("Sprite", entity, [this](Entity& entity){
			auto& color = m_Context->Registry().get<SpriteComponent>(entity.GetID()).Color;
			auto& texture = m_Context->Registry().get<SpriteComponent>(entity.GetID()).Texture;
			auto& tilingFactor = m_Context->Registry().get<SpriteComponent>(entity.GetID()).TilingFactor;
			
			// color
			ImGui::ColorEdit4("Цвет", glm::value_ptr(color));
			
			// texture
			ImGui::Button("Текстура", ImVec2(100.0f, 0.0f));

			if (ImGui::BeginDragDropTarget()){
				const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Asset_Texture");
				if (payload != nullptr){
					std::filesystem::path path = std::string((char*)payload->Data);
					std::string filepath = (ClientData::g_AssetsDirectory / path).string();
					texture = Texture2D::Create(filepath);
					
					ImGui::EndDragDropTarget();
				}
			}
			
			// tiling factor
			ImGui::DragFloat("Плитка", &tilingFactor, 0.1f, 0.0f, 100.0f);
			
		});
	}
	
	//bool SceneHierarchyPanel::OnMouseButtonPressed(BSE::MouseButtonPressed& event){
	//	return true;
	//}
}
