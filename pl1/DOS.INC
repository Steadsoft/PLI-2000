
/****************************************************************************/
/* This include file defines all of the DOS and BIOS service subroutines.   */
/****************************************************************************/

    dcl dos$read_joystick               entry (bin(15),   /* switches      */
                                               bin(15),   /* A x           */
                                               bin(15),   /* A y           */
                                               bin(15),   /* B x           */
                                               bin(15),   /* B y           */
                                               bin(15));  /* status        */ 
            

    dcl dos$set_mouse_sensitivity       entry (bin(15),   /* Hor mickeys   */
                                               bin(15),   /* Ver mickeys   */
                                               bin(15));  /* Double thresh */

    dcl dos$set_repeat_rate             entry (bin(15),   /* KB delay */
                                               bin(15));  /* KB rate  */

    dcl dos$clear_screen                entry;

    dcl dos$reboot                      entry;

    dcl dos$get_dos_memory              entry (bin(15));

    dcl dos$get_video_info              entry (bin(15),   /* columns   */
                                               bin(15),   /* curr mode */
                                               bin(15));  /* curr page */

    dcl dos$set_video_mode              entry (bin(15));

    dcl dos$set_overscan                entry (bin(15));

    dcl dos$set_pixel                   entry (bin(15),   /* value */
                                               bin(15),   /* page  */
                                               bin(15),   /* x     */
                                               bin(15));  /* y     */

    dcl dos$get_pixel                   entry (bin(15),   /* value */
                                               bin(15),   /* page  */
                                               bin(15),   /* x     */
                                               bin(15));  /* y     */

    dcl dos$get_current_disk            entry (bin(15));

    dcl dos$set_current_disk            entry (bin(15));

    dcl dos$terminate                   entry (bin(15));

    dcl dos$get_cursor                  entry (bin(15),
                                               bin(15));

    dcl dos$set_cursor                  entry (bin(15),    /* page */
                                               bin(15),    /* y    */
                                               bin(15));   /* x    */

    dcl dos$reset_mouse                 entry (bin(15));   /* status code */

    dcl dos$hide_mouse                  entry;

    dcl dos$show_mouse                  entry;

    dcl dos$set_mouse_int_rate          entry (bin(15));   /* int flags    */

    dcl dos$get_mouse_status            entry (bin(15),    /* mouse status */
                                               bin(15),    /* X co-ord     */
                                               bin(15));   /* Y co-ord     */

    dcl dos$set_mouse_position          entry (bin(15),
                                               bin(15));

    dcl dos$get_dos_info                entry (bin(15),    /* Major Ver no */
                                               bin(15),    /* Minor Ver no */
                                               bin(15));   /* OEM Serial   */

    dcl dos$open_file                   entry (char(*) var, /* path     */
                                               bin(15),     /* mode     */
                                               bin(15),     /* handle   */
                                               bin(15));    /* error    */ 

/*************************** end of include file ****************************/


 

