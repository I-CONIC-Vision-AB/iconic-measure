# iconic-measure
Library and sample application for 3D measurements in image width hidden depth map
See [here](https://drive.google.com/file/d/1WKryW56hkotbfvl1B7s-fkTOKb_MrOiO/view?usp=share_link).
## Build procedure
- Install Visual Studio including C++ desktop module
- Follow the steps in the ReadMe at the bottom of [wxWebMap](https://github.com/I-CONIC-Vision-AB/wxWebMap) to install vcpkg, but do not run the install line starting with ```.\vcpkg.exe install```. Instead run this install command:
```.\vcpkg.exe install --triplet=x64-windows boost-compute boost-thread boost-timer boost-geometry boost-test boost-smart-ptr opengl opencl glew wxwidgets```. This will take a while. 
- Download I-CONIC API from [here](https://drive.google.com/file/d/1phWqDfITjaeuj5fQ7yT201WRIaH_maUk/view?usp=share_link)
- Unzip downloaded file
- Clone the ```iconic-measure``` repository from [here](https://github.com/I-CONIC-Vision-AB/iconic-measure)
- Open Visual Studio and select Open Folder. Go to the cloned ```iconic-measure``` folder
- You will get an error that ```ICONIC_PATH``` is not set. 
-- Open ```CMakeLists.txt``` and set ICONIC_PATH to the path to the unzipped I-CONIC API, or
-- Add ```-DICONIC_PATH:STRING="C:/Dev/iconic-api-binary(or your path)"``` to CMake Command Arguments in ```Project->CMake Setting```
-- Save. This should result in CMake finished successfully
- Select ```Project->CMake settings...```. At the left add a configuration (plus icon) and select ```x64-Release```. 
-- For the x64-Release configuration do as for x64-Debug. i.e. Add ```-DICONIC_PATH:STRING="C:/Dev/iconic-api-binary(or your path)"``` to CMake Command Arguments
-- Enter ```Ctrl+S``` to save. 
- At the top of Visual Studio change from x64-Debug configuration to x64-Release configuration.


Ange path till upp-packad folder som ICONIC_PATH i CMakeLists.txt i iconic-measure 
Skapa x64-Release configuration i Visual Studio när du öppnat VS
Byt till x64-Release i VS
Build all
Kör IconicMeasure.exe
File->Open; select video
Space bar; Play
Space bar again; Pause
Flytta runt och zooma med mus
View->Measure; Gå över till mät-mode
Klicka i bild -> Röd punkt och koordinater i status bar
