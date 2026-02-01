

/*
  main.c
  STM32 KEY 입력 및 FND 출력 핵심 로직만 가져왔습니다.
  Mode-1 시계 1초 갱신
  Mode-2 Up 카운팅
  Mode-3 Down 카운팅
  Mode-4 구구단

 [구현 내용 요약]
 - EXTI 인터럽트를 이용한 Key 입력 처리
 - Timer 인터럽트(TIM2, TIM7, TIM12)를 이용한 주기 제어
 - Mode 기반 기능 분기 (시계 / Up / Down 카운팅 / 구구단)
 - SysTick 기반 FND 출력
*/

// 인터럽트 콜백 함수
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

	// 500ms 카운팅
	if (htim->Instance == TIM2)
	{
		if(fg->start_stop_flag == 1 && kp->key_cnt == 2)
		{
			ct->cnt16 = (ct->cnt16 >= 4999) ? 0 : ct->cnt16 + 1;
		}
		else if(fg->start_stop_flag == 1 && kp->key_cnt == 3)
		{
			ct->dcnt16 = (ct->dcnt16 <= 1) ? 5000 : ct->dcnt16 - 1;
		}
	}

	// 1sec 마다 시간 계산
	if (htim->Instance == TIM7)
	{
		if(tm->sec >= 59)
		{
			tm->sec = 0;

			tm->min++;
			if(tm->min > 59)
			{
				tm->min = 0;
			}
		}
		tm->sec++;
	}

	// 800ms 마다 구구단
	if(htim->Instance == TIM12)
	{
		if(fg->start_stop_flag == 1 && kp->key_cnt == 4)
		{
			if(ct->go_y >= 9)
			{
				ct->go_y = 0;

				ct->go_x++;
				if(ct->go_x > 9)
				{
					ct->go_x = 1;
				}
			}
			ct->go_y++;
		}
	}
}

// 외부인터럽트 콜백 함수
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	switch(GPIO_Pin)
	{
	case SW1_Pin:
		if(HAL_GPIO_ReadPin(SW1_GPIO_Port, SW1_Pin) == 0)
		{
			HAL_Delay(20);
			if(HAL_GPIO_ReadPin(SW1_GPIO_Port, SW1_Pin) == 0)
			{
				HAL_Delay(10);
				if(HAL_GPIO_ReadPin(SW1_GPIO_Port, SW1_Pin) == 0)
				{
					HAL_Delay(5);
					if(HAL_GPIO_ReadPin(SW1_GPIO_Port, SW1_Pin) == 0)
					{
						HAL_Delay(1);
						if(fg->start_stop_flag == 0)
						{
							kp->key_cnt = (kp->key_cnt >= 4) ? 1 : kp->key_cnt + 1;
							fg->key_flag = 1;
						}
					}
				}
			}
		}
		break;
case SW2_Pin:
		if(HAL_GPIO_ReadPin(GPIOA, SW2_Pin) == 0)
		{
			HAL_Delay(20);
			if(HAL_GPIO_ReadPin(GPIOA, SW2_Pin) == 0)
			{
				HAL_Delay(10);
				if(HAL_GPIO_ReadPin(GPIOA, SW2_Pin) == 0)
				{
					HAL_Delay(5);
					if(HAL_GPIO_ReadPin(GPIOA, SW2_Pin) == 0)
					{
						HAL_Delay(1);
						fg->start_stop_flag = (fg->start_stop_flag == 0) ? 1 : 0;
					}
				}
			}
		}
		break;

	case SW3_Pin:
		if(HAL_GPIO_ReadPin(GPIOB, SW3_Pin) == 0)
		{
			HAL_Delay(20);
			if(HAL_GPIO_ReadPin(GPIOB, SW3_Pin) == 0)
			{
				HAL_Delay(10);
				if(HAL_GPIO_ReadPin(GPIOB, SW3_Pin) == 0)
				{
					HAL_Delay(5);
					if(HAL_GPIO_ReadPin(GPIOB, SW3_Pin) == 0)
					{
						HAL_Delay(1);
						if(fg->start_stop_flag == 0) fg->sw3_flag = 1;
					}
				}
			}
		}
		break;
	}
}

void HAL_SYSTICK_Callback()
{
	// FND Display
	if(kp->key_cnt == 1) 		fnd_595_out((tm->min * 100) + tm->sec);
	else if(kp->key_cnt == 2) 	fnd_595_out(ct->cnt16);
	else if(kp->key_cnt == 3) 	fnd_595_out(ct->dcnt16);
	else if(kp->key_cnt == 4) 	fnd_595_out((ct->go_x * 1000) + (ct->go_y * 100) + (ct->go_x * ct->go_y));
}

// 인터럽트 타이머 시작
HAL_TIM_Base_Start_IT(&htim2);
HAL_TIM_Base_Start_IT(&htim7);
HAL_TIM_Base_Start_IT(&htim12);

/* USER CODE END 2 */

/* Infinite loop */
/* USER CODE BEGIN WHILE */
while (1)
{
	// key 입력
	if(fg->key_flag == 1)
	{
		mode_Display();
	}
	if(fg->sw3_flag == 1)
	{
		  counting();
	}
}
