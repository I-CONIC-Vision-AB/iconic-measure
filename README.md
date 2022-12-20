# iconic-measure
Library and sample application for 3D measurements in image width hidden depth map
See [here](https://drive.google.com/file/d/1WKryW56hkotbfvl1B7s-fkTOKb_MrOiO/view?usp=share_link).
## Build procedure
* Install Visual Studio including C++ desktop module
* Follow the steps in the ReadMe at the bottom of [wxWebMap](https://github.com/I-CONIC-Vision-AB/wxWebMap) to install vcpkg, but do not run the install line starting with ```.\vcpkg.exe install```. Instead run this install command:
```.\vcpkg.exe install --triplet=x64-windows boost-compute boost-thread boost-timer boost-geometry boost-test boost-smart-ptr opengl opencl glew wxwidgets```. This will take a while. 
* Download I-CONIC API from [here](https://drive.google.com/file/d/1phWqDfITjaeuj5fQ7yT201WRIaH_maUk/view?usp=share_link)
* Unzip downloaded file
* Clone the ```iconic-measure``` repository from [here](https://github.com/I-CONIC-Vision-AB/iconic-measure)
* Open Visual Studio and select Open Folder. Go to the cloned ```iconic-measure``` folder
* You will get an error that ```ICONIC_PATH``` is not set. 
    * Open ```CMakeLists.txt``` and set ICONIC_PATH to the path to the unzipped I-CONIC API, or
    * Add ```-DICONIC_PATH:STRING="C:/Dev/iconic-api-binary(or your path)"``` to CMake Command Arguments in ```Project->CMake Setting```
    * Save. This should result in CMake finished successfully
* Select ```Project->CMake settings...```. At the left add a configuration (plus icon) and select ```x64-Release```. 
    * For the x64-Release configuration do as for x64-Debug. i.e. Add ```-DICONIC_PATH:STRING="C:/Dev/iconic-api-binary(or your path)"``` to CMake Command Arguments
    * Enter ```Ctrl+S``` to save. 
* At the top of Visual Studio change from x64-Debug configuration to x64-Release configuration.
* Build->Build all
* Select IconicMeasure.exe in Select Startup Item
* Debug->Run
## Running IconicMeasure.exe
* File->Open; Open a video file
* Space bar; Play video
* Space bar again; Pause video
* Move around with mouse, zoom with mouse wheel
* View->Measure; enter measure mode
* Mouse click; Creates a red point and shows camera coordinates in status bar 