#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"

#include "Walnut/Image.h"
#include "Walnut/Timer.h"

#include "RayTracing/scene.cuh"

using namespace Walnut;

class ExampleLayer : public Walnut::Layer {
public:
	virtual void OnUIRender() override {
		ImGui::Begin("Settings");
		ImGui::Text("Last render: %.3fms", m_LastRenderTime);
		ImGui::Text("FrameRate: %.3f", 1000.0f / m_LastRenderTime);
		if(ImGui::Button("Render")) {
			Render();
		}
		ImGui::End();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("Viewport");

		m_ViewportWidth = ImGui::GetContentRegionAvail().x;
		m_ViewportHeight = ImGui::GetContentRegionAvail().y;

		auto image = m_scene.GetFinalImage();
		if(image) {
			ImGui::Image(image->GetDescriptorSet(), { (float) image->GetWidth(), (float) image->GetHeight() },
				ImVec2(0, 1), ImVec2(1, 0));
		}

		ImGui::End();
		ImGui::PopStyleVar();

		handleKeys();

		Render();
	}

	void Render() {
		Timer timer;

		m_scene.OnResize(m_ViewportWidth, m_ViewportHeight);
		m_scene.Render();

		m_LastRenderTime = timer.ElapsedMillis();
	}

	bool handleKeys() {
		auto keyDown = ImGui::IsKeyDown;
		if(keyDown(ImGuiKey_W)) {
			m_scene.keyPressedW();
		}
		if(keyDown(ImGuiKey_A)) {
			m_scene.keyPressedA();
		}
		if(keyDown(ImGuiKey_S)) {
			m_scene.keyPressedS();
		}
		if(keyDown(ImGuiKey_D)) {
			m_scene.keyPressedD();
		}
		if(keyDown(ImGuiKey_UpArrow)) {
			m_scene.keyPressedUpArrow();
		}
		if(keyDown(ImGuiKey_DownArrow)) {
			m_scene.keyPressedDownArrow();
		}
		if(keyDown(ImGuiKey_LeftArrow)) {
			m_scene.keyPressedLeftArrow();
		}
		if(keyDown(ImGuiKey_RightArrow)) {
			m_scene.keyPressedRightArrow();
		}
		if(keyDown(ImGuiKey_R)) {
			m_scene.keyPressedR();
		}
		if(keyDown(ImGuiKey_Space)) {
			m_scene.keyPressedSpace();
		}
		if(keyDown(ImGuiKey_LeftCtrl)) {
			m_scene.keyPressedLCtrl();
		}
		return true;
	}

private:
	fRT::Scene m_scene;
	uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;
	
	float m_LastRenderTime = 0.0f;
};

Walnut::Application* Walnut::CreateApplication(int argc, char** argv) {
	Walnut::ApplicationSpecification spec;
	spec.Name = "Ray Tracing";

	Walnut::Application* app = new Walnut::Application(spec);
	app->PushLayer<ExampleLayer>();
	app->SetMenubarCallback([app]() {
		if(ImGui::BeginMenu("File")) {
			if(ImGui::MenuItem("Exit")) {
				app->Close();
			}
			else if(ImGui::MenuItem("Bruh")) {

			}
			ImGui::EndMenu();
		}
		}
	);
	return app;
}