/* BASIC INTERRUPT VECTOR TABLE FOR STM8S103F3
 * All interrupts mapped to NonHandledInterrupt to avoid linker errors.
 */

typedef void @far (*interrupt_handler_t)(void);

struct interrupt_vector
{
    unsigned char interrupt_instruction;
    interrupt_handler_t interrupt_handler;
};

/* Заглушка для всіх непередбачених переривань (Захист від зависання) */
@far @interrupt void NonHandledInterrupt (void)
{
    return;
}

/* Стандартна функція запуску Cosmic (точка входу) */
extern void _stext();

/* Декларація нашого обробника системного часу (лежить в іншому файлі) */
extern @far @interrupt void TIM4_UPD_OVF_IRQHandler(void);

struct interrupt_vector const _vectab[] = {
    {0x82, (interrupt_handler_t)_stext}, /* reset */
    {0x82, NonHandledInterrupt}, /* trap  */
    {0x82, NonHandledInterrupt}, /* irq0  */
    {0x82, NonHandledInterrupt}, /* irq1  */
    {0x82, NonHandledInterrupt}, /* irq2  */
    {0x82, NonHandledInterrupt}, /* irq3  */
    {0x82, NonHandledInterrupt}, /* irq4  */
    {0x82, NonHandledInterrupt}, /* irq5  */
    {0x82, NonHandledInterrupt}, /* irq6  */
    {0x82, NonHandledInterrupt}, /* irq7  */
    {0x82, NonHandledInterrupt}, /* irq8  */
    {0x82, NonHandledInterrupt}, /* irq9  */
    {0x82, NonHandledInterrupt}, /* irq10 */
    {0x82, NonHandledInterrupt}, /* irq11 */
    {0x82, NonHandledInterrupt}, /* irq12 */
    {0x82, NonHandledInterrupt}, /* irq13 */
    {0x82, NonHandledInterrupt}, /* irq14 */
    {0x82, NonHandledInterrupt}, /* irq15 */
    {0x82, NonHandledInterrupt}, /* irq16 */
    {0x82, NonHandledInterrupt}, /* irq17 */
    {0x82, NonHandledInterrupt}, /* irq18 */
    {0x82, NonHandledInterrupt}, /* irq19 */
    {0x82, NonHandledInterrupt}, /* irq20 */
    {0x82, NonHandledInterrupt}, /* irq21 */
    {0x82, NonHandledInterrupt}, /* irq22 */
    
    /* Підключено обробник системного часу (1 мілісекунда) */
    {0x82, (interrupt_handler_t)TIM4_UPD_OVF_IRQHandler}, /* irq23 - TIM4 Update/Overflow */
    
    {0x82, NonHandledInterrupt}, /* irq24 */
    {0x82, NonHandledInterrupt}, /* irq25 */
    {0x82, NonHandledInterrupt}, /* irq26 */
    {0x82, NonHandledInterrupt}, /* irq27 */
    {0x82, NonHandledInterrupt}, /* irq28 */
    {0x82, NonHandledInterrupt}, /* irq29 */
};