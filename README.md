# iconic-measure
Library and sample application for 3D measurements in image with hidden depth map
See [here](https://drive.google.com/file/d/1WKryW56hkotbfvl1B7s-fkTOKb_MrOiO/view?usp=share_link).
## Build procedure
* Install Visual Studio including C++ desktop module
* Follow the steps in the ReadMe at the bottom of [wxWebMap](https://github.com/I-CONIC-Vision-AB/wxWebMap) to install vcpkg, but do not run the install line starting with ```.\vcpkg.exe install``` 
(if you have run it already, no problem, but run the following as well). Instead run this install command:
```.\vcpkg.exe install --triplet=x64-windows boost-compute boost-thread boost-timer boost-geometry boost-test boost-smart-ptr opengl opencl glew wxwidgets```. This will take a while. 
* Download I-CONIC API 
* Unzip downloaded file
* Clone the ```iconic-measure``` repository from [here](https://github.com/I-CONIC-Vision-AB/iconic-measure)
* Open Visual Studio and select Open Folder. Go to the cloned ```iconic-measure``` folder
* You will get an error that ```ICONIC_PATH``` is not set. 
    * Open ```CMakeLists.txt``` and set ICONIC_PATH to the path for the unzipped I-CONIC API, or
    * Add ```-DICONIC_PATH:STRING="C:/Dev/iconic-api-binary(or your path)"``` to CMake Command Arguments in ```Project->CMake Setting```
    * Save. This should result in CMake finished successfully
* Select ```Project->CMake settings...```. At the left add a configuration (plus icon) and select ```x64-Release```. 
    * For the x64-Release configuration do as for x64-Debug. i.e. Add ```-DICONIC_PATH:STRING="C:/Dev/iconic-api-binary(or your path)"``` to CMake Command Arguments
    * Enter ```Ctrl+S``` to save. 
* At the top of Visual Studio change from ```x64-Debug``` configuration to ```x64-Release``` configuration.
* *Build->Build all*
* Select ```IconicMeasure.exe``` in *Select Startup Item* at the top of Visual Studio
* *Debug->Run*
## Running IconicMeasure.exe
* You will get a message that you do not have an I-CONIC API license. Send the license request to info@i-conic.eu to request for a license
* *File->Open*; Open a video file
* *Space bar*; Play video
* *Space bar* again; Pause video
* Move around with mouse, zoom with mouse wheel
* *View->Measure*; enter measure mode
* Mouse click; Creates a red point and shows camera coordinates in status bar 
