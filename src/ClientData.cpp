#include "./ClientData.h"

bool ClientData::ViewPortFocused = false;
BSE::Scene* ClientData::m_ActiveScene = nullptr;
BSE::Entity* ClientData::m_Square = nullptr;
BSE::Entity* ClientData::m_CameraA = nullptr;
BSE::Entity* ClientData::m_CameraB = nullptr;

BSE::FrameBuffer* ClientData::m_FrameBuffer = nullptr;

int ClientData::quadsMaxX = 0;
int ClientData::quadsMaxY = 0;

bool ClientData::EditorCameraOn = false;
