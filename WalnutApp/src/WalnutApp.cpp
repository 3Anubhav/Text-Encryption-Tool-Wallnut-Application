#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"
#include "Walnut/Image.h"

#include <string>
#include <cstring>
#include <imgui_internal.h>
#include <vector>

// === Fixed XOR + Base64 system (no extra chars, reversible) ===
static std::string XorEncrypt(const std::string& input)
{
    const std::string key = "WalnutSecret";
    std::string output = input;
    for (size_t i = 0; i < input.size(); ++i)
        output[i] = input[i] ^ key[i % key.size()];
    return output;
}

static std::string Base64Encode(const std::string& input)
{
    static const char encodeTable[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    std::string output;
    int val = 0, valb = -6;
    for (unsigned char c : input)
    {
        val = (val << 8) + c;
        valb += 8;
        while (valb >= 0)
        {
            output.push_back(encodeTable[(val >> valb) & 0x3F]);
            valb -= 6;
        }
    }
    if (valb > -6)
        output.push_back(encodeTable[((val << 8) >> (valb + 8)) & 0x3F]);
    while (output.size() % 4)
        output.push_back('=');
    return output;
}

static std::string Base64Decode(const std::string& input)
{
    static const int decodeTable[256] = {
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,62,-1,-1,-1,63,
        52,53,54,55,56,57,58,59,60,61,-1,-1,-1,-1,-1,-1,
        -1,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,
        15,16,17,18,19,20,21,22,23,24,25,-1,-1,-1,-1,-1,
        -1,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,
        41,42,43,44,45,46,47,48,49,50,51
    };
    std::string output;
    int val = 0, valb = -8;
    for (unsigned char c : input)
    {
        if (decodeTable[c] == -1) continue;
        val = (val << 6) + decodeTable[c];
        valb += 6;
        if (valb >= 0)
        {
            output.push_back(char((val >> valb) & 0xFF));
            valb -= 8;
        }
    }
    return output;
}

static std::string EncryptText(const std::string& input)
{
    return Base64Encode(XorEncrypt(input));
}

static std::string DecryptText(const std::string& input)
{
    return XorEncrypt(Base64Decode(input));
}

// === UI Layer ===
class TextEncryptionLayer : public Walnut::Layer
{
public:
    void OnAttach() override
    {
        ApplyCustomTheme();
    }

    void ApplyCustomTheme()
    {
        ImGuiStyle& style = ImGui::GetStyle();

        // Modern rounded corners
        style.WindowRounding = 12.0f;
        style.FrameRounding = 8.0f;
        style.GrabRounding = 8.0f;
        style.PopupRounding = 8.0f;
        style.ChildRounding = 8.0f;
        style.ScrollbarRounding = 8.0f;
        style.TabRounding = 8.0f;

        // Spacing and padding
        style.WindowPadding = ImVec2(20, 20);
        style.FramePadding = ImVec2(16, 10);
        style.ItemSpacing = ImVec2(12, 12);
        style.ItemInnerSpacing = ImVec2(8, 6);

        // Colors - Modern dark blue/purple theme
        ImVec4* colors = ImGui::GetStyle().Colors;

        // Primary colors
        colors[ImGuiCol_WindowBg] = ImVec4(0.08f, 0.09f, 0.12f, 0.94f);
        colors[ImGuiCol_ChildBg] = ImVec4(0.10f, 0.11f, 0.15f, 1.00f);
        colors[ImGuiCol_PopupBg] = ImVec4(0.10f, 0.11f, 0.15f, 0.94f);

        // Text colors
        colors[ImGuiCol_Text] = ImVec4(0.95f, 0.96f, 1.00f, 1.00f);
        colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.60f, 1.00f);

        // Button colors
        colors[ImGuiCol_Button] = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
        colors[ImGuiCol_ButtonHovered] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
        colors[ImGuiCol_ButtonActive] = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);

        // Frame background (input fields)
        colors[ImGuiCol_FrameBg] = ImVec4(0.16f, 0.17f, 0.21f, 1.00f);
        colors[ImGuiCol_FrameBgHovered] = ImVec4(0.20f, 0.21f, 0.27f, 1.00f);
        colors[ImGuiCol_FrameBgActive] = ImVec4(0.26f, 0.27f, 0.34f, 1.00f);

        // Headers and separators
        colors[ImGuiCol_Header] = ImVec4(0.26f, 0.59f, 0.98f, 0.31f);
        colors[ImGuiCol_HeaderHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
        colors[ImGuiCol_HeaderActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
        colors[ImGuiCol_Separator] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
        colors[ImGuiCol_SeparatorHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.78f);
        colors[ImGuiCol_SeparatorActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);

        // Title bar
        colors[ImGuiCol_TitleBg] = ImVec4(0.10f, 0.11f, 0.15f, 1.00f);
        colors[ImGuiCol_TitleBgActive] = ImVec4(0.10f, 0.11f, 0.15f, 1.00f);
        colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.10f, 0.11f, 0.15f, 0.75f);

        // Scrollbar
        colors[ImGuiCol_ScrollbarBg] = ImVec4(0.10f, 0.11f, 0.15f, 1.00f);
        colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
        colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.78f);
        colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);

        // Borders
        colors[ImGuiCol_Border] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
        colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    }

    void OnUIRender() override
    {
        // Center window and auto-resize dynamically
        ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Once, ImVec2(0.5f, 0.5f));
        ImGui::SetNextWindowSize(ImVec2(650, 800), ImGuiCond_Once);

        ImGui::Begin("Text Encryption Tool", nullptr,
            ImGuiWindowFlags_NoResize | 
            ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoSavedSettings);

        // Header section with improved styling
        ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[0]);
        ImGui::TextColored(ImVec4(0.26f, 0.59f, 0.98f, 1.00f), "ENCRYPTION TOOL");
        ImGui::PopFont();
        
        ImGui::TextWrapped("Securely encrypt and decrypt your text using XOR");
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        // --- Input Text ---
        ImGui::Text("Input Text:");
        static char inputBuffer[4096];
        std::strncpy(inputBuffer, m_InputText.c_str(), sizeof(inputBuffer));
        inputBuffer[sizeof(inputBuffer) - 1] = '\0';

        // Calculate available width for text areas
        float textAreaWidth = ImGui::GetContentRegionAvail().x;
        float inputHeight = 120.0f;
        
        if (ImGui::InputTextMultiline("##input", inputBuffer, sizeof(inputBuffer), 
            ImVec2(textAreaWidth, inputHeight), 
            ImGuiInputTextFlags_AllowTabInput))
        {
            m_InputText = inputBuffer;
        }

        // Character count
        ImGui::TextDisabled("Characters: %zu", m_InputText.length());
        ImGui::Spacing();

        // --- Encrypt / Decrypt Buttons ---
        float buttonWidth = (textAreaWidth - ImGui::GetStyle().ItemSpacing.x) / 2.0f;
        
        // Encrypt Button (Green theme)
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.16f, 0.50f, 0.16f, 0.60f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.16f, 0.70f, 0.16f, 1.00f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.16f, 0.80f, 0.16f, 1.00f));
        if (ImGui::Button("ENCRYPT", ImVec2(buttonWidth, 45)))
        {
            if (!m_InputText.empty())
            {
                m_EncryptedText = EncryptText(m_InputText);
                m_DecryptedText.clear();
            }
        }
        ImGui::PopStyleColor(3);

        ImGui::SameLine();

        // Decrypt Button (Red theme)
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.60f, 0.20f, 0.20f, 0.60f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.80f, 0.20f, 0.20f, 1.00f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.90f, 0.20f, 0.20f, 1.00f));
        if (ImGui::Button("DECRYPT", ImVec2(buttonWidth, 45)))
        {
            if (!m_InputText.empty())
            {
                m_DecryptedText = DecryptText(m_InputText);
                m_EncryptedText.clear();
            }
        }
        ImGui::PopStyleColor(3);

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        // --- Encrypted Text ---
        if (!m_EncryptedText.empty())
        {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.26f, 0.59f, 0.98f, 1.00f));
            ImGui::Text("ENCRYPTED TEXT:");
            ImGui::PopStyleColor();
            
            static char encBuffer[4096];
            std::strncpy(encBuffer, m_EncryptedText.c_str(), sizeof(encBuffer));
            encBuffer[sizeof(encBuffer) - 1] = '\0';
            
            float outputHeight = 100.0f;
            
            ImGui::InputTextMultiline("##encrypted", encBuffer, sizeof(encBuffer), 
                ImVec2(textAreaWidth, outputHeight), 
                ImGuiInputTextFlags_ReadOnly);

            // Copy button with improved styling
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.26f, 0.59f, 0.98f, 0.40f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.26f, 0.59f, 0.98f, 0.80f));
            if (ImGui::Button("COPY TO CLIPBOARD", ImVec2(-1, 40)))
            {
                ImGui::SetClipboardText(encBuffer);
                m_ShowCopiedToast = true;
                m_CopyTime = ImGui::GetTime();
            }
            ImGui::PopStyleColor(2);
            ImGui::Spacing();
        }

        // --- Decrypted Text ---
        if (!m_DecryptedText.empty())
        {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.16f, 0.70f, 0.16f, 1.00f));
            ImGui::Text("DECRYPTED TEXT:");
            ImGui::PopStyleColor();
            
            static char decBuffer[4096];
            std::strncpy(decBuffer, m_DecryptedText.c_str(), sizeof(decBuffer));
            decBuffer[sizeof(decBuffer) - 1] = '\0';
            
            float outputHeight = 100.0f;
            
            ImGui::InputTextMultiline("##decrypted", decBuffer, sizeof(decBuffer), 
                ImVec2(textAreaWidth, outputHeight), 
                ImGuiInputTextFlags_ReadOnly);
            ImGui::Spacing();
        }

        ImGui::Separator();
        ImGui::Spacing();

        // --- Clear All Button ---
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.50f, 0.50f, 0.50f, 0.40f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.60f, 0.60f, 0.60f, 0.80f));
        if (ImGui::Button("CLEAR ALL", ImVec2(-1, 45)))
        {
            m_InputText.clear();
            m_EncryptedText.clear();
            m_DecryptedText.clear();
        }
        ImGui::PopStyleColor(2);

        // --- Copy Notification ---
        if (m_ShowCopiedToast)
        {
            float timeElapsed = static_cast<float>(ImGui::GetTime() - m_CopyTime);
            if (timeElapsed < 1.5f)
            {
                // Center the notification
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.16f, 0.70f, 0.16f, 1.00f));
                float textWidth = ImGui::CalcTextSize("Text copied to clipboard!").x;
                ImGui::SetCursorPosX((ImGui::GetWindowWidth() - textWidth) * 0.5f);
                ImGui::Text("Text copied to clipboard!");
                ImGui::PopStyleColor();
            }
            else
            {
                m_ShowCopiedToast = false;
            }
        }

        // Ensure the window doesn't become too small
        ImVec2 currentWindowSize = ImGui::GetWindowSize();
        if (currentWindowSize.x < 600 || currentWindowSize.y < 600)
        {
            ImGui::SetWindowSize(ImVec2(
                std::max(currentWindowSize.x, 600.0f),
                std::max(currentWindowSize.y, 600.0f)
            ));
        }

        ImGui::End();
    }

private:
    std::string m_InputText;
    std::string m_EncryptedText;
    std::string m_DecryptedText;

    bool m_ShowCopiedToast = false;
    double m_CopyTime = 0.0;
};

// === Walnut Entry Point ===
Walnut::Application* Walnut::CreateApplication(int argc, char** argv)
{
    Walnut::ApplicationSpecification spec;
    spec.Name = "Text Encryption Tool";
    
    // Set initial window size
    spec.Width = 650;
    spec.Height = 800;
    
    Walnut::Application* app = new Walnut::Application(spec);
    app->PushLayer<TextEncryptionLayer>();
    return app;
}