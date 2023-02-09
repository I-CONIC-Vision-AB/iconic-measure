# iconic-measure {#mainpage}

Library and sample application for 3D measurements in image with hidden depth map. See [Stakeholders Request](https://drive.google.com/file/d/1WKryW56hkotbfvl1B7s-fkTOKb_MrOiO/view?usp=share_link) for more info.

## Build procedure

> **Note**
> To build and run this application, a graphics card with OpenCL/OpenGL interoperability is required (AMD cards or integrated graphics will _probably not_ work, however a dedicated NVIDIA GPU will most likely).

1. Install Visual Studio, including C++ desktop module.
2. Install vcpkg using [their instructions](https://vcpkg.io/en/getting-started.html), make sure to choose an install location that doesn't contain spaces and isn't a system folder. A valid example is `C:/src/vcpkg`.
3. Install project dependencies using vcpkg:  
   `./vcpkg.exe install --triplet=x64-windows boost-compute boost-geometry boost-iostreams boost-smart-ptr boost-test boost-thread boost-timer clfft eigen3 glew glfw3 libgeotiff opencl opengl openimageio[webp] proj4 wxwidgets`  
   This will take a while (up to an hour).
4. Bind vcpkg to Visual Studio (more info in the [wxWebMap](https://github.com/I-CONIC-Vision-AB/wxWebMap#build-instructions) repository):  
   `./vcpkg integrate install`
5. Clone the `iconic-measure` repository from Visual Studio using _File -> Clone repository_ and writing `https://github.com/I-CONIC-Vision-AB/iconic-measure` as _Repository location_.
6. Open Visual Studio and select Open Folder. Go to the cloned `iconic-measure` folder.
7. You will get an error that `ICONIC_PATH` is not set.
   - Download I-CONIC API (prefferably both debug and release versions)
   - Unzip downloaded file
   - Open _Project -> CMake Setting_ and add this to _CMake command arguments_ with your iconic-api debug path:  
      `-DICONIC_PATH:STRING="C:/Dev/iconic-api-binary/x64-windows-debug"`
   - At the left add a configuration (plus icon) and select _x64-Release_.
   - For x64-Release configuration do similarly to x64-Debug, add this to _CMake command arguments_ with your iconic-api release path:  
     `-DICONIC_PATH:STRING="C:/Dev/iconic-api-binary/x64-windows-release"`
   - [Optional, but recommended] Change _Configuration type_ to `RelWithDebInfo` to enable debugging symbols (will probably already be selected)
   - Save the file. This should result in CMake finished successfully.
8. At the top of Visual Studio select either `x64-Debug` configuration or `x64-Release` configuration.
9. Build the project:  
   `Build -> Build all`
10. Select `IconicMeasure.exe` in _Select Startup Item_ at the top of Visual Studio
11. Run the built binary:  
    `Debug -> Run`

## Running IconicMeasure.exe

> **Warning** You will get a message that you do not have an I-CONIC API license. Send the license request to info@i-conic.eu to request a license.

- _File -> Open folder_: Open a folder with an image, a depth map and a camera file (or download the [example data](https://drive.google.com/file/d/1h-bBEX3nqHUjjmbcQ2lAMTOktAqihPhV))
- _Tab_: Go to the next (and only) frame
- Move around with mouse, zoom with mouse wheel
- _View -> Measure_: enter measure mode
- Mouse click; Creates a red point and shows camera coordinates and 3D coordinates in status bar

## Generating code documentation

> The documentation for the `main` branch is a available as a website at https://i-conic-vision-ab.github.io/iconic-measure/

1. Download doxygen from [from their website](https://doxygen.nl/download.html#srcbin) (Windows binary setup is easiest)
2. Open the installed `doxywizard.exe`
3. _File -> Open_ and select `iconic-measure/doc/iconic-measure.doxygen` (You may want to associate extension .doxygen with the doxywizard to enable double-clicking the file in the future. Otherwise you can find it in _File -> Open recent_ the next time)
4. Click the `Run` tab
5. Click the `Run doxygen` button
6. Click the `Show HTML output` button. This opens the documentation. It can from now on also be opened directly under `iconic-measure/doc/html/index.html`

### Usage with Visual Studio

> In Visual Studio, write `/**` above a function or variable definition and press enter to auto generate a doxygen skeleton for that symbol. Place this documentation above the symbol in the corresponding header file.

- If it doesn't work, change _Generated documentation comments style_ to `Doxygen (/**)` under _Tools -> Options -> Text Editor -> C/C++ -> Code Style -> General_.
- You may also want to try the _Visual Studio_ extension [VSDoxyHighlighter](https://marketplace.visualstudio.com/items?itemName=Sedenion.VSDoxyHighlighter) or the _Visual Studio Code_ extension [Doxygen Documentation Generator](https://marketplace.visualstudio.com/items?itemName=cschlosser.doxdocgen).

### Example doxygen comment

```c++
/**
 * @brief Checks if there are more sheep than cows on the farm.
 *
 * Uses the superadvanced algorithm LavaMint™ to calculate stuff. Lorem ipsum dolor sit amet.
 * Donec a ipsum sit amet diam scelerisque mollis.
 * Can also work as a spaceship computer.
 *
 * @param sheepAmount The number of sheep on the farm, >=0
 * @param cowPercent The percent of cows on the farm, >=0
 * @return true if there are more sheep than cows
 * @return false if there are the same amount of cows and sheep
 *
 * @warning Will crash if sheepAmount < 0
 * @warning Will crash if cowPercent < 0
 * @warning Will crash if sheepAmount < cowPercent
 * @attention Remember that the percentage 10% is written like 0.10
 *
 * @todo Fix warnings
 * @todo Add ability to have pigs on the farm
*/
```

More commands are available in the [Doxygen Documentation](https://doxygen.nl/manual/commands.html) but be sure to use the at `@` prefix instead of a backslash `\`.
