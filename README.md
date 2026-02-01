# STM32 Interrupt 기반 멀티펑션보드 Key Mode 제어 및 FND 출력
3일간 진행한 미니 프로젝트로 STM32 + 멀티펑션보드 기반 스위치 처리를 통한 FND 출력 입니다.

## 최종 구현 사진 및 영상
[링크](https://drive.google.com/drive/folders/1Oj89G7a-d3ho9iwJ4gv4as-oEvTSQAml)

## 프로젝트 개요
- 기간: 3일
- 인원: 1명(개인 프로젝트)
- 목적: 본 프로젝트는 STM32 멀티펑션보드에서 **Key 입력(EXTI)** 과  
**Timer Interrupt(TIM2, TIM7, TIM12)** 를 이용해  
Mode 별 동작을 수행하고 결과를 **FND** 로 출력하는 미니 프로젝트입니다.
> 해당 저장소에는 핵심 로직과 동작 구조 설명만 포함되어 있습니다.
> 핵심 로직은 'main.c' 단일 파일에 집중했습니다.

---

## 기능 요약
| Mode | 기능 설명 |
|------|----------|
| Mode-1 | 시계 기능 (1초 단위 시간 갱신) |
| Mode-2 | Up 카운팅 (500ms 단위 증가) |
| Mode-3 | Down 카운팅 (500ms 단위 감소) |
| Mode-4 | 구구단 출력 (800ms 단위 진행) |

---

## Key 동작 정의
| Key | 기능 |
|-----|------|
| SW1 | Mode 변경 (Start/Stop_flag가 0일 때만 동작) |
| SW2 | Start/Stop Toggle |
| SW3 | 기능 실행 트리거 (Start/Stop_flag가 0일 때만 set) |

> Key 입력은 `HAL_GPIO_EXTI_Callback()`에서 처리합니다.

---

## 타이머 역할
| Timer | 주기 | 역할 |
|------|------|------|
| TIM2  | 500ms 카운팅 기반 | Mode-2/3 카운터 업데이트 |
| TIM7  | 1초 | 시계 시간 계산(sec/min) |
| TIM12 | 800ms | 구구단 진행(go_x/go_y 업데이트) |

> Timer Update는 `HAL_TIM_PeriodElapsedCallback()`에서 처리합니다.

---

## Display(FND) 출력
| Mode | 출력 값 |
|------|---------|
| 1 | (min * 100) + sec |
| 2 | cnt16 |
| 3 | dcnt16 |
| 4 | (go_x * 1000) + (go_y * 100) + (go_x * go_y) |

> FND 출력은 `HAL_SYSTICK_Callback()`에서 수행합니다.

---

## 전체 동작 흐름
1. **EXTI 인터럽트**
   - Key 입력 감지 및 상태 flag 설정
2. **Timer 인터럽트**
   - 시간 / 카운터 / 구구단 데이터 갱신
3. **SysTick 콜백**
   - 현재 Mode에 맞는 FND 데이터 출력
4. **main loop**
   - flag 기반 화면 갱신 및 기능 처리

## 개발 과정
1. 멀티펑션보드에 FND 출력 시도
2. 외부 인터럽트 방식으로 Switch 동작 구현
3. 각 모드별로 타이머 셋팅
4. Switch 3번에 Long Press 기능 구현 시도
5. 일정 부족으로 Long Press 기능 제거


## 아쉬운 점
- Long Press 기능 구현 못한 부분
- FND 값 구분을 위한 DP 활용이나 SW3 토글 기능 설계를 사전에 충분히 고려하지 못한 점

## 배운 점
- 인터럽트 기반 입력 처리 구조에 대한 이해
- 다중 타이머를 활용한 역할 분리 설계
- Flag 중심 제어 방식 경험
