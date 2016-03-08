# TODOs
- General
  - [ ] code cleanup and refactoring
  - [ ] documentation


- ColorUtils
  - [ ] white balance calculations (preliminary interface available)
  - [ ] implement linear HSV->RGB conversion 
  - [ ] implement rainbow HSV->RGB conversion

- Animations
  - [ ] extend animation interface for effects (lower/faster/brighter/darker/color settings)
  	- [x] method for brighter/darker
  	- [x] method for faster/slower
  	- [ ] method for changing params
  - [ ] create standard animations 

- RGB controls
  - [ ] rgb to hsv conversion
  - [ ] rgb transition methods -> use hsv transition


# DONE
- General
  - [x] change colormode of controller (RGB, RGBWW, RGBCW, RGBWWCW)
  - [x] refactor to provide callback function when an Animation has finished 
    The idea is to allow saving of the last value/state of the animation
    
- Color Utils
  - [x] HSV -> RGB conversion
  - [x] Adjustment for HSV base colors (yellow, green, magenta, blue, cyan, red)
  - [x] Max Brightness correction per channel
  - [x] brightness lookup table
  

- Animations
  - [x] HSV Transistions
  - [x] Animation queue -> allow for queueing animations 

- PWM
  - [x] abstraction for pwm output to allow to use ESP Hardware pwm when uing SMING framework
  - [x] keep compatibility with arduino APIs
