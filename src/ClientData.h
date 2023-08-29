#ifndef BSEEDITOR_CLIENTDATA_H
#define BSEEDITOR_CLIENTDATA_H

#include <BSE_Client.h>

struct ClientData {
	static bool ViewPortFocused;
	
	
	// Scene Viewer
	static BSE::Scene* m_ActiveScene;
	static BSE::Entity* m_Square;
	static BSE::Entity* m_CameraA;
	static BSE::Entity* m_CameraB;
	
	static int quadsMaxX;
	static int quadsMaxY;
	
	// routines
	static void NewScene(){
		delete m_ActiveScene;
		m_ActiveScene = new BSE::Scene();
		m_ActiveScene->SetCameraController(BSE::GameData::m_CameraController);
	}
};

#endif
