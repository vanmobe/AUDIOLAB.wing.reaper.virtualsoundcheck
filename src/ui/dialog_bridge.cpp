/*
 * Cross-platform dialog bridge.
 */

#include "internal/dialog_bridge.h"
#include "wingconnector/reaper_extension.h"
#include "wingconnector/wing_osc.h"
#ifdef __APPLE__
#include "internal/wing_connector_dialog_macos.h"
#endif
#include "reaper_plugin_functions.h"

#include <algorithm>
#include <cctype>
#include <cstdio>
#include <set>
#include <sstream>
#include <string>

namespace WingConnector {
namespace {

std::string Trim(const std::string& input) {
    size_t start = 0;
    while (start < input.size() && std::isspace(static_cast<unsigned char>(input[start]))) {
        ++start;
    }
    size_t end = input.size();
    while (end > start && std::isspace(static_cast<unsigned char>(input[end - 1]))) {
        --end;
    }
    return input.substr(start, end - start);
}

std::string ToUpper(std::string value) {
    std::transform(value.begin(), value.end(), value.begin(),
                   [](unsigned char c) { return static_cast<char>(std::toupper(c)); });
    return value;
}

bool PromptSingleInput(const char* title,
                       const char* caption,
                       const std::string& initial_value,
                       std::string& output) {
    char buffer[1024];
    std::snprintf(buffer, sizeof(buffer), "%s", initial_value.c_str());
    if (!GetUserInputs(title, 1, caption, buffer, static_cast<int>(sizeof(buffer)))) {
        return false;
    }
    output = Trim(buffer);
    return true;
}

std::set<int> ParseChannelSelection(const std::string& selection_str, int max_channels) {
    std::set<int> selected;
    if (selection_str.empty()) {
        return selected;
    }

    std::stringstream ss(selection_str);
    std::string token;
    while (std::getline(ss, token, ',')) {
        token = Trim(token);
        if (token.empty()) {
            continue;
        }

        const size_t dash_pos = token.find('-');
        if (dash_pos != std::string::npos) {
            if (dash_pos == 0 || dash_pos == token.size() - 1) {
                continue;
            }
            try {
                int start = std::stoi(token.substr(0, dash_pos));
                int end = std::stoi(token.substr(dash_pos + 1));
                if (start > end) {
                    std::swap(start, end);
                }
                for (int i = start; i <= end; ++i) {
                    if (i > 0 && i <= max_channels) {
                        selected.insert(i);
                    }
                }
            } catch (...) {
                // Ignore malformed token.
            }
            continue;
        }

        try {
            const int channel = std::stoi(token);
            if (channel > 0 && channel <= max_channels) {
                selected.insert(channel);
            }
        } catch (...) {
            // Ignore malformed token.
        }
    }

    return selected;
}

void SaveConfig(ReaperExtension& extension) {
    extension.GetConfig().SaveToFile(WingConfig::GetConfigPath());
}

void SetWingIp(ReaperExtension& extension, const std::string& ip) {
    auto& config = extension.GetConfig();
    config.wing_ip = ip;
    config.wing_port = 2223;
    config.listen_port = 2223;
    SaveConfig(extension);
}

#ifndef __APPLE__
void HandleScan(ReaperExtension& extension) {
    auto wings = extension.DiscoverWings(1500);
    if (wings.empty()) {
        ShowMessageBox(
            "No Wing console discovered on the network.\n"
            "You can still set the IP manually from Settings.",
            "Wing Connector",
            0);
        return;
    }

    std::ostringstream options;
    options << "Discovered consoles:\n";
    for (size_t i = 0; i < wings.size(); ++i) {
        options << (i + 1) << ": " << wings[i].console_ip;
        if (!wings[i].name.empty()) {
            options << " (" << wings[i].name << ")";
        }
        options << "\n";
    }
    options << "\nEnter the number to select.";

    std::string pick;
    if (!PromptSingleInput("Wing Connector - Discovery", options.str().c_str(), "1", pick)) {
        return;
    }

    int idx = 0;
    try {
        idx = std::stoi(pick);
    } catch (...) {
        ShowMessageBox("Invalid selection.", "Wing Connector", 0);
        return;
    }

    if (idx < 1 || static_cast<size_t>(idx) > wings.size()) {
        ShowMessageBox("Selection out of range.", "Wing Connector", 0);
        return;
    }

    SetWingIp(extension, wings[static_cast<size_t>(idx - 1)].console_ip);
    ShowMessageBox("Wing IP updated from discovery results.", "Wing Connector", 0);
}

void HandleConnect(ReaperExtension& extension) {
    if (extension.IsConnected()) {
        extension.DisconnectFromWing();
        ShowMessageBox("Disconnected from Wing.", "Wing Connector", 0);
        return;
    }

    auto& config = extension.GetConfig();
    if (Trim(config.wing_ip).empty()) {
        std::string ip;
        if (!PromptSingleInput("Wing Connector", "Wing IP", "", ip)) {
            return;
        }
        if (ip.empty()) {
            ShowMessageBox("Wing IP is required to connect.", "Wing Connector", 0);
            return;
        }
        SetWingIp(extension, ip);
    }

    const bool ok = extension.ConnectToWing();
    ShowMessageBox(ok ? "Connected to Wing." : "Failed to connect to Wing.",
                   "Wing Connector",
                   0);
}

void HandleSetupLiveRecording(ReaperExtension& extension) {
    if (!extension.IsConnected()) {
        ShowMessageBox("Connect to Wing first.", "Wing Connector", 0);
        return;
    }

    auto channels = extension.GetAvailableChannels();
    if (channels.empty()) {
        ShowMessageBox("No channels found on Wing.", "Wing Connector", 0);
        return;
    }

    std::ostringstream summary;
    summary << "Found " << channels.size() << " channels.\n"
            << "Select channels using ranges (example: 1,3-6)\n"
            << "Use ALL to select all.\n";

    std::string selection_expr;
    if (!PromptSingleInput("Wing Connector - Channel Selection",
                           summary.str().c_str(),
                           "ALL",
                           selection_expr)) {
        return;
    }

    const std::string normalized = ToUpper(Trim(selection_expr));
    const bool select_all = normalized.empty() || normalized == "ALL";
    const std::set<int> selected = select_all ? std::set<int>() : ParseChannelSelection(selection_expr, 256);

    int selected_count = 0;
    for (auto& channel : channels) {
        channel.selected = select_all || selected.find(channel.channel_number) != selected.end();
        if (channel.selected) {
            ++selected_count;
        }
    }

    if (selected_count == 0) {
        ShowMessageBox("No channels selected.", "Wing Connector", 0);
        return;
    }

    std::string setup_alt;
    if (!PromptSingleInput("Wing Connector - Live/Soundcheck",
                           "Configure ALT source soundcheck now? (1=yes, 0=no)",
                           "1",
                           setup_alt)) {
        return;
    }
    const bool enable_soundcheck = (setup_alt != "0");

    extension.SetupSoundcheckFromSelection(channels, enable_soundcheck);
    ShowMessageBox("Live recording setup completed. Check REAPER console for details.",
                   "Wing Connector",
                   0);
}

void HandleToggleSoundcheck(ReaperExtension& extension) {
    extension.ToggleSoundcheckMode();
    ShowMessageBox(extension.IsSoundcheckModeEnabled()
                       ? "Soundcheck mode enabled."
                       : "Soundcheck mode disabled.",
                   "Wing Connector",
                   0);
}

void HandleSettings(ReaperExtension& extension) {
    auto& config = extension.GetConfig();

    std::string ip;
    if (!PromptSingleInput("Wing Connector - Settings", "Wing IP", config.wing_ip, ip)) {
        return;
    }
    if (ip.empty()) {
        ShowMessageBox("Wing IP cannot be empty.", "Wing Connector", 0);
        return;
    }

    std::string output_mode;
    if (!PromptSingleInput("Wing Connector - Settings",
                           "Output mode (USB or CARD)",
                           config.soundcheck_output_mode,
                           output_mode)) {
        return;
    }
    output_mode = ToUpper(output_mode);
    if (output_mode != "CARD") {
        output_mode = "USB";
    }

    std::string midi_actions;
    if (!PromptSingleInput("Wing Connector - Settings",
                           "Enable Wing MIDI actions? (1=yes, 0=no)",
                           config.configure_midi_actions ? "1" : "0",
                           midi_actions)) {
        return;
    }
    const bool enable_midi = (midi_actions != "0");

    SetWingIp(extension, ip);
    config.soundcheck_output_mode = output_mode;
    config.configure_midi_actions = enable_midi;
    SaveConfig(extension);
    extension.EnableMidiActions(enable_midi);

    std::string io_details;
    const bool mode_ok = extension.CheckOutputModeAvailability(config.soundcheck_output_mode, io_details);

    std::ostringstream result;
    result << "Settings saved.\n\n"
           << "Wing IP: " << config.wing_ip << "\n"
           << "Output mode: " << config.soundcheck_output_mode << "\n"
           << "MIDI actions: " << (enable_midi ? "ON" : "OFF") << "\n\n"
           << io_details;
    ShowMessageBox(result.str().c_str(),
                   mode_ok ? "Wing Connector - Settings Saved" : "Wing Connector - I/O Warning",
                   0);
}

void RunCrossPlatformDialog() {
    auto& extension = ReaperExtension::Instance();

    for (;;) {
        std::ostringstream prompt;
        prompt << "Status: " << (extension.IsConnected() ? "Connected" : "Not Connected");
        if (!extension.GetConfig().wing_ip.empty()) {
            prompt << " | Wing IP: " << extension.GetConfig().wing_ip;
        }
        prompt << "\n\nChoose action:\n"
               << "1 = Scan network for Wing\n"
               << "2 = Connect/Disconnect\n"
               << "3 = Setup live recording\n"
               << "4 = Toggle soundcheck mode\n"
               << "5 = Settings\n"
               << "0 = Close";

        std::string action;
        if (!PromptSingleInput("Wing Connector", prompt.str().c_str(), "1", action)) {
            return;
        }

        if (action == "0") {
            return;
        } else if (action == "1") {
            HandleScan(extension);
        } else if (action == "2") {
            HandleConnect(extension);
        } else if (action == "3") {
            HandleSetupLiveRecording(extension);
        } else if (action == "4") {
            HandleToggleSoundcheck(extension);
        } else if (action == "5") {
            HandleSettings(extension);
        } else {
            ShowMessageBox("Unknown action.", "Wing Connector", 0);
        }
    }
}
#endif

} // namespace

void ShowMainDialog() {
#ifdef __APPLE__
    ShowWingConnectorDialog();
#else
    RunCrossPlatformDialog();
#endif
}

} // namespace WingConnector
