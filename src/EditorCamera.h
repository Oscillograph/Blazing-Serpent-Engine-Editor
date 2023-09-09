#ifndef BSE_EDITOR_EDITORCAMERA_H
#define BSE_EDITOR_EDITORCAMERA_H

#include <BSE_Client.h>

namespace BSE {
	// This is basically The Cherno's Editor Camera class from:
	// https://www.youtube.com/watch?v=8zVtRpBTwe0
	
	// this has to have its own CameraController built into it
	class EditorCamera : public OrthographicCamera {
	public:
		EditorCamera() = default;
		EditorCamera(float fov, float aspectRatio, float nearClip, float farClip);
		
		void OnUpdate(float t);
		void OnEvent(Event& e);
		
		inline float GetDistance(){ return m_Distance; }
		inline float SetDistance(float distance){ m_Distance = distance; }
		
		inline void SetViewportSize(float width, float height){
			m_ViewportWidth = width;
			m_ViewportHeight = height;
			UpdateProjection();
		}
		
		// these two provided by OrthographicCamera class
		// const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }
		// glm::mat4 GetViewProjectionMatrix() const { return m_ViewProjectionMatrix; }
		
		glm::vec3 GetUpDirection();
		glm::vec3 GetRightDirection();
		glm::vec3 GetForwardDirection();
		// this one provided by OrthographicCamera class
		// const glm::vec3& GetPosition() const {return m_Position; }
		glm::quat GetOrientation();
		
		inline float GetPitch(){ return m_Pitch; }
		inline float GetYaw(){ return m_Yaw; }
		
	private:
		void UpdateProjection();
		void UpdateView();
		
		bool OnMouseScroll(MouseScrolledEvent& e);
		
		void MousePan(const glm::vec2 delta);
		void MouseRotate(const glm::vec2 delta);
		void MouseZoom(float delta);
		
		glm::vec3 CalculatePosition();
		
		std::pair<float, float> PanSpeed();
		float RotationSpeed();
		float ZoomSpeed();
		
		// --------------------------------------
		float m_FOV = glm::radians(45.0f);
		float m_AspectRatio = 1.778f;
		float m_NearClip = 0.1f;
		float m_FarClip = 1000.0f;
		float m_Distance = 0.0f;
		
		
		glm::mat4 m_ViewMatrix;
		glm::mat4 m_ProjectionMatrix;
		glm::mat4 m_ViewProjectionMatrix;
		
		glm::vec3 m_Position = {0.0f, 0.0f, -10.0f};
		glm::vec3 m_FocalPoint = {0.0f, 0.0f, 0.0f};
		
		glm::vec2 m_InitialMousePosition;
		
		float m_Pitch = 0.0f;
		float m_Yaw = 0.0f;
		float m_ViewportWidth = 1280.0f;
		float m_ViewportHeight = 768.0f;
	};
}

#endif
