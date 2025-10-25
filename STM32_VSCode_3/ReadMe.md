[Steps to setup environment for embedded software development in VS Code]
   - Download and install the STM32CubeMX software with the latest version. The version used when this file was documented was 6.15.0. Additionally, install the STM32CLT and STM32MCUFinder.
   - Download the CMSIS library package as per the following steps:
   - Download the STM32F4 device pack:
         - Go to https://www.keil.com/dd2/pack/ and search for "Keil.STM32F4xx_DFP".
         - Download and execute the file Keil.STM32F4xx_DFP.3.1.1.pack. Set the desired directory.
         - Set the CMSIS_PACK_ROOT environment variable:
               *** Name: CMSIS_PACK_ROOT
               *** Value: *\Arm\Packs
         - Restart VS Code after setting the variable.
* Create a project by selecting your required MCU or Board. For example, STM32F429ZIT6, STM32F429 Discovery Board, etc.
* At the time of Code Generation, must ensure that the Toolchain selected is "CMake". This is vital for developing software using VSCode.
* Open VS Code. The version used when documenting was 1.103.2 (user setup)
* Install the extension "STM32Cube for Visual Studio Code" of v2.1.1 or newer version. 
* Open the Settings page of the above mentioned extension, set the paths to the STM32CubeMX.exe, STM32CubeMCUFinder.exe, and the path where the STM32CLT is installed.
* Now, open the folder where the STM32 project was created in Step 2, in VS Code.
* A pop-up may appear that will ask for configuring the opened project compatible with STM32Cube. If it appears, click OK.
* Find the button "Build" down below in the status bar of VS Code. Click it. This should build and generate the ELF.
* Go to "Run and Debug", prepare the "launch.json" with debugger type as stlinkgdbtarget.
* Select the debug configuration, and click the Play button, to start downloading and debugging.  
