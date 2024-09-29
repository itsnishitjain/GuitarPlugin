# VST Guitar Plugin with Graph Automation and Computer Vision

This project is a VST guitar plugin developed using JUCE, Google MediaPipe, and C++. The plugin is compatible with Digital Audio Workstations (DAWs) and allows guitar players to control their guitar tone by mapping parameters (such as distortion and reverb) onto a 2D graph. The plugin also integrates hand gesture recognition via MediaPipe, allowing users to change their guitar tones dynamically based on their hand gestures.

## Features
- **2D Graph Tone Control:** Control the blend of distortion and reverb effects by dragging a point on the graph.
- **Custom Distortion and Reverb Effects:** Implemented using feedback delay loops with adjustable volume and blend parameters.
- **Gesture-Based Tone Control:** Uses Google MediaPipe to track hand gestures and map them to changes in tone.
- **Real-time Audio Processing:** The plugin processes audio in real-time, providing seamless feedback for musicians.

## Technologies Used
- **JUCE:** Framework for building the VST plugin.
- **Google MediaPipe:** For hand gesture recognition.
- **C++:** Core programming language for implementing audio effects and integration.

## Setup and Installation

### Prerequisites
1. **JUCE Framework:** Make sure you have JUCE installed on your machine. You can download it [here](https://juce.com/get-juce).
2. **C++ Compiler:** You'll need a C++ compiler installed (e.g., GCC, Clang, or MSVC).
3. **Google MediaPipe:** Follow the [installation instructions](https://google.github.io/mediapipe/getting_started/install.html) to set up MediaPipe in your development environment.

### Cloning the Repository
```bash
git clone https://github.com/yourusername/vst-guitar-plugin.git
cd vst-guitar-plugin
```

### Building the Project in JUCE

1.	Open the JUCE Projucer application.
2.	Click on File > Open Project and select the .jucer file from the repository folder.
3.	After the project loads, go to File > Save and Open in IDE (choose your preferred IDE like Xcode, Visual Studio, or CLion).
4.	In the IDE, build the project. The VST plugin will compile, and the binary will be generated in the Builds folder for your platform (e.g., VST3 or AU format).

### Running the Plugin in a DAW
1.	Copy the generated VST3 plugin (from the Builds folder) into your DAW’s VST plugins directory.
•	For Windows: C:/Program Files/Common Files/VST3/
•	For macOS: /Library/Audio/Plug-Ins/VST3/
2.	Open your DAW (e.g., Ableton, FL Studio, etc.) and scan for new plugins.
3.	Load the plugin on a track and use the 2D graph or hand gestures to modify the guitar tone in real-time.

### How to Use
1.	Tone Control via Graph:
•	The X and Y axes represent different effect parameters (e.g., distortion and reverb).
•	Drag the point on the graph to adjust the blend of these effects.
2.	Gesture Control:
•	With a webcam connected, the plugin uses hand gestures (via Google MediaPipe) to dynamically adjust tone parameters. Ensure MediaPipe is set up correctly for this functionality to work.

