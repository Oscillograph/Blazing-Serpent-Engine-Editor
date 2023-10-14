#ifndef BSE_EDITOR_EDITORCAMERACONTROLLER_H
#define BSE_EDITOR_EDITORCAMERACONTROLLER_H

#include <BSE_Client.h>
#include "./EditorCamera.h"

namespace BSE {
	class EditorCameraController : public GeneralCameraController {
	public:
		EditorCameraController(float fov = 45.0f, float aspectRatio = 1.778f, float znear = 0.1f, float zfar = 1000.0f);
		~EditorCameraController();
		
		inline CameraProjectionType GetProjectionType(){ return m_ProjectionType; }
		inline void SetProjectionType(CameraProjectionType projectionType){ m_ProjectionType = projectionType; }
		
		// Perspective stuff
		inline float GetPerspectiveVerticalFOV(){ return m_PerspectiveVerticalFOV; }
		inline float GetPerspectiveHorizontalFOV(){ return m_PerspectiveHorizontalFOV; }
		inline float GetPerspectiveNear(){ return m_PerspectiveNear; }
		inline float GetPerspectiveFar(){ return m_PerspectiveFar; }
		
		inline void SetPerspectiveVerticalFOV(float value){ m_PerspectiveVerticalFOV = value; SetProjectionDefault(); }
		inline void SetPerspectiveHorizontalFOV(float value){ m_PerspectiveHorizontalFOV = value; SetProjectionDefault(); }
		inline void SetPerspectiveNear(float value){ m_PerspectiveNear = value; SetProjectionDefault(); }
		inline void SetPerspectiveFar(float value){ m_PerspectiveFar = value; SetProjectionDefault(); }
		
		// Orthographic stuff
		inline float GetOrthographicZNear(){ return m_Camera->GetNearClip(); }
		inline void SetOrthographicZNear(float zNear){ m_Camera->SetNearClip(zNear); }
		
		inline float GetOrthographicZFar(){ return m_Camera->GetFarClip(); }
		inline void SetOrthographicZFar(float zFar){ m_Camera->SetFarClip(zFar);}
		
		// void SetProjection(float left, float right, float top, float bottom);
		inline void SetOrthographicProjectionDefault(){
			BSE_INFO("SetOrthographicProjectionDefault(); // Deprecated function");
		}
		
		inline void SetPerspectiveProjectionDefault(){
			glm::mat4 projection = glm::perspective(
				m_Camera->GetFOV(), 
				m_Camera->GetAspectRatio(), 
				m_Camera->GetNearClip(),
				m_Camera->GetFarClip()
				);
			m_Camera->SetProjection(projection);
		}
		
		inline void SetProjectionDefault(){
			SetPerspectiveProjectionDefault();
		}
		
		inline void SetZoomLevel(float zoomLevel){
			m_ZoomLevel = zoomLevel;
			m_Camera->SetDistance(m_ZoomLevel);
			SetProjectionDefault();
		}
		inline float GetZoomLevel() {
			m_ZoomLevel = m_Camera->GetDistance();
			return m_ZoomLevel; 
		}
		
		inline float GetAspectRatio() { return m_AspectRatio; }
		inline void SetAspectRatio(float aspectRatio){
			m_AspectRatio = aspectRatio;
			m_Camera->SetAspectRatio(m_AspectRatio);
			SetProjectionDefault();
		}
		
		inline void RefreshCamera() { SetProjectionDefault(); }
		
		inline void SetSize(float width, float height) {
			m_Width = width;
			m_Height = height;
			m_Camera->SetViewPort(width, height);
		}
		inline void SetSize(uint32_t width, uint32_t height, float size) {
			m_Width = (float)width;
			m_Height = (float)height;
			m_Camera->SetViewPort(m_Width, m_Height);
		}
		
		inline void SetConstantAspectRatio(bool isConstant) { m_ConstantAspectRatio = isConstant; }
		inline bool GetConstantAspectRatio() { return m_ConstantAspectRatio; }
		inline bool ToggleConstantAspectRatio() {
			m_ConstantAspectRatio = m_ConstantAspectRatio == true ? false : true; 
			return m_ConstantAspectRatio; 
		}
		
		
		
		inline void AllowRotation(bool isAllowed) { m_Rotation = isAllowed; }
		inline bool RotationStatus() { return m_Rotation; }
		inline void Rotate(const glm::vec3& rotation) { m_Camera->SetRotation(rotation); }
		inline void Rotate(float rotation) { m_Camera->SetRotation(rotation); }
		
		inline OrthographicCameraBounds& GetBounds() { return m_CameraBounds; }
		
		void OnUpdate(float time) override;
		void OnResize(float width, float height) override;
		void OnEvent(Event& e) override;
		
		inline OrthographicCamera* GetCamera() { return m_Camera; }
		inline void SetCamera(EditorCamera* camera, bool destroyPrevCamera = false) {
			if (destroyPrevCamera && (m_Camera != nullptr)){
				delete m_Camera;
			}
			m_Camera = camera; 
		}
	private:
		CameraProjectionType m_ProjectionType;
		
		bool OnMouseScrolled(MouseScrolledEvent& e);
		bool OnWindowResized(WindowResizeEvent& e);
		
		float m_AspectRatio = 1.0f;
		float m_AspectRatioPrev = 1.0f;
		float m_WidthPrev = 0.0f;
		float m_HeightPrev = 0.0f;
		
		float m_ZoomLevel = 1.0f;
		float m_ZoomMin = -2.0f;
		float m_ZoomMax = 2.0f;
		
		float m_Width = 600.0f;
		float m_Height = 400.0f;
		float m_Size = 10.0f;
		
		// GeneralCamera* m_Camera;
		EditorCamera* m_Camera;
		
		bool m_Rotation;
		bool m_ConstantAspectRatio = true;
		
		glm::vec3 m_CameraPosition = {0.0f, 0.0f, 0.0f};
		float m_CameraMoveSpeed = 5.0f;
		float m_CameraRotateSpeed = 10.0f;
		
		OrthographicCameraBounds m_CameraBounds;
		
		// Perspective stuff
		float m_PerspectiveHorizontalFOV = 60.0f;
		float m_PerspectiveVerticalFOV = 45.0f;
		float m_PerspectiveNear = 0.01f;
		float m_PerspectiveFar = 1000.0f;
	};
}

#endif
