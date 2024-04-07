HC-SR501 Passive Infrared sensor is specifically designed for the detection of objects that emmit heat such as people or animals using infrared radiation.

A PIR Sensor consists of two main parts:
1. Pyroelectric Sensor
2. Fresnel Lens: Focuses the infrared signal on the pyroelectric sensor.

![[Pasted image 20240407192833.png]]

### Pyroelectric Sensor:

A pyroelectric sensor comprises a window featuring two rectangular apertures and is typically crafted from a material such as coated silicon, enabling the passage of infrared radiation. Positioned behind the window are two distinct infrared sensor electrodes, with one responsible for generating a positive output and the other for generating a negative output.

These electrodes are interconnected in a manner that nullifies each other's effect, as the objective is to discern alterations in IR levels rather than ambient IR levels. Consequently, when one half perceives a variance in IR radiation compared to the other, an output is generated.

Under static conditions, where no movement occurs in proximity to the sensor, both apertures detect an equal amount of infrared radiation, resulting in a null output signal.

However, when a warm entity, such as a human or animal, moves past the sensor, it initially obstructs one of the apertures, causing a positive disparity between the two halves. As the warm entity progresses through and exits the sensing area, the reverse occurs, leading to a negative disparity, thereby prompting the sensor to produce a distinct output. By analyzing this alteration in voltage, motion is detected.

### The Fresnel Lens:

The Fresnel lens employed in this context enhances the range and field of view of the PIR sensor. Its sleek, lightweight design coupled with its exceptional light-collecting ability renders it highly advantageous for creating compact yet potent PIRs.

Comprising a sequence of concentric grooves etched into the plastic, a Fresnel lens functions as a collection of individual refractive surfaces that converge parallel light rays onto a focal point. Consequently, despite its reduced dimensions, a Fresnel lens is capable of light focusing akin to that achieved by a traditional optical lens.


![[Pasted image 20240407193317.png]]

## HC-SR501 PIR Sensor Hardware Overview

 The HC-SR501 PIR sensor is favored for detecting human presence due to its low power consumption, cost-effectiveness, and ease of use.

The HC-SR501 PIR sensor operates on 5V to 12V power and ground connection, providing a high output when motion is detected and a low output during idle periods.

Integration with a microcontroller allows for responsive actions such as controlling lighting and activating fans.

The sensor consumes less than 2mA of current and can detect motion up to 7 meters with sensitivity control.

### BISS0001 PIR Controller:

![[Pasted image 20240407193758.png]]

This chip takes the output from the Pyroelectric sensor and does some minor processing on it to emit a digital output pulse.

![[BISS0001.pdf]]

### Power:

The module comes with a 3.3V precision voltage regulator, so it can be powered by any DC voltage from 4.5 to 12 volts, although 5V is commonly used.

The module comes with a protection diode (also known as a safety diode) to protect the module from reverse voltage and current. So even if you accidentally connect the power with incorrect polarity, your module will not be damaged.

![[Pasted image 20240407193856.png]]

### Sensitivity Adjustment:

The PIR sensor has a potentiometer on the back to adjust the sensitivity.

![[Pasted image 20240407193923.png]]

The potentiometer regulates the maximum detection range of the sensor. Sensitivity adjustment is possible within a range spanning approximately 3 meters to 7 meters (9 to 21 feet). It's important to note that room layout and structure may influence the effective range achieved. Turning the potentiometer clockwise enhances sensitivity and extends the detection range, while rotating it counterclockwise reduces sensitivity and shortens the range.

### Time-Delay Adjustment:

There is another potentiometer on the back of the PIR sensor to adjust the Time-Delay.

![[Pasted image 20240407194141.png]]

The potentiometer serves to adjust the duration for which the output remains in a HIGH state following motion detection. This duration can be varied from 1 second to approximately 3 minutes. Rotating the potentiometer clockwise extends the delay period, whereas turning it counterclockwise reduces the delay duration.

### Trigger Selection Jumper:

The Trigger Selection Jumper facilitates the selection between two trigger modes, governing the sensor's response upon detecting motion.

 - In Single Trigger Mode, a single trigger is activated despite continuous motion.

 - Conversely, Multiple Trigger Mode generates a sequence of triggers in response to continuous motion.

The board incorporates a berg jumper (or solder bridge jumper in some modules) to enable users to toggle between these two modes.

![[Pasted image 20240407194334.png]]

Selecting option L activates the single trigger mode. In this mode, the sensor immediately triggers the output to go HIGH upon detecting motion. The output remains in the HIGH state for a duration determined by the Time-Delay potentiometer. Subsequent motion detections are disregarded until the output returns to LOW at the end of the time delay. If motion persists after the output resets to LOW, it will trigger the output to go HIGH again. As depicted in the accompanying image, Motion #3 is entirely disregarded in this mode.

![[Pasted image 20240407194417.png]]

Option H is chosen to activate the multiple trigger mode. In this mode, the sensor promptly elevates the output to a HIGH state upon detecting motion. The output persists in the HIGH state for a duration dictated by the Time-Delay potentiometer. Unlike the single trigger mode, successive motion detections are not inhibited; instead, the time delay resets with each motion detection. Following cessation of motion, the output transitions to a LOW state only after the time delay elapses. This operational characteristic is why this mode is referred to as the multiple trigger mode.

![[Pasted image 20240407194454.png]]

### Technical Specifications

Here are the specifications:

|                      |                            |
| -------------------- | -------------------------- |
| Operating Voltage    | 4.5 – 20V (typically 5V)   |
| Maximum Current Draw | < 2mA                      |
| Time Delay           | ~ 1 sec to 3 min           |
| Detection Distance   | 3 – 7 meters (9 – 21 feet) |
| Detection Angle      | 120 degrees (typically)    |

### HC - SR501 PIR Sensor Pinout:

