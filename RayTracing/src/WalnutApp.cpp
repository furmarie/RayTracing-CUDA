#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"

#include "Walnut/Image.h"
#include "Walnut/Timer.h"

#include "RayTracing/scene.cuh"

using namespace Walnut;

class ExampleLayer : public Walnut::Layer {
public:
	virtual void OnUIRender(Application* app) override {
		if(ImGui::IsKeyDown(ImGuiKey_Escape)) {
			if(app) {
				app->Close();
				return;
			}
		}
		ImGui::Begin("Settings");
		ImGui::Text("Last render: %.3fms", m_LastRenderTime);
		ImGui::Text("FrameRate: %.3f", 1000.0f / m_LastRenderTime);
		if(ImGui::Button("Render")) {
			Render();
		}
		ImGui::ColorPicker3("Sphere Colour", &sphereColour.x);

		ImGui::End();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

		ImGui::Begin("Viewport");

		ImVec2 contentRegion = ImGui::GetContentRegionAvail();
		m_ViewportWidth = contentRegion.x;
		m_ViewportHeight = contentRegion.y;

		auto image = m_scene.GetFinalImage();
		if(image) {
			ImGui::Image(image->GetDescriptorSet(), { (float) image->GetWidth(), (float) image->GetHeight() },
				ImVec2(0, 1), ImVec2(1, 0));
		}

		ImGui::End();
		ImGui::PopStyleVar();

		mousePos = ImGui::GetMousePos();

		if(ImGui::IsKeyPressed(ImGuiKey_M)) {
			m_disableMouse = !m_disableMouse;
			app->disableMouse(m_disableMouse);
		}


		handleColours();

		handleMouse();

		// TODO handle keys when window focused
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

	bool handleMouse() {
		return m_scene.mouseMoved({ mousePos.x, mousePos.y }, m_disableMouse);
	}

	bool handleColours() {
		return m_scene.handleColours(sphereColour);
	}

private:
	fRT::Scene m_scene;
	uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;

	ImVec2 mousePos;

	float m_LastRenderTime = 0.0f;

	// Temporarily here
	vec3 sphereColour;

	// if mouse camera movement enabled, disableMouse is true then
	bool m_disableMouse = false;
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