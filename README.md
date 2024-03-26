<h1 align="center" id="title">On The Road To Mastery</h1>

<p align="center"><img src="https://socialify.git.ci/Heliam31/RoadToMastery/image?description=1&amp;descriptionEditable=Cooperating%2C%20Line%20Following%20and%20Maze%20Solving%20robots%20!&amp;font=Inter&amp;language=1&amp;logo=https%3A%2F%2Fwiki.stmicroelectronics.cn%2Fstm32mcu%2Fnsfr_img_auth.php%2Farchive%2F4%2F4e%2F20200318133105%2521STM32.png&amp;name=1&amp;owner=1&amp;pattern=Circuit%20Board&amp;theme=Light" alt="project-image"></p>

<p id="description">This project takes place within our M2 Long Project UE where two groups collaborate to form a unique project. A SIAME team composed of 5 students will be responsible for the hardware and development of the robots while an RSD team formed of 2 students will handle communications between the robots and the Raspberry Pi as well as monitoring. The project was proposed by Mohamed Benallal and Hugo Blaise to Mr. Hugues Cassé and presents itself as a follow-up to the line-following robot project carried out in L3 by a part of the group.</p>

<p align="center"><img src="https://img.shields.io/badge/Built%20By-M2--SECIL%3C/%3E-blue?style=for-the-badge" alt="shields"> <img src="https://img.shields.io/badge/Open%20Source-%F0%9F%92%9C-green?style=for-the-badge" alt="shields"> <img src="https://img.shields.io/badge/BARE--METAL-black?style=for-the-badge&amp;logo=c" alt="shields"> <img src="https://img.shields.io/badge/Maintained-No-red?style=for-the-badge" alt="shields"></p>

  
  
<h2>🧐 Features</h2>

Here're some of the project's best features:

*   Line follower
*   Communication : Robot I2C <--BLE--> Server
*   Sonar obstacle detection
*   Colored keys and doors detection

<h2>🛠️ Installation Steps:</h2>

<p>1. Dependencies</p>

```
GCC G++ Make and Openocd
```

<p>2. Connection to STM32F4 in one terminal</p>

```
make openocd
```

<p>3. Inject code in another terminal</p>

```
make
```

<p>4. Optional : display logs in terminal</p>

```
make log
```

<h2>🔍 How To Use:</h2>

<p>1. Calibration</p>

```
Lay robot's IR leds on top of black and white surface then press the button
```

<p>2. Start</p>

```
When ready press the button again to start !
```

  
  
<h2>💻 Built with</h2>

Technologies used in the project:

*   STM32F4
*   ESP32
*   Raspberry Pi 4
*   BLE
*   Pololu QTR8RC
*   HC-SR04
*   TCS-3200
*   Custom Bare-Metal Library

<h2>🛡️ License:</h2>

This project is licensed under the MIT

<h2>💖Like our work?</h2>
Unfortunately we are not providing any support for the moment. Some unfortunately unimplemented features are :

- RFID Key System
- Local demo labyrinth
- Constant loop frequency
- Green Color
- Color calibration
- Advanced more complexe labyrinth (fake intersections)

Ce document à pour but d'apporter des informations complémentaires au rapport de projet. Pour toute question contactez nous sur le Discord SECIL !