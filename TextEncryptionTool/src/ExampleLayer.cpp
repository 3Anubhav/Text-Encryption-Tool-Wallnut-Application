#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"

#include <string>

static std::string EncryptDecrypt(const std::string& text, char key)
{
    std::string result = text;
    for (size_t i = 0; i < text.size(); ++i)
        result[i] = text[i] ^ key;
    return result;
}

class TextEncryptionLayer : public Walnut::Layer
{
public:
    void OnUIRender() override
    {
        ImGui::Begin("🔒 Text Encryption Tool");

        ImGui::Text("Enter a single character as key:");
        ImGui::InputText("##key", m_KeyBuffer, 2);

        ImGui::Text("Enter text:");
        ImGui::InputTextMultiline("##text", &m_InputText);

        if (ImGui::Button("Encrypt"))
        {
            if (m_KeyBuffer[0] != '\0')
                m_Result = EncryptDecrypt(m_InputText, m_KeyBuffer[0]);
            else
                m_Result = "Error: Please enter a key!";
        }

        ImGui::SameLine();

        if (ImGui::Button("Decrypt"))
        {
            if (m_KeyBuffer[0] != '\0')
                m_Result = EncryptDecrypt(m_InputText, m_KeyBuffer[0]);
            else
                m_Result = "Error: Please enter a key!";
        }

        ImGui::Separator();
        ImGui::Text("Output:");
        ImGui::InputTextMultiline("##output", &m_Result, ImVec2(-1, 100), ImGuiInputTextFlags_ReadOnly);

        ImGui::End();
    }

private:
    char m_KeyBuffer[2] = { 0 };
    std::string m_InputText;
    std::string m_Result;
};

Walnut::Application* Walnut::CreateApplication(int argc, char** argv)
{
    Walnut::ApplicationSpecification spec;
    spec.Name = "Text Encryption Tool";

    Walnut::Application* app = new Walnut::Application(spec);
    app->PushLayer<TextEncryptionLayer>();
    return app;
}
