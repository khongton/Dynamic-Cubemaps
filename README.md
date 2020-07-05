# Dynamic Cubemaps

This is a graphics application I wrote as a final for my graphics class in Spring 2016. It's a standalone demo of a dynamic cubemap, and features reflection, refraction, and skyboxing. 

![Program Render](/media/teapot.gif)

# How to Build

## Pre-requisite Software
This build is only for Windows installations, using Visual Studio 14 2015. You'll need the following libraries in order to compile this program:
* Visual Studio [Download Here](https://www.visualstudio.com/vs/)
* CMake [Download here](https://cmake.org/download/) Make sure to add ```cmake``` to system path when asked. Double check that you can run CMake from the command prompt.
* Eigen 3.3 [Source here](http://eigen.tuxfamily.org/index.php?title=Main_Page)
* GLFW [Source here](http://www.glfw.org/download.html)
* GLEW [Compiled Binaries Here](http://glew.sourceforge.net/) 

Download them and unzip them somewhere. Before you can use them, you'll need to set the environment variables so Windows can use them. 

Go to: Control Panel -> System -> Advanced System Settings -> Environment Variables -> User Variables. Add the following variables:
* Add ```EIGEN3_INCLUDE_DIR``` to ```path/to/eigen``` 
* Add ```GLFW_DIR``` to ```path/to/GLFW```
* Add ```GLEW_DIR``` to ```path/to/GLEW```

Each of the ```path/to/*``` should point to the absolute path of the libraries you downloaded.  

## Building the Project
I suggest making a folder named ```build``` in the same directory as the unzipped file contents. From the ```build``` folder, run ```cmake ..``` This will build all the necessary program files. This might take a while, so please be patient. Once it's done, you'll see a Visual Studio solution named ```Final```.  

You'll want to set the project titled ```Final``` from the Solution Explorer as your Start Up Project. You can do this by Right-Clicking on it, and choosing```Set as Start Up Project```. Additionally, you'll need to set the filepath of the resource directory. You can do this by Right-Clicking the ```Final``` project and selecting ```Properties```, which will open up a new window. Click on ```Debugging```, and in the Command Arguments field, enter the absolute path to the resources directory you opened. Click OK when you're done.

You should now be able to compile and build the project by hitting CTRL+F5. 

# Project Website

~~[View the Website I created for this project](http://users.csc.calpoly.edu/~ssueda/teaching/CSC471/2016S/demos/khongton/index.html) to learn more about the project in depth.~~ This website link is no longer supported by my university.

[View the alternative github.io website](https://khongton.github.io/Dynamic-Cubemaps/) in case the link above becomes broken. The content is exactly the same!
