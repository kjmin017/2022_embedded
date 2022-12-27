# 임베디드설계실험
- 하드웨어의 기본적인 이해도 향상과 그에 따른 ARM Cortex-M3(STM32F107) 기반 임베디드 시스템의 설계 기술 습득
- ARM 기반의 마이크로 프로세서 및 센서와 같은 각종 I/O 디바이스를 제어하기 위한 시스템 소프트웨어 기술 습득
- 임베디드 시스템의 소프트웨어 및 하드웨어 디버깅 능력 향상
## 작업 환경
- c
- IAR Embedded Workbench
## 주차별 프로젝트 소개
1. Week3
- Joystick을 이용하여 Led 제어
2. Week4
- Joystick과 User Button을 이용하여 Led와 릴레이 모듈에 연결된 모터 제어/Scatter File 이해
3. Week5
- 오실로스코프를 통해 Clock을 확인하고 UART통신의 원리를 습득하여 Polling 방식을 이용한 시리얼 통신 및 Clock Control 구현
4. Week6
- Interrupt방식을 활용하여 GPIO 제어 및 UART통신 구현
5. Week7
- 기판 납땜을 통해 보드와 Bluetooth 모듈을 연결하고 스마트폰과의 통신 구현
6. Week10
- TFT-LCD와 ADC에 대한 개념을 이해하고 조도센서 값을 LCD에 출력
7. Week11
- Timer 모듈에 대해 이해하고 이를 통해 led와 서브모터 출력 제어
8. Week12
- DMA에 대해 이해하고 이전의 Interrupt방식을 통해 구현한 ADC출력을 DMA를 통해 출력
9. Termproject
- 지금까지 배운 개념을 사용해 온도/습도/조도/수위를 체크해 Bluetooth알림을 보내고 물을 주는 스마트 화분 제작
