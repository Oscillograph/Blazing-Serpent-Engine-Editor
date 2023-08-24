#include "./SceneHierarchyPanel.h"

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
		
		if (opened){
			ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;
			bool opened = ImGui::TreeNodeEx((void*)((uint32_t)entity.GetID() + 1000), flags, name.c_str());
			if (opened){
				ImGui::TreePop();
			}
			ImGui::TreePop();
		}
	}
	
	void SceneHierarchyPanel::DrawComponents(Entity& entity){
		// BSE_INFO("Inspecting Entity id: {0}", (uint32_t)entity.GetID());
		if (entity.HasComponent<NameComponent>()){
			if (ImGui::TreeNodeEx((void*)typeid(NameComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Name")) {
				auto& name = m_Context->Registry().get<NameComponent>(entity.GetID()).Name;
				static char nameBuffer[256];
				memset(nameBuffer, 0, sizeof(nameBuffer));
				strcpy_s(nameBuffer, sizeof(nameBuffer), name.c_str());
				
				if (ImGui::InputText("Имя", nameBuffer, sizeof(nameBuffer))){
					name = std::string(nameBuffer);
				}
				ImGui::TreePop();
			}
		}
		
		if (entity.HasComponent<TransformComponent>()){
			if (ImGui::TreeNodeEx((void*)typeid(TransformComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Transform")) {
				auto& transform = m_Context->Registry().get<TransformComponent>(entity.GetID()).Transform;
				if (ImGui::DragFloat3("Позиция", glm::value_ptr(transform[3]), 0.5f)){
					// somefin if needed
				}
				ImGui::TreePop();
			}
		}
		
		if (entity.HasComponent<NativeScriptComponent>()){
			
		}

		if (entity.HasComponent<CameraControllerComponent>()){
			if (ImGui::TreeNodeEx((void*)typeid(CameraControllerComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Camera Controller")) {
				auto& cameraController = m_Context->Registry().get<CameraControllerComponent>(entity.GetID()).CameraController;
				glm::vec3 rotation = cameraController->GetCamera()->GetRotation();
				if (ImGui::DragFloat3("Поворот", glm::value_ptr(rotation), 0.1f)){
					cameraController->Rotate(rotation);
				}
				glm::vec3 position = cameraController->GetCamera()->GetPosition();
				if (ImGui::DragFloat3("Положение", glm::value_ptr(position), 0.1f)){
					cameraController->GetCamera()->SetPosition(position);
				}
				ImGui::TreePop();
			}
		}
	}
}
