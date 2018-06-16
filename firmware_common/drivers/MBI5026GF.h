#ifndef __MBI5026GF_H
#define __MBI5026GF_H

/**********************************************************************************************************************
Type Definitions
**********************************************************************************************************************/

/*---The following define need to change for the right parameter---*/
#define MBI_PIN_CLK  AT91C_PIO_PA14
#define MBI_PIN_SDI  AT91C_PIO_PA15
#define MBI_PIN_LE   AT91C_PIO_PA11
#define MBI_PIN_OE   AT91C_PIO_PA12
#define MBI_LE_TO_H  (AT91C_BASE_PIOA -> PIO_SODR = MBI_PIN_LE)
#define MBI_LE_TO_L  (AT91C_BASE_PIOA -> PIO_CODR = MBI_PIN_LE)
#define MBI_OE_TO_H  (AT91C_BASE_PIOA -> PIO_SODR = MBI_PIN_OE)
#define MBI_OE_TO_L  (AT91C_BASE_PIOA -> PIO_CODR = MBI_PIN_OE)

#define MBI_CLK_STATE (AT91C_BASE_PIOA -> PIO_PDSR & MBI_PIN_CLK)
#define MBI_CLK_TO_H  (AT91C_BASE_PIOA -> PIO_SODR = MBI_PIN_CLK)
#define MBI_CLK_TO_L  (AT91C_BASE_PIOA -> PIO_CODR = MBI_PIN_CLK)
#define MBI_SDI_TO_H  (AT91C_BASE_PIOA -> PIO_SODR = MBI_PIN_SDI)
#define MBI_SDI_TO_L  (AT91C_BASE_PIOA -> PIO_CODR = MBI_PIN_SDI)

#define MBI_PIOA_INIT (MBI_PIN_LE | MBI_PIN_OE | MBI_PIN_CLK | MBI_PIN_SDI)
//#define MBI_PIOB_INIT (<All pins for PIOB |>)

/*-----------------------------------------------------------------*/


/**********************************************************************************************************************
Constants / Definitions
**********************************************************************************************************************/


/**********************************************************************************************************************
Function Declarations
**********************************************************************************************************************/

/*--------------------------------------------------------------------------------------------------------------------*/
/* Public functions                                                                                                   */
/*--------------------------------------------------------------------------------------------------------------------*/
void MBI_SEND_U8(u8);

/*--------------------------------------------------------------------------------------------------------------------*/
/* Protected functions                                                                                                */
/*--------------------------------------------------------------------------------------------------------------------*/
void MBI_PINS_INIT(void);


/*--------------------------------------------------------------------------------------------------------------------*/
/* Private functions                                                                                                  */
/*--------------------------------------------------------------------------------------------------------------------*/


#endif /* __MBI5026GF_H */


/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
