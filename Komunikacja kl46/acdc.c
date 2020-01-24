#include "acdc.h"

#define ACDC_DATA_SIZE 2  // x Bytes
#define ACDC_TIME_SIZE 4
#define ACDC_TAG 104 // i.e 72 or 40 or 104 or 8  // determines output files
#define ACDC_ARR_SIZE 30

void acdcint(void){

	
    SIM->SCGC6 |= SIM_SCGC6_ADC0_MASK;
    ADC0->CFG1 |= ADC_CFG1_ADICLK(1)| ADC_CFG1_ADIV(2)| ADC_CFG2_ADLSTS_MASK;
    ADC0->CFG2 |= ADC_CFG2_ADHSC_MASK | ADC_CFG2_ADLSTS(1);
    ADC0->SC2 |= ADC_SC2_REFSEL(0);
    ADC0->SC3 |= ADC_SC3_AVGE_MASK |  ADC_SC3_AVGS(3);
    ADC0->SC3 |= ADC_SC3_CAL_MASK;
 
     
            while(ADC0->SC3 & ADC_SC3_CAL_MASK){}
    do{
    SIM->SCGC6 |= SIM_SCGC6_ADC0_MASK;
    ADC0->CFG1 |= ADC_CFG1_ADICLK(1)| ADC_CFG1_ADIV(2)| ADC_CFG2_ADLSTS_MASK;
    ADC0->CFG2 |= ADC_CFG2_ADHSC_MASK | ADC_CFG2_ADLSTS(1);
    ADC0->SC2 |= ADC_SC2_REFSEL(0);
    ADC0->SC3 |= ADC_SC3_AVGE_MASK |  ADC_SC3_AVGS(3);
    ADC0->SC3 |= ADC_SC3_CAL_MASK;
    }while(ADC0->SC3 & ADC_SC3_CALF_MASK );
     
                 
        ADC0->PG = ((    ADC0->CLP4+ADC0->CLP3+ADC0->CLP2+ADC0->CLP1 +ADC0->CLP0)/2);
        ADC0->PG |= 0xF0000000;
        ADC0->MG = ((ADC0->CLM4 + ADC0->CLM3 + ADC0->CLM2 + ADC0->CLM1 + ADC0->CLM0)/2);
        ADC0->MG |= 0xF0000000;     
     
    ADC0->CFG1 |= (ADC_CFG1_ADICLK(1) | ADC_CFG1_ADLSMP_MASK | ADC_CFG1_MODE(1));
    ADC0->SC1[0] |= ADC_SC1_AIEN_MASK;
}

uint16_t swiatlo(void){
	uint16_t t = 0;
        ADC0->SC1[0] = ADC_SC1_ADCH(3);
        while(ADC0->SC2 & ADC_SC2_ADACT_MASK);
    while(!(ADC0->SC1[0] & ADC_SC1_COCO_MASK));
    t = (ADC0->R[0]);
		t = (t*3.3 * 1000/4096);
    return t;
}

uint16_t get_acdc(void){
	uint32_t temp_time = 0;
	uint16_t temp = 0;

	if (calculate == 1){ 
		temp = swiatlo();
		calculate = 0;
		temp_time = get_pit_time();
		mass_append(&Pit_priv, &Arr3, temp , temp_time, 0, ACDC_ARR_SIZE, ACDC_TAG, ACDC_DATA_SIZE, ACDC_TIME_SIZE);
//		arr_append(&Pit_priv, &Arr2, temp >> 8 , 0, 30, 8);
//		arr_append(&Pit_priv, &Arr2, temp , 0, 30, 8);
//		arr_append(&Pit_priv, &Arr2, temp_time >> 24 , 0, 30, 8);
//		arr_append(&Pit_priv, &Arr2, temp_time >> 16 , 0, 30, 8);
//		arr_append(&Pit_priv, &Arr2, temp_time >> 8 , 0, 30, 8);
//		arr_append(&Pit_priv, &Arr2, temp_time  , 0, 30, 8);
	}
	return temp;
}

