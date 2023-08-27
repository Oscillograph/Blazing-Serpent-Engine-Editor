#include "./SceneHierarchyPanel.h"
#include <BSE_Client.h> // just to make RedPanda C++ parse well the code below

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
		ImGui::End();
		
		ImGui::Begin("Свойства");
		if (m_SelectedEntity){
			DrawComponents(m_SelectionContext);
		}
		ImGui::End();
	}
	
	void SceneHierarchyPanel::DrawEntityNode(Entity& entity){
		auto& name = m_Context->Registry().get<NameComponent>(entity.GetID()).Name;
		// ImGui::Text("%s", name.c_str());
		
		ImGuiTreeNodeFlags flags = ((m_SelectionContext == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
		bool opened = ImGui::TreeNodeEx((void*)(uint32_t)entity.GetID(), flags, name.c_str());
		
		if (ImGui::IsItemClicked()){
			m_SelectedEntity = true;
			m_SelectionContext = entity;
			// BSE_INFO("Clicked Entity id: {0}", (uint32_t)entity.GetID());
		}
		
		// for child entities to be implemented in future
		if (opened){
			ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;
			bool opened = ImGui::TreeNodeEx((void*)((uint32_t)entity.GetID() + 10000), flags, name.c_str());
			if (opened){
				ImGui::TreePop();
			}
			ImGui::TreePop();
		}
	}
	
	template <typename Component>
	void SceneHierarchyPanel::DrawComponent(const char* label, Entity& entity, VoidFn func){
		if (entity.HasComponent<Component>()){
			if (ImGui::TreeNodeEx((void*)typeid(Component).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, label)) {
				func(entity);
				ImGui::TreePop();
			}
		}
	}
	
	void SceneHierarchyPanel::DrawComponents(Entity& entity){
		// BSE_INFO("Inspecting Entity id: {0}", (uint32_t)entity.GetID());
		DrawComponent<NameComponent>("Name", entity, [this](Entity& entity){
			auto& name = m_Context->Registry().get<NameComponent>(entity.GetID()).Name;
			char buffer[256]; 
			sprintf(buffer, "Имя##%d", (int)entity.GetID());
			ImGui::InputText(buffer, &name);
		});
		
		DrawComponent<TransformComponent>("Transform", entity, [this](Entity& entity){
			auto& transform = m_Context->Registry().get<TransformComponent>(entity.GetID());
			char buffer[256]; 
			sprintf(buffer, "Позиция##%d", (int)entity.GetID());
			ImGui::DragFloat3(buffer, glm::value_ptr(transform.Translation), 0.1f);
			
			sprintf(buffer, "Поворот##%d", (int)entity.GetID());
			ImGui::DragFloat3(buffer, glm::value_ptr(transform.Rotation), 0.1f);
			
			sprintf(buffer, "Размер##%d", (int)entity.GetID());
			ImGui::DragFloat3(buffer, glm::value_ptr(transform.Scale), 0.1f);
		});
		
		// DrawComponent<NativeScriptComponent>("Native Script", entity, [this](Entity& entity){ });

		
		DrawComponent<CameraControllerComponent>("Camera Controller", entity, [this](Entity& entity){
			auto& cameraController = m_Context->Registry().get<CameraControllerComponent>(entity.GetID()).CameraController;
			
			if (ClientData::m_ActiveScene->GetCameraController() == cameraController){
				if (ImGui::Button("Отвязать от сцены##cameraComponent")){
					ClientData::m_ActiveScene->SetCameraController(nullptr);
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
				glm::vec3 rotation = cameraController->GetCamera()->GetRotation();
				if (ImGui::DragFloat3("Поворот", glm::value_ptr(rotation), 0.1f)){
					cameraController->Rotate(rotation);
				}
				
				glm::vec3 position = cameraController->GetCamera()->GetPosition();
				if (ImGui::DragFloat3("Положение", glm::value_ptr(position), 0.1f)){
					cameraController->GetCamera()->SetPosition(position);
					cameraController->SetProjectionDefault();
				}
				
				float fov = cameraController->GetPerspectiveVerticalFOV();
				if (ImGui::DragFloat("FOV", &fov, 0.1f)){
					cameraController->SetPerspectiveVerticalFOV(fov);
				}
				
				float zNear = cameraController->GetPerspectiveNear();
				if (ImGui::DragFloat("Ближняя зона", &zNear, 0.1f)){
					cameraController->SetPerspectiveNear(zNear);
				}
				
				float zFar = cameraController->GetPerspectiveFar();
				if (ImGui::DragFloat("Дальняя зона", &zFar, 0.1f)){
					cameraController->SetPerspectiveFar(zFar);
				}
			}
			
			if (cameraController->GetProjectionType() == CameraProjectionType::Orthographic){
				float zoomlevel = cameraController->GetZoomLevel();
				if (ImGui::DragFloat("Размер", &zoomlevel, 0.1f)){
					cameraController->SetZoomLevel(zoomlevel);
				}
				float zNear = cameraController->GetOrthographicZNear();
				if (ImGui::DragFloat("Ближняя зона", &zNear, 0.1f)){
					cameraController->SetOrthographicZNear(zNear);
				}
				
				float zFar = cameraController->GetOrthographicZFar();
				if (ImGui::DragFloat("Дальняя зона", &zFar, 0.1f)){
					cameraController->SetOrthographicZFar(zFar);
				}
			}
		});
		
		DrawComponent<SpriteComponent>("Sprite", entity, [this](Entity& entity){
			auto& color = m_Context->Registry().get<SpriteComponent>(entity.GetID()).Color;
			
			ImGui::ColorEdit4("Цвет", glm::value_ptr(color));
		});
	}
}
