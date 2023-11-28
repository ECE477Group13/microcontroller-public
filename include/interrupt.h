#ifndef __INTERRUPT_H__
#define __INTERRUPT_H__

void init_interrupts();
void intr_enable();
void intr_disable();


void ext_but1_intr();
void ext_but2_intr();
void int_but_intr(); 
void imu_intr();     
void batt_chg_intr();

#endif