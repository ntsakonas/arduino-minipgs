# Arduino minipgs
This is a simple pocket-sized gps using an arduino nano board and low cost GPS module (NEO-7M).
A small 128x64 OLED screen is used to display basic GPS information,such as latitude, longitude, date and time. As a bonus,the maindenhead locator is also displayed.

A breadboarded schematic is shown below (made with Fritzing. Credits to Robert Eisele for the Fritzing GPS module part and for the idea of using TinyGPS lib, and to user `mtd` from the Fritzing forums for the OLED screen part)  


![minigps_bb](https://user-images.githubusercontent.com/5435240/50376052-53aa9b80-05ff-11e9-89c0-52d2a1ef03b7.png)


The project is built using PlatformIO and all the libraries required are copied into the `libs` folder so that you do not have to worry about having them installed.

The code is very straight forward and needs no special explanations.

A prototype was built in a 2-layer sandwitch form on a 3cmx7cm prototype board. It is a bit bulky as a lot of connectors are used but it can be made vey flat if the connectors are removed. 


![img_20181222_154833](https://user-images.githubusercontent.com/5435240/50376246-9a4dc500-0602-11e9-9e57-4bf26f79ffc0.jpg)
![img_20181222_154906](https://user-images.githubusercontent.com/5435240/50376249-a2a60000-0602-11e9-8c3f-eb6a93a9dc61.jpg)
![img_20181222_154914](https://user-images.githubusercontent.com/5435240/50376250-ab96d180-0602-11e9-9f08-9fc232e46e52.jpg)


There are no photos of the gps while in operation as the multiplexing of the screen does not allow any decent photos to be taken.


References/Credits
- https://github.com/infusion/Fritzing/tree/master/GY-NEO6MV2%20GPS
- https://www.xarg.org/2016/06/neo6mv2-gps-module-with-arduino/
- http://forum.fritzing.org/t/oled-128x64-i2c-monochrome-display-ssd1306-created/1202
