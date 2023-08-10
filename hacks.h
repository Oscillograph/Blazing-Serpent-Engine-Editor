#ifndef BSESANDBOX_HACKS_H
#define BSESANDBOX_HACKS_H


#define FixImGuiContext(m_Context) \
	if (m_Context != nullptr){									\
		if (m_Context != ImGui::GetCurrentContext()){ 			\
			BSE_TRACE("SandboxGui: Current Context is different!"); \
			ImGui::SetCurrentContext(m_Context);				\
			BSE_TRACE("SandboxGui: Current Context is set again!"); \
		} \
	}

#endif
