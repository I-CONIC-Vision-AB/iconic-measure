# iconic-measure {#mainpage}
Library and sample application for 3D measurements in image with hidden depth map
See [here](https://drive.google.com/file/d/1WKryW56hkotbfvl1B7s-fkTOKb_MrOiO/view?usp=share_link).
## Build procedure
* Install Visual Studio including C++ desktop module
* Follow the steps in the ReadMe at the bottom of [wxWebMap](https://github.com/I-CONIC-Vision-AB/wxWebMap) to install vcpkg, but do not run the install line starting with ```.\vcpkg.exe install``` 
(if you have run it already, no problem, but run the following as well). Instead run this install command: ```.\vcpkg.exe install --triplet=x64-windows boost-compute boost-thread boost-timer boost-geometry boost-test boost-smart-ptr opengl opencl glew eigen wxwidgets```. This will take a while. 
* Download I-CONIC API 
* Unzip downloaded file
* Clone the ```iconic-measure``` repository from [here](https://github.com/I-CONIC-Vision-AB/iconic-measure)
* Open Visual Studio and select Open Folder. Go to the cloned ```iconic-measure``` folder
* You will get an error that ```ICONIC_PATH``` is not set. 
    * Add ```-DICONIC_PATH:STRING="C:/Dev/iconic-api-binary/x64-windows-debug"``` to CMake Command Arguments in ```Project->CMake Setting```
    *  Enter ```Ctrl+S``` to save. This should result in CMake finished successfully
* Select ```Project->CMake settings...```. At the left add a configuration (plus icon) and select ```x64-Release```. 
    * Add ```-DICONIC_PATH:STRING="C:/Dev/iconic-api-binary/x64-windows-release"``` to CMake Command Arguments in ```Project->CMake Setting```
    * [Optional, but recommended] Change ```Configuration type``` to ```RelWithDebInfo``` to enable debugging your code
    * Enter ```Ctrl+S``` to save. 
* At the top of Visual Studio change from ```x64-Debug``` configuration to ```x64-Release``` configuration.
* *Build->Build all*
* Select ```IconicMeasure.exe``` in *Select Startup Item* at the top of Visual Studio
* *Debug->Run*
## Generating code documentation
* Download doxygen from [here](https://doxygen.nl/download.html) (e.g. Windows setup binaries)
* Open the installed ```doxywizard.exe```
* ```File->Open``` and select ```iconic-measure/doc/iconic-measure.doxygen``` (You may want to associate extension .doxygen with the doxywizard to enable double-clicking the file in the future. Otherwise you can find it in ```File->Open recent...``` the next time)
* Click the ```Run``` tab
* Click the ```Run doxygen``` button
* Clik the ```Show HTML output``` button. This opens the documentation. It can from now on also be opened directly under ```iconic-measure/doc/html/index.html```
## Running IconicMeasure.exe
* You will get a message that you do not have an I-CONIC API license. Send the license request to info@i-conic.eu to request for a license
* *File->Open folder*; Open a folder with an image, a depth map and a camera file
* *Tab*; Go to the next (and only) frame
* Move around with mouse, zoom with mouse wheel
* *View->Measure*; enter measure mode
* Mouse click; Creates a red point and shows camera coordinates and 3D coordiantes in status bar. Note that the Z coordinate is defined as distance to viewer, not height from the ground. A smaller value thus has a higher altitude from the ground. 
