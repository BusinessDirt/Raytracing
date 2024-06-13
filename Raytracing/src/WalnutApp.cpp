#pragma once

#include <iostream>
#include <thread>
#include <chrono>

#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"

#include "Walnut/Image.h"
#include "Walnut/UI/UI.h"

#include "Rendering/Renderer.h"

class ExampleLayer : public Walnut::Layer
{
public:
	virtual void OnUIRender() override
	{
		ImGui::SetNextWindowSize({ 200, 720 }, ImGuiCond_FirstUseEver);
		ImGui::Begin("Settings");
		ImGui::Text("Last render: %s", m_Renderer.GetRenderTime().c_str());
		if (ImGui::Button("Render"))
			m_Renderer.StartRender(m_ViewportWidth, m_ViewportHeight, m_Samples, m_MaxDepth, m_SelectedScene);
		
		if (ImGui::Button("Abort"))
			m_Renderer.StopRender();

		bool sampleChange = false;
		ImGui::Text("Samples Per Pixel");
		m_Samples, sampleChange = ImGui::InputInt(" ", &m_Samples, 1, 2, 0);

		bool depthChange = false;
		ImGui::Text("Max Depth");
		m_MaxDepth, depthChange = ImGui::InputInt("  ", &m_MaxDepth, 1, 2, 0);

		const char* items[] = { "Random Spheres", "Checkered Spheres", "Earth", "Perlin Spheres"};
		ImGui::Combo("Scene", &m_SelectedScene, items, IM_ARRAYSIZE(items));

		ImGui::End();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::SetNextWindowSize({1080, 720}, ImGuiCond_FirstUseEver);
		ImGui::Begin("Viewport");

		m_ViewportWidth = (uint32_t)ImGui::GetContentRegionAvail().x;
		m_ViewportHeight = (uint32_t)ImGui::GetContentRegionAvail().y;

		m_Renderer.Update();

		std::shared_ptr<Walnut::Image> image = m_Renderer.GetFinalImage();
		if (image)
			ImGui::Image(image->GetDescriptorSet(),{ (float)image->GetWidth(), (float)image->GetHeight() });

		ImGui::End();
		ImGui::PopStyleVar();
	}
private:
	Renderer m_Renderer;
	uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;

	int m_Samples = 20;
	int m_MaxDepth = 20;

	int m_SelectedScene = 0;
};

Walnut::Application* Walnut::CreateApplication(int argc, char** argv)
{
	Walnut::ApplicationSpecification spec;
	spec.Name = "Ray Tracing";
	spec.Height = 720;
	spec.Width = 1280;

	Walnut::Application* app = new Walnut::Application(spec);
	app->PushLayer<ExampleLayer>();
	app->SetMenubarCallback([app]()
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Exit"))
				{
					app->Close();
				}
				ImGui::EndMenu();
			}
		});
	return app;
}