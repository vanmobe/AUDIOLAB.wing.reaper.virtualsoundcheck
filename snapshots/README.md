# REAPER Configuration Snapshots

This folder contains REAPER snapshot files (`.RTrackTemplate` or config files) that provide pre-configured settings for Wing Connector.

## Available Snapshots

### Wing Control Buttons (Coming Soon)

**File:** `wing-control-buttons.snap` *(will be uploaded)*

**What it includes:**
- Pre-configured MIDI mappings for Wing custom buttons
- REAPER actions mapped to Wing button controls
- All Wing Connector actions accessible from your Wing console

**Quick Install:**
1. Download the snap file (link will be provided)
2. Import into REAPER (see instructions below)
3. Done! Control REAPER from your Wing buttons

---

## Manual Setup (Without Snap File)

If you prefer to configure manually or the snap file is not yet available, follow these steps:

### Setting Up Wing Custom Buttons to Control REAPER

The Behringer Wing allows you to assign custom buttons to send MIDI commands, which can trigger actions in REAPER.

#### Step 1: Configure Wing Custom Buttons

1. **On your Behringer Wing:**
   - Press **Setup** button
   - Navigate to **Remote** → **Custom Controls**
   - Select a button slot (e.g., Custom Button 1-8)

2. **Set MIDI Command:**
   - Set **Type** to: `MIDI`
   - Set **Message Type** to: `Note On` or `Control Change`
   - Set **Channel** to: `1` (or your preferred channel)
   - Set **Note/CC Number** to: A unique number (e.g., `60`, `61`, `62`, etc.)
   - Set **Value** to: `127` (maximum)
   - Set **Destination** to: `USB` (to send to your computer)

3. **Assign to Physical Button:**
   - Navigate to the button configuration page
   - Assign your custom control to a physical Wing button or softkey

4. **Repeat** for each action you want to control (Connect, Refresh, Toggle Monitoring, etc.)

#### Step 2: Configure REAPER to Receive MIDI

1. **Open REAPER Preferences:**
   - Go to: `Preferences` → `Audio` → `MIDI Devices`

2. **Enable Wing MIDI Input:**
   - Find your Wing in the input devices list (should appear as USB MIDI device)
   - Check **Enable input from this device**
   - Set mode to: `Enable input for control messages`

3. **Configure Control Surface:**
   - Go to: `Preferences` → `Control/OSC/web`
   - Click **Add**
   - Select `MIDI` as control surface type
   - Choose your Wing device as MIDI input

#### Step 3: Map Wing Buttons to REAPER Actions

1. **Open Actions List:**
   - Go to: `Actions` → `Show action list`

2. **Find Wing Connector Actions:**
   - Search for: `Wing:`
   - You should see:
     - `Wing: Connect to Behringer Wing`
     - `Wing: Refresh Tracks`
     - `Wing: Toggle Monitoring`
     - (and any other Wing Connector actions)

3. **Assign MIDI to Action:**
   - Select the action (e.g., "Wing: Connect to Behringer Wing")
   - Click **Add** button at the bottom (in the Shortcuts section)
   - In the dialog, press the corresponding button on your Wing
   - REAPER should detect the MIDI message (e.g., "Note On C4 Chan 1")
   - Click **OK** to confirm

4. **Repeat** for each action you want to map

5. **Test:**
   - Press the button on your Wing
   - The corresponding REAPER action should trigger!

#### Recommended Button Mappings

Here's a suggested setup for common Wing Connector actions:

| Wing Button | MIDI Note | REAPER Action | Purpose |
|-------------|-----------|---------------|---------|
| Custom 1 | C4 (60) | Wing: Connect to Behringer Wing | Initial connection/setup |
| Custom 2 | C#4 (61) | Wing: Refresh Tracks | Update track info |
| Custom 3 | D4 (62) | Wing: Toggle Monitoring | Real-time sync on/off |
| Custom 4 | D#4 (63) | Transport: Record | Start/stop recording |
| Custom 5 | E4 (64) | Transport: Play/Stop | Playback control |
| Custom 6 | F4 (65) | Transport: Play/Pause | Pause/resume |

---

## Using the Snap File (When Available)

Once the snap file is uploaded, you can import it directly:

### Method 1: Import via REAPER

1. **Download** `wing-control-buttons.snap` from this folder
2. **In REAPER:**
   - Go to: `Actions` → `Show action list`
   - Click **Import/export**
   - Select **Import**
   - Choose the downloaded `.snap` file
3. **Done!** All mappings are now configured

### Method 2: Manual File Copy

Alternatively, you can copy configuration files directly:

1. **Download the snap file**
2. **Locate your REAPER resource path:**
   - macOS: `~/Library/Application Support/REAPER/`
   - Windows: `%APPDATA%\REAPER\`
   - Linux: `~/.config/REAPER/`

3. **Copy files** to appropriate subfolder
4. **Restart REAPER**

---

## Troubleshooting

### Wing buttons not triggering REAPER actions

1. **Check MIDI connection:**
   - In REAPER, go to: `View` → `MIDI devices`
   - Verify Wing is listed and has activity indicator when you press buttons

2. **Verify MIDI messages:**
   - Open: `View` → `ReaControlMIDI`
   - Press Wing button and confirm message appears

3. **Check action mappings:**
   - Go to: `Actions` → `Show action list`
   - Find Wing action
   - Verify MIDI shortcut is listed

4. **Confirm Wing MIDI settings:**
   - On Wing: `Setup` → `Remote` → check `External MIDI Control` is set to `USB`

### MIDI messages received but actions not triggering

- Make sure the MIDI note/CC numbers match exactly
- Try removing and re-adding the MIDI mapping in REAPER
- Restart REAPER after making changes

---

## Support

For help with MIDI configuration:
- See [INSTALL.md](../INSTALL.md) for general installation
- See [README.md](../README.md) for Wing Connector usage
- Check Wing manual for Custom Controls documentation

## Contributing

If you create useful configurations:
- Share your snap files!
- Document your button mappings
- Help improve these instructions
