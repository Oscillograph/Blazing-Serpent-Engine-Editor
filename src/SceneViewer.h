#ifndef BSEEDITOR_SCENEVIEWER_H
#define BSEEDITOR_SCENEVIEWER_H

#include <BSE_Client.h>

class SceneViewer : public BSE::Layer {
public:
	SceneViewer()
	: BSE::Layer("SceneViewer")
	{
		BSE_TRACE("Layer Constructor Enter");
		m_Window = BSE::Application::Get()->GetWindow();
		m_Title = m_Window->GetTitle();
	}
	
	~SceneViewer()
	{
		if (m_CameraController != nullptr){
			delete m_CameraController;
			m_CameraController = nullptr;
		}
	}
	
	void OnAttach() override {
		// ------------------------------------------------
		// OpenGL camera controller
		unsigned int w = BSE::Application::Get()->GetWindow()->GetWidth();
		unsigned int h = BSE::Application::Get()->GetWindow()->GetHeight();
		float uw = (float)w/(float)h;
		// float uh = 2*(float)h/(float)h;
		m_CameraController = new BSE::OrthographicCameraController(uw, 1.5f, true);
		// ------------------------------------------------
		BSE::FrameBufferSpecification fbSpec;
		fbSpec.Width = m_Window->GetWidth();
		fbSpec.Height = m_Window->GetHeight();
		m_FrameBufferA = BSE::FrameBuffer::Create(fbSpec);
		m_FrameBufferB = BSE::FrameBuffer::Create(fbSpec);
		BSE::GameData::m_FrameBuffer = m_FrameBufferA;
		
	}
	
	void OnDetach() override {
		
	}
	
	int OnUpdate(float time) override {
		BSE_PROFILER(u8"Sandbox2D Layer OnUpdate");
		layerTime += time;
		static int frame = 0;
		// BSE_TRACE("Layer time counter increased");
		
		// --------------------------------------------------
		// RENDER
		if (BSE::RenderCommand::GetAPI() != nullptr){
			if (layerTime >= BSE_FPS60){
				BSE_PROFILE_SCOPE("Sandbox2D OnUpdate");
				
				// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
				// CAMERA
				m_CameraController->OnUpdate(layerTime);
				// BSE_TRACE("Camera controller updated");
				// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
				
				// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
				// Show some info about FPS on WindowTitle
				int fps = round(1 / layerTime);
				if (frame == fps){
					frame = 0;
				}
				char buf[5];
				sprintf(buf, "%d", fps);
				m_Window->SetTitle(m_Title + " :: FPS = " + buf);
				// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
				// Bind a buffer to work with
				{
					BSE_PROFILE_SCOPE(u8"Framebuffer Bind");
					if (BSE::GameData::m_FrameBuffer == m_FrameBufferA) {
						m_FrameBufferB->Bind();
					} else {
						m_FrameBufferA->Bind();
					}
				}
				
				// BSE_TRACE("Window Title changed");
				BSE::Renderer2D::Clear({0.2f, 0.2f, 0.4f, 1});
				BSE::Renderer2D::BeginScene(m_CameraController->GetCamera());
				
				for (int x = 0; x < 100; x++){
					for (int y = 0; y < 100; y++){
						BSE::Renderer2D::DrawFilledRect(
							{-1.0f + x*0.06f, -1.0f + y*0.06f}, 
							{0.05f, 0.05f},
							// 0.0f + frame * (180 / fps),
							{0.2f, 0.7f, 0.3f, 1.0f}
							);
					}
					// BSE_INFO("x = {0}, y = {1}", x , y);
				}
				
				BSE::Renderer2D::EndScene();
				
				// Unbind current buffer
				{
					BSE_PROFILE_SCOPE(u8"Framebuffer Unbind");
					if (BSE::GameData::m_FrameBuffer == m_FrameBufferA) {
						m_FrameBufferB->Unbind();
					} else {
						m_FrameBufferA->Unbind();
					}
				}
				
				// swap buffers
				BSE::GameData::m_FrameBuffer = (BSE::GameData::m_FrameBuffer == m_FrameBufferA) ? m_FrameBufferB : m_FrameBufferA;
				
				layerTime = 0.0f;
				frame++;
				return 1; // 1 - changed something
			} else {
				return 0; // 0 - no changes on screen
			}
		}
		return 0; // RendererAPI == nullptr
	}
	
	void OnEvent(BSE::Event& event) override {
		m_CameraController->OnEvent(event);
	}
	
private:
	float layerTime = 0.0f;
	
	BSE::Window* m_Window = nullptr;
	std::string m_Title;
	BSE::OrthographicCameraController* m_CameraController = nullptr;
	
	BSE::FrameBuffer* m_FrameBufferA;
	BSE::FrameBuffer* m_FrameBufferB;
};

#endif
